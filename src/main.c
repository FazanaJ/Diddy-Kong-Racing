#include "main.h"
#include "game.h"
#include "memory.h"
#include "stacks.h"
#include "thread0_epc.h"
#include "thread3_main.h"
#include "PR/os_internal.h"
#include "PRinternal/piint.h"
#include "joypad.h"
#include "video.h"
#include "string.h"
#include "asset_loading.h"

/************ .bss ************/

u64 *gThread3Stack;
OSThread gThread1; // OSThread for thread 1
OSThread gThread3; // OSThread for thread 3
u16 gPlatform;
u8 gPlatformSet;
u8 gExpansionPak;
u8 gUseExpansionMemory;
UserConfig gConfig;

s32 gOverlayCacheSize;
s32 *gOverlayCache;
s16 *gOverlayCacheIDs;
u8 *gOverlayCacheRefs;

/******************************/

#define FRAMETIME_COUNT 30
#define OVERLAY_COUNT 100

u32 frameTimes[FRAMETIME_COUNT];
u8 curFrameTimeIndex = 0;
f32 gFPS;

// Call once per frame
void calculate_and_update_fps(void) {
    u32 newTime = osGetCount();
    u32 oldTime = frameTimes[curFrameTimeIndex];
    f32 divisor;
    frameTimes[curFrameTimeIndex] = newTime;

    if (__osBbIsBb) {
        divisor = IQUE_DIVISOR;
    } else {
        divisor = 1.0f;
    }

    curFrameTimeIndex++;
    if (curFrameTimeIndex >= FRAMETIME_COUNT) {
        curFrameTimeIndex = 0;
    }
    DEBUG_VAR(gDebug->fpsGraph[gDebug->iter], MIN(OS_CYCLES_TO_USEC((u32)(((f32)((newTime - oldTime) / 10)) / 2.9f)), 66666));
    gFPS = (FRAMETIME_COUNT * 1000000.0f) / (OS_CYCLES_TO_USEC(newTime - oldTime) * divisor);
}

extern u8 __osContPifRam[];
extern u8 __osContLastCmd;
void __osSiGetAccess(void);
void __osSiRelAccess(void);
void __osPiGetAccess(void);
void __osPiRelAccess(void);
u32 get_clockspeed(void);
void skGetId(u32 *arg);
u32 emux_detect(void); // defined in asm/emux.s

static void get_pj64_version() {
    u32 *pifRam32;
    s32 i;
    u8 pifCheck;
    const u8 cicTest[] = {
        0x0F, 0x0F,
        0xEC, 0x3C, 0xB6, 0x76, 0xB8, 0x1D, 0xBB, 0x8F,
        0x6B, 0x3A, 0x80, 0xEC, 0xED, 0xEA, 0x5B
    };
    // When calling this function, we know that the emulator is some version of Project 64,
    // and it isn't using the PJ64 4.0 interpreter core. Figure out which version it is.
    
    // PJ64 4.0 dynarec core doesn't update the COUNT register correctly within recompiled functions
    if (get_clockspeed() == 0) {
        gPlatform |= PJ64_4;
        return;
    }

    // Instead of implementing this PIF command correctly, PJ64 versions prior to 3.0 just have
    // a set of hardcoded values for some requests. At least one of these hardcoded values has a
    // typo in it, making it give an incorrect result.
    __osSiGetAccess();
    pifRam32 = (u32*)__osContPifRam;
    for (i = 0; i < 15; i++) pifRam32[i] = 0;
    pifRam32[15] = 2;    

    bcopy(cicTest, &__osContPifRam[46], 17);

    __osSiRawStartDma(OS_WRITE, __osContPifRam);
    osRecvMesg(si_mesg(), NULL, OS_MESG_BLOCK);
    __osContLastCmd = 254;

    __osSiRawStartDma(OS_READ, __osContPifRam);
    osRecvMesg(si_mesg(), NULL, OS_MESG_BLOCK);
    pifCheck = __osContPifRam[54];
    __osSiRelAccess();

    if (pifCheck == 0xB0) {
        gPlatform |= PJ64_1;
    } else {
        gPlatform |= PJ64_3;
    }
}

static u8 check_cache_emulation() {
    u8 cacheEmulated;
    // Disable interrupts to ensure that nothing evicts the variable from cache while we're using it.
    u32 saved = __osDisableInt();
    // Create a variable with an initial value of 1. This value will remain cached.
    volatile u8 sCachedValue = 1;
    // Overwrite the variable directly in RDRAM without going through cache.
    // This should preserve its value of 1 in dcache if dcache is emulated correctly.
    *(u8*)(K0_TO_K1(&sCachedValue)) = 0;
    // Read the variable back from dcache, if it's still 1 then cache is emulated correctly.
    // If it's zero, then dcache is not emulated correctly.
    cacheEmulated = sCachedValue;
    // Restore interrupts
    __osRestoreInt(saved);
    return cacheEmulated;
}

void get_platform(void) {
    u32 cf;
    u32 magic;
    u16 halfMagic;
    if ((u32)IO_READ(DPC_PIPEBUSY_REG) | (u32)IO_READ(DPC_TMEM_REG) | (u32)IO_READ(DPC_BUFBUSY_REG)) {
        gPlatform |= emux_detect() ? ARES : CONSOLE;

        if (gPlatform & CONSOLE) {
            if (__osBbIsBb) {
                gPlatform = IQUE | CONSOLE;
                debug_printf("iQue Player detected.\n");
                return;
            } else {
                char region[5];
                if (osTvType == OS_TV_PAL) {
                    sprintf(region, "PAL");
                } else if (osTvType == OS_TV_NTSC) {
                    sprintf(region, "NTSC");
                } else {
                    sprintf(region, "MPAL");
                }
                debug_printf("%s N64 Console detected.\n", region);
            }
        } else {
            debug_printf("Ares N64 Emulator Detected.\n");
        }
        return;
    }

    gPlatform |= EMULATOR;
    cf = get_clockspeed();
    if (cf > 1) {
        gPlatform |= CF_2;
    }

    // Perform a read from unmapped PIF ram.
    // On console and well behaved emulators, this echos back the lower half of
    // the requested memory address, repeating it if a whole word is requested.
    // So in this case, it should result in 0x01040104
    osPiRawReadIo(0x1fd00104u, &magic);
    if (magic == 0u) {
        // Older versions of mupen (and pre-2.12 ParallelN64) just always read 0
        gPlatform |= MUPEN_OLD;
        debug_printf("Mupen64 Emulator Detected.\n");
    } else {
        __osPiGetAccess();
        while (IO_READ(PI_STATUS_REG) & (PI_STATUS_DMA_BUSY|PI_STATUS_IO_BUSY));
        halfMagic = *((volatile u16*)0xbfd00106u);
        __osPiRelAccess();

        // Now do a halfword read instead.
        switch (halfMagic) {
            // This is the correct result (echo back the lower half of the requested address)
            case 0x0106: {
                // Test to see if the libpl emulator extension is present.
                osPiRawWriteIo(0x1ffb0000u, 0u);
                osPiRawReadIo(0x1ffb0000u, &magic);
                if (magic == 0x00500000u) {
                    // libpl is supported. Must be ParallelN64
                    gPlatform |= PARALLEL_LAUNCHER;
                    debug_printf("Ares N64 Emulator Detected.\n");
                    break;
                }
                
                // If the cache is emulated, it's Ares
                if (check_cache_emulation()) {
                    gPlatform |= ARES;
                    debug_printf("Ares N64 Emulator Detected.\n");
                    break;
                }

                // its the Project64 4.0 interpreter core
                gPlatform |= PJ64_4;
                debug_printf("Project64 Emulator Detected.\n");
                break;
            }
            // This looks like it should be the expected result considering what we got when we
            // requested the whole word, but that's actually wrong. Later versions of mupen
            // (and the Simple64 fork of it) get this wrong.
            case 0x0104:
                if (check_cache_emulation()) {
                    gPlatform |= SIMPLE64;
                    debug_printf("Simple64 Emulator Detected.\n");
                } else {
                    gPlatform |= MUPEN_NEXT;
                    debug_printf("Mupen64 Emulator Detected.\n");
                }
                break;
            // If reading a word gives the correct response, but reading a halfword always gives 0,
            // then we are dealing with some version of Project 64. Call into this helper function
            // to find out which version we're dealing with.
            case 0x0000:
                get_pj64_version();
                debug_printf("Project64 Emulator Detected.\n");
                break;
            // No known emulator gives any other value. If we somehow manage to get here, just return 0
            default:
                debug_printf("Cannot determine run environment.\n");
                gPlatform = 0;
                break;
        }
    }
    debug_printf("Counter Factor Setting: %d.\n", cf);
} 

#define STEP 0x100000
#define SIZE_4MB 0x400000
#define SIZE_8MB 0x800000

/**
 * Straight port from Libultra, returns the memory size.
*/
u32 osGetMemSize(void) {
    vu32 *ptr;
    u32 size = SIZE_4MB;
    u32 data0;
    u32 data1;

    if (__osBbIsBb) {
        return osMemSize;
    }

    while (size < SIZE_8MB) {
        ptr = (vu32 *) (K1BASE + size);

        data0 = *ptr;
        data1 = ptr[STEP / 4 - 1];

        *ptr ^= ~0;
        ptr[STEP / 4 - 1] ^= ~0;

        if ((*ptr != (data0 ^ ~0)) || (ptr[STEP / 4 - 1] != (data1 ^ ~0))) {
            return size;
        }

        *ptr = data0;
        ptr[STEP / 4 - 1] = data1;

        size += STEP;
    }

    return size;
}

void config_init(void) {
    gConfig.frameCap = 1;
    gConfig.terrainQuality = 1;
    //osTvType = OS_TV_NTSC;
}

s32 memsize_get(void) {
    u32 memSize = osGetMemSize();

    if (gUseExpansionMemory == FALSE) {
        memSize = 0x400000;
    }

    return memSize;
}

/**
 *  Calls osGetMemSize and sets if the expansion pak exists, and whether or not it can be used.
*/
void memsize_init(void) {
    #ifdef FORCE_4MB_MEMORY
        gExpansionPak = FALSE;
        gUseExpansionMemory = FALSE;
        debug_printf("4MB Memory Forced.\n");
        return;
    #endif
        if (osGetMemSize() > 0x400000) {
            gExpansionPak = TRUE;
            debug_printf("Expansion Pak Detected\n");
    #if EXPANSION_PAK_SUPPORT == 0
            gUseExpansionMemory = FALSE;
    #else
            gUseExpansionMemory = TRUE;
    #endif
        } else {
            debug_printf("Expansion Pak Missing\n");
            gExpansionPak = FALSE;
            gUseExpansionMemory = FALSE;
        }
}

/**
 * Where it all begins.
 * Once the boot procedure is finished in IPL3, this function is run.
 * It kicks things off by initialising thread1, which serves as the top level
 * Official name: boot
 */
void mainproc(void) {
    osInitialize();
    if (__osBbIsBb > 2) {
        __osBbIsBb = 0;
    }
    gPlatformSet = FALSE;
#ifdef AVOID_UB
    bzero(&gMainMemoryPool, RAM_END - (s32) (&main_BSS_START));
#endif
    memsize_init();
    mempool_init_main();
#ifdef DEBUG
    debug_init();
#endif
    gThread3Stack = (u64 *) mempool_alloc(STACK_GAME, PP_RAM_STACK);
    osCreateThread(&gThread1, 1, &thread1_main, 0, gThread3Stack + STACKSIZE(STACK_GAME), OS_PRIORITY_IDLE);
    gThread3Stack[STACKSIZE(STACK_GAME) - 1] = 0;
    gThread3Stack[0] = 0;
    osStartThread(&gThread1);
}

void crash_init(void);

/**
 * Initialise the crash handler thread, then initialise the main game thread.
 * Reset the start and endpoint of the game thread stack, then set thread priority to zero, effectively
 * stopping this thread, as it's no longer needed.
 */
void thread1_main(UNUSED void *unused) {
    s32 i;

    video_alloc();
    crash_init();
    config_init();
    gOverlayCache = mempool_alloc_safe(7 *OVERLAY_COUNT, PP_RAM_ASSET_CACHE);
    gOverlayCacheIDs = (s16 *) ((u8 *) gOverlayCache + (OVERLAY_COUNT * 4));
    gOverlayCacheRefs = (u8 *) ((u8 *) gOverlayCacheIDs + (OVERLAY_COUNT * 2));
    gOverlayCacheSize = 0;
    for (i = 0; i < OVERLAY_COUNT; i++) {
        gOverlayCacheIDs[i] = -1;
    }
    osCreateThread(&gThread3, 3, &thread3_main, 0, gThread3Stack + STACKSIZE(STACK_GAME), 10);
    gThread3Stack[STACKSIZE(STACK_GAME) - 1] = 0;
    gThread3Stack[0] = 0;
    osStartThread(&gThread3);
    while (1) {}
}

extern u8 *ovltable_ROM_START[];
extern u8 *ovltable_ROM_END[];
extern u8 *overlays_ROM_START[];
extern u8 *overlays_ROM_END[];
extern u8 *overlays_TEXT_START[];

// Assumes these are defined somewhere:
#define OP_MASK       0xFC000000
#define OP_LUI        0x3C000000
#define OP_ADDIU      0x24000000
#define OP_ORI        0x34000000
#define OP_LW    0x23
#define OP_SW    0x2B
#define OP_LH    0x21
#define OP_LHU   0x25
#define OP_LB    0x20
#define OP_LBU   0x24
#define OP_SH    0x29
#define OP_SB    0x28
#define OP_LWC1  0x31
#define OP_SWC1  0x39


// helper to split address with carry-aware rule (assembler behavior)
static inline void split_address(u32 addr, u16 *hi_out, u16 *lo_out) {
    u32 upper = (addr + 0x8000) >> 16; // add carry if low >= 0x8000
    u16 low = (u16)(addr & 0xFFFF);
    *hi_out = (u16)upper;
    *lo_out = low;
}

static inline u16 imm16(u32 instr) { return (u16)(instr & 0xFFFF); }
static inline u32 opcode(u32 instr) { return instr & OP_MASK; }
static inline u32 rt_field(u32 instr) { return (instr >> 16) & 0x1F; }
static inline u32 rs_field(u32 instr) { return (instr >> 21) & 0x1F; }

void overlay_reloc(s32 overlayID) {
    OverlayFile file;
    MapSymbol symbol;
    u32 searchAddr;
    u32 overlayPos[2];
    u32 overlayPos2[2];
    s32 i, j, size;
    void *overlay;

    overlay = NULL;
    for (i = 0; i < gOverlayCacheSize; i++) {
        if (gOverlayCacheIDs[i] == overlayID) {
            overlay = (void *) gOverlayCache[i];
            break;
        }
    }
    if (overlay == NULL) {
        //debug_printf("overlay_reloc: overlay %d not found in cache\n", overlayID);
        return;
    }

    // Read overlay entry and symbol table bounds
    searchAddr = (u32)ovltable_ROM_START;
    dmacopy(searchAddr + (sizeof(OverlayFile) * overlayID), (u32)&file, sizeof(OverlayFile));
    dmacopy(searchAddr + (sizeof(OverlayFile) * overlayID) + 32, (u32)&overlayPos, 8);
    dmacopy(searchAddr + (sizeof(OverlayFile) * (overlayID + 1)) + 32, (u32)&overlayPos2, 8);

    searchAddr = ((u32)ovltable_ROM_START) + overlayPos[0];
    size = overlayPos2[0] - overlayPos[0];

    u32 *text = (u32 *) overlay;
    s32 nInstr = file.textSize / 4;

    // Process all symbols
    for (i = 0; i < size; i += sizeof(MapSymbol)) {
        dmacopy(searchAddr + i, (u32)&symbol, sizeof(MapSymbol));

        u32 oldAddr = (u32)overlays_TEXT_START + symbol.address; // or just symbol.address if that's ROM addr
        u32 offset = 0;

        if (symbol.address >= file.textAddr && symbol.address < file.textAddr + file.textSize)
            offset = symbol.address - file.textAddr;
        else if (symbol.address >= file.dataAddr && symbol.address < file.dataAddr + file.dataSize)
            offset = file.textSize + (symbol.address - file.dataAddr);
        else if (symbol.address >= file.rodataAddr && symbol.address < file.rodataAddr + file.rodataSize)
            offset = file.textSize + file.dataSize + (symbol.address - file.rodataAddr);
        else if (symbol.address >= file.bssAddr && symbol.address < file.bssAddr + file.bssSize)
            offset = file.textSize + file.dataSize + file.rodataSize + (symbol.address - file.bssAddr);
        else
            continue;

        u32 newAddr = (u32) ((u8 *) overlay + offset);

        u16 oldHi_c, oldLo_c, newHi_c, newLo_c;
        split_address(oldAddr, &oldHi_c, &oldLo_c);
        split_address(newAddr, &newHi_c, &newLo_c);

        u16 oldHi_r = (u16)(oldAddr >> 16), oldLo_r = (u16)(oldAddr & 0xFFFF);
        u16 newHi_r = (u16)(newAddr >> 16), newLo_r = (u16)(newAddr & 0xFFFF);

        const int WINDOW = 16;

        //debug_printf("overlay_reloc: processing symbol %s ROM=%08X RAM=%08X\n", symbol.name, oldAddr, newAddr);

        // Scan .text for LUI / memory instructions
        for (j = 0; j < nInstr - 1; ++j) {
            u32 instr = text[j];
            if (opcode(instr) != OP_LUI) continue;

            u16 luiImm = imm16(instr);
            u32 destReg = rt_field(instr);
            if (luiImm != oldHi_c && luiImm != oldHi_r) continue;

            for (int k = 1; k <= WINDOW && (j + k) < nInstr; ++k) {
                u32 next = text[j + k];
                u32 nextOp = opcode(next);

                int patched = 0;

                if ((nextOp == OP_ADDIU || nextOp == OP_ORI) &&
                    rs_field(next) == destReg && rt_field(next) == destReg &&
                    ((u16)imm16(next) == oldLo_c || (u16)imm16(next) == oldLo_r)) {

                    u16 useHi = (luiImm == oldHi_c) ? newHi_c : newHi_r;
                    u16 useLo = ((u16)imm16(next) == oldLo_c) ? newLo_c : newLo_r;
                    text[j] = (OP_LUI | (destReg << 16) | useHi);
                    text[j + k] = (nextOp | (destReg << 21) | (destReg << 16) | useLo);
                    patched = 1;

                    //debug_printf("overlay_reloc: patched LUI+ADDIU at instr %d,%d (reg=%d) to %04X/%04X\n", j, j + k, destReg, useHi, useLo);
                }
                // Memory ops
                else if ((nextOp == OP_LW || nextOp == OP_SW || nextOp == OP_LH ||
                          nextOp == OP_LHU || nextOp == OP_LB || nextOp == OP_LBU ||
                          nextOp == OP_SH || nextOp == OP_SB || nextOp == OP_LWC1 || nextOp == OP_SWC1) &&
                         rs_field(next) == destReg &&
                         ((u16)imm16(next) == oldLo_c || (u16)imm16(next) == oldLo_r)) {

                    u16 useHi = (luiImm == oldHi_c) ? newHi_c : newHi_r;
                    u16 useLo = ((u16)imm16(next) == oldLo_c) ? newLo_c : newLo_r;
                    u32 rt = rt_field(next);
                    text[j] = (OP_LUI | (destReg << 16) | useHi);
                    text[j + k] = (nextOp | (rt << 16) | (destReg << 21) | useLo);
                    patched = 1;

                    //debug_printf("overlay_reloc: patched LUI+MEM at instr %d,%d (reg=%d) to %04X/%04X\n", j, j + k, destReg, useHi, useLo);
                }

                if (patched) break;
            }
        }

        // Scan .text for JAL instructions
        for (j = 0; j < nInstr; ++j) {
            u32 instr = text[j];
            u32 op = (instr >> 26) & 0x3F;
            if (op != 0x03) continue; // JAL

            u32 oldTarget = ((instr & 0x03FFFFFF) << 2) | (((u32) text + (j * 4)) & 0xF0000000);
            if (oldTarget == oldAddr) {
                text[j] = (instr & 0xFC000000) | ((newAddr >> 2) & 0x03FFFFFF);
                //debug_printf("overlay_reloc: patched JAL at instr %d from %08X -> %08X\n", j, oldTarget, newAddr);
            } else {
                //debug_printf("overlay_reloc: checked JAL at instr %d target=%08X, not patched\n", j, oldTarget);
            }
        }
    }
}



void *overlay_load(s32 overlayID) {
    OverlayFile file;
    s32 size;
    u32 searchAddr;
    void *overlay;
    s32 found;
    s32 i;

    for (i = 0; i < gOverlayCacheSize; i++) {
        if (gOverlayCacheIDs[i] == overlayID) {
            gOverlayCacheRefs[i]++;
            return (s32 *) gOverlayCache[i];
        }
    }

    searchAddr = (u32) ovltable_ROM_START;
    dmacopy(searchAddr + (sizeof(OverlayFile) * overlayID), (u32) &file, sizeof(OverlayFile));

    size = file.textSize + file.dataSize + file.rodataSize + file.bssSize;

    //debug_printf("Size: %X   %X %X %X %X\n", size, file.textSize, file.bssSize, file.dataSize, file.rodataSize);

    overlay = mempool_alloc(size, PP_RAM_OVERLAYS);

    if (overlay == NULL) {
        return NULL;
    }

    //debug_printf("Addr:  %X %X %X %X\n", file.textAddr, file.dataAddr, file.rodataAddr, file.bssAddr);

    file.textAddr += (u32) overlays_ROM_START;
    file.dataAddr += (u32) overlays_ROM_START;
    file.rodataAddr += (u32) overlays_ROM_START;

    //debug_printf("1st DMA: %X %X\n", (u32) overlay, file.textAddr);
    if (file.textSize) {
        dmacopy(file.textAddr, (u32) overlay, file.textSize);
    }
    //debug_printf("2nd DMA: %X %X\n", (u32) ((u8 *) overlay + file.textSize), file.dataAddr);
    if (file.dataSize) {
        dmacopy(file.dataAddr, (u32) ((u8 *) overlay + file.textSize), file.dataSize);
    }
    //debug_printf("3rd DMA: %X %X\n", (u32) ((u8 *) overlay + (file.textSize + file.dataSize)), file.rodataAddr);
    if (file.rodataSize) {
        dmacopy(file.rodataAddr, (u32) ((u8 *) overlay + (file.textSize + file.dataSize)), file.rodataSize);
    }
    //debug_printf("4th DMA: %X %X\n", (u32) ((u8 *) overlay + (file.textSize + file.dataSize + file.rodataSize)), file.bssAddr);
    if (file.bssSize) {
        bzero((void *) ((u8 *) overlay + (file.textSize + file.dataSize + file.rodataSize)), file.bssSize);
    }

    found = FALSE;
    for (i = 0; i < gOverlayCacheSize; i++) {
        if (gOverlayCacheIDs[i] == -1) {
            gOverlayCacheIDs[i] = overlayID;
            gOverlayCache[i] = (s32) overlay;
            gOverlayCacheRefs[i] = 1;
            found = TRUE;
            //debug_printf("New Overlay Pos: %d\n", i);
            break;
        }
    }

    if (found == FALSE) {
        gOverlayCacheIDs[gOverlayCacheSize] = overlayID;
        gOverlayCache[gOverlayCacheSize] = (s32) overlay;
        gOverlayCacheRefs[gOverlayCacheSize] = 1;
        //debug_printf("New Overlay Pos: %d\n", gOverlayCacheSize);
        gOverlayCacheSize++;
    }

    //debug_dump_hex(overlay, file.textSize, 16);
    overlay_reloc(overlayID);
    osWritebackDCache(overlay, size - file.bssSize);
    //osWritebackDCacheAll();
    //osInvalDCache(overlay, size - file.bssSize);
    //debug_dump_hex(overlay, file.textSize, 16);

    return overlay;
}

void *overlay_symbol(s32 overlayID, const char *symbol) {
    OverlayFile file;
    MapSymbol mapSymbol;
    u32 searchAddr;
    u32 overlayPos[2], overlayPos2[2];
    u32 i;
    u32 searchSize;
    void *overlay;

    overlay = NULL;
    for (i = 0; i < (u32) gOverlayCacheSize; i++) {
        if (gOverlayCacheIDs[i] == overlayID) {
            overlay = (void *) gOverlayCache[i];
            break;
        }
    }
    if (overlay == NULL) {
        return NULL;
    }
    //debug_printf("Sym Overlay Pos: %d\n", i);

    // Read overlay entry and symbol table bounds
    searchAddr = (u32)ovltable_ROM_START;
    dmacopy(searchAddr + sizeof(OverlayFile) * overlayID, (u32)&file, sizeof(OverlayFile));
    dmacopy(searchAddr + sizeof(OverlayFile) * overlayID + 32, (u32)&overlayPos, 8);
    dmacopy(searchAddr + sizeof(OverlayFile) * (overlayID + 1) + 32, (u32)&overlayPos2, 8);

    searchAddr = ((u32)ovltable_ROM_START) + overlayPos[0];
    searchSize = overlayPos2[0] - overlayPos[0];

    for (i = 0; i + sizeof(MapSymbol) <= searchSize; i += sizeof(MapSymbol)) {
        dmacopy(searchAddr + i, (u32)&mapSymbol, sizeof(MapSymbol));

        // Use full fixed-size compare (map symbols may not be null-terminated)
        if (strncmp(mapSymbol.name, symbol, 32) != 0) continue;

        u32 offset = 0;
        if (mapSymbol.address >= file.textAddr && mapSymbol.address < file.textAddr + file.textSize)
            offset = mapSymbol.address - file.textAddr;
        else if (mapSymbol.address >= file.dataAddr && mapSymbol.address < file.dataAddr + file.dataSize)
            offset = file.textSize + (mapSymbol.address - file.dataAddr);
        else if (mapSymbol.address >= file.rodataAddr && mapSymbol.address < file.rodataAddr + file.rodataSize)
            offset = file.textSize + file.dataSize + (mapSymbol.address - file.rodataAddr);
        else if (mapSymbol.address >= file.bssAddr && mapSymbol.address < file.bssAddr + file.bssSize)
            offset = file.textSize + file.dataSize + file.rodataSize + (mapSymbol.address - file.bssAddr);
        else {
            //debug_printf("overlay_symbol: %s address out of overlay bounds\n", symbol);
            return NULL;
        }

        if (offset >= file.textSize + file.dataSize + file.rodataSize + file.bssSize) {
            //debug_printf("overlay_symbol: %s offset past overlay RAM size\n", symbol);
            return NULL;
        }

        void *runtimeAddr = overlay + offset;
        //debug_printf("Resolved symbol %s: ROM %08X -> RAM %08X (offset %X)\n", mapSymbol.name, mapSymbol.address, (u32)runtimeAddr, offset);

        return runtimeAddr;
    }

    return NULL;
}

void overlay_free(s32 overlayID) {
    s32 i;
    
    for (i = 0; i < gOverlayCacheSize; i++) {
        if (gOverlayCacheIDs[i] == overlayID) {
            gOverlayCacheRefs[i]--;
            if (gOverlayCacheRefs[i] == 0) {
                mempool_free((void *) gOverlayCache[i]);
                gOverlayCacheIDs[i] = -1;
            }
            break;
        }
    }

}