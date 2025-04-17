#include "main.h"
#include "thread0_epc.h"
#include "game.h"
#include "libultra/src/libc/rmonPrintf.h"
#include "stacks.h"
#include "printf.h"
#include "joypad.h"
#include "PR/os_internal_reg.h"
#include "PRinternal/piint.h"
#include "usb/usb.h"

/************ .bss ************/

u64 gThread1Stack[STACKSIZE(STACK_IDLE)];
u64 gThread3Stack[STACKSIZE(STACK_GAME)];
OSThread gThread1; // OSThread for thread 1
OSThread gThread3; // OSThread for thread 3
u16 gPlatform;
u8 gPlatformSet;
u8 gExpansionPak;
u8 gUseExpansionMemory;
UserConfig gConfig;

/******************************/

#define FRAMETIME_COUNT 30

u32 frameTimes[FRAMETIME_COUNT];
u8 curFrameTimeIndex = 0;
f32 gFPS;

// Call once per frame
void calculate_and_update_fps(void) {
    u32 newTime = osGetCount();
    u32 oldTime = frameTimes[curFrameTimeIndex];
    f32 divisor;
    frameTimes[curFrameTimeIndex] = newTime;

    /*if (__osBbIsBb) {
        divisor = IQUE_DIVISOR;
    } else {*/
        divisor = 1.0f;
    //}

    curFrameTimeIndex++;
    if (curFrameTimeIndex >= FRAMETIME_COUNT) {
        curFrameTimeIndex = 0;
    }

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
            /*if (__osBbIsBb) {
                gPlatform = IQUE | CONSOLE;
                //puppyprint_log(LOG_INFO, "iQue Player detected.\n");
                return;
            } else {*/
                char region[5];
                if (osTvType == OS_TV_PAL) {
                    //puppyprintf(region, "PAL");
                } else if (osTvType == OS_TV_NTSC) {
                    //puppyprintf(region, "NTSC");
                } else {
                    //puppyprintf(region, "MPAL");
                }
                //puppyprint_log(LOG_INFO, "%s N64 Console detected.\n", region);
            //}
        } else {
            //puppyprint_log(LOG_INFO, "Ares N64 Emulator Detected.\n");
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
        //puppyprint_log(LOG_INFO, "Mupen64 Emulator Detected.\n");
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
                    //puppyprint_log(LOG_INFO, "Ares N64 Emulator Detected.\n");
                    break;
                }
                
                // If the cache is emulated, it's Ares
                if (check_cache_emulation()) {
                    gPlatform |= ARES;
                    //puppyprint_log(LOG_INFO, "Ares N64 Emulator Detected.\n");
                    break;
                }

                // its the Project64 4.0 interpreter core
                gPlatform |= PJ64_4;
                //puppyprint_log(LOG_INFO, "Project64 Emulator Detected.\n");
                break;
            }
            // This looks like it should be the expected result considering what we got when we
            // requested the whole word, but that's actually wrong. Later versions of mupen
            // (and the Simple64 fork of it) get this wrong.
            case 0x0104:
                if (check_cache_emulation()) {
                    gPlatform |= SIMPLE64;
                    //puppyprint_log(LOG_INFO, "Simple64 Emulator Detected.\n");
                } else {
                    gPlatform |= MUPEN_NEXT;
                    //puppyprint_log(LOG_INFO, "Mupen64 Emulator Detected.\n");
                }
                break;
            // If reading a word gives the correct response, but reading a halfword always gives 0,
            // then we are dealing with some version of Project 64. Call into this helper function
            // to find out which version we're dealing with.
            case 0x0000:
                get_pj64_version();
                //puppyprint_log(LOG_INFO, "Project64 Emulator Detected.\n");
                break;
            // No known emulator gives any other value. If we somehow manage to get here, just return 0
            default:
                //puppyprint_log(LOG_INFO, "Cannot determine run environment.\n");
                gPlatform = 0;
                break;
        }
    }
    //puppyprint_log(LOG_INFO, "Counter Factor Setting: %d.\n", cf);
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

    /*if (__osBbIsBb) {
        return osMemSize;
    }*/

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
    gConfig.antiAliasing = AA_OFF;
    gConfig.terrainQuality = 0;
    gConfig.dedither = FALSE;
    gConfig.frameCap = 1;
    gConfig.multiAA = AA_OFF;
}

/**
 *  Calls osGetMemSize and sets if the expansion pak exists, and whether or not it can be used.
*/
void memsize_init(void) {
    #ifdef FORCE_4MB_MEMORY
        gExpansionPak = FALSE;
        gUseExpansionMemory = FALSE;
        //puppyprint_log(LOG_INFO, "4MB Memory Forced.\n");
        return;
    #endif
        if (osGetMemSize() > 0x400000) {
            gExpansionPak = TRUE;
            //puppyprint_log(LOG_INFO, "Expansion Pak Detected\n");
    #if EXPANSION_PAK_SUPPORT == 0
            gUseExpansionMemory = FALSE;
    #else
            gUseExpansionMemory = TRUE;
    #endif
        } else {
            //puppyprint_log(LOG_INFO, "Expansion Pak Missing\n");
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
    osTvType = OS_TV_NTSC;
    gPlatformSet = FALSE;
#ifdef AVOID_UB
    bzero(&gMainMemoryPool, RAM_END - (s32) (&gMainMemoryPool));
#endif
    memsize_init();
    debug_init();
    osCreateThread(&gThread1, 1, &thread1_main, 0, &gThread1Stack[STACKSIZE(STACK_IDLE)], OS_PRIORITY_IDLE);
    gThread1Stack[STACKSIZE(STACK_IDLE) - 1] = 0;
    gThread1Stack[0] = 0;
    debug_ram(-STACK_IDLE, PP_RAM_CODE);
    debug_ram(STACK_IDLE, PP_RAM_STACK);
    osStartThread(&gThread1);
}

void crash_init(void);

/**
 * Initialise the crash handler thread, then initialise the main game thread.
 * Reset the start and endpoint of the game thread stack, then set thread priority to zero, effectively
 * stopping this thread, as it's no longer needed.
 */
void thread1_main(UNUSED void *unused) {
    crash_init();
    config_init();
    osCreateThread(&gThread3, 3, &thread3_main, 0, &gThread3Stack[STACKSIZE(STACK_GAME)], 10);
    gThread3Stack[STACKSIZE(STACK_GAME) - 1] = 0;
    gThread3Stack[0] = 0;
    debug_ram(-STACK_GAME, PP_RAM_CODE);
    debug_ram(STACK_GAME, PP_RAM_STACK);
    osStartThread(&gThread3);
    osSetThreadPri(NULL, OS_PRIORITY_IDLE);
    while (1) {}
}

/**
 * Increments the start and endpoint of the stack.
 * They should have an equal value, so if they don't, that triggers a printout saying a stack wraparound has occured.
 * Official Name: bootCheckStack
 */
void thread3_verify_stack(void) {
}

#ifdef DEBUG

ALIGNED8 DebugData gDebug;

void debug_init(void) {
    bzero(&gDebug, sizeof(DebugData));
    gDebug.enabled = FALSE;
    gDebug.iter = 0;
}

typedef char *outfun(char *dst, const char *src, size_t count);

void debug_log(s32 logLevel, char *str, ...) {
}

void debug_fillrect(Gfx **gfx, s32 x1, s32 y1, s32 x2, s32 y2, u32 colour) {
    s32 alpha = (colour) & 0xFF;
    if (alpha != 255) {
        gDPSetPrimColor((*gfx)++, 0, 0, (colour >> 24) & 0xFF, (colour >> 16) & 0xFF, (colour >> 8) & 0xFF, alpha);
    }
    gDPPipeSync((*gfx)++);
    gDPSetCombineMode((*gfx)++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    if (x1 < 0) {
        x1 = 0;
    }
    if (y1 < 0) {
        y1 = 0;
    }
    if (x2 > SCREEN_WIDTH) {
        x2 = SCREEN_WIDTH;
    }
    if (y2 > SCREEN_HEIGHT) {
        y2 = SCREEN_HEIGHT;
    }
    if (alpha == 255) {
        gDPSetRenderMode((*gfx)++, G_RM_NOOP, G_RM_NOOP2);
        gDPSetCycleType((*gfx)++, G_CYC_FILL);
        gDPSetFillColor((*gfx)++, GPACK_RGBA5551((colour >> 24) & 0xFF, (colour >> 16) & 0xFF, (colour >> 8) & 0xFF, 1) | 
                       (GPACK_RGBA5551((colour >> 24) & 0xFF, (colour >> 16) & 0xFF, (colour >> 8) & 0xFF, 1) << 16));
    } else {
        gDPSetRenderMode((*gfx)++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetCycleType((*gfx)++, G_CYC_1CYCLE);
    }
    gDPFillRectangle((*gfx)++, x1, y1, x2, y2);
}

#define RDP_TO_USEC(x) ((x * 10) / 625)

void debug_timer_update(DebugData *d, s32 field, u32 time) {
    s32 i;
    s32 it;
    if (time > OS_USEC_TO_CYCLES(99999)) {
        time = OS_USEC_TO_CYCLES(99999);
    }
    //for (i = 0; i < ABS(d->iter - d->prevIter); i++) {
    //    it = d->prevIter + i;
        it = d->iter;
        if (it >= NUM_PERF_ITERATIONS) {
            it -= NUM_PERF_ITERATIONS;
        }
        d->timers[field][it] += time;
        d->timers[field][PERF_AGGREGATE] += d->timers[field][it];
        d->timers[field][PERF_TOTAL] = d->timers[field][PERF_AGGREGATE] / NUM_PERF_ITERATIONS;
    //}
}

void debug_rdp(void) {
    DebugData *d = &gDebug;
    debug_timer_update(d, PP_RDP_CLK, RDP_TO_USEC(IO_READ(DPC_CLOCK_REG)));
    debug_timer_update(d, PP_RDP_BUF, RDP_TO_USEC(IO_READ(DPC_BUFBUSY_REG)));
    debug_timer_update(d, PP_RDP_BUS, RDP_TO_USEC(IO_READ(DPC_PIPEBUSY_REG)));
    debug_timer_update(d, PP_RDP_TMM, RDP_TO_USEC(IO_READ(DPC_TMEM_REG)));
    
    d->rdpTotal = d->timers[PP_RDP_CLK][PERF_TOTAL];
}

void debug_rsp(s32 context) {
    DebugData *d = &gDebug;
    u32 time = osGetCount();

    switch (context) {
        case RSP_GFX_START:
            d->rspTimers[context][d->rspGfxIter] = time;
            break;
        case RSP_GFX_END:
            d->rspTimers[context][d->rspGfxIter++] = time;
            if (d->rspGfxIter > 3) {
                d->rspGfxIter = 3;
            }
            break;
        case RSP_AUD_START:
            d->rspTimers[context][d->rspAudIter] = time;
            break;
        case RSP_AUD_END:
            d->rspTimers[context][d->rspAudIter++] = time;
            if (d->rspAudIter > 3) {
                d->rspAudIter = 3;
            }
            break;
    }
}

s32 debug_tag_index(s32 colourTag) {
    switch (colourTag) {
        case COLOUR_TAG_RED:
            return PP_RAM_RED;
            break;
        case COLOUR_TAG_GREEN:
            return PP_RAM_GREEN;
            break;
        case COLOUR_TAG_BLUE:
            return PP_RAM_BLUE;
            break;
        case COLOUR_TAG_YELLOW:
            return PP_RAM_YELLOW;
            break;
        case COLOUR_TAG_MAGENTA:
            return PP_RAM_MAGENTA;
            break;
        case COLOUR_TAG_CYAN:
            return PP_RAM_CYAN;
            break;
        case COLOUR_TAG_WHITE:
            return PP_RAM_WHITE;
            break;
        case COLOUR_TAG_GREY:
            return PP_RAM_GREY;
            break;
        case COLOUR_TAG_SEMITRANS_GREY:
            return PP_RAM_GREY_XLU;
            break;
        case COLOUR_TAG_ORANGE:
            return PP_RAM_ORANGE;
            break;
        case COLOUR_TAG_BLACK:
            return PP_RAM_BLACK;
            break;
        case COLOUR_TAG_LIGHT_ORANGE:
            return PP_RAM_LIGHT_ORANGE;
            break;
        default:
            return colourTag % PP_RAM_TOTAL;
    }
    return PP_RAM_WHITE;
}

void debug_ram(s32 size, s32 tag) {
    if (tag != PP_RAM_SUBPOOLS) {
        gDebug.ramTotal += size;
    }
    gDebug.ramSegments[debug_tag_index(tag)] += size;
}

const char *sMinimalText[] = {
    "CPU",
    "RSP",
    "RDP"
};

void debug_page_minimal(DebugData *d, Gfx **dList, s32 updateRate) {
    char textBytes[32];
    s32 i;
    s32 y;
    u32 *time;
    f32 ram;
    char *ramStr[] = {"KB", "MB"};
    s32 lol;
    u32 ramCount;

    if (d->pageViewMode == 0) {
        debug_fillrect(dList, 8, 8, 112, 60 + 2, 0x0000007F);
        set_text_font(ASSET_FONTS_SMALLFONT);
        set_text_colour(255, 255, 255, 255, 255);
        set_text_background_colour(0, 0, 0, 0);
        set_kerning(FALSE);
        sprintf(textBytes, "FPS: %2.2f", (f64) gFPS);
        draw_text(dList, 10, 10, textBytes, ALIGN_TOP_LEFT);
        sprintf(textBytes, "(%2.1fms)", (f64) (1000.0f / gFPS));
        draw_text(dList, 112 - 4, 10, textBytes, ALIGN_TOP_RIGHT);
        y = 20;

        time = &d->cpuTotal;
        for (i = 0; i < 3; i++) {
            sprintf(textBytes, "%s: %d", sMinimalText[i], (u32) *time);
            draw_text(dList, 10, y, textBytes, ALIGN_TOP_LEFT);
            sprintf(textBytes, "(%d%%)", *time / 333);
            draw_text(dList, 112 - 4, y, textBytes, ALIGN_TOP_RIGHT);
            time++;
            y += 10;
        }
        lol = 0;
        if (gUseExpansionMemory) {
            ramCount = 0x800000;
        } else {
            ramCount = 0x400000;
        }
        ram = ((u32) ramCount - gDebug.ramTotal) / 1024.0f;
        if (ram < 0.0f) {
            ram = 0.0f;
        }
        if (ram > 1000.0f) {
            ram /= 1024.0f;
            lol = 1;
        }
        sprintf(textBytes, "RAM: %2.3f%s", (f32) ram, ramStr[lol]);
        draw_text(dList, 10, y, textBytes, ALIGN_TOP_LEFT);
        
    }
}

void debug_render(Gfx **dList, s32 updateRate) {
    DebugData *d = &gDebug;

    if (d->enabled == FALSE) {
        return;
    }

    switch (d->pageCurrent) {
        case PAGE_MINIMAL:
            debug_page_minimal(d, dList, updateRate);
            break;
    }
}

void debug_thread(s32 field, s32 offset) {
    DebugData *d = &gDebug;
    s32 count = field >> 1;
    d->threadTimers[field][d->threadIter[count]] = osGetCount() - offset;
    if (field % 2) {
        d->threadIter[count]++;
        if (d->threadIter[count] > NUM_THREAD_ITERATIONS) {
            d->threadIter[count] = NUM_THREAD_ITERATIONS;
        }
    }
}

s32 debug_thread_compare(DebugData *d, s32 threadID, u32 lowTime, u32 highTime) {
    s32 i;
    s32 j;
    u32 ret;
    s32 count;
    s32 t;
    
    if (threadID <= 0) {
        return 0;
    }

    count = threadID >> 1;


    ret = 0;
    for (i = 0; i < count; i++) {
        t = i << 1;
        for (j = 0; j < d->threadIter[i]; j++) {
            if (lowTime < d->threadTimers[t][j] && highTime > d->threadTimers[t + 1][j]) {
                ret += d->threadTimers[t + 1][j] - d->threadTimers[t][j];
            }
        }
    }

    return ret;
}

void debug_newframe(s32 updateRate) {
    s32 i;
    s32 j;
    s32 it;
    DebugData *d = &gDebug;

    //for (j = 0; j < ABS(d->iter - d->prevIter); j++) {
        //it = d->prevIter + j;
        it = d->iter;
        if (it >= NUM_PERF_ITERATIONS) {
            it -= NUM_PERF_ITERATIONS;
        }
        for (i = 0; i < PP_TOTAL; i++) {
            d->timers[i][PERF_AGGREGATE] -= d->timers[i][it];
            d->timers[i][it] = 0;
        }
    //}
}

char *sMemDumpStrings[] = {
    "",
    "Allocated",
    "Fixed\t",
    "FixedAlloc",
    "4\t",
    "4 Allocated",
    "4 Fixed\t",
    "4 FixedAlloc"
};

char *sPuppyprintMemColours[] = {
    MEMSTRINGS
};

void debug_ram_dump(void) {
    int flags;
    int nextIndex;
    int i;
    s32 colourTag;
    MemoryPoolSlot *slot;
    u32 ramTotal = osGetMemSize();

    for (i = 0; i <= gNumberOfMemoryPools; i++) {
        debug_printf("------------- Pool: %d\t Size: %X (%2.3fKiB)\t %2.2f%%\t Slots: %d/%d -------------\n", i, 
        gMemoryPools[i].size, (double) gMemoryPools[i].size / 1024.0, 
        (double) ((f32) gMemoryPools[i].size / (f32) ramTotal) * 100.0, gMemoryPools[i].curNumSlots, gMemoryPools[i].maxNumSlots);
        slot = &gMemoryPools[i].slots[0];
        
        do {
            flags = slot->flags;
            nextIndex = slot->nextIndex;

            colourTag = debug_tag_index(slot->colourTag);

            if (flags == SLOT_FREE) {
                debug_printf("Pool: %d Idx: %d   \t Free Slot\t\t\t\t Size: 0x%X\t (%2.3fKiB) \t %2.2f%%\t Addr: %X\n", i, slot->index, slot->size, (double) slot->size / 1024.0, 
                (double) ((f32) slot->size / (f32) ramTotal) * 100.0, slot->data);
            } else {
                debug_printf("Pool: %d Idx: %d   \t %s\t Tag: %s \t\t Size: 0x%X\t (%2.3fKiB) \t %2.2f%% \t Addr: %X\n", i, slot->index, sMemDumpStrings[flags], 
                sPuppyprintMemColours[colourTag], slot->size, (double) slot->size / 1024.0, (double) ((f32) slot->size / (f32) ramTotal) * 100.0, slot->data);
            }

            skip:
            if (nextIndex == -1) {
                continue;
            } else {
                slot = &gMemoryPools[i].slots[slot->nextIndex];
            }
        } while (nextIndex != -1);
    }
}

void debug_update(s32 updateRate) {
    s32 i;
    s32 j;
    u32 highTime;
    DebugData *d = &gDebug;
    s32 inputPressed;
    s32 inputHeld;
    s32 count;
    s32 offset;

    inputPressed = 0;
    inputHeld = 0;
    for (i = 0; i < 4; i++) {
        inputPressed |= input_pressed(i);
        inputHeld |= input_held(i);
    }

    if (inputPressed & L_TRIG) {
        d->enabled ^= 1;
    }

    if (inputPressed & R_JPAD) {
        debug_ram_dump();
    }

    d->cpuTotal = 0;
    for (i = 0; i < THREAD_CONTEXT_COUNT; i += 2) {
        highTime = 0;
        count = i >> 1;
        for (j = 0; j < d->threadIter[count]; j++) {
            highTime += OS_CYCLES_TO_USEC(d->threadTimers[i + 1][j] - d->threadTimers[i][j]);
            offset = OS_CYCLES_TO_USEC(debug_thread_compare(d, i, d->threadTimers[i][j], d->threadTimers[i + 1][j]));
            highTime -= offset;
            if (i == THREAD3_START) {
                debug_timer_update(d, PP_YIELD3, offset);
            }
        }
        debug_timer_update(d, PP_THREAD5 + count, highTime);
        d->timers[PP_THREAD5 + count][PERF_TOTAL] = d->timers[PP_THREAD5 + count][PERF_AGGREGATE] / NUM_PERF_ITERATIONS;
        d->cpuTotal += d->timers[PP_THREAD5 + count][PERF_TOTAL];
    }
    
    bzero(&d->threadIter, THREAD_CONTEXT_COUNT);
    d->timers[PP_YIELD3][PERF_TOTAL] = d->timers[PP_YIELD3][PERF_AGGREGATE] / NUM_PERF_ITERATIONS;

    for (i = 0; i < d->rspGfxIter; i++) {
        debug_timer_update(d, PP_RSP_GFX, OS_CYCLES_TO_USEC(d->rspTimers[RSP_GFX_END][i] - d->rspTimers[RSP_GFX_START][i]));
    }
    for (i = 0; i < d->rspAudIter; i++) {
        debug_timer_update(d, PP_RSP_AUD, OS_CYCLES_TO_USEC(d->rspTimers[RSP_AUD_END][i] - d->rspTimers[RSP_AUD_START][i]));
    }
    d->rspGfxIter = 0;
    d->rspAudIter = 0;
    d->timers[PP_RSP_GFX][PERF_TOTAL] = d->timers[PP_RSP_GFX][PERF_AGGREGATE] / NUM_PERF_ITERATIONS;
    // Temp measure until I figure out why aud profiles are messing up
    d->timers[PP_RSP_AUD][PERF_AGGREGATE] = 0;
    for (i = 0; i < NUM_PERF_ITERATIONS; i++) {
        d->timers[PP_RSP_AUD][PERF_AGGREGATE] += d->timers[PP_RSP_AUD][i];
    }
    d->timers[PP_RSP_AUD][PERF_TOTAL] = d->timers[PP_RSP_AUD][PERF_AGGREGATE] / NUM_PERF_ITERATIONS;
    d->rspTotal = d->timers[PP_RSP_GFX][PERF_TOTAL];// + d->timers[PP_RSP_AUD][PERF_TOTAL];

    d->prevIter = d->iter;
    //d->iter += updateRate;
    d->iter++;

    if (d->iter >= NUM_PERF_ITERATIONS) {
        d->iter -= NUM_PERF_ITERATIONS;
    }
    debug_newframe(updateRate);
}

#include "usb/dkr_usb.c"
#include "usb/usb.c"
#include "usb/reset.c"

#endif