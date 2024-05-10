/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800D6600 */

#include "macros.h"
#include "libultra_internal.h"
#include "osint.h"

void osDestroyThread(void) {
    register u32 saveMask;
    register OSThread *pred;
    register OSThread *succ;

    saveMask = __osDisableInt();


    if (__osActiveQueue == __osRunningThread) {
        __osActiveQueue = __osActiveQueue->tlnext;
    } else {
        pred = __osActiveQueue;
        while (pred->priority != -1) {
            succ = pred->tlnext;
            if (succ == __osRunningThread) {
                pred->tlnext = __osRunningThread->tlnext;
                break;
            }
            pred = succ;
        }
    }

        __osDispatchThread();

    __osRestoreInt(saveMask);
}

