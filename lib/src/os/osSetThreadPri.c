/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800CC840 */

#include "libultra_internal.h"
#include "osint.h"

extern OSThread *__osRunningThread;
extern OSThread *__osRunQueue;

void osSetThreadPri(OSPri pri) {
    register u32 saveMask;

    saveMask = __osDisableInt();

    if (__osRunningThread->priority != pri) {
        __osRunningThread->priority = pri;


        if (__osRunningThread->priority < __osRunQueue->priority) {
            __osRunningThread->state = OS_STATE_RUNNABLE;
            __osEnqueueAndYield(&__osRunQueue);
        }
    }

    __osRestoreInt(saveMask);
}
