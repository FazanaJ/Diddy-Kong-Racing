#include "overlay.h"

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

/**
    Basic relocating overlay system

    This works a little unlike you'd expect a good system to, because of reasons related to me
    not wanting to deal with the build system.
    At compile time, everything in the overlays folder will be compiled into one binary blob,
    then linked into the end of the main code segment.
    This generates a valid map, which is then parsed externally, and a lookup table is generated
    that contains sizes and offets for the overlays, as well as symbols.
    This is a hilariously poor approach, but it's among the easiest to implement.
    When the game loads an overlay, it will load each individual part of the overlay, because
    it will be split into each section (text, data, rodata, bss).
    Lastly, each address will be relocated using the table to correct the offsets into memory.

    Accessing symbols is to just call overlay_symbol, and pass the symbol name.

    Current limtations:
    - Because this parses a map, static variables and rodata are quite spotty.
    - Because all the sections are separated from each other in the compiled binary, this
      system cannot support compression.
    - For reasons I do not yet know, GCC does not work. Likely related to patching.

    Data layout:
    1: Each overlay file has an OverlayFile entry. The first 32 bytes match where the overlay
       is in ROM, so it can be DMA'd in. The last 4 bytes, symbolAddr, is for knowing where
       the symbol list in 2. start for that overlay.
    2. Each overlay file in the same order has all its symbols in order with address offsets.
*/

typedef struct OverlayFile {
    u32 textAddr;
    u32 textSize;
    u32 dataAddr;
    u32 dataSize;
    u32 rodataAddr;
    u32 rodataSize;
    u32 bssAddr;
    u32 bssSize;
    u32 symbolAddr;
} OverlayFile;

#define OVERLAY_CACHE_COUNT 100
#define OVERLAY_DEPENDANCIES 4

s32 gOverlayCacheSize;
s32 *gOverlayCache;
s16 *gOverlayCacheIDs;
u8 *gOverlayCacheRefs;

extern u8 *ovltable_ROM_START[];
extern u8 *ovltable_ROM_END[];
extern u8 *overlays_ROM_START[];
extern u8 *overlays_ROM_END[];
extern u8 *overlays_TEXT_START[];
extern u8 *ovlNames_ROM_START[];
extern u8 *ovlNames_ROM_END[];

// Assumes these are defined somewhere:
#define OP_MASK             0xFC000000
#define OP_LUI              0x3C000000
#define OP_ADDIU            0x24000000
#define OP_ORI              0x34000000
#define OPCODE_MASK         0xFC000000u
#define OPCODE_SHIFT        26
#define JUMP_TARGET_MASK    0x03FFFFFFu
#define JUMP_PC_MASK        0xF0000000u
#define OP_JAL              0x03
#define OP_LB               0x20
#define OP_LH               0x21
#define OP_LW               0x23
#define OP_LBU              0x24
#define OP_LHU              0x25
#define OP_SB               0x28
#define OP_SH               0x29
#define OP_SW               0x2B
#define OP_LWC1             0x31
#define OP_SWC1             0x39

void overlay_init(void) {
    s32 i;
    gOverlayCache = mempool_alloc_safe(7 * OVERLAY_CACHE_COUNT, PP_RAM_ASSET_CACHE);
    gOverlayCacheIDs = (s16 *) ((u8 *) gOverlayCache + (OVERLAY_CACHE_COUNT * 4));
    gOverlayCacheRefs = (u8 *) ((u8 *) gOverlayCacheIDs + (OVERLAY_CACHE_COUNT * 2));
    gOverlayCacheSize = 0;
    for (i = 0; i < OVERLAY_CACHE_COUNT; i++) {
        gOverlayCacheIDs[i] = -1;
    }
}

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

void overlay_reloc(s32 overlayID, void *overlay) {
    OverlayFile file;
    MapSymbol symbol;
    u32 searchAddr;
    u32 overlayPos[2];
    u32 overlayPos2[2];
    s32 i, j, size;

    // Read overlay entry and symbol table bounds
    searchAddr = (u32)ovltable_ROM_START;
    dmacopy(searchAddr + (sizeof(OverlayFile) * overlayID), (u32) &file, sizeof(OverlayFile));
    dmacopy(searchAddr + (sizeof(OverlayFile) * overlayID) + 32, (u32) &overlayPos, 8);
    dmacopy(searchAddr + (sizeof(OverlayFile) * (overlayID + 1)) + 32, (u32) &overlayPos2, 8);

    searchAddr = ((u32)ovltable_ROM_START) + overlayPos[0];
    size = overlayPos2[0] - overlayPos[0];

    u32 *text = (u32 *) overlay;
    s32 nInstr = file.textSize / 4;

    // Process all symbols
    for (i = 0; i < size; i += sizeof(MapSymbol)) {
        dmacopy(searchAddr + i, (u32) &symbol, sizeof(MapSymbol));

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

        u16 oldHi_r = (u16) (oldAddr >> 16), oldLo_r = (u16) (oldAddr & 0xFFFF);
        u16 newHi_r = (u16) (newAddr >> 16), newLo_r = (u16) (newAddr & 0xFFFF);

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
                    rs_field(next) == destReg &&
                    ((u16) imm16(next) == oldLo_c || (u16) imm16(next) == oldLo_r)) {

                    u16 useHi = (luiImm == oldHi_c) ? newHi_c : newHi_r;
                    u16 useLo = ((u16) imm16(next) == oldLo_c) ? newLo_c : newLo_r;
                    u32 rt = rt_field(next); // destination register

                    text[j] = (OP_LUI | (destReg << 16) | useHi);
                    text[j + k] = (nextOp | (rt << 16) | (destReg << 21) | useLo);
                    patched = 1;

                    //debug_printf("overlay_reloc: patched LUI+ADDIU at instr %d,%d (reg=%d) to %04X/%04X\n", j, j + k, destReg, useHi, useLo);
                }
                // Memory ops
                else if ((nextOp == OP_LW || nextOp == OP_SW || nextOp == OP_LH ||
                          nextOp == OP_LHU || nextOp == OP_LB || nextOp == OP_LBU ||
                          nextOp == OP_SH || nextOp == OP_SB || nextOp == OP_LWC1 || nextOp == OP_SWC1) &&
                         rs_field(next) == destReg &&
                         ((u16) imm16(next) == oldLo_c || (u16) imm16(next) == oldLo_r)) {

                    u16 useHi = (luiImm == oldHi_c) ? newHi_c : newHi_r;
                    u16 useLo = ((u16) imm16(next) == oldLo_c) ? newLo_c : newLo_r;
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
            u32 op = (instr >> OPCODE_SHIFT) & 0x3F;
            if (op != OP_JAL) continue;

            u32 oldTarget = ((instr & JUMP_TARGET_MASK) << 2) | (((u32) text + (j * 4)) & JUMP_PC_MASK);
            if (oldTarget == oldAddr) {
                text[j] = (instr & OPCODE_MASK) | ((newAddr >> 2) & JUMP_TARGET_MASK);
                //debug_printf("overlay_reloc: patched JAL at instr %d from %08X -> %08X\n", j, oldTarget, newAddr);
            } else {
                //debug_printf("overlay_reloc: checked JAL at instr %d target=%08X, not patched\n", j, oldTarget);
            }
        }
    }
}

/**
 * Loads an overlay from ROM into RAM.
 * Automatically ready to use.
 */
void *overlay_load(s32 overlayID) {
    OverlayFile file;
    s32 size;
    u32 searchAddr;
    void *overlay;
    char overlayName[32];
    s16 *deps;
    u32 startOffset;
    s32 found;
    s32 i;
    u32 first = osGetCount();

    for (i = 0; i < gOverlayCacheSize; i++) {
        if (gOverlayCacheIDs[i] == overlayID) {
            gOverlayCacheRefs[i]++;
            return (s32 *) gOverlayCache[i];
        }
    }

    searchAddr = (u32) ovltable_ROM_START;
    dmacopy(searchAddr + (sizeof(OverlayFile) * overlayID), (u32) &file, sizeof(OverlayFile));

    startOffset = OVERLAY_DEPENDANCIES * sizeof(s16);
    size = file.textSize + file.dataSize + file.rodataSize + file.bssSize;

    //debug_printf("Size: %X   %X %X %X %X\n", size, file.textSize, file.bssSize, file.dataSize, file.rodataSize);

    overlay = mempool_alloc(size + startOffset, PP_RAM_OVERLAYS);

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

    for (i = 0; i < OVERLAY_DEPENDANCIES; i++) {
        deps = ((s16 *) ((u8 *) overlay + size)) + i;
        *deps = -1;
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
    
    overlay_reloc(overlayID, overlay);
    osWritebackDCache(overlay, file.textSize);
    osInvalICache(overlay, file.textSize);

    if (gDebug) {
        dmacopy((u32) ovlNames_ROM_START + (overlayID * 32), (u32) &overlayName, 32);
        debug_printf("Loaded overlay [%s] (%2.4fs)\n", overlayName, (f32) (osGetCount() - first)  / 46875000.0f);
    }
    return overlay;
}

/**
 * Loads a new overlay and marks the given overlay to have the newly loaded one as a dependancy.
 * This gives the new overlay a reference, and keeps it loaded if it may otherwise be unloaded.
 * If this overlay is freed, it will automatically clear the reference, unloading it if necessary.
 */
void *overlay_load_dep(s32 curOvlID, s32 newOvlID) {
    void *curOvl;
    void *newOvl;
    OverlayFile file;
    s32 size;
    s32 i;
    s16 *deps;

    curOvl = NULL;
    newOvl = overlay_load(newOvlID);

    if (newOvl == NULL) {
        return NULL;
    }

    for (i = 0; i < gOverlayCacheSize; i++) {
        if (gOverlayCacheIDs[i] == curOvlID) {
            curOvl = (void *) gOverlayCache[i];
        }
    }

    if (curOvl == NULL) {
        overlay_free(newOvlID);
        return NULL;
    }

    dmacopy((u32) ovltable_ROM_START + (sizeof(OverlayFile) * curOvlID), (u32) &file, sizeof(OverlayFile));
    size = file.textSize + file.dataSize + file.rodataSize + file.bssSize;
    for (i = 0; i < OVERLAY_DEPENDANCIES; i++) {
        deps = ((s16 *) ((u8 *) curOvl + size)) + i;
        if (*deps == -1) {
            *deps = newOvlID;
            break;
        }
    }

    return newOvl;
}

/**
 * Returns the address of the given symbol name in memory.
 * Returns NULL if nothing's found or if the overlay does not exist.
*/
void *overlay_symbol(s32 overlayID, const char *symbol) {
    OverlayFile file;
    MapSymbol mapSymbol;
    u32 searchAddr;
    u32 overlayPos[2], overlayPos2[2];
    u32 i;
    s32 len;
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

    len = strlen(symbol);

    if (len > 31) {
        len = 31;
    }


    // Read overlay entry and symbol table bounds
    searchAddr = (u32) ovltable_ROM_START;
    dmacopy(searchAddr + sizeof(OverlayFile) * overlayID, (u32) &file, sizeof(OverlayFile));
    dmacopy(searchAddr + sizeof(OverlayFile) * overlayID + 32, (u32) &overlayPos, 8);
    dmacopy(searchAddr + sizeof(OverlayFile) * (overlayID + 1) + 32, (u32) &overlayPos2, 8);

    searchAddr = ((u32) ovltable_ROM_START) + overlayPos[0];
    searchSize = overlayPos2[0] - overlayPos[0];

    for (i = 0; i + sizeof(MapSymbol) <= searchSize; i += sizeof(MapSymbol)) {
        dmacopy(searchAddr + i, (u32) &mapSymbol, sizeof(MapSymbol));

        //debug_printf("Checking %s against %s\n", symbol, mapSymbol.name);

        // Use full fixed-size compare (map symbols may not be null-terminated)
        if (strncmp(mapSymbol.name, symbol, len) != 0) {
            continue;
        }

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

/**
 * Attempts to free the overlay from memory if no other instances
 * are using it.
 */
void overlay_free(s32 overlayID) {
    s32 i;
    s32 j;
    s32 size;
    s16 *deps;
    char overlayName[32];
    OverlayFile file;
    
    for (i = 0; i < gOverlayCacheSize; i++) {
        if (gOverlayCacheIDs[i] == overlayID) {
            gOverlayCacheRefs[i]--;
            if (gOverlayCacheRefs[i] == 0) {
                
                if (gDebug) {
                    dmacopy((u32) ovlNames_ROM_START + (overlayID * 32), (u32) &overlayName, 32);
                    debug_printf("Freeing overlay [%s]\n", overlayName);
                }
                dmacopy((u32) ovltable_ROM_START + (sizeof(OverlayFile) * overlayID), (u32) &file, sizeof(OverlayFile));
                size = file.textSize + file.dataSize + file.rodataSize + file.bssSize;
                for (j = 0; j < OVERLAY_DEPENDANCIES; j++) {
                    deps = ((s16 *) ((u8 *) gOverlayCache[i] + size)) + j;
                    //debug_printf("Dep %d : %d\n", j, *deps);
                    if (*deps >= 0) {
                        overlay_free(*deps);
                    }
                }
                mempool_free((void *) gOverlayCache[i]);
                gOverlayCacheIDs[i] = -1;
            }
            break;
        }
    }
}

/**
 * Loads and overlay to call one function before closing it.
 * Relies on no args.
 */
s32 overlay_run(s32 overlayID, const char *funcName) {
    s32 (*func)();
    s32 ret;

    overlay_load(overlayID);
    func = overlay_symbol(overlayID, funcName);
    ret = (*func)();
    overlay_free(overlayID);
    return ret;
}