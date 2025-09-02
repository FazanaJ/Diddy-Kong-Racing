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
#include "overlay.h"

/************ .bss ************/

u64 *gThread3Stack;
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
    bzero(&main_BSS_START, RAM_END - (s32) (&main_BSS_START));
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
    overlay_init();
    video_alloc();
    crash_init();
    config_init();
    osCreateThread(&gThread3, 3, &thread3_main, 0, gThread3Stack + STACKSIZE(STACK_GAME), 10);
    gThread3Stack[STACKSIZE(STACK_GAME) - 1] = 0;
    gThread3Stack[0] = 0;
    osStartThread(&gThread3);
    while (1) {}
}
