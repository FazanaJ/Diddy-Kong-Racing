/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800C8BB0 */

#include "types.h"
#include "macros.h"
#include "libultra_internal.h"

extern OSThread *__osRunningThread;

//GLOBAL_ASM("lib/asm/non_matchings/unknown_0C96F0/osRecvMesg.s")
s32 osRecvMesg(OSMesgQueue *mq, OSMesg *msg, s32 flags)
{
    register u32 saveMask;
    saveMask = __osDisableInt();

    while (MQ_IS_EMPTY(mq))
    {
        if (flags == OS_MESG_NOBLOCK)
        {
            __osRestoreInt(saveMask);
            return -1;
        }
        __osRunningThread->state = OS_STATE_WAITING;
        __osEnqueueAndYield(&mq->mtqueue);
    }

    if (msg != NULL)
    {
        *msg = mq->msg[mq->first];
    }
    mq->first = (mq->first + 1) % mq->msgCount;
    mq->validCount--;
    if (mq->fullqueue->next != NULL)
    {
        osStartThread(__osPopThread(&mq->fullqueue));
    }
    __osRestoreInt(saveMask);
    return 0;
}