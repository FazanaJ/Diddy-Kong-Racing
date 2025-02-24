/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800D1EF0 */

#include "ultra64.h"
#include "piint.h"
#include "src/main.h"
#include "siint.h"
#include "PR/os_internal_flash.h"


#ifdef FLASHRAM

ALIGNED8 u32 __osFlashID[4];
ALIGNED8 OSIoMesg __osFlashMsg;
ALIGNED8 OSMesgQueue __osFlashMessageQ;
ALIGNED8 OSPiHandle __osFlashHandler;
ALIGNED8 OSPiHandle __Dom1SpeedParam;
ALIGNED8 OSPiHandle __Dom2SpeedParam;
ALIGNED8 OSPiHandle* __osCurrentHandle[2] = { &__Dom1SpeedParam, &__Dom2SpeedParam };
OSPiHandle* __osPiTable = NULL;
OSMesg __osFlashMsgBuf[1];
s32 __osFlashVersion;
s32 sFlashInit = FALSE;

static inline s32 __osEPiRawReadIo(OSPiHandle* pihandle, u32 devAddr, u32* data) {
    register u32 stat;
    register u32 domain;

    EPI_SYNC(pihandle, stat, domain);
    *data = IO_READ(pihandle->baseAddress | devAddr);

    return 0;
}

static inline s32 __osEPiRawWriteIo(OSPiHandle* pihandle, u32 devAddr, u32 data) {
    register u32 stat;
    register u32 domain;

    EPI_SYNC(pihandle, stat, domain);
    IO_WRITE(pihandle->baseAddress | devAddr, data);

    return 0;
}

static inline s32 osEPiStartDma(OSPiHandle* pihandle, OSIoMesg* mb, s32 direction) {
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

static inline s32 osEPiWriteIo(OSPiHandle* pihandle, u32 devAddr, u32 data) {
    register s32 ret;

    __osPiGetAccess();
    ret = __osEPiRawWriteIo(pihandle, devAddr, data);
    __osPiRelAccess();

    return ret;
}

static inline s32 osEPiReadIo(OSPiHandle* pihandle, u32 devAddr, u32* data) {
    register s32 ret;

    __osPiGetAccess();
    ret = __osEPiRawReadIo(pihandle, devAddr, data);
    __osPiRelAccess();

    return ret;
}

static inline s32 osEPiLinkHandle(OSPiHandle* EPiHandle) {
    u32 saveMask = __osDisableInt();

    EPiHandle->next = __osPiTable;
    __osPiTable = EPiHandle;

    __osRestoreInt(saveMask);
    return 0;
}

static void osFlashClearStatus(void) {
    // select status mode
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_STATUS);
    // clear status
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress, 0);
    return;
}

static u32 __osFlashGetAddr(u32 page_num) {
    u32 devAddr;

    if (__osFlashVersion == OLD_FLASH) {
        // Account for hardware bug in old flash where the address bits are shifted 1-off where they should be
        devAddr = page_num * (FLASH_BLOCK_SIZE >> 1);
    } else {
        devAddr = page_num * FLASH_BLOCK_SIZE;
    }

    return devAddr;
}

OSPiHandle* osFlashInit(void) {
    u32 flash_type;
    u32 flash_maker;

    if (sFlashInit) {
        return (OSPiHandle *) 1;
    }

    osCreateMesgQueue(&__osFlashMessageQ, __osFlashMsgBuf, ARRAY_COUNT(__osFlashMsgBuf));

    if (__osFlashHandler.baseAddress == PHYS_TO_K1(FLASH_START_ADDR)) {
        return &__osFlashHandler;
    }

    __osFlashHandler.type = DEVICE_TYPE_FLASH;
    __osFlashHandler.baseAddress = PHYS_TO_K1(FLASH_START_ADDR);
    __osFlashHandler.latency = FLASH_LATENCY;
    __osFlashHandler.pulse = FLASH_PULSE;
    __osFlashHandler.pageSize = FLASH_PAGE_SIZE;
    __osFlashHandler.relDuration = FLASH_REL_DURATION;
    __osFlashHandler.domain = PI_DOMAIN2;
    __osFlashHandler.speed = 0;

    bzero(&__osFlashHandler.transferInfo, sizeof(__OSTranxInfo));

    osEPiLinkHandle(&__osFlashHandler);
    osFlashReadId(&flash_type, &flash_maker);

    if (flash_maker == FLASH_VERSION_MX_C || flash_maker == FLASH_VERSION_MX_A ||
        flash_maker == FLASH_VERSION_MX_PROTO_A) {
        __osFlashVersion = OLD_FLASH;
    } else {
        __osFlashVersion = NEW_FLASH;
    }

    sFlashInit = TRUE;

    return &__osFlashHandler;
}

s32 osFlashReadArray(OSIoMesg* mb, s32 priority, u32 page_num, void* dramAddr, u32 n_pages, OSMesgQueue* mq) {
    u32 ret;
    u32 tmp;
    u32 end_page;
    u32 one_dma_pages;

    // select read array mode
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_READ_ARRAY);
    // dummy read?
    osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &tmp);

    // DMA requested pages
    mb->hdr.pri = priority;
    mb->hdr.retQueue = mq;
    mb->dramAddr = dramAddr;

    end_page = page_num + n_pages - 1;

    if ((end_page & 0xF00) != (page_num & 0xF00)) {
        one_dma_pages = 256 - (page_num & 0xFF);
        n_pages -= one_dma_pages;
        mb->size = one_dma_pages * FLASH_BLOCK_SIZE;
        mb->devAddr = __osFlashGetAddr(page_num);
        osEPiStartDma(&__osFlashHandler, mb, OS_READ);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        page_num = (page_num + 256) & 0xF00;
        mb->dramAddr = (void *)(u32)mb->dramAddr + mb->size;
    }

    while (n_pages > 256) {
        one_dma_pages = 256;
        n_pages -= 256;
        mb->size = one_dma_pages * FLASH_BLOCK_SIZE;
        mb->devAddr = __osFlashGetAddr(page_num);
        osEPiStartDma(&__osFlashHandler, mb, OS_READ);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        page_num += 256;
        mb->dramAddr = (void *)(u32)mb->dramAddr + mb->size;
    }

    mb->size = n_pages * FLASH_BLOCK_SIZE;
    mb->devAddr = __osFlashGetAddr(page_num);

    ret = osEPiStartDma(&__osFlashHandler, mb, OS_READ);

    return ret;
}

void osFlashReadId(u32* flash_type, u32* flash_maker) {
    u8 tmp;

    // why read status ?
    osFlashReadStatus(&tmp);

    // select silicon id read mode
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_ID);

    // read silicon id using DMA
    __osFlashMsg.hdr.pri = OS_MESG_PRI_NORMAL;
    __osFlashMsg.hdr.retQueue = &__osFlashMessageQ;
    __osFlashMsg.dramAddr = __osFlashID;
    __osFlashMsg.devAddr = 0;
    __osFlashMsg.size = 2 * sizeof(u32);

    osInvalDCache(__osFlashID, sizeof(__osFlashID));
    osEPiStartDma(&__osFlashHandler, &__osFlashMsg, OS_READ);
    osRecvMesg(&__osFlashMessageQ, NULL, OS_MESG_BLOCK);

    *flash_type = __osFlashID[0];
    *flash_maker = __osFlashID[1];
    return;
}

void osFlashReadStatus(u8* flash_status) {
    u32 status;

    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_STATUS);
    // read status
    osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);

    // why twice ?
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_STATUS);
    osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);

    *flash_status = status & 0xFF;
    return;
}

s32 osFlashSectorErase(u32 page_num) {
    u32 status;
    OSTimer mytimer;
    OSMesgQueue timerMesgQueue;
    OSMesg dummy;

    // start sector erase operation
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_SECTOR_ERASE | page_num);
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_EXECUTE_ERASE);

    // wait for completion by polling erase-busy flag
    osCreateMesgQueue(&timerMesgQueue, &dummy, 1);

    do {
        osSetTimer(&mytimer, OS_USEC_TO_CYCLES(12500), 0, &timerMesgQueue, &dummy);
        osRecvMesg(&timerMesgQueue, &dummy, OS_MESG_BLOCK);
        osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);
    } while ((status & FLASH_STATUS_ERASE_BUSY) == FLASH_STATUS_ERASE_BUSY);

    // check erase operation status, clear status
    osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);
    osFlashClearStatus();

    if (((status & 0xFF) == 8) || ((status & 0xFF) == 0x48) || ((status & 8) == 8)) {
        return FLASH_STATUS_ERASE_OK;
    } else {
        return FLASH_STATUS_ERASE_ERROR;
    }
}

void osFlashSectorEraseThrough(u32 page_num) {
    // start sector erase operation
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_SECTOR_ERASE | page_num);
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_EXECUTE_ERASE);
}

s32 osFlashWriteArray(u32 page_num) {
    u32 status;
    OSTimer mytimer;
    OSMesgQueue timerMesgQueue;
    OSMesg dummy;

    if ((u32)__osFlashVersion == NEW_FLASH) {
        osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_PAGE_PROGRAM);
    }

    // start program page operation
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_PROGRAM_PAGE | page_num);

    // wait for completion by polling write-busy flag
    osCreateMesgQueue(&timerMesgQueue, &dummy, 1);
    do {
        osSetTimer(&mytimer, OS_USEC_TO_CYCLES(200), 0, &timerMesgQueue, &dummy);
        osRecvMesg(&timerMesgQueue, &dummy, OS_MESG_BLOCK);
        osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);
    } while ((status & FLASH_STATUS_WRITE_BUSY) == FLASH_STATUS_WRITE_BUSY);

    // check program operation status, clear status
    osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);
    osFlashClearStatus();

    if (((status & 0xFF) == 4) || ((status & 0xFF) == 0x44) || ((status & 4) == 4)) {
        return FLASH_STATUS_WRITE_OK;
    } else {
        return FLASH_STATUS_WRITE_ERROR;
    }
}

s32 osFlashWriteBuffer(OSIoMesg* mb, s32 priority, void* dramAddr, OSMesgQueue* mq) {
    s32 ret;

    // select page program mode
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_PAGE_PROGRAM);

    // DMA 128-byte page
    mb->hdr.pri = priority;
    mb->hdr.retQueue = mq;
    mb->dramAddr = dramAddr;
    mb->devAddr = 0;
    mb->size = FLASH_BLOCK_SIZE;

    ret = osEPiStartDma(&__osFlashHandler, mb, OS_WRITE);

    return ret;
}
#endif