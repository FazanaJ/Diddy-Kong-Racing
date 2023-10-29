/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800D2260 */

#include "libultra_internal.h"
#include "viint.h"

void osViSetSpecialFeatures(u32 set, u32 unset) {    
    register u32 saveMask;
    saveMask = __osDisableInt();

    __osViNext->control |= set;
    __osViNext->control &= ~unset;

    if (set & VI_CTRL_DITHER_FILTER_ON) {
        __osViNext->control &= ~VI_CTRL_ANTIALIAS_MASK;
    } else if (unset & VI_CTRL_DITHER_FILTER_ON) {
        __osViNext->control |= __osViNext->modep->comRegs.ctrl & VI_CTRL_ANTIALIAS_MASK;
    }
    __osViNext->state |= VI_STATE_08;

    __osRestoreInt(saveMask);
}
