#ifndef _STACKS_H_
#define _STACKS_H_

#include "types.h"

#define STACK_IDLE      0x48
#define STACK_SCHED     0x200
#define STACK_AUD       0x1000
#define STACK_GAME      0x2000
#define STACK_BGLOAD    0x3000
#define STACK_USB       0x200
#define STACK_CRASH     0x400

#define STACKSIZE(x) (x / sizeof(u64))

extern u8 D_80129AB0[0x100];
extern u16 D_80129BB0[512];
extern s32 gObjectStackTrace[3];

#endif
