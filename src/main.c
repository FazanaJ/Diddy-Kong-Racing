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

/************ .bss ************/

u64 gThread3Stack[THREAD3_STACK / sizeof(u64)];
OSThread gThread1; // OSThread for thread 1
OSThread gThread3; // OSThread for thread 3
u8 gPlatformSet = FALSE;
u16 gPlatform = 0;

/******************************/

u32 get_clockspeed(void);
void skGetId(u32 *arg);

void check_cache_emulation() {
    // Disable interrupts to ensure that nothing evicts the variable from cache while we're using it.
    u32 saved = __osDisableInt();
    // Create a variable with an initial value of 1. This value will remain cached.
    volatile u8 sCachedValue = 1;
    // Overwrite the variable directly in RDRAM without going through cache.
    // This should preserve its value of 1 in dcache if dcache is emulated correctly.
    *(u8 *) (K0_TO_K1(&sCachedValue)) = 0;
    // Read the variable back from dcache, if it's still 1 then cache is emulated correctly.
    // If it's zero, then dcache is not emulated correctly.
    if (sCachedValue) {
        gPlatform |= ARES;
    }
    // Restore interrupts
    __osRestoreInt(saved);
}

void get_platform(void) {
    // u32 notiQue;
    gPlatform = 0;

    if (__osBbIsBb) {
        gPlatform = IQUE | CONSOLE;
        puppyprint_log("iQue Player detected.");
        return;
    }

    // Read the RDP timing registers. Emulators read them as zero.
    if ((u32) IO_READ(DPC_PIPEBUSY_REG) | (u32) IO_READ(DPC_TMEM_REG) | (u32) IO_READ(DPC_BUFBUSY_REG)) {
        gPlatform |= CONSOLE;
    } else {
        gPlatform |= EMULATOR;
        // Simple64 and Ares correctly emulate cache on the N64, perform this check to single them out.
        check_cache_emulation();
    }

    // Find the counter factor of an emulator. Console will return 0. This is mainly for Project64, which can default to
    // 2, which can affect performance.
    if (get_clockspeed() > 1) {
        gPlatform |= CF_2;
    }

    if (*((volatile u64 *) 0xb4000008u) == 0x00080008000C000Cull && !(gPlatform & ARES)) {
        gPlatform |= PJ64;
    }

#ifdef PUPPYPRINT_DEBUG
    // Piece together a string to print out.
    if (gPlatform & EMULATOR) {
        if (gPlatform & ARES) {
            puppyprint_log("AresN64/Simple64 Emulator detected.");
        } else if (gPlatform & PJ64) {
            puppyprint_log("Project 64 detected.");
        } else {
            puppyprint_log("N64 Emulator detected.");
        }
    } else {
        puppyprint_log("N64 Console detected.");
    }
    // Skip all this on console. This is emulator related info.
    if ((gPlatform & CONSOLE) == FALSE) {
        u32 cf;
        if (gPlatform & CF_2) {
            cf = 2;
        } else {
            cf = 1;
        }
        puppyprint_log("Counter Factor Setting: %d.", cf);
    }
#endif
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

void find_expansion_pak(void) {
#ifdef FORCE_4MB_MEMORY
    gExpansionPak = FALSE;
    gUseExpansionMemory = FALSE;
    puppyprint_log("4MB Memory Forced.");
    return;
#endif
    if (osGetMemSize() > 0x400000) {
        gExpansionPak = TRUE;
        puppyprint_log("Expansion Pak Detected");
#if EXPANSION_PAK_SUPPORT == 0
        gUseExpansionMemory = FALSE;
#else
        gUseExpansionMemory = TRUE;
#endif
    } else {
        puppyprint_log("Expansion Pak Missing");
        gExpansionPak = FALSE;
        gUseExpansionMemory = FALSE;
    }
}

/******************************/

extern MemoryPoolSlot gMainMemoryPool;
extern void *gBssSectionStart;

/**
 * Where it all begins.
 * Once the boot procedure is finished in IPL3, this function is run.
 * It kicks things off by initialising thread1, which serves as the top level
 * Official name: boot
 */
void main2(void) {
    osInitialize();
    bzero(&gBssSectionStart, RAM_END - (s32) (&gBssSectionStart));
    osCreateThread(&gThread1, 1, &thread1_main, 0, &gThread3Stack[THREAD1_STACK / (sizeof(u64))], OS_PRIORITY_IDLE);
    osStartThread(&gThread1);
}

extern OSSched gMainSched;

#ifdef EXPANSION_PAK_REQUIRED
void draw_memory_error_screen(void) {
    Gfx *dlist;
    init_main_memory_pool();
    func_800C6170();
    func_80076BA0();
    func_80008040(); // Should be very similar to func_8005F850
    func_8007AC70(); // Should be very similar to func_8005F850
    func_800B5E88();
    osCreateScheduler(&gMainSched, 0, /*priority*/ 13, (u8) 0, 1);
    init_video(VIDEO_MODE_LOWRES_LPN);

    dlist = gDisplayLists[0];
    change_vi(&gGlobalVI, SCREEN_WIDTH, SCREEN_HEIGHT);
    osViSetMode(&gGlobalVI);
    load_font(FONT_SMALL);
    set_text_font(FONT_SMALL);
    draw_text(dlist++, SCREEN_WIDTH_HALF, SCREEN_HEIGHT_HALF, "Expansion Pak Required", ALIGN_MIDDLE_CENTER);
    set_rsp_segment(&dlist, 0, 0);
    set_rsp_segment(&dlist, 1, (s32) gVideoCurrFramebuffer);
    set_rsp_segment(&dlist, 2, gVideoLastDepthBuffer);
    set_rsp_segment(&dlist, 4, (s32) gVideoCurrFramebuffer - 0x500);
    init_rsp(&dlist);
    init_rdp_and_framebuffer(&dlist);
    setup_ostask_xbus(gDisplayLists[0], dlist, 0);
    gDPFullSync(dlist++);
    gSPEndDisplayList(dlist++);
    wait_for_gfx_task();
    osViBlack(FALSE);
    osViSwapBuffer(gVideoCurrFramebuffer);
}
#endif

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
    crash_screen_init();
    find_expansion_pak();
#ifdef PUPPYPRINT_DEBUG
    bzero(&gPuppyPrint, sizeof(gPuppyPrint));
#endif
#ifdef EXPANSION_PAK_REQUIRED
    if (!gExpansionPak) {
        draw_memory_error_screen();
    } else {
#endif
        osCreateThread(&gThread3, 3, &thread3_main, 0, &gThread3Stack[THREAD3_STACK / sizeof(u64)], 10);
        gThread3Stack[THREAD3_STACK / sizeof(u64)] = 0;
        gThread3Stack[0] = 0;
        osStartThread(&gThread3);
        osSetThreadPri(OS_PRIORITY_IDLE);
#ifdef EXPANSION_PAK_REQUIRED
    }
#endif
    while (1) {}
}

s32 _Printf(outfun prout, char *dst, const char *fmt, va_list args);

static char *proutSprintf(char *dst, const char *src, size_t count) {
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

u32 gPokeThread[4];
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
u8 sRAMPrintOrder[MEMP_TOTAL + 12];
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

void puppyprint_input(void) {
    // Allow toggling of the profiler.
    if (get_buttons_held_from_player(0) & U_JPAD && get_buttons_pressed_from_player(0) & L_TRIG) {
        gPuppyPrint.enabled ^= 1;
        return; // Sanitisation is healthy.
    }
    if (gPuppyPrint.enabled == FALSE) {
        return;
    }

    // Handle opening and changing page in the menu.
    if (get_buttons_pressed_from_player(0) & L_TRIG) {
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
        if (get_buttons_pressed_from_player(0) & U_JPAD) {
            gPuppyPrint.menuOption--;
            if (gPuppyPrint.menuOption <= -1) {
                gPuppyPrint.menuOption = PAGE_COUNT - 1;
                gPuppyPrint.menuScroll = PAGE_COUNT - 5;
            }
        } else if (get_buttons_pressed_from_player(0) & D_JPAD) {
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
            if (get_buttons_held_from_player(0) & D_JPAD) {
                gPuppyPrint.pageScroll += sLogicUpdateRate * 2;
                if (gPuppyPrint.pageScroll > ((NUM_LOG_LINES) * 10) - maxPrints) {
                    gPuppyPrint.pageScroll = ((NUM_LOG_LINES) * 10) - maxPrints;
                }
            } else if (get_buttons_held_from_player(0) & U_JPAD) {
                gPuppyPrint.pageScroll -= sLogicUpdateRate * 2;
                if (gPuppyPrint.pageScroll < 0) {
                    gPuppyPrint.pageScroll = 0;
                }
            }
        }

        if (gPuppyPrint.page == PAGE_MEMORY) {
            s32 maxPrints = (gScreenHeight - 36);
            if (get_buttons_held_from_player(0) & D_JPAD) {
                gPuppyPrint.pageScroll += sLogicUpdateRate * 2;
                if (gPuppyPrint.pageScroll > (10 * (MEMP_TOTAL + 10)) - maxPrints) {
                    gPuppyPrint.pageScroll = (10 * (MEMP_TOTAL + 10)) - maxPrints;
                }
            } else if (get_buttons_held_from_player(0) & U_JPAD) {
                gPuppyPrint.pageScroll -= sLogicUpdateRate * 2;
                if (gPuppyPrint.pageScroll < 0) {
                    gPuppyPrint.pageScroll = 0;
                }
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
    u32 flags = disable_interrupts();
    for (i = 0; i < PP_RDP_BUS; i++) {
        gPuppyPrint.timers[i][PERF_AGGREGATE] -= gPuppyPrint.timers[i][perfIteration];
        gPuppyPrint.timers[i][perfIteration] = 0;
    }
    profiler_reset_objects();
    gPuppyPrint.textureLoads = 0;
    puppyprint_input();
    enable_interrupts(flags);
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
    if (snapshot > NUM_THREAD_ITERATIONS - 1) {
        snapshot = NUM_THREAD_ITERATIONS - 1;
    }
    gPuppyPrint.threadTimes[snapshot][eventID] = osGetCount();
    // Thread endings are even numbers.
    if (eventID % 2) {
        gPuppyPrint.threadIteration[eventID / 2]++;
    }
}

#define TEXT_OFFSET 10

void draw_blank_box(s32 x1, s32 y1, s32 x2, s32 y2, u32 colour) {
    gDPSetPrimColor(gCurrDisplayList++, 0, 0, (colour << 24) & 0xFF, (colour << 16) & 0xFF, (colour << 8) & 0xFF,
                    (colour) & 0xFF);
    gDPPipeSync(gCurrDisplayList++);
    gDPSetCombineMode(gCurrDisplayList++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
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
        gDPSetRenderMode(gCurrDisplayList++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    } else {
        gDPSetRenderMode(gCurrDisplayList++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    }
    gDPSetCycleType(gCurrDisplayList++, G_CYC_1CYCLE);
    gDPFillRectangle(gCurrDisplayList++, x1, y1, x2, y2);
}

void puppyprint_render_minimal(void) {
    char textBytes[16];
    draw_blank_box(TEXT_OFFSET - 2, 8, 112, 50 + 2, 0x0000007F);
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

#ifdef EXPANSION_PAK_REQUIRED
#define TOTALRAM 0x800000
#elif EXPANSION_PAK_SUPPORT == 1
#define TOTALRAM osGetMemSize()
#else
#define TOTALRAM 0x400000
#endif

void puppyprint_render_overview(void) {
    char textBytes[32];
    s32 i;
    s32 y;
    puppyprint_render_minimal();
    // Draw triangle, vertex and overall RAM on bottom left.
    draw_blank_box(((gScreenWidth / 2) / 3) - 42, gScreenHeight - 50, ((gScreenWidth / 2) / 3) + 62, gScreenHeight - 6,
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
    draw_blank_box(gScreenWidth - 124, 8, gScreenWidth - 8, 74, 0x0000007F);
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

void puppyprint_render_breakdown(void) {
    char textBytes[32];
    s32 y;
    s32 i;

    puppyprint_render_minimal();
    y = 8;
    draw_blank_box(gScreenWidth - 144, 0, gScreenWidth, gScreenHeight, 0x00000064);
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

void puppyprint_render_rcp(void) {
    char textBytes[32];
    int y = 8;

    puppyprint_render_minimal();

    
    draw_blank_box(gScreenWidth - 100, 0, gScreenWidth, gScreenHeight, 0x00000064);
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
    puppyprintf(textBytes, "%d", gPuppyPrint.timers[PP_RSP_YLD][PERF_TOTAL]);
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

void puppyprint_render_memory(void) {
    char textBytes[24];
    s32 y;
    u32 i;

    y = 36 - gPuppyPrint.pageScroll;
    draw_blank_box(gScreenWidth - 160, 0, gScreenWidth, gScreenHeight, 0x00000064);
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
    for (i = 1; i < MEMP_TOTAL + 12; i++) {
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

void puppyprint_render_objects(void) {
    char textBytes[48];
    s32 y;
    s32 i;

    puppyprint_render_minimal();
    y = 8;
    draw_blank_box(gScreenWidth - 144, 0, gScreenWidth, gScreenHeight, 0x00000064);
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

void puppyprint_render_log(void) {
    s32 i;
    s32 y;
    s32 sineTime = 224 + (sinf(sTimerTemp * 25.0f) * 32.0f);
    s32 firstDraw = TRUE;
    draw_blank_box(0, 0, gScreenWidth, gScreenHeight, 0x00000064);
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

void render_page_menu(void) {
    char textBytes[16];
    s32 i;
    s32 y;
    s32 sineTime = 192 + (sinf(sTimerTemp * 50.0f) * 64.0f);
    draw_blank_box(TEXT_OFFSET - 2, 56, 112, 108 + 2, 0x0000007F);
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
        case PAGE_COVERAGE:
            break;
    }

    if (gPuppyPrint.menuOpen) {
        render_page_menu();
    }
}

void puppyprint_log(const char *str, ...) {
    s32 i;
    char textBytes[127];
    va_list arguments;

    bzero(textBytes, sizeof(textBytes));
    va_start(arguments, str);
    if ((_Printf(proutSprintf, textBytes, str, arguments)) <= 0) {
        va_end(arguments);
        return;
    }
#ifdef UNF
    osSyncPrintf(textBytes);
#endif
    for (i = 0; i < (NUM_LOG_LINES - 1); i++) {
        bcopy(gPuppyPrint.logText[i + 1], gPuppyPrint.logText[i], 127);
    }
    bcopy(textBytes, gPuppyPrint.logText[NUM_LOG_LINES - 1], 127);
    va_end(arguments);
}

void swapu(u8 *xp, u8 *yp) {
    u8 temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void swapu16(u16 *xp, u16 *yp) {
    u16 temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void calculate_print_order(void) {
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
    for (i = 1; i < MEMP_TOTAL + 12; i++) {
        sRAMPrintOrder[i] = i;
    }
    gPuppyPrint.ramPools[MEMP_CODE] = (u32) &gMainMemoryPool - 0x80000000;

    // One by one move boundary of unsorted subarray
    for (i = 1; i < MEMP_TOTAL + 12; i++) {

        if (gPuppyPrint.ramPools[sRAMPrintOrder[i]] == 0) {
            continue;
        }
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < MEMP_TOTAL + 12; j++) {
            if (gPuppyPrint.ramPools[sRAMPrintOrder[j]] > gPuppyPrint.ramPools[sRAMPrintOrder[min_idx]]) {
                min_idx = j;
            }
        }

        // Swap the found minimum element
        // with the first element
        swapu(&sRAMPrintOrder[min_idx], &sRAMPrintOrder[i]);
    }
}

void calculate_obj_print_order(void) {
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

void calculate_individual_thread_timers(void) {
    s32 i;
    s32 j;
    u32 highTime;
    u32 lowTime;
    u32 normalTime;
    u32 offsetTime;
    f32 divisor;

    if (__osBbIsBb) {
        divisor = IQUE_DIVISOR;
    } else {
        divisor = 1.0f;
    }

    // Audio thread is basically top prio, so no need to do any further shenanigans
    highTime = 0;
    lowTime = 0xFFFFFFFF;
    for (i = THREAD3_START; i <= THREAD3_END; i++) {
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
    gPuppyPrint.audTime[PERF_AGGREGATE] -= gPuppyPrint.audTime[perfIteration];
    gPuppyPrint.audTime[perfIteration] = MIN(highTime - lowTime, (s32) (OS_USEC_TO_CYCLES(99999) * divisor));
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
    gPuppyPrint.gameTime[perfIteration] = MIN(highTime - lowTime - offsetTime, (s32) (OS_USEC_TO_CYCLES(99999) * divisor));
    gPuppyPrint.gameTime[PERF_AGGREGATE] += gPuppyPrint.gameTime[perfIteration];

    gPuppyPrint.gameYield[PERF_AGGREGATE] -= gPuppyPrint.gameYield[perfIteration];
    gPuppyPrint.gameYield[perfIteration] = MIN(offsetTime, (s32) (OS_USEC_TO_CYCLES(99999) * divisor));
    gPuppyPrint.gameYield[PERF_AGGREGATE] += gPuppyPrint.gameYield[perfIteration];
    if (gPuppyPrint.shouldUpdate) {
        gPuppyPrint.audTime[PERF_TOTAL] = OS_CYCLES_TO_USEC(gPuppyPrint.audTime[PERF_AGGREGATE]) / NUM_PERF_ITERATIONS;
        gPuppyPrint.gameTime[PERF_TOTAL] =
            OS_CYCLES_TO_USEC(gPuppyPrint.gameTime[PERF_AGGREGATE]) / NUM_PERF_ITERATIONS;
        gPuppyPrint.gameYield[PERF_TOTAL] =
            OS_CYCLES_TO_USEC(gPuppyPrint.gameYield[PERF_AGGREGATE]) / NUM_PERF_ITERATIONS;
    }
}

void calculate_core_timers(void) {
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
    u32 highTime = 0;
    u32 lowTime = 0xFFFFFFFF;
    u32 flags = disable_interrupts();
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

    if (gPuppyPrint.shouldUpdate) {
        gPuppyPrint.timers[PP_RSP_AUD][PERF_TOTAL] =
            (OS_CYCLES_TO_USEC(gPuppyPrint.timers[PP_RSP_AUD][PERF_AGGREGATE]) * divisor) / NUM_PERF_ITERATIONS;
        gPuppyPrint.timers[PP_RSP_GFX][PERF_TOTAL] =
            (OS_CYCLES_TO_USEC(gPuppyPrint.timers[PP_RSP_GFX][PERF_AGGREGATE]) * divisor) / NUM_PERF_ITERATIONS;
        gPuppyPrint.timers[PP_RSP_YLD][PERF_TOTAL] =
            (OS_CYCLES_TO_USEC(gPuppyPrint.timers[PP_RSP_YLD][PERF_AGGREGATE]) * divisor) / NUM_PERF_ITERATIONS;

        for (i = 1; i < PP_RDP_BUS; i++) {
            gPuppyPrint.timers[i][PERF_TOTAL] =
                (OS_CYCLES_TO_USEC(gPuppyPrint.timers[i][PERF_AGGREGATE]) * divisor) / NUM_PERF_ITERATIONS;
        }

        for (i = 1; i < NUM_OBJECT_PRINTS; i++) {
            gPuppyPrint.objTimers[i][PERF_TOTAL] = ((gPuppyPrint.objTimers[i][PERF_AGGREGATE]) * divisor) / NUM_PERF_ITERATIONS;
        }
        gPuppyPrint.timers[PP_RDP_BUF][PERF_TOTAL] =
            (gPuppyPrint.timers[PP_RDP_BUF][PERF_AGGREGATE] * 10) / ((625 * NUM_PERF_ITERATIONS) / divisor);
        gPuppyPrint.timers[PP_RDP_BUS][PERF_TOTAL] =
            (gPuppyPrint.timers[PP_RDP_BUS][PERF_AGGREGATE] * 10) / ((625 * NUM_PERF_ITERATIONS) / divisor);
        gPuppyPrint.timers[PP_RDP_TMM][PERF_TOTAL] =
            (gPuppyPrint.timers[PP_RDP_TMM][PERF_AGGREGATE] * 10) / ((625 * NUM_PERF_ITERATIONS) / divisor);
        gPuppyPrint.timers[PP_RDP_CLK][PERF_TOTAL] =
            (gPuppyPrint.timers[PP_RDP_CLK][PERF_AGGREGATE] * 10) / ((625 * NUM_PERF_ITERATIONS) / divisor);
        gPuppyPrint.rspTime = gPuppyPrint.timers[PP_RSP_AUD][PERF_TOTAL] + gPuppyPrint.timers[PP_RSP_GFX][PERF_TOTAL];
        gPuppyPrint.rdpTime = gPuppyPrint.timers[PP_RDP_CLK][PERF_TOTAL];
    }
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
    enable_interrupts(flags);
}

void puppyprint_update_rsp(u8 flags) {
    switch (flags) {
        case RSP_GFX_START:
            gPuppyPrint.rspGfxBufTime = (u32) osGetCount();
            gPuppyPrint.rspPauseTime = 0;
            gPuppyPrint.timers[PP_RSP_YLD][PERF_AGGREGATE] -= gPuppyPrint.timers[PP_RSP_YLD][perfIteration];
            gPuppyPrint.timers[PP_RSP_YLD][PERF_AGGREGATE] = 0;
            break;
        case RSP_AUDIO_START:
            gPuppyPrint.rspAudioBufTime = (u32) osGetCount();
            break;
        case RSP_GFX_PAUSED:
            gPuppyPrint.rspPauseTime = (u32) osGetCount();
            break;
        case RSP_GFX_RESUME:
            gPuppyPrint.rspPauseTime = (u32) osGetCount() - gPuppyPrint.rspPauseTime;
            gPuppyPrint.timers[PP_RSP_YLD][perfIteration] += gPuppyPrint.rspPauseTime;
            if (gPuppyPrint.timers[PP_RSP_YLD][perfIteration] > OS_USEC_TO_CYCLES(99999)) {
                gPuppyPrint.timers[PP_RSP_YLD][perfIteration] = OS_USEC_TO_CYCLES(99999);
            }
            gPuppyPrint.timers[PP_RSP_YLD][PERF_AGGREGATE] += gPuppyPrint.timers[PP_RSP_YLD][perfIteration];
            break;
        case RSP_GFX_FINISHED:
            gPuppyPrint.timers[PP_RSP_GFX][PERF_AGGREGATE] -= gPuppyPrint.timers[PP_RSP_GFX][perfIteration];
            gPuppyPrint.timers[PP_RSP_GFX][perfIteration] =
                (u32) (osGetCount() - gPuppyPrint.rspGfxBufTime) - gPuppyPrint.rspPauseTime;
            if (gPuppyPrint.timers[PP_RSP_GFX][perfIteration] > OS_USEC_TO_CYCLES(99999)) {
                gPuppyPrint.timers[PP_RSP_GFX][perfIteration] = OS_USEC_TO_CYCLES(99999);
            }
            gPuppyPrint.timers[PP_RSP_GFX][PERF_AGGREGATE] += gPuppyPrint.timers[PP_RSP_GFX][perfIteration];
            break;
        case RSP_AUDIO_FINISHED:
            gPuppyPrint.timers[PP_RSP_AUD][PERF_AGGREGATE] -= gPuppyPrint.timers[PP_RSP_AUD][perfIteration];
            gPuppyPrint.timers[PP_RSP_AUD][perfIteration] = (u32) osGetCount() - gPuppyPrint.rspAudioBufTime;
            if (gPuppyPrint.timers[PP_RSP_AUD][perfIteration] > OS_USEC_TO_CYCLES(99999)) {
                gPuppyPrint.timers[PP_RSP_AUD][perfIteration] = OS_USEC_TO_CYCLES(99999);
            }
            gPuppyPrint.timers[PP_RSP_AUD][PERF_AGGREGATE] += gPuppyPrint.timers[PP_RSP_AUD][perfIteration];
            break;
    }
}

void count_triangles_in_dlist(u8 *dlist, u8 *dlistEnd) {
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
