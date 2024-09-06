/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800D4DE0 */

#include "libultra_internal.h"

extern OSTimer *__osTimerList;
OSTime __osInsertTimer(OSTimer *t);
void __osSetTimerIntr(OSTime tim);

s32 osSetTimer(OSTimer *t, OSTime value, OSTime interval, OSMesgQueue *mq, OSMesg msg) {
    OSTime time;
    t->next = NULL;
    t->prev = NULL;
    t->interval = interval;

    if(value != 0)
        t->remaining = value;
    else
        t->remaining = interval;
    t->mq = mq;
    t->msg = msg;
    time = __osInsertTimer(t);

    if(__osTimerList->next == t)
        __osSetTimerIntr(time);

    return 0;
}

s32 osStopTimer(OSTimer *t) {
	register u32 savedMask;
	OSTimer *timep;

	if (t->next == NULL) {
		return -1;
	}

	savedMask = __osDisableInt();

	timep = t->next;

	if (timep != __osTimerList) {
		timep->remaining += t->remaining;
	}

	t->prev->next = t->next;
	t->next->prev = t->prev;
	t->next = NULL;
	t->prev = NULL;

	if (__osTimerList->next == __osTimerList) {
		__osSetCompare(0);
	}

	__osRestoreInt(savedMask);

	return 0;
}