/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800CE280 */

#include "libultra_internal.h"
#include "PR/rcp.h"
#include "controller.h"
#include "siint.h"

#if defined(EEP4K) || defined(EEP16K)
extern u8 __osContLastCmd;
extern OSPifRam __osEepPifRam;
s32 __osEepromRead16K;
void __osPackEepReadData(s32 address);
s32 __osEepStatus(OSMesgQueue *mq, OSContStatus *data);
s32 __osSiRawStartDma(s32, void *);

s32 osEepromRead(OSMesgQueue *mq, s32 address, u8 *buffer) {
    s32 ret = 0;
    s32 i;
    s32 type;
    u8 *ptr;
    OSContStatus sdata;
    __OSContEepromFormat eepromformat;

    if (__osBbIsBb) {

        if (__osBbEepromSize == 0x200) {
            if (address >= 0x200 / sizeof(u64)) {
                ret = -1;
            }
        } else if (__osBbEepromSize != 0x800) {
            ret = 8;
            if (address >= 0x800 / sizeof(u64)) {
                ret = -1;
            }
        }

        if (ret == 0) {
            int i;

            __osSiGetAccess();
            for (i = 0; i < 8; i++) {
                buffer[i] = *(u8*)(__osBbEepromAddress + (address * 8) + i);
            }
            __osSiRelAccess();
        }

        return ret;
    }

    ptr = (u8*)&__osEepPifRam.ramarray;
    __osSiGetAccess();
    ret = __osEepStatus(mq, &sdata);
    type = sdata.type & (CONT_EEPROM | CONT_EEP16K);

    if (ret == 0) {
        switch (type) {
            case CONT_EEPROM:
                if (address >= EEPROM_MAXBLOCKS) {
                    ret = CONT_RANGE_ERROR;
                }
                break;
            case CONT_EEPROM | CONT_EEP16K:
                if (address >= EEP16K_MAXBLOCKS) {
                    // not technically possible
                    ret = CONT_RANGE_ERROR;
                } else {
                    __osEepromRead16K = 1;
                }
                break;
            default:
                ret = CONT_NO_RESPONSE_ERROR;
        }
    }

    if (ret != 0) {
        __osSiRelAccess();
        return ret;
    }

    while (sdata.status & CONT_EEPROM_BUSY) {
        __osEepStatus(mq, &sdata);
    }

    __osPackEepReadData(address);
    ret = __osSiRawStartDma(OS_WRITE, &__osEepPifRam); // send command to pif
    osRecvMesg(mq, NULL, OS_MESG_BLOCK);
    ret = __osSiRawStartDma(OS_READ, &__osEepPifRam); // recv response
    __osContLastCmd = CONT_CMD_READ_EEPROM;
    osRecvMesg(mq, NULL, OS_MESG_BLOCK);

    for (i = 0; i < MAXCONTROLLERS; i++) {
        // skip the first 4 bytes
        ptr++;
    }

    eepromformat = *(__OSContEepromFormat*)ptr;
    ret = CHNL_ERR(eepromformat);

    if (ret == 0) {
        for (i = 0; i < ARRLEN(eepromformat.data); i++) {
            *buffer++ = eepromformat.data[i];
        }
    }
    __osSiRelAccess();
    return ret;
}

void __osPackEepReadData(s32 address) {
    u8* ptr = (u8*)&__osEepPifRam.ramarray;
    __OSContEepromFormat eepromformat;
    int i;

    __osEepPifRam.pifstatus = CONT_CMD_EXE;

    eepromformat.txsize = CONT_CMD_READ_EEPROM_TX;
    eepromformat.rxsize = CONT_CMD_READ_EEPROM_RX;
    eepromformat.cmd = CONT_CMD_READ_EEPROM;
    eepromformat.address = address;

    for (i = 0; i < MAXCONTROLLERS; i++) {
        *ptr++ = 0;
    }

    *(__OSContEepromFormat*)(ptr) = eepromformat;
    ptr += sizeof(__OSContEepromFormat);
    ptr[0] = CONT_CMD_END;
}
#endif