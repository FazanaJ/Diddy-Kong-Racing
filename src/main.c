/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x80065D40 */

#include "main.h"
#include "thread0_epc.h"
#include "game.h"
#include "lib/src/libc/rmonPrintf.h"
#include "lib/src/libc/xprintf.h"
#include "string.h"
#include "stdarg.h"
#include "controller.h"
#include "printf.h"
#include "particles.h"
#include "objects.h"
#include "math_util.h"
#include "usb/dkr_usb.h"
#include "audiosfx.h"

/************ .bss ************/

u64 gThread3Stack[THREAD3_STACK / sizeof(u64)];
OSThread gThread1; // OSThread for thread 1
OSThread gThread3; // OSThread for thread 3
u8 gPlatformSet = FALSE;
u16 gPlatform = 0;

/******************************/


extern u8 __osContPifRam[];
extern u8 __osContLastCmd;
void __osSiGetAccess(void);
void __osSiRelAccess(void);
void __osPiGetAccess(void);
void __osPiRelAccess(void);
u32 get_clockspeed(void);
void skGetId(u32 *arg);
u32 emux_detect(void); // defined in asm/emux.s

INLINE void get_pj64_version() {
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
    u32 *pifRam32 = (u32*)__osContPifRam;
    for (s32 i = 0; i < 15; i++) pifRam32[i] = 0;
    pifRam32[15] = 2;

    const u8 cicTest[] = {
        0x0F, 0x0F,
        0xEC, 0x3C, 0xB6, 0x76, 0xB8, 0x1D, 0xBB, 0x8F,
        0x6B, 0x3A, 0x80, 0xEC, 0xED, 0xEA, 0x5B
    };

    memcpy(&__osContPifRam[46], cicTest, 17);

    __osSiRawStartDma(OS_WRITE, __osContPifRam);
    osRecvMesg(&sSIMesgQueue, NULL, OS_MESG_BLOCK);
    __osContLastCmd = 254;

    __osSiRawStartDma(OS_READ, __osContPifRam);
    osRecvMesg(&sSIMesgQueue, NULL, OS_MESG_BLOCK);
    const u8 pifCheck = __osContPifRam[54];
    __osSiRelAccess();

    if (pifCheck == 0xB0) {
        gPlatform |= PJ64_1;
    } else {
        gPlatform |= PJ64_3;
    }
}

static u8 check_cache_emulation() {
    // Disable interrupts to ensure that nothing evicts the variable from cache while we're using it.
    u32 saved = __osDisableInt();
    // Create a variable with an initial value of 1. This value will remain cached.
    volatile u8 sCachedValue = 1;
    // Overwrite the variable directly in RDRAM without going through cache.
    // This should preserve its value of 1 in dcache if dcache is emulated correctly.
    *(u8*)(K0_TO_K1(&sCachedValue)) = 0;
    // Read the variable back from dcache, if it's still 1 then cache is emulated correctly.
    // If it's zero, then dcache is not emulated correctly.
    const u8 cacheEmulated = sCachedValue;
    // Restore interrupts
    __osRestoreInt(saved);
    return cacheEmulated;
}

void get_platform(void) {
    if ((u32)IO_READ(DPC_PIPEBUSY_REG) | (u32)IO_READ(DPC_TMEM_REG) | (u32)IO_READ(DPC_BUFBUSY_REG)) {
        gPlatform |= emux_detect() ? ARES : CONSOLE;

        if (gPlatform & CONSOLE) {
            if (__osBbIsBb) {
                gPlatform = IQUE | CONSOLE;
                puppyprint_log(LOG_INFO, "iQue Player detected.\n");
                return;
            } else {
                char region[5];
                if (osTvType == TV_TYPE_PAL) {
                    puppyprintf(region, "PAL");
                } else if (osTvType == TV_TYPE_NTSC) {
                    puppyprintf(region, "NTSC");
                } else {
                    puppyprintf(region, "MPAL");
                }
                puppyprint_log(LOG_INFO, "%s N64 Console detected.\n", region);
            }
        } else {
            puppyprint_log(LOG_INFO, "Ares N64 Emulator Detected.\n");
        }
        return;
    }

    gPlatform |= EMULATOR;
    u32 cf = get_clockspeed();
    if (cf > 1) {
        gPlatform |= CF_2;
    }

    // Perform a read from unmapped PIF ram.
    // On console and well behaved emulators, this echos back the lower half of
    // the requested memory address, repeating it if a whole word is requested.
    // So in this case, it should result in 0x01040104
    u32 magic;
    osPiReadIo(0x1fd00104u, &magic);
    if (magic == 0u) {
        // Older versions of mupen (and pre-2.12 ParallelN64) just always read 0
        gPlatform |= MUPEN_OLD;
        puppyprint_log(LOG_INFO, "Mupen64 Emulator Detected.\n");
    } else {
        __osPiGetAccess();
        while (IO_READ(PI_STATUS_REG) & (PI_STATUS_DMA_BUSY|PI_STATUS_IO_BUSY));
        const u16 halfMagic = *((volatile u16*)0xbfd00106u);
        __osPiRelAccess();

        // Now do a halfword read instead.
        switch (halfMagic) {
            // This is the correct result (echo back the lower half of the requested address)
            case 0x0106: {
                // Test to see if the libpl emulator extension is present.
                osPiWriteIo(0x1ffb0000u, 0u);
                osPiReadIo(0x1ffb0000u, &magic);
                if (magic == 0x00500000u) {
                    // libpl is supported. Must be ParallelN64
                    gPlatform |= PARALLEL_LAUNCHER;
                    puppyprint_log(LOG_INFO, "Ares N64 Emulator Detected.\n");
                    break;
                }
                
                // If the cache is emulated, it's Ares
                if (check_cache_emulation()) {
                    gPlatform |= ARES;
                    puppyprint_log(LOG_INFO, "Ares N64 Emulator Detected.\n");
                    break;
                }

                // its the Project64 4.0 interpreter core
                gPlatform |= PJ64_4;
                puppyprint_log(LOG_INFO, "Project64 Emulator Detected.\n");
                break;
            }
            // This looks like it should be the expected result considering what we got when we
            // requested the whole word, but that's actually wrong. Later versions of mupen
            // (and the Simple64 fork of it) get this wrong.
            case 0x0104:
                if (check_cache_emulation()) {
                    gPlatform |= SIMPLE64;
                    puppyprint_log(LOG_INFO, "Simple64 Emulator Detected.\n");
                } else {
                    gPlatform |= MUPEN_NEXT;
                    puppyprint_log(LOG_INFO, "Mupen64 Emulator Detected.\n");
                }
                break;
            // If reading a word gives the correct response, but reading a halfword always gives 0,
            // then we are dealing with some version of Project 64. Call into this helper function
            // to find out which version we're dealing with.
            case 0x0000:
                get_pj64_version();
                puppyprint_log(LOG_INFO, "Project64 Emulator Detected.\n");
                break;
            // No known emulator gives any other value. If we somehow manage to get here, just return 0
            default:
                puppyprint_log(LOG_INFO, "Cannot determine run environment.\n");
                gPlatform = 0;
                break;
        }
    }
    puppyprint_log(LOG_INFO, "Counter Factor Setting: %d.\n", cf);
} 

#define STEP 0x100000
#define SIZE_4MB 0x400000
#define SIZE_8MB 0x800000

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

INLINE void find_expansion_pak(void) {
#ifdef FORCE_4MB_MEMORY
    gExpansionPak = FALSE;
    gUseExpansionMemory = FALSE;
    puppyprint_log(LOG_INFO, "4MB Memory Forced.\n");
    return;
#endif
    if (osGetMemSize() > 0x400000) {
        gExpansionPak = TRUE;
        puppyprint_log(LOG_INFO, "Expansion Pak Detected\n");
#if EXPANSION_PAK_SUPPORT == 0
        gUseExpansionMemory = FALSE;
#else
        gUseExpansionMemory = TRUE;
#endif
    } else {
        puppyprint_log(LOG_INFO, "Expansion Pak Missing\n");
        gExpansionPak = FALSE;
        gUseExpansionMemory = FALSE;
    }
}

/******************************/

extern MemoryPoolSlot gMainMemoryPool;
extern void *gAudioHeapStack;

/**
 * Where it all begins.
 * Once the boot procedure is finished in IPL3, this function is run.
 * It kicks things off by initialising thread1, which serves as the top level
 * Official name: boot
 */
void main2(void) {
    osInitialize();
    osTvType = TV_TYPE_NTSC; // Temporary while PAL is still broken
    bzero(&gAudioHeapStack, RAM_END - (s32) (&gAudioHeapStack));
    osCreateThread(&gThread1, 1, &thread1_main, 0, &gThread3Stack[THREAD1_STACK / (sizeof(u64))], OS_PRIORITY_IDLE);
    osStartThread(&gThread1);
}

/**
 * Initialise the crash handler thread, then initialise the main game thread.
 * Reset the start and endpoint of the game thread stack, then set thread priority to zero, effectively
 * stopping this thread, as it's no longer needed.
 */
void thread1_main(UNUSED void *unused) {
    // The iQue will read either 1 or 2. The N64 is uninitialised, therefore, should read literally anything else.
    if (__osBbIsBb > 2) {
        __osBbIsBb = 0;
    }
    // thread0_create();
    init_PI_mesg_queue();
#ifdef ENABLE_USB
    if (__osBbIsBb == FALSE) {
        init_usb_thread();
    }
#endif
    crash_screen_init();
    find_expansion_pak();
#ifdef PUPPYPRINT_DEBUG
    bzero(&gPuppyPrint, sizeof(gPuppyPrint));
    gPuppyPrint.logLevel = LOG_EXTRA;
#endif
    osCreateThread(&gThread3, 3, &thread3_main, 0, &gThread3Stack[THREAD3_STACK / sizeof(u64)], 10);
    gThread3Stack[THREAD3_STACK / sizeof(u64)] = 0;
    gThread3Stack[0] = 0;
    osStartThread(&gThread3);
    osSetThreadPri(OS_PRIORITY_IDLE);
    while (1) {}
}

s32 _Printf(outfun prout, char *dst, const char *fmt, va_list args);

INLINE char *proutSprintf(char *dst, const char *src, size_t count) {
    char *ret = dst;
    bcopy((char *) src, dst, count);
    return ret + count;
}

int puppyprintf(char *dst, const char *fmt, ...) {
    s32 ans;
    va_list ap;
    va_start(ap, fmt);
    ans = _Printf(proutSprintf, dst, fmt, ap);
    if (ans >= 0) {
        dst[ans] = 0;
    }
    return ans;
}

#ifdef PUPPYPRINT_DEBUG
extern Gfx *gCurrDisplayList;
extern s32 gSPTaskNum;
extern s32 gCurrNumF3dCmdsPerPlayer;
extern s32 sLogicUpdateRate;

u8 perfIteration = 0;
f32 gFPS = 0;
u8 gWidescreen = 0;
s32 sTriCount = 0;
s32 sRectCount = 0;
s32 sVtxCount = 0;
s32 prevTime = 0;
u32 sTimerTemp = 0;
u8 gShowHiddenGeometry = FALSE;
u8 gShowHiddenObjects = FALSE;
u8 sPrintOrder[PP_RSP_GFX];
u8 sRAMPrintOrder[MEMP_TOTAL];
u16 sObjPrintOrder[NUM_OBJECT_PRINTS];
struct PuppyPrint gPuppyPrint;
char *sPuppyPrintStrings[] = { PP_STRINGS };
char *sPuppyPrintPageStrings[] = { PP_PAGES };
char *sPuppyPrintMainTimerStrings[] = { PP_MAINDRAW };
char *sPuppyprintMemColours[] = { PP_MEM };

#define FRAMETIME_COUNT 10

u32 frameTimes[FRAMETIME_COUNT];
u8 curFrameTimeIndex = 0;

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
    gFPS = (FRAMETIME_COUNT * 1000000.0f) / (OS_CYCLES_TO_USEC(newTime - oldTime) * divisor);
}

void rdp_profiler_update(u32 *time, u32 time2) {
    time[PERF_AGGREGATE] -= time[perfIteration];
    time[perfIteration] = time2;
    time[PERF_AGGREGATE] += time[perfIteration];
}

void profiler_update(u32 *time, u32 time2) {
    u32 tmm = (osGetCount() - time2);
    if (tmm > OS_USEC_TO_CYCLES(99999)) {
        tmm = OS_USEC_TO_CYCLES(99999);
    }
    time[PERF_AGGREGATE] -= time[perfIteration];
    time[perfIteration] = tmm;
    time[PERF_AGGREGATE] += time[perfIteration];
}

void profiler_offset(u32 *time, u32 offset) {
    time[PERF_AGGREGATE] -= offset;
    time[perfIteration] -= offset;
}

void profiler_add(u32 time, u32 offset) {
    u32 newTime = osGetCount();
    u32 offsetTime = find_thread_interrupt_offset(offset, newTime);
    offset = newTime - offset - offsetTime;
    if (offset > OS_USEC_TO_CYCLES(99999)) {
        offset = OS_USEC_TO_CYCLES(99999);
    }

    gPuppyPrint.timers[time][PERF_AGGREGATE] += offset;
    gPuppyPrint.timers[time][perfIteration] += offset;
}

INLINE void puppyprint_input(void) {
    u32 inputHeld;
    u32 inputPressed;

    for (int i = 0; i < 4; i++) {
        if ((inputHeld = get_buttons_held_from_player(i))) {
            break;
        }
    }
    
    for (int i = 0; i < 4; i++) {
        if ((inputPressed = get_buttons_pressed_from_player(i))) {
            break;
        }
    }


    // Allow toggling of the profiler.
    if (inputHeld & U_JPAD && inputPressed & L_TRIG) {
        gPuppyPrint.enabled ^= 1;
        return; // Sanitisation is healthy.
    }
    if (gPuppyPrint.enabled == FALSE) {
        return;
    }

    // Handle opening and changing page in the menu.
    if (inputPressed & L_TRIG) {
        if (gPuppyPrint.menuOption != PAGE_COVERAGE) {
            gPuppyPrint.menuOpen ^= 1;
            if (gPuppyPrint.page != gPuppyPrint.menuOption) {
                gPuppyPrint.pageScroll = 0;
            }
            gPuppyPrint.page = gPuppyPrint.menuOption;
        } else {
            gPuppyPrint.showCvg ^= TRUE;
        }
    }

    if (gPuppyPrint.menuOpen) {
        if (inputPressed & U_JPAD) {
            gPuppyPrint.menuOption--;
            if (gPuppyPrint.menuOption <= -1) {
                gPuppyPrint.menuOption = PAGE_COUNT - 1;
                gPuppyPrint.menuScroll = PAGE_COUNT - 5;
            }
        } else if (inputPressed & D_JPAD) {
            gPuppyPrint.menuOption++;
            if (gPuppyPrint.menuOption >= PAGE_COUNT) {
                gPuppyPrint.menuOption = 0;
                gPuppyPrint.menuScroll = 0;
            }
        }
        if (gPuppyPrint.menuScroll + 4 < gPuppyPrint.menuOption) {
            gPuppyPrint.menuScroll++;
        } else if (gPuppyPrint.menuScroll > gPuppyPrint.menuOption) {
            gPuppyPrint.menuScroll--;
        }
    } else {
        if (gPuppyPrint.page == PAGE_LOG) {
            s32 maxPrints = (gScreenHeight - 36);
            if (inputHeld & D_JPAD) {
                gPuppyPrint.pageScroll += sLogicUpdateRate * 2;
                if (gPuppyPrint.pageScroll > ((NUM_LOG_LINES) * 10) - maxPrints) {
                    gPuppyPrint.pageScroll = ((NUM_LOG_LINES) * 10) - maxPrints;
                }
            } else if (inputHeld & U_JPAD) {
                gPuppyPrint.pageScroll -= sLogicUpdateRate * 2;
                if (gPuppyPrint.pageScroll < 0) {
                    gPuppyPrint.pageScroll = 0;
                }
            }
        }

        if (gPuppyPrint.page == PAGE_MEMORY) {
            s32 maxPrints = (gScreenHeight - 36);
            if (inputHeld & D_JPAD) {
                gPuppyPrint.pageScroll += sLogicUpdateRate * 2;
                if (gPuppyPrint.pageScroll > (10 * (MEMP_TOTAL + 10)) - maxPrints) {
                    gPuppyPrint.pageScroll = (10 * (MEMP_TOTAL + 10)) - maxPrints;
                }
            } else if (inputHeld & U_JPAD) {
                gPuppyPrint.pageScroll -= sLogicUpdateRate * 2;
                if (gPuppyPrint.pageScroll < 0) {
                    gPuppyPrint.pageScroll = 0;
                }
            }
            if (inputPressed & R_JPAD) {
                ram_dump();
            }
        }
    }
}

void profiler_reset_objects(void) {
    s32 i;
    for (i = 0; i < NUM_OBJECT_PRINTS; i++) {
        gPuppyPrint.objTimers[i][PERF_AGGREGATE] -= gPuppyPrint.objTimers[i][perfIteration];
        gPuppyPrint.objTimers[i][perfIteration] = 0;
        gPuppyPrint.objCounts[i] = 0;
        gPuppyPrint.objHeaders[i] = NULL;
    }
}

void profiler_reset_values(void) {
    s32 i;
    u32 flags = interrupts_disable();
    for (i = 0; i < PP_RDP_BUS; i++) {
        gPuppyPrint.timers[i][PERF_AGGREGATE] -= gPuppyPrint.timers[i][perfIteration];
        gPuppyPrint.timers[i][perfIteration] = 0;
    }
    profiler_reset_objects();
    gPuppyPrint.textureLoads = 0;
    puppyprint_input();
    interrupts_enable(flags);
}

void profiler_add_obj(u32 objID, u32 time, ObjectHeader *header) {
    u32 tempTime = OS_CYCLES_TO_USEC(osGetCount() - time);
    if (objID >= NUM_OBJECT_PRINTS) {
        return;
    }
    gPuppyPrint.objCounts[objID]++;
    gPuppyPrint.objTimers[objID][PERF_AGGREGATE] += tempTime;
    gPuppyPrint.objTimers[objID][perfIteration] += tempTime;
    gPuppyPrint.objHeaders[objID] = header;
}

void profiler_snapshot(s32 eventID) {
    u32 snapshot = gPuppyPrint.threadIteration[eventID / 2];
    if (snapshot >= NUM_THREAD_ITERATIONS - 1) {
        snapshot = NUM_THREAD_ITERATIONS - 1;
    }
    gPuppyPrint.threadTimes[snapshot][eventID] = osGetCount();
    // Thread endings are even numbers.
    if (eventID % 2) {
        gPuppyPrint.threadIteration[eventID / 2]++;
    }
}

#define TEXT_OFFSET 10

void draw_blank_box(Gfx **gfx, s32 x1, s32 y1, s32 x2, s32 y2, u32 colour) {
    gDPSetPrimColor((*gfx)++, 0, 0, (colour >> 24) & 0xFF, (colour >> 16) & 0xFF, (colour >> 8) & 0xFF,
                    (colour) & 0xFF);
    gDPPipeSync((*gfx)++);
    gDPSetCombineMode((*gfx)++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    if (x1 < 0) {
        x1 = 0;
    }
    if (y1 < 0) {
        y1 = 0;
    }
    if (x2 > gScreenWidth) {
        x2 = gScreenWidth;
    }
    if (y2 > gScreenHeight) {
        y2 = gScreenHeight;
    }
    if ((colour & 0xFF) == 255) {
        gDPSetRenderMode((*gfx)++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    } else {
        gDPSetRenderMode((*gfx)++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    }
    gDPSetCycleType((*gfx)++, G_CYC_1CYCLE);
    gDPFillRectangle((*gfx)++, x1, y1, x2, y2);
}

void puppyprint_render_minimal(void) {
    char textBytes[16];
    draw_blank_box(&gCurrDisplayList, TEXT_OFFSET - 2, 8, 112, 50 + 2, 0x0000007F);
    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_colour(255, 255, 255, 255, 255);
    set_text_background_colour(0, 0, 0, 0);
    set_kerning(FALSE);
    puppyprintf(textBytes, "FPS: %2.2f", (f64) gFPS);
    draw_text(&gCurrDisplayList, TEXT_OFFSET, 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "CPU: %dus", gPuppyPrint.cpuTime[PERF_TOTAL]);
    draw_text(&gCurrDisplayList, TEXT_OFFSET, 20, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "(%d%%)", gPuppyPrint.cpuTime[PERF_TOTAL] / 333);
    draw_text(&gCurrDisplayList, 112 - 4, 20, textBytes, ALIGN_TOP_RIGHT);
    puppyprintf(textBytes, "RSP: %dus", gPuppyPrint.rspTime);
    draw_text(&gCurrDisplayList, TEXT_OFFSET, 30, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "(%d%%)", gPuppyPrint.rspTime / 333);
    draw_text(&gCurrDisplayList, 112 - 4, 30, textBytes, ALIGN_TOP_RIGHT);
    puppyprintf(textBytes, "RDP: %dus", gPuppyPrint.rdpTime);
    draw_text(&gCurrDisplayList, TEXT_OFFSET, 40, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "(%d%%)", gPuppyPrint.rdpTime / 333);
    draw_text(&gCurrDisplayList, 112 - 4, 40, textBytes, ALIGN_TOP_RIGHT);
}

INLINE void puppyprint_render_overview(void) {
    char textBytes[32];
    s32 i;
    s32 y;
    puppyprint_render_minimal();
    // Draw triangle, vertex and overall RAM on bottom left.
    draw_blank_box(&gCurrDisplayList, ((gScreenWidth / 2) / 3) - 42, gScreenHeight - 50, ((gScreenWidth / 2) / 3) + 62, gScreenHeight - 6,
                   0x0000007F);
    puppyprintf(textBytes, "Textures: %d", gPuppyPrint.textureLoads);
    draw_text(&gCurrDisplayList, ((gScreenWidth / 2) / 3) + 10, gScreenHeight - 48, textBytes, ALIGN_TOP_CENTER);
    puppyprintf(textBytes, "RAM: 0x%06X",
                TOTALRAM - gPuppyPrint.ramPools[MEMP_OVERALL] - ((u32) &gMainMemoryPool - 0x80000000));
    draw_text(&gCurrDisplayList, ((gScreenWidth / 2) / 3) + 10, gScreenHeight - 38, textBytes, ALIGN_TOP_CENTER);
    puppyprintf(textBytes, "Tri: %d Vtx: %d", sTriCount, sVtxCount);
    draw_text(&gCurrDisplayList, ((gScreenWidth / 2) / 3) + 10, gScreenHeight - 28, textBytes, ALIGN_TOP_CENTER);
    puppyprintf(textBytes, "Gfx: %d / %d", ((u32) gCurrDisplayList - (u32) gDisplayLists[gSPTaskNum]) / sizeof(Gfx),
                gCurrNumF3dCmdsPerPlayer);
    draw_text(&gCurrDisplayList, ((gScreenWidth / 2) / 3) + 10, gScreenHeight - 18, textBytes, ALIGN_TOP_CENTER);

    // Draw important timings on the top right.
    draw_blank_box(&gCurrDisplayList, gScreenWidth - 124, 8, gScreenWidth - 8, 74, 0x0000007F);
    puppyprintf(textBytes, "Game: %dus", gPuppyPrint.gameTime[PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 122, 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "(%d%%)", gPuppyPrint.gameTime[PERF_TOTAL] / 333);
    draw_text(&gCurrDisplayList, gScreenWidth - 8 - 4, 10, textBytes, ALIGN_TOP_RIGHT);
    puppyprintf(textBytes, "Audio:  %dus", gPuppyPrint.audTime[PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 122, 20, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "(%d%%)", gPuppyPrint.audTime[PERF_TOTAL] / 333);
    draw_text(&gCurrDisplayList, gScreenWidth - 8 - 4, 20, textBytes, ALIGN_TOP_RIGHT);
    y = 32;
    for (i = 0; i < PP_MAIN_TIMES_TOTAL - PP_MAIN_TIME_OFFSET; i++) {
        puppyprintf(textBytes, "%s: %dus", sPuppyPrintMainTimerStrings[i], gPuppyPrint.coreTimers[i][PERF_TOTAL]);
        draw_text(&gCurrDisplayList, gScreenWidth - 122, y, textBytes, ALIGN_TOP_LEFT);
        puppyprintf(textBytes, "(%d%%)", gPuppyPrint.coreTimers[i][PERF_TOTAL] / 333);
        draw_text(&gCurrDisplayList, gScreenWidth - 8 - 4, y, textBytes, ALIGN_TOP_RIGHT);
        y += 10;
    }
}

INLINE void puppyprint_render_breakdown(void) {
    char textBytes[32];
    s32 y;
    s32 i;

    puppyprint_render_minimal();
    y = 8;
    draw_blank_box(&gCurrDisplayList, gScreenWidth - 144, 0, gScreenWidth, gScreenHeight, 0x00000064);
    gDPPipeSync(gCurrDisplayList++);
    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_colour(255, 255, 255, 255, 255);
    set_text_background_colour(0, 0, 0, 0);
    set_kerning(FALSE);
    for (i = 0; i < PP_RSP_GFX; i++) {
        if (gPuppyPrint.timers[sPrintOrder[i]][PERF_TOTAL] == 0) {
            continue;
        }
        puppyprintf(textBytes, "%s \t%dus (%d%%)", sPuppyPrintStrings[sPrintOrder[i]],
                    gPuppyPrint.timers[sPrintOrder[i]][PERF_TOTAL],
                    gPuppyPrint.timers[sPrintOrder[i]][PERF_TOTAL] / 333);
        draw_text(&gCurrDisplayList, gScreenWidth - 136, y, textBytes, ALIGN_TOP_LEFT);
        y += 10;
        if (y > gScreenHeight - 16) {
            break;
        }
    }
}

INLINE void puppyprint_render_rcp(void) {
    char textBytes[32];
    int y = 8;

    puppyprint_render_minimal();

    
    draw_blank_box(&gCurrDisplayList, gScreenWidth - 100, 0, gScreenWidth, gScreenHeight, 0x00000064);
    puppyprintf(textBytes, "CPU");
    draw_text(&gCurrDisplayList, gScreenWidth - 50, y, textBytes, ALIGN_TOP_CENTER);
    puppyprintf(textBytes, "Thread 3:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 12, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", gPuppyPrint.gameTime[PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "Thread 4:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", gPuppyPrint.audTime[PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "Thread 5:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", gPuppyPrint.schedTime[PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "Yield 3:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", gPuppyPrint.gameYield[PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);

    puppyprintf(textBytes, "RDP");
    draw_text(&gCurrDisplayList, gScreenWidth - 50, y += 14, textBytes, ALIGN_TOP_CENTER);
    puppyprintf(textBytes, "Clock:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 12, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", gPuppyPrint.timers[PP_RDP_CLK][PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "BufBusy:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", gPuppyPrint.timers[PP_RDP_BUF][PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "PipeBusy:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", gPuppyPrint.timers[PP_RDP_BUS][PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "TexMem:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", gPuppyPrint.timers[PP_RDP_TMM][PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "No Cmd:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", gPuppyPrint.timers[PP_RDP_CLK][PERF_TOTAL] - gPuppyPrint.timers[PP_RDP_BUF][PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "Fullsync:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", gPuppyPrint.timers[PP_RDP_CLK][PERF_TOTAL] - gPuppyPrint.timers[PP_RDP_BUS][PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);

    puppyprintf(textBytes, "RSP");
    draw_text(&gCurrDisplayList, gScreenWidth - 50, y += 14, textBytes, ALIGN_TOP_CENTER);
    puppyprintf(textBytes, "Graphics:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 12, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", gPuppyPrint.timers[PP_RSP_GFX][PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "Audio:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", gPuppyPrint.timers[PP_RSP_AUD][PERF_TOTAL]);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "Yield:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", (s32) ((f32) gPuppyPrint.rspYield * ((f32) 30 / (f32) NUM_PERF_ITERATIONS)));
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "Triangles:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", sTriCount);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "Rects:");
    draw_text(&gCurrDisplayList, gScreenWidth - 96, y += 10, textBytes, ALIGN_TOP_LEFT);
    puppyprintf(textBytes, "%d", sRectCount);
    draw_text(&gCurrDisplayList, gScreenWidth - 38, y, textBytes, ALIGN_TOP_LEFT);

}

INLINE void puppyprint_render_memory(void) {
    char textBytes[24];
    s32 y;
    u32 i;

    y = 36 - gPuppyPrint.pageScroll;
    draw_blank_box(&gCurrDisplayList, gScreenWidth - 160, 0, gScreenWidth, gScreenHeight, 0x00000064);
    gDPPipeSync(gCurrDisplayList++);
    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_colour(255, 255, 255, 255, 255);
    set_text_background_colour(0, 0, 0, 0);
    set_kerning(FALSE);
    puppyprintf(textBytes, "Free 0x%06X, (%2.2f%%)",
                TOTALRAM - gPuppyPrint.ramPools[MEMP_OVERALL] - gPuppyPrint.ramPools[MEMP_CODE],
                (f64) (((f32) (TOTALRAM - gPuppyPrint.ramPools[MEMP_OVERALL] - gPuppyPrint.ramPools[MEMP_CODE]) /
                        (f32) TOTALRAM) *
                       100.0f));
    draw_text(&gCurrDisplayList, gScreenWidth - 78, 8, textBytes, ALIGN_TOP_CENTER);
    puppyprintf(textBytes, "Total 0x%06X", TOTALRAM);
    draw_text(&gCurrDisplayList, gScreenWidth - 78, 18, textBytes, ALIGN_TOP_CENTER);
    gDPSetScissor(gCurrDisplayList++, G_SC_NON_INTERLACE, gScreenWidth - 156, 32, gScreenWidth, gScreenHeight);
    for (i = 1; i < MEMP_TOTAL; i++) {
        if (gPuppyPrint.ramPools[sRAMPrintOrder[i]] == 0) {
            continue;
        }
        if (y < 24 || y > gScreenHeight) {
            y += 10;
            continue;
        }
        draw_text(&gCurrDisplayList, gScreenWidth - 156, y, sPuppyprintMemColours[sRAMPrintOrder[i]], ALIGN_TOP_LEFT);
        puppyprintf(textBytes, "0x%X (%2.2f%%)", gPuppyPrint.ramPools[sRAMPrintOrder[i]],
                    (f64) (((f32) gPuppyPrint.ramPools[sRAMPrintOrder[i]] / (f32) TOTALRAM) * 100.0f));
        draw_text(&gCurrDisplayList, gScreenWidth - 88, y, textBytes, ALIGN_TOP_LEFT);
        y += 10;
    }
    gDPSetScissor(gCurrDisplayList++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
}

#undef TOTALRAM

INLINE void puppyprint_render_objects(void) {
    char textBytes[48];
    s32 y;
    s32 i;

    puppyprint_render_minimal();
    y = 8;
    draw_blank_box(&gCurrDisplayList, gScreenWidth - 144, 0, gScreenWidth, gScreenHeight, 0x00000064);
    gDPPipeSync(gCurrDisplayList++);
    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_colour(255, 255, 255, 255, 255);
    set_text_background_colour(0, 0, 0, 0);
    set_kerning(FALSE);
    for (i = 0; i < NUM_OBJECT_PRINTS; i++) {
        if (gPuppyPrint.objTimers[sObjPrintOrder[i]][PERF_TOTAL] == 0 ||
            gPuppyPrint.objHeaders[sObjPrintOrder[i]] == NULL) {
            continue;
        }
        puppyprintf(textBytes, "(%02d)  %dus (%d%%) %s", gPuppyPrint.objCounts[sObjPrintOrder[i]],
                    gPuppyPrint.objTimers[sObjPrintOrder[i]][PERF_TOTAL],
                    gPuppyPrint.objTimers[sObjPrintOrder[i]][PERF_TOTAL] / 333,
                    gPuppyPrint.objHeaders[sObjPrintOrder[i]]->internalName);
        draw_text(&gCurrDisplayList, gScreenWidth - 136, y, textBytes, ALIGN_TOP_LEFT);
        y += 10;
        if (y > gScreenHeight - 16) {
            break;
        }
    }
}

INLINE void puppyprint_render_log(void) {
    s32 i;
    s32 y;
    s32 sineTime = 224 + (sins_f(sTimerTemp * 2500.0f) * 32.0f);
    s32 firstDraw = TRUE;
    draw_blank_box(&gCurrDisplayList, 0, 0, gScreenWidth, gScreenHeight, 0x00000064);
    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_colour(255, 255, 255, 255, 255);
    set_text_background_colour(0, 0, 0, 0);
    set_kerning(FALSE);

    y = 4 - gPuppyPrint.pageScroll;
    for (i = NUM_LOG_LINES; i > 0; i--) {
        if (gPuppyPrint.logText[i][0] == NULL) {
            continue;
        }
        if (y <= -20 || y >= gScreenHeight) {
            firstDraw = FALSE;
            y += 10;
            continue;
        }
        if (firstDraw) {
            set_text_colour(255, sineTime, sineTime, 255, 255);
        } else {
            set_text_colour(255, 255, 255, 255, 255);
        }
        draw_text(&gCurrDisplayList, 8, y, gPuppyPrint.logText[i], ALIGN_TOP_LEFT);
        y += 10;
        firstDraw = FALSE;
    }
}

void puppyprint_load_snapshot(s32 type, s32 time) {
    time = osGetCount() - time;
    if (gPuppyPrint.loading) {
        gPuppyPrint.loadTimes[type] += time;
        if (type == PP_LOAD_TOTAL) {
            gPuppyPrint.loading = FALSE;
        }
    }
}

void puppyprint_reset_load(void) {
    gPuppyPrint.loading = TRUE;
    bzero(&gPuppyPrint.loadTimes, sizeof(gPuppyPrint.loadTimes));
}

INLINE void puppyprint_render_load(void) {
    char textBytes[48];
    s32 y;

    puppyprint_render_minimal();
    y = 8;
    draw_blank_box(&gCurrDisplayList, gScreenWidth - 144, 0, gScreenWidth, gScreenHeight, 0x00000064);
    gDPPipeSync(gCurrDisplayList++);
    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_colour(255, 255, 255, 255, 255);
    set_text_background_colour(0, 0, 0, 0);
    set_kerning(FALSE);

    gPuppyPrint.loadTimes[PP_LOAD_ETC] = gPuppyPrint.loadTimes[PP_LOAD_TOTAL] - (gPuppyPrint.loadTimes[PP_LOAD_DMA] + 
    gPuppyPrint.loadTimes[PP_LOAD_DECOMPRESS] + gPuppyPrint.loadTimes[PP_LOAD_MALLOC] + gPuppyPrint.loadTimes[PP_LOAD_OBJECTS]);

    puppyprintf(textBytes, "Total: %2.3fs", (f64) (f32)(gPuppyPrint.loadTimes[PP_LOAD_TOTAL] / 46875000.0f));
    draw_text(&gCurrDisplayList, gScreenWidth - 72, y + 10, textBytes, ALIGN_TOP_CENTER);
    y += 12;
    puppyprintf(textBytes, "DMA: %2.3fs", (f64) (f32)(gPuppyPrint.loadTimes[PP_LOAD_DMA] / 46875000.0f));
    draw_text(&gCurrDisplayList, gScreenWidth - 72, y + 10, textBytes, ALIGN_TOP_CENTER);
    y += 12;
    puppyprintf(textBytes, "Decompress: %2.3fs", (f64) (f32)(gPuppyPrint.loadTimes[PP_LOAD_DECOMPRESS] / 46875000.0f));
    draw_text(&gCurrDisplayList, gScreenWidth - 72, y + 10, textBytes, ALIGN_TOP_CENTER);
    y += 12;
    puppyprintf(textBytes, "Alloc: %2.3fs", (f64) (f32)(gPuppyPrint.loadTimes[PP_LOAD_MALLOC] / 46875000.0f));
    draw_text(&gCurrDisplayList, gScreenWidth - 72, y + 10, textBytes, ALIGN_TOP_CENTER);
    y += 12;
    puppyprintf(textBytes, "Objects: %2.3fs", (f64) (f32)(gPuppyPrint.loadTimes[PP_LOAD_OBJECTS] / 46875000.0f));
    draw_text(&gCurrDisplayList, gScreenWidth - 72, y + 10, textBytes, ALIGN_TOP_CENTER);
    y += 12;
    puppyprintf(textBytes, "Etc: %2.3fs", (f64) (f32)(gPuppyPrint.loadTimes[PP_LOAD_ETC] / 46875000.0f));
    draw_text(&gCurrDisplayList, gScreenWidth - 72, y + 10, textBytes, ALIGN_TOP_CENTER);
    y += 12;
}

INLINE void puppyprint_render_audio(void) {
    char textBytes[64];
    u32 audChan = 0;
    s32 xOrigin = (gScreenWidth / 2) - ((32 * 7) / 2);
    s32 x;
    Gfx *gfx = gCurrDisplayList;
    draw_blank_box(&gfx, (gScreenWidth / 2) - 120, (gScreenHeight / 2) - 80, (gScreenWidth / 2) + 120, (gScreenHeight / 2) + 100, 0x00000064);

    unk800DC6BC *p = gAlSndPlayerPtr;
    ALEvent *e = &p->nextEvent;
    ALSeqPlayer *s = gMusicPlayer;
    ALSeqPlayer *j = gJinglePlayer;
    ALEventQueue *evtq;

    x = xOrigin;
    // sfx
    for (int i = 0; i < p->soundChannels; i++) {
        //debug_printf("%d\n", p->soundChannels);
        u32 col;
        if (audChan) {
            col = 0x00FF00FF;
        } else {
            col = 0xFF0000FF;
        }
        draw_blank_box(&gfx, x, (gScreenHeight / 2) + 76, x + 4, (gScreenHeight / 2) + 80, col);
        x += 7;
    }
    x = xOrigin;
    // bgm
    evtq = &gMusicPlayer->evtq;
    for (int i = 0; i < s->maxChannels; i++) {
        u32 col;

        if (audChan) {
            col = 0x00FF00FF;
        } else {
            col = 0xFF0000FF;
        }
        draw_blank_box(&gfx, x, (gScreenHeight / 2) + 84, x + 4, (gScreenHeight / 2) + 88, col);
        x += 7;
    }
    x = xOrigin;
    // jingle
    for (int i = 0; i < j->maxChannels; i++) {
        u32 col;
        if (audChan) {
            col = 0x00FF00FF;
        } else {
            col = 0xFF0000FF;
        }
        draw_blank_box(&gfx, x, (gScreenHeight / 2) + 92, x + 4, (gScreenHeight / 2) + 96, col);
        x += 7;
    }
    gDPPipeSync(gfx++);
    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_colour(255, 255, 255, 255, 255);
    set_text_background_colour(0, 0, 0, 0);
    set_kerning(FALSE);

    gCurrDisplayList = gfx;
}

void puppyprint_render_coverage(Gfx **dList) {
    gSPClearGeometryMode((*dList)++, G_ZBUFFER);
    gDPPipeSync((*dList)++);
    gDPSetCycleType((*dList)++, G_CYC_1CYCLE);
    gDPSetBlendColor((*dList)++, 0xFF, 0xFF, 0xFF, 0xFF);
    gDPSetPrimDepth((*dList)++, 0xFFFF, 0xFFFF);
    gDPSetDepthSource((*dList)++, G_ZS_PRIM);
    gDPSetRenderMode((*dList)++, G_RM_VISCVG, G_RM_VISCVG2);
    gDPFillRectangle((*dList)++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
}

INLINE void render_page_menu(void) {
    char textBytes[16];
    s32 i;
    s32 y;
    s32 sineTime = 192 + (sins_f(sTimerTemp * 5000.0f) * 64.0f);
    draw_blank_box(&gCurrDisplayList, TEXT_OFFSET - 2, 56, 112, 108 + 2, 0x0000007F);
    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_background_colour(0, 0, 0, 0);
    set_kerning(FALSE);

    y = -gPuppyPrint.menuScroll * 10;
    for (i = 0; i < PAGE_COUNT; i++) {
        if (y <= -10) {
            y += 10;
            continue;
        }
        if (y >= 50) {
            break;
        }
        if (i == gPuppyPrint.menuOption) {
            set_text_colour(255, sineTime, sineTime, 255, 255);
        } else {
            set_text_colour(255, 255, 255, 255, 255);
        }
        puppyprintf(textBytes, "%s", sPuppyPrintPageStrings[i]);
        draw_text(&gCurrDisplayList, TEXT_OFFSET, 58 + y, textBytes, ALIGN_TOP_LEFT);
        y += 10;
    }
}

void render_profiler(void) {
    gDPSetScissor(gCurrDisplayList++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);

    switch (gPuppyPrint.page) {
        case PAGE_MINIMAL:
            puppyprint_render_minimal();
            break;
        case PAGE_OVERVIEW:
            puppyprint_render_overview();
            break;
        case PAGE_BREAKDOWN:
            puppyprint_render_breakdown();
            break;
        case PAGE_RCP:
            puppyprint_render_rcp();
            break;
        case PAGE_MEMORY:
            puppyprint_render_memory();
            break;
        case PAGE_OBJECTS:
            puppyprint_render_objects();
            break;
        case PAGE_LOG:
            puppyprint_render_log();
            break;
        case PAGE_LOAD:
            puppyprint_render_load();
            break;
        case PAGE_AUDIO:
            puppyprint_render_audio();
            break;
        case PAGE_COVERAGE:
            break;
    }

    if (gPuppyPrint.menuOpen) {
        render_page_menu();
    }
}

char *gLogLevelStrings[] = {
    "FATAL - ",
    "ERROR - ",
    "WARN  - ",
    "INFO  - ",
    "EXTRA - ",
};

void puppyprint_log(s32 logType, const char *str, ...) {
    if (logType > gPuppyPrint.logLevel) {
        return;
    }
    s32 len;
    char textBytes[127];
    va_list arguments;

    //bzero(textBytes, sizeof(textBytes));
    va_start(arguments, str);
    if ((len = _Printf(proutSprintf, textBytes, str, arguments)) <= 0) {
        va_end(arguments);
        return;
    }
    textBytes[len] = '\0';
    debug_printf("%s %s", gLogLevelStrings[logType], textBytes);
    for (int i = 0; i < (NUM_LOG_LINES - 1); i++) {
        bcopy(gPuppyPrint.logText[i + 1], gPuppyPrint.logText[i], 127);
    }
    bcopy(textBytes, gPuppyPrint.logText[NUM_LOG_LINES - 1], len + 1);
    va_end(arguments);
}

INLINE void swapu(u8 *xp, u8 *yp) {
    u8 temp = *xp;
    *xp = *yp;
    *yp = temp;
}

INLINE void swapu16(u16 *xp, u16 *yp) {
    u16 temp = *xp;
    *xp = *yp;
    *yp = temp;
}

INLINE void calculate_print_order(void) {
    u32 i, j, min_idx;
    for (i = 0; i < PP_RSP_GFX; i++) {
        sPrintOrder[i] = i;
    }

    // One by one move boundary of unsorted subarray
    for (i = 0; i < PP_RSP_AUD; i++) {

        if (gPuppyPrint.timers[sPrintOrder[i]][PERF_TOTAL] == 0) {
            continue;
        }
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < PP_RSP_AUD; j++) {
            if (gPuppyPrint.timers[sPrintOrder[j]][PERF_TOTAL] > gPuppyPrint.timers[sPrintOrder[min_idx]][PERF_TOTAL]) {
                min_idx = j;
            }
        }

        // Swap the found minimum element
        // with the first element
        swapu(&sPrintOrder[min_idx], &sPrintOrder[i]);
    }
}

void calculate_ram_print_order(void) {
    u32 i, j, min_idx;
    for (i = 1; i < MEMP_TOTAL; i++) {
        sRAMPrintOrder[i] = i;
    }
    gPuppyPrint.ramPools[MEMP_CODE] = (u32) &gMainMemoryPool - K0BASE;

    // One by one move boundary of unsorted subarray
    for (i = 1; i < MEMP_TOTAL; i++) {

        if (gPuppyPrint.ramPools[sRAMPrintOrder[i]] == 0) {
            continue;
        }
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < MEMP_TOTAL; j++) {
            if (gPuppyPrint.ramPools[sRAMPrintOrder[j]] > gPuppyPrint.ramPools[sRAMPrintOrder[min_idx]]) {
                min_idx = j;
            }
        }

        // Swap the found minimum element
        // with the first element
        swapu(&sRAMPrintOrder[min_idx], &sRAMPrintOrder[i]);
    }
}

INLINE void calculate_obj_print_order(void) {
    u32 i, j, min_idx;
    for (i = 0; i < NUM_OBJECT_PRINTS; i++) {
        sObjPrintOrder[i] = i;
    }

    // One by one move boundary of unsorted subarray
    for (i = 0; i < NUM_OBJECT_PRINTS; i++) {

        if (gPuppyPrint.objTimers[sObjPrintOrder[i]][PERF_TOTAL] == 0) {
            continue;
        }
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < NUM_OBJECT_PRINTS; j++) {
            if (gPuppyPrint.objTimers[sObjPrintOrder[j]][PERF_TOTAL] >
                gPuppyPrint.objTimers[sObjPrintOrder[min_idx]][PERF_TOTAL]) {
                min_idx = j;
            }
        }

        // Swap the found minimum element
        // with the first element
        swapu16(&sObjPrintOrder[min_idx], &sObjPrintOrder[i]);
    }
}

void update_rdp_profiling(void) {
    rdp_profiler_update(gPuppyPrint.timers[PP_RDP_BUF], IO_READ(DPC_BUFBUSY_REG));
    rdp_profiler_update(gPuppyPrint.timers[PP_RDP_TMM], IO_READ(DPC_TMEM_REG));
    rdp_profiler_update(gPuppyPrint.timers[PP_RDP_BUS], IO_READ(DPC_PIPEBUSY_REG));
    rdp_profiler_update(gPuppyPrint.timers[PP_RDP_CLK], IO_READ(DPC_CLOCK_REG));
}

s32 find_thread_interrupt_offset(u32 lowTime, u32 highTime) {
    s32 i;
    u32 offsetTime = 0;
    // Find if there's been an audio thread update during this thread.
    for (i = 0; i < gPuppyPrint.threadIteration[THREAD3_END / 2]; i++) {
        if (gPuppyPrint.threadTimes[i][THREAD3_END] < highTime && gPuppyPrint.threadTimes[i][THREAD3_START] > lowTime) {
            offsetTime += gPuppyPrint.threadTimes[i][THREAD3_END] - gPuppyPrint.threadTimes[i][THREAD3_START];
        }
    }
    return offsetTime;
}

INLINE void calculate_individual_thread_timers(void) {
    s32 i;
    s32 j;
    u32 highTime;
    u32 lowTime;
    u32 normalTime;
    u32 offsetTime;
    f32 divisor;

    highTime = 0;
    for (j = 0; j < gPuppyPrint.threadIteration[THREAD5_START / 2]; j ++) {
        highTime += gPuppyPrint.threadTimes[gPuppyPrint.threadIteration[j]][THREAD5_END] - gPuppyPrint.threadTimes[gPuppyPrint.threadIteration[j]][THREAD5_START];
    }
    gPuppyPrint.schedTime[PERF_AGGREGATE] -= gPuppyPrint.schedTime[perfIteration];
    gPuppyPrint.schedTime[perfIteration] = MIN(highTime, (s32) (OS_USEC_TO_CYCLES(99999)));
    gPuppyPrint.schedTime[PERF_AGGREGATE] += gPuppyPrint.schedTime[perfIteration];

    // Audio thread is basically top prio, so no need to do any further shenanigans
    highTime = 0;
    for (j = 0; j < gPuppyPrint.threadIteration[THREAD3_START / 2]; j ++) {
        highTime += gPuppyPrint.threadTimes[gPuppyPrint.threadIteration[j]][THREAD3_END] - gPuppyPrint.threadTimes[gPuppyPrint.threadIteration[j]][THREAD3_START];
    }
    gPuppyPrint.audTime[PERF_AGGREGATE] -= gPuppyPrint.audTime[perfIteration];
    gPuppyPrint.audTime[perfIteration] = MIN(highTime, (s32) (OS_USEC_TO_CYCLES(99999)));
    gPuppyPrint.audTime[PERF_AGGREGATE] += gPuppyPrint.audTime[perfIteration];
    // Game thread, unfortunately, does not. We have to take the times of the audio too, so we can offset the values for
    // accuracy.
    highTime = 0;
    offsetTime = 0;
    lowTime = 0xFFFFFFFF;
    for (i = THREAD4_START; i <= THREAD4_END; i++) {
        for (j = 0; j < gPuppyPrint.threadIteration[i / 2]; j++) {
            // If an iteration crosses over mid read, the low time could be zero, which would ruin this whole gig.
            normalTime = gPuppyPrint.threadTimes[gPuppyPrint.threadIteration[j]][i];
            if (normalTime < lowTime && normalTime != 0) {
                lowTime = normalTime;
            }
            offsetTime += find_thread_interrupt_offset(lowTime, normalTime);
            if (normalTime > highTime) {
                highTime = normalTime;
            }
        }
    }
    gPuppyPrint.gameTime[PERF_AGGREGATE] -= gPuppyPrint.gameTime[perfIteration];
    gPuppyPrint.gameTime[perfIteration] = MIN(highTime - lowTime - offsetTime, (s32) (OS_USEC_TO_CYCLES(99999)));
    gPuppyPrint.gameTime[PERF_AGGREGATE] += gPuppyPrint.gameTime[perfIteration];

    gPuppyPrint.gameYield[PERF_AGGREGATE] -= gPuppyPrint.gameYield[perfIteration];
    gPuppyPrint.gameYield[perfIteration] = MIN(offsetTime, (s32) (OS_USEC_TO_CYCLES(99999)));
    gPuppyPrint.gameYield[PERF_AGGREGATE] += gPuppyPrint.gameYield[perfIteration];
    if (gPuppyPrint.shouldUpdate) {
        if (__osBbIsBb) {
            divisor = IQUE_DIVISOR;
        } else {
            divisor = 1.0f;
        }
        gPuppyPrint.schedTime[PERF_TOTAL] = OS_CYCLES_TO_USEC(gPuppyPrint.schedTime[PERF_AGGREGATE] * divisor) / NUM_PERF_ITERATIONS;
        gPuppyPrint.audTime[PERF_TOTAL] = OS_CYCLES_TO_USEC(gPuppyPrint.audTime[PERF_AGGREGATE] * divisor) / NUM_PERF_ITERATIONS;
        gPuppyPrint.gameTime[PERF_TOTAL] = OS_CYCLES_TO_USEC(gPuppyPrint.gameTime[PERF_AGGREGATE] * divisor) / NUM_PERF_ITERATIONS;
        gPuppyPrint.gameYield[PERF_TOTAL] = OS_CYCLES_TO_USEC(gPuppyPrint.gameYield[PERF_AGGREGATE] * divisor) / NUM_PERF_ITERATIONS;
    }
}

INLINE void calculate_core_timers(void) {
    s32 i;
    s32 lowTime;
    s32 highTime;
    s32 offsetTime = 0;

    for (i = 0; i < PP_MAIN_TIMES_TOTAL; i++) {
        offsetTime = 0;
        lowTime = gPuppyPrint.mainTimerPoints[0][i];
        highTime = gPuppyPrint.mainTimerPoints[1][i];
        offsetTime = find_thread_interrupt_offset(lowTime, highTime);
        gPuppyPrint.coreTimers[i][PERF_AGGREGATE] -= gPuppyPrint.coreTimers[i][perfIteration];
        gPuppyPrint.coreTimers[i][perfIteration] = MIN(highTime - lowTime - offsetTime, (s32) OS_USEC_TO_CYCLES(99999));
        gPuppyPrint.coreTimers[i][PERF_AGGREGATE] += gPuppyPrint.coreTimers[i][perfIteration];
        if (gPuppyPrint.shouldUpdate) {
            gPuppyPrint.coreTimers[i][PERF_TOTAL] =
                OS_CYCLES_TO_USEC(gPuppyPrint.coreTimers[i][PERF_AGGREGATE]) / NUM_PERF_ITERATIONS;
        }
    }

    gPuppyPrint.coreTimers[PP_OBJECTS][perfIteration] -= gPuppyPrint.coreTimers[PP_RACER][perfIteration];
    gPuppyPrint.coreTimers[PP_OBJECTS][PERF_AGGREGATE] -= gPuppyPrint.coreTimers[PP_RACER][perfIteration];
    gPuppyPrint.coreTimers[PP_LEVELGFX][perfIteration] -= gPuppyPrint.coreTimers[PP_OBJGFX][perfIteration];
    gPuppyPrint.coreTimers[PP_LEVELGFX][PERF_AGGREGATE] -= gPuppyPrint.coreTimers[PP_OBJGFX][perfIteration];
    gPuppyPrint.coreTimers[PP_LEVELGFX][perfIteration] -= gPuppyPrint.coreTimers[PP_PARTICLEGFX][perfIteration];
    gPuppyPrint.coreTimers[PP_LEVELGFX][PERF_AGGREGATE] -= gPuppyPrint.coreTimers[PP_PARTICLEGFX][perfIteration];
}

/// Add whichever times you wish to create aggregates of.
void puppyprint_calculate_average_times(void) {
    s32 i;
    s32 j;
    u32 highTime;
    u32 lowTime;
    u32 flags = interrupts_disable();
    f32 divisor;

    gPuppyPrint.updateTimer += sLogicUpdateRate;
    if (gPuppyPrint.updateTimer >= 4) {
        gPuppyPrint.updateTimer -= 4;
        gPuppyPrint.shouldUpdate = TRUE;
    }

    if (__osBbIsBb) {
        divisor = IQUE_DIVISOR;
    } else {
        divisor = 1.0f;
    }
    
    highTime = 0;
    for (i = 0; i < gPuppyPrint.rspGfxIter; i++) {
        highTime += gPuppyPrint.rspGfx[1][i] - gPuppyPrint.rspGfx[0][i];
    }
    gPuppyPrint.timers[PP_RSP_GFX][PERF_AGGREGATE] -= gPuppyPrint.timers[PP_RSP_GFX][perfIteration];
    gPuppyPrint.timers[PP_RSP_GFX][perfIteration] = MIN(highTime, OS_USEC_TO_CYCLES(99999));
    gPuppyPrint.timers[PP_RSP_GFX][PERF_AGGREGATE] += gPuppyPrint.timers[PP_RSP_GFX][perfIteration];
    gPuppyPrint.rspGfxIter = 0;

    if (gPuppyPrint.shouldUpdate) {
        gPuppyPrint.timers[PP_RSP_AUD][PERF_TOTAL] =
            (OS_CYCLES_TO_USEC(gPuppyPrint.timers[PP_RSP_AUD][PERF_AGGREGATE]) * divisor) / NUM_PERF_ITERATIONS;
        gPuppyPrint.timers[PP_RSP_GFX][PERF_TOTAL] =
            (OS_CYCLES_TO_USEC(gPuppyPrint.timers[PP_RSP_GFX][PERF_AGGREGATE]) * divisor) / NUM_PERF_ITERATIONS;

        for (i = 1; i < PP_RDP_BUS; i++) {
            gPuppyPrint.timers[i][PERF_TOTAL] =
                (OS_CYCLES_TO_USEC(gPuppyPrint.timers[i][PERF_AGGREGATE]) * divisor) / NUM_PERF_ITERATIONS;
        }

        for (i = 1; i < NUM_OBJECT_PRINTS; i++) {
            gPuppyPrint.objTimers[i][PERF_TOTAL] = ((gPuppyPrint.objTimers[i][PERF_AGGREGATE]) * divisor) / NUM_PERF_ITERATIONS;
        }
        gPuppyPrint.timers[PP_RDP_BUF][PERF_TOTAL] =
            ((gPuppyPrint.timers[PP_RDP_BUF][PERF_AGGREGATE] * 10) / (625 * NUM_PERF_ITERATIONS)) * divisor;
        gPuppyPrint.timers[PP_RDP_BUS][PERF_TOTAL] =
            ((gPuppyPrint.timers[PP_RDP_BUS][PERF_AGGREGATE] * 10) / (625 * NUM_PERF_ITERATIONS)) * divisor;
        gPuppyPrint.timers[PP_RDP_TMM][PERF_TOTAL] =
            ((gPuppyPrint.timers[PP_RDP_TMM][PERF_AGGREGATE] * 10) / (625 * NUM_PERF_ITERATIONS)) * divisor;
        gPuppyPrint.timers[PP_RDP_CLK][PERF_TOTAL] =
            ((gPuppyPrint.timers[PP_RDP_CLK][PERF_AGGREGATE] * 10) / (625 * NUM_PERF_ITERATIONS)) * divisor;
        gPuppyPrint.rspTime = gPuppyPrint.timers[PP_RSP_AUD][PERF_TOTAL] + gPuppyPrint.timers[PP_RSP_GFX][PERF_TOTAL];
        gPuppyPrint.rdpTime = gPuppyPrint.timers[PP_RDP_CLK][PERF_TOTAL];
    }
    highTime = 0;
    lowTime = 0xFFFFFFFF;
    // Find the earliest snapshot and the latest snapshot.
    for (i = 0; i < NUM_THREAD_TIMERS; i++) {
        for (j = 0; j < gPuppyPrint.threadIteration[i / 2]; j++) {
            // If an iteration crosses over mid read, the low time could be zero, which would ruin this whole gig.
            if (gPuppyPrint.threadTimes[gPuppyPrint.threadIteration[j]][i] < lowTime &&
                gPuppyPrint.threadTimes[gPuppyPrint.threadIteration[j]][i] != 0) {
                lowTime = gPuppyPrint.threadTimes[gPuppyPrint.threadIteration[j]][i];
            }
            if (gPuppyPrint.threadTimes[gPuppyPrint.threadIteration[j]][i] > highTime) {
                highTime = gPuppyPrint.threadTimes[gPuppyPrint.threadIteration[j]][i];
            }
        }
    }
    calculate_core_timers();
    calculate_individual_thread_timers();
    gPuppyPrint.cpuTime[PERF_AGGREGATE] -= gPuppyPrint.cpuTime[perfIteration];
    gPuppyPrint.cpuTime[perfIteration] = MIN(highTime - lowTime, OS_USEC_TO_CYCLES(99999));
    gPuppyPrint.cpuTime[PERF_AGGREGATE] += gPuppyPrint.cpuTime[perfIteration];
    if (gPuppyPrint.shouldUpdate) {
        gPuppyPrint.cpuTime[PERF_TOTAL] = (OS_CYCLES_TO_USEC(gPuppyPrint.cpuTime[PERF_AGGREGATE]) * divisor) / NUM_PERF_ITERATIONS;
    }
    bzero(&gPuppyPrint.threadIteration, sizeof(gPuppyPrint.threadIteration));
    bzero(&gPuppyPrint.threadTimes, sizeof(gPuppyPrint.threadTimes));
    bzero(&gPuppyPrint.mainTimerPoints, sizeof(gPuppyPrint.mainTimerPoints));
    if (gPuppyPrint.shouldUpdate) {
        if (gPuppyPrint.enabled) {
            if (gPuppyPrint.page == PAGE_BREAKDOWN) {
                calculate_print_order();
            } else if (gPuppyPrint.page == PAGE_OBJECTS) {
                calculate_obj_print_order();
            } else if (gPuppyPrint.page == PAGE_MEMORY) {
                calculate_ram_print_order();
            }
        }
    }
    gPuppyPrint.shouldUpdate = FALSE;
    interrupts_enable(flags);
}

void puppyprint_update_rsp(u8 flags) {
    u32 time = osGetCount();
    switch (flags) {
        case RSP_GFX_START:
            gPuppyPrint.rspGfx[0][gPuppyPrint.rspGfxIter] = time;
            break;
        case RSP_AUDIO_START:
            gPuppyPrint.rspAudioBufTime = time;
            break;
        case RSP_GFX_PAUSED:
            gPuppyPrint.rspGfx[1][gPuppyPrint.rspGfxIter++] = time;
            gPuppyPrint.rspYield++;
            break;
        case RSP_GFX_RESUME:
            gPuppyPrint.rspGfx[0][gPuppyPrint.rspGfxIter] = time;
            break;
        case RSP_GFX_FINISHED:
            gPuppyPrint.rspGfx[1][gPuppyPrint.rspGfxIter++] = time;
            break;
        case RSP_AUDIO_FINISHED:
            gPuppyPrint.timers[PP_RSP_AUD][PERF_AGGREGATE] -= gPuppyPrint.timers[PP_RSP_AUD][perfIteration];
            gPuppyPrint.timers[PP_RSP_AUD][perfIteration] = time - gPuppyPrint.rspAudioBufTime;
            if (gPuppyPrint.timers[PP_RSP_AUD][perfIteration] > OS_USEC_TO_CYCLES(99999)) {
                gPuppyPrint.timers[PP_RSP_AUD][perfIteration] = OS_USEC_TO_CYCLES(99999);
            }
            gPuppyPrint.timers[PP_RSP_AUD][PERF_AGGREGATE] += gPuppyPrint.timers[PP_RSP_AUD][perfIteration];
            break;
    }
}

INLINE void count_triangles_in_dlist(u8 *dlist, u8 *dlistEnd) {
    s32 triCount = 0;
    s32 vtxCount = 0;
    s32 rectCount = 0;
    while (dlist < dlistEnd) {
        switch (dlist[0]) {
            case G_TRIN: // TRIN
                triCount += (dlist[1] >> 4) + 1;
                break;
            case G_VTX:
                vtxCount += (dlist[1] >> 4) + 1;
                break;
            case G_TEXRECT:
            case G_TEXRECTFLIP:
            case G_FILLRECT:
                rectCount++;
                break;
        }
        dlist += sizeof(Gfx);
    }
    sTriCount = triCount;
    sRectCount = rectCount;
    sVtxCount = vtxCount;
}

void count_triangles(u8 *dlist, u8 *dlistEnd) {
    sTimerTemp++;
    if ((sTimerTemp % 8) == 0) {
        count_triangles_in_dlist(dlist, dlistEnd);
    }
}
#endif
