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

#ifdef PUPPYPRINT_DEBUG
void set_crash_object(ObjectHeader *objectID, s32 act);
#else
#define set_crash_object(objectID, act)
#endif

#endif
