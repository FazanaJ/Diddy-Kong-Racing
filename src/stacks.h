#ifndef _STACKS_H_
#define _STACKS_H_

#include "types.h"

#define STACK_CRASH     0x400
#define STACK_CRASH2    0x200
#define STACK_SCHED     0x200
#define STACK_AUD       0x1000
#define STACK_GAME      0x2000
#define STACK_BGLOAD    0x3000
#define STACK_USB       0x200

#define STACKSIZE(x) (x / sizeof(u64))

#endif
