#include "thread30_bgload.h"
#include "macros.h"
#include <ultra64.h>
#include "game.h"
#include "stacks.h"
#include "main.h"

/************ .data ************/

s32 gThread30NeedToLoadLevel = FALSE;
s32 gThread30LevelIdToLoad = -1;
s32 gThread30CutsceneIdToLoad = -1;
s32 gThread30LoadDelay = 0;

/*******************************/

/************ .bss ************/

OSThread *gThread30;
OSMesgQueue gThread30MesgQueue;
OSMesg gThread30Message[2];
u64 *gThread30Stack;

/*****************************/

/**
 * Initalizes and starts thread30()
 */
void bgload_init(void) {
    osCreateMesgQueue(&gThread30MesgQueue, gThread30Message, ARRAY_COUNT(gThread30Message));
}

/**
 * Stop thread30()
 * Official Name: amStop
 */
void bgload_kill(void) {
    osStopThread(gThread30);
    osDestroyThread(gThread30);
    mempool_free(gThread30Stack);
    mempool_free(gThread30);
    gThread30Stack = NULL;
}

/**
 * Returns the value in gThread30NeedToLoadLevel.
 */
s32 bgload_active(void) {
    return gThread30NeedToLoadLevel;
}

/**
 * If gThread30NeedToLoadLevel is set, then decrement gThread30LoadDelay. If gThread30LoadDelay is 0,
 * then signal thread30 to load a level.
 */
void bgload_tick(void) {
    if (gThread30NeedToLoadLevel && gThread30LoadDelay > 0) {
        gThread30LoadDelay--;
        if (gThread30LoadDelay == 0) {
            // Signal thread30 that the level needs to load.
            osSendMesg(&gThread30MesgQueue, (OSMesg *) OS_MESG_TYPE_LOOPBACK, OS_MESG_NOBLOCK);
        }
    }

    // Kill the thread and everything involved if not in use.
    if (gThread30NeedToLoadLevel == FALSE && gThread30Stack != NULL) {
        bgload_kill();
    }
}

/**
 * Sets a level id and cutscene id to be loaded. Used in the tracks menu & credits menu.
 * Returns 1 if successful, or 0 if gThread30NeedToLoadLevel was already set.
 */
s32 bgload_start(s32 levelId, s32 cutsceneId) {
    if (!gThread30NeedToLoadLevel) {
        gThread30LoadDelay = 4;
        gThread30LevelIdToLoad = levelId;
        gThread30CutsceneIdToLoad = cutsceneId;
        gThread30NeedToLoadLevel = TRUE;
        gThread30Stack = mempool_alloc_safe(STACK_BGLOAD, PP_RAM_STACK);
        gThread30 = mempool_alloc_safe(sizeof(OSThread), PP_RAM_STACK);
        osCreateThread(gThread30, 30, &thread30_bgload, NULL, gThread30Stack + STACKSIZE(STACK_BGLOAD), 8);
        gThread30Stack[STACKSIZE(STACK_BGLOAD) - 1] = 0;
        gThread30Stack[0] = 0;
        osStartThread(gThread30);
        return TRUE;
    }
    return FALSE;
}

/**
 * Waits for a signal from the main thread to load a level asynchronously.
 */
void thread30_bgload(UNUSED void *arg) {
    OSMesg mesg = 0;
    while (TRUE) {
        // Wait for a signal from the main thread
        osRecvMesg(&gThread30MesgQueue, &mesg, OS_MESG_BLOCK);
        if (mesg != (OSMesg) OS_MESG_TYPE_LOOPBACK) {
            continue;
        }
        // -1 means there won't be any racers loaded.
        load_level_for_menu(gThread30LevelIdToLoad, -1, gThread30CutsceneIdToLoad);
        gThread30NeedToLoadLevel = FALSE;
    }
}
