#include "main.h"
#include "thread0_epc.h"
#include "game.h"
#include "libultra/src/libc/rmonPrintf.h"
#include "stacks.h"

/************ .bss ************/

u64 gThread1Stack[STACKSIZE(STACK_IDLE)];
u64 gThread3Stack[STACKSIZE(STACK_GAME)];
OSThread gThread1; // OSThread for thread 1
OSThread gThread3; // OSThread for thread 3
u8 gExpansionPak;
u8 gUseExpansionMemory;
UserConfig gConfig;

/******************************/

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
    gConfig.antiAliasing = -1;
    gConfig.terrainQuality = 0;
    gConfig.dedither = FALSE;
    gConfig.frameCap = 1;
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
#ifdef AVOID_UB
    bzero(&gMainMemoryPool, RAM_END - (s32) (&gMainMemoryPool));
#endif
    memsize_init();
    osCreateThread(&gThread1, 1, &thread1_main, 0, &gThread1Stack[STACKSIZE(STACK_IDLE)], OS_PRIORITY_IDLE);
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
    gThread3Stack[STACKSIZE(STACK_GAME) - 1]++;
    gThread3Stack[0]++;
    if ((gThread3Stack[STACKSIZE(STACK_GAME) - 1] != gThread3Stack[0])) {
        rmonPrintf("WARNING: Stack overflow/underflow!!!\n");
    }
}
