/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800CE210 */

#include "libultra_internal.h"
#include "siint.h"

#if defined(EEP4K) || defined(EEP16K)
s32 __osEepStatus(OSMesgQueue *mq, OSContStatus *data);
extern s32 __osEepromRead16K;

s32 osEepromProbe(OSMesgQueue *mq) {
    s32 ret = 0;
    s32 type;
    OSContStatus sdata;

    if (__osBbIsBb) {
        __osSiGetAccess();
        if (__osBbEepromSize == 0x200) {
            ret = EEPROM_TYPE_4K;
        } else if (__osBbEepromSize == 0x800) {
            ret = EEPROM_TYPE_16K;
        }
        __osSiRelAccess();
        return ret;
    }

    __osSiGetAccess();
    ret = __osEepStatus(mq, &sdata);
    type = sdata.type & (CONT_EEPROM | CONT_EEP16K);

    if (ret != 0) {
        ret = 0;
    } else {
        switch (type) {
            case CONT_EEPROM:
                ret = EEPROM_TYPE_4K;
                break;
            case CONT_EEPROM | CONT_EEP16K:
                ret = EEPROM_TYPE_16K;
                break;
            default:
                ret = 0;
                break;
        }
    }

    __osEepromRead16K = 0;
    __osSiRelAccess();
    return ret;
}
#endif