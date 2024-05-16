#ifndef _ULTRA64_OS_INTERNAL_H_
#define _ULTRA64_OS_INTERNAL_H_

/* Internal functions used by the operating system */
/* Do not include this header in application code */

#include "os_thread.h"

/*
 * Leo Disk
 */

/* transfer mode */

#define LEO_BLOCK_MODE	1
#define LEO_TRACK_MODE	2
#define LEO_SECTOR_MODE	3

/* Variables */
/*
 * CPU counter increments at 3/4 of bus clock rate:
 *
 * Bus Clock	Proc Clock	Counter (1/2 Proc Clock)
 * ---------	----------	------------------------
 * 62.5 Mhz	93.75 Mhz	46.875 Mhz
 */
extern u64 osClockRate;

#define	OS_CLOCK_RATE		62500000
#define	OS_CPU_COUNTER		(OS_CLOCK_RATE*3/4)
#define OS_NSEC_TO_CYCLES(n)	(((u32)(n)*(OS_CPU_COUNTER/15625000))/(1000000000/15625000))
#define OS_USEC_TO_CYCLES(n)	(((u32)(n)*(OS_CPU_COUNTER/15625))/(1000000/15625))
#define OS_CYCLES_TO_NSEC(c)	(((u32)(c)*(1000000000/15625000))/(OS_CPU_COUNTER/15625000))
#define OS_CYCLES_TO_USEC(c)	(((u32)(c)*(1000000/15625))/(OS_CPU_COUNTER/15625))

/* Functions */

u32 __osProbeTLB(void *);
u32 __osDisableInt(void);
void __osRestoreInt(u32);
OSThread *__osGetCurrFaultedThread(void);

/* Address translation routines and macros */

extern u32         osVirtualToPhysical(void *);
extern void *         osPhysicalToVirtual(u32);

#define    OS_K0_TO_PHYSICAL(x)    (u32)(((char *)(x)-0x80000000))
#define    OS_K1_TO_PHYSICAL(x)    (u32)(((char *)(x)-0xa0000000))

#define    OS_PHYSICAL_TO_K0(x)    (void *)(((u32)(x)+0x80000000))
#define    OS_PHYSICAL_TO_K1(x)    (void *)(((u32)(x)+0xa0000000))

extern u32 __osBbEepromAddress;
extern u32 __osBbEepromSize;
extern u32 __osBbSramAddress;
extern u32 __osBbSramSize;
extern u32 __osBbFlashAddress;
extern u32 __osBbFlashSize;
extern u32 __osBbPakAddress[4];
extern u32 __osBbPakSize;
extern u32 __osBbIsBb;
extern u32 __osBbHackFlags;
extern u32 osMemSize;

#endif
