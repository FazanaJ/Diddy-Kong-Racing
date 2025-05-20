#include "PRinternal/controller.h"
#include "PRinternal/siint.h"
#include "PRinternal/piint.h"
#include "src/main.h"

#define SRAM_START_ADDR         0x08000000
#define SRAM_SIZE               0x8000
#define SRAM_LATENCY            0x5
#define SRAM_PULSE              0x0c
#define SRAM_PAGE_SIZE          0xd
#define SRAM_REL_DURATION       0x2
#define PI_DOMAIN2              1

#if defined(EEP4K) || defined(EEP16K)
extern s32 __osEepromRead16K;
s32 osEepromProbe(OSMesgQueue* mq) {
    s32 ret = 0;
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
    if (ret == 0 && (sdata.type & CONT_EEPROM) != 0) {
        ret = EEPROM_TYPE_4K;
    } else {
        ret = 0;
    }

    __osEepromRead16K = 0;
    __osSiRelAccess();
    return ret;
}
#else if defined(SRAM)
OSPiHandle*		nuPiSramHandle;
OSPiHandle	SramHandle;
OSPiHandle* __osPiTable = NULL;
u8 sSramInit = FALSE;

s32 osEPiLinkHandle(OSPiHandle* EPiHandle) {
    u32 saveMask = __osDisableInt();

    EPiHandle->next = __osPiTable;
    __osPiTable = EPiHandle;

    __osRestoreInt(saveMask);
    return 0;
}

s32 osEPiStartDma(OSPiHandle* pihandle, OSIoMesg* mb, s32 direction) {
    register s32 ret;

    if (!__osPiDevMgr.active) {
        return -1;
    }

    mb->piHandle = pihandle;

    if (direction == OS_READ) {
        mb->hdr.type = OS_MESG_TYPE_EDMAREAD;
    } else {
        mb->hdr.type = OS_MESG_TYPE_EDMAWRITE;
    }

    if (mb->hdr.pri == OS_MESG_PRI_HIGH) {
        ret = osJamMesg(osPiGetCmdQueue(), (OSMesg)mb, OS_MESG_NOBLOCK);
    } else {
        ret = osSendMesg(osPiGetCmdQueue(), (OSMesg)mb, OS_MESG_NOBLOCK);
    }

    return ret;
}

int nuPiInitSram(void) {
    if (sSramInit) {
        return 2;
    }

    if (__osBbIsBb) {
        __osPiGetAccess();
        if (__osBbSramSize != 0x8000) {
            return 0;
        }
        __osPiRelAccess();
        sSramInit = TRUE;
        return 1;
    }

    if (SramHandle.baseAddress == PHYS_TO_K1(SRAM_START_ADDR)) {
	    return 0;
    }
    
    /* Fill basic information */
    SramHandle.type = DEVICE_TYPE_SRAM;
    SramHandle.baseAddress = PHYS_TO_K1(SRAM_START_ADDR);
    
    /* Get Domain parameters */
    SramHandle.latency = (u8)SRAM_LATENCY;
    SramHandle.pulse = (u8)SRAM_PULSE;
    SramHandle.pageSize = (u8)SRAM_PAGE_SIZE;
    SramHandle.relDuration = (u8)SRAM_REL_DURATION;
    SramHandle.domain = PI_DOMAIN2;
    
    /* Fill speed and transferInfo to zero */
    SramHandle.speed = 0;
    bzero((void *)&(SramHandle.transferInfo),
	  sizeof(SramHandle.transferInfo));
    
    /* Put the SramHandle onto PiTable*/
    osEPiLinkHandle(&SramHandle);
    nuPiSramHandle = &SramHandle;
    sSramInit = TRUE;
	return 1;
}

int nuPiReadWriteSram(u32 addr, void* buf_ptr, u32 size, s32 flag) {  
    OSIoMesg	dmaIoMesgBuf;
    OSMesgQueue dmaMesgQ;
    OSMesg	dmaMesgBuf;

    if (__osBbIsBb) {

        if (__osBbSramSize == 0x8000) {
            if (addr >= 0x8000) {
                return -1;
            }

            __osPiGetAccess();
            if (flag == OS_READ) {
                wcopy((u32 *) (__osBbSramAddress + addr), buf_ptr, size);
            } else {
                wcopy(buf_ptr, (u32 *) (__osBbSramAddress + addr), size);
            }
            __osPiRelAccess();
            
            return 0;
        }
        return -1;
    }

    /* Create the message queue. */
    osCreateMesgQueue(&dmaMesgQ, &dmaMesgBuf, 1);

    dmaIoMesgBuf.hdr.pri      = OS_MESG_PRI_NORMAL;
    dmaIoMesgBuf.hdr.retQueue = &dmaMesgQ;
    dmaIoMesgBuf.dramAddr     = buf_ptr;
    dmaIoMesgBuf.devAddr      = (u32)addr;
    dmaIoMesgBuf.size         = size;

    if (flag == OS_READ){
	    /* Make CPU cache invalid */
        debug_printf("Reading 0x%X bytes at 0x%X from sram.\n", size, (u32) addr);
	    osInvalDCache((void*)buf_ptr, (s32)size);
    } else {
        /* Write back */
        debug_printf("Writing 0x%X bytes at 0x%X to sram.\n", size, (u32) addr);
        osWritebackDCache((void*)buf_ptr, (s32)size);
    }
    osEPiStartDma(nuPiSramHandle, &dmaIoMesgBuf, flag);

    /* Wait for the end */
    (void)osRecvMesg(&dmaMesgQ, &dmaMesgBuf, OS_MESG_BLOCK);
    return 0;
}
#endif
