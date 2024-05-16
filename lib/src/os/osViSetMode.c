/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800D1CA0 */

#include "libultra_internal.h"
#include "viint.h"

void osViSetMode(OSViMode *modep) {
    register u32 saveMask;
    saveMask = __osDisableInt();
    if (__osBbIsBb) {
        modep->comRegs.ctrl &= ~0x2000;
    }
    __osViNext->modep = modep;
    __osViNext->state = VI_STATE_01;
    __osViNext->control = __osViNext->modep->comRegs.ctrl;
    __osRestoreInt(saveMask);
}
