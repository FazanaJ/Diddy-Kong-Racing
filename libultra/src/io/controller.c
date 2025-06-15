#include "PRinternal/macros.h"
#include "PR/os_internal.h"
#include "PRinternal/controller.h"
#include "PRinternal/siint.h"

#define HALF_MIL_CYLCES 500000U
#define ONE_MIL_CYLCES 1000000U
#define HALF_A_SECOND HALF_MIL_CYLCES * osClockRate / ONE_MIL_CYLCES

OSPifRam __osContPifRam;
u8 __osContLastCmd;
u8 __osMaxControllers;

OSTimer __osEepromTimer;
OSMesgQueue __osEepromTimerQ ALIGNED(0x8);
OSMesg __osEepromTimerMsg;
u8 __osControllerMask;

s32 __osContinitialized = FALSE;
extern OSTime osClockRate;

s32 osContInit(OSMesgQueue* mq, u8* bitpattern, OSContStatus* data) {
    OSMesg dummy;
    s32 ret = 0;
    OSTime t;
    OSTimer mytimer;
    OSMesgQueue timerMesgQueue;

    if (__osContinitialized) {
        return 0;
    }

    __osContinitialized = TRUE;
    __osControllerMask = CONT_P1 | CONT_P2 | CONT_P3 | CONT_P4;

    t = osGetTime();
    if (HALF_A_SECOND > t) {
        osCreateMesgQueue(&timerMesgQueue, &dummy, 1);
        osSetTimer(&mytimer, HALF_A_SECOND - t, 0, &timerMesgQueue, &dummy);
        osRecvMesg(&timerMesgQueue, &dummy, OS_MESG_BLOCK);
    }

    __osMaxControllers = MAXCONTROLLERS;

    __osPackRequestData(CONT_CMD_REQUEST_STATUS);

    ret = __osSiRawStartDma(OS_WRITE, __osContPifRam.ramarray);
    osRecvMesg(mq, &dummy, OS_MESG_BLOCK);

    ret = __osSiRawStartDma(OS_READ, __osContPifRam.ramarray);
    osRecvMesg(mq, &dummy, OS_MESG_BLOCK);

    __osContGetInitData(bitpattern, data);
    __osContLastCmd = CONT_CMD_REQUEST_STATUS;
    __osSiCreateAccessQueue();
    osCreateMesgQueue(&__osEepromTimerQ, &__osEepromTimerMsg, 1);

    return ret;
}

void __osContGetInitData(u8* pattern, OSContStatus* data) {
    u8* ptr;
    __OSContRequesFormat requestHeader;
    int i;
    u8 bits = 0;

    ptr = (u8*)__osContPifRam.ramarray;
    for (i = 0; i < __osMaxControllers; i++, ptr += sizeof(requestHeader), data++) {
        requestHeader = *(__OSContRequesFormat*)ptr;
        data->errno = CHNL_ERR(requestHeader);

        if (data->errno != 0) {
            continue;
        }

        data->type = requestHeader.typel << 8 | requestHeader.typeh;
        data->status = requestHeader.status;
        bits |= 1 << i;
    }
    *pattern = bits;
}

void __osPackRequestData(u8 cmd) {
    u8* ptr;
    __OSContRequesFormat requestHeader;
    s32 i;

    for (i = 0; i <= ARRLEN(__osContPifRam.ramarray); i++) {
        __osContPifRam.ramarray[i] = 0;
    }

    __osContPifRam.pifstatus = CONT_CMD_EXE;
    ptr = (u8*)__osContPifRam.ramarray;
    requestHeader.dummy = CONT_CMD_NOP;
    requestHeader.txsize = CONT_CMD_RESET_TX;
    requestHeader.rxsize = CONT_CMD_RESET_RX;
    requestHeader.cmd = cmd;
    requestHeader.typeh = CONT_CMD_NOP;
    requestHeader.typel = CONT_CMD_NOP;
    requestHeader.status = CONT_CMD_NOP;
    requestHeader.dummy1 = CONT_CMD_NOP;

    for (i = 0; i < __osMaxControllers; i++) {
        *(__OSContRequesFormat*)ptr = requestHeader;
        ptr += sizeof(requestHeader);
    }
    *ptr = CONT_CMD_END;
}

/*
 * This function specifies the number of devices for the functions to access when those functions access
 * to multiple direct SI devices.
 */
s32 osContSetCh(u8 ch) {
    return osContSetMask((1 << ch) - 1);
}

/*
 * Use a mask to detect controller channels instead, allowing the PIF to skip channels instead of
 * spending time reading data.
 * This does not work on iQue Player.
 */
s32 osContSetMask(u8 ch) {
    s32 ret = 0;
    s32 i;

    if (__osBbIsBb) {

        __osControllerMask = (CONT_P1 | CONT_P2 | CONT_P3 | CONT_P4);
        __osMaxControllers = MAXCONTROLLERS;
        return ret;
    }
    
    __osSiGetAccess();

    if ((ch == 0) || (ch > (CONT_P1 | CONT_P2 | CONT_P3 | CONT_P4))) {
        __osControllerMask = (CONT_P1 | CONT_P2 | CONT_P3 | CONT_P4);
        __osMaxControllers = MAXCONTROLLERS;
    } else {
        __osControllerMask = ch;
        for (i = 0; i < MAXCONTROLLERS; i++) {
            if (ch & (1 << i)) {
                __osMaxControllers = i;
            }
        }
        __osMaxControllers++;
    }

    __osContLastCmd = CONT_CMD_END;
    __osSiRelAccess();
    return ret;
}