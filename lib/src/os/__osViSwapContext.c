/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800D6090 */

#include "macros.h"
#include "libultra_internal.h"
#include "viint.h"

//TODO: These variable declarations should really be elsewhere.
struct __osThreadTail
{
    OSThread *next;
    OSPri priority;
} __osThreadTail;

struct __osThreadTail __osThreadTail = {0, -1};
OSThread *__osRunQueue = (OSThread *)&__osThreadTail;
OSThread *__osActiveQueue = (OSThread *)&__osThreadTail;

OSThread *__osRunningThread = NULL;
OSThread *__osFaultedThread = NULL;

void __osViSwapContext() {
    register OSViMode *vm;
    register __OSViContext *vc;
    u32 origin;
    u32 hStart;
    u32 nomValue;
    u32 field;

    field = 0;
    vc = __osViNext;
    vm = vc->modep;

    field = IO_READ(VI_CURRENT_REG) & 1; //field num

    origin = osVirtualToPhysical(vc->framep) + (vm->fldRegs[field].origin);
    
	if (vc->state & VI_STATE_XSCALE_UPDATED) {
        vc->x.scale |= (vm->comRegs.xScale & ~VI_SCALE_MASK);
    } else {
        vc->x.scale = vm->comRegs.xScale;
    }
    
	if (vc->state & VI_STATE_YSCALE_UPDATED) {
        nomValue = vm->fldRegs[field].yScale & VI_SCALE_MASK;
        vc->y.scale = vc->y.factor * nomValue;
        vc->y.scale |= vm->fldRegs[field].yScale & ~VI_SCALE_MASK;
    } else {
        vc->y.scale = vm->fldRegs[field].yScale;
    }

    hStart = vm->comRegs.hStart;

    if (vc->state & VI_STATE_BLACK) {
        hStart = 0;
    }
    
	if (vc->state & VI_STATE_REPEATLINE) {
        vc->y.scale = 0;
        origin = osVirtualToPhysical(vc->framep);
    }
    
	if (vc->state & VI_STATE_FADE) {
        vc->y.scale = (vc->y.offset << VI_SUBPIXEL_SH) & (VI_2_10_FPART_MASK << VI_SUBPIXEL_SH);
        origin = osVirtualToPhysical(vc->framep);
    }

    IO_WRITE(VI_ORIGIN_REG, origin);
    IO_WRITE(VI_WIDTH_REG, vm->comRegs.width);
    IO_WRITE(VI_BURST_REG, vm->comRegs.burst);
    IO_WRITE(VI_V_SYNC_REG, vm->comRegs.vSync);
    IO_WRITE(VI_H_SYNC_REG, vm->comRegs.hSync);
    IO_WRITE(VI_LEAP_REG, vm->comRegs.leap);
    IO_WRITE(VI_H_START_REG, hStart);
    IO_WRITE(VI_V_START_REG, vm->fldRegs[field].vStart);
    IO_WRITE(VI_V_BURST_REG, vm->fldRegs[field].vBurst);
    IO_WRITE(VI_INTR_REG, vm->fldRegs[field].vIntr);
    IO_WRITE(VI_X_SCALE_REG, vc->x.scale);
    IO_WRITE(VI_Y_SCALE_REG, vc->y.scale);
    IO_WRITE(VI_CONTROL_REG, vc->control);

    __osViNext = __osViCurr;
    __osViCurr = vc;
    //*__osViNext = *__osViCurr;
    wcopy(__osViCurr, __osViNext, sizeof(__OSViContext));
}
