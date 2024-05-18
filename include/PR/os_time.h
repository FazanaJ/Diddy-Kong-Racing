#ifndef _ULTRA64_TIME_H_
#define _ULTRA64_TIME_H_

#include <PR/ultratypes.h>
#include <PR/os_message.h>

/* Types */

typedef u64 OSTime;

typedef struct OSTimer_s {
    struct OSTimer_s *next;
    struct OSTimer_s *prev;
    OSTime interval;
    OSTime remaining;
    OSMesgQueue *mq;
    OSMesg *msg;
} OSTimer;

/* Functions */

extern OSTime __osCurrentTime;

static inline void osSetTime(OSTime time) {
    __osCurrentTime = time;
}

OSTime osGetTime(void);
s32 osSetTimer(OSTimer *t, OSTime value, OSTime interval, OSMesgQueue *mq, OSMesg msg);

#endif
