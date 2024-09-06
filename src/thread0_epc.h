#ifndef _THREAD0_EPC_H_
#define _THREAD0_EPC_H_

#include "types.h"
#include "libultra_internal.h"
#include "macros.h"
#include "config.h"
#include "structs.h"

enum ObjectActions {
    CRASH_OBJ_NONE,
    CRASH_OBJ_SPAWN,
    CRASH_OBJ_UPDATE,
    CRASH_OBJ_RENDER,
};

enum CrashPages {
    CRASH_PAGE_REGISTERS,
    CRASH_PAGE_OBJECT,
    CRASH_PAGE_LOG,
    CRASH_PAGE_ASSERT,
    CRASH_PAGE_MEMORY,
    CRASH_PAGE_NONE,
    CRASH_PAGE_COUNT
};

typedef struct {
    OSThread thread;
    u64 stack[THREAD2_STACK / sizeof(u64)];
    OSMesgQueue mesgQueue;
    OSMesg mesg;
    u16 *framebuffer;
    u16 width;
    u16 height;
} CrashData;

extern CrashData gCrashScreen;

#ifdef DETAILED_CRASH
void set_crash_object(Object *objectID, s32 act);
#else
#define set_crash_object(objectID, act)
#endif

#endif
