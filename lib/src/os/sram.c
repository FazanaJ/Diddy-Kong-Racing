/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800D1EF0 */

/*======================================================================*/
/*		NuSYS							*/
/*		nupisraminit.c						*/
/*									*/
/*		Copyright (C) 1997, NINTENDO Co,Ltd.			*/
/*									*/
/*----------------------------------------------------------------------*/
/* Ver 1.2	98/07/4		Created by Kensaku Ohki(SLANP)		*/
/*----------------------------------------------------------------------*/
/* $Id: nupiinitsram.c,v 1.2 1998/07/11 11:22:46 ohki Exp $	*/
/*======================================================================*/
#include <ultra64.h>
#include "piint.h"
#include "src/main.h"

#define SRAM_START_ADDR         0x08000000
#define SRAM_SIZE               0x8000
#define SRAM_LATENCY            0x5
#define SRAM_PULSE              0x0c
#define SRAM_PAGE_SIZE          0xd
#define SRAM_REL_DURATION       0x2
#define PI_DOMAIN2              1

OSPiHandle*		nuPiSramHandle;
static OSPiHandle	SramHandle;
OSPiHandle* __osPiTable = NULL;

static inline s32 osEPiLinkHandle(OSPiHandle* EPiHandle) {
    u32 saveMask = __osDisableInt();

    EPiHandle->next = __osPiTable;
    __osPiTable = EPiHandle;

    __osRestoreInt(saveMask);
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

/*----------------------------------------------------------------------*/
/*	nuPiSramInit  - Initialization of handle for SRAM		*/
/*	Initialize handle for SRAM					*/
/*	IN:	None							*/
/*	RET:	If detected, return 1, otherwise 0							*/
/*----------------------------------------------------------------------*/
int nuPiInitSram(void) {
    if (SramHandle.baseAddress == PHYS_TO_K1(SRAM_START_ADDR)) {
	    return 2;
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
	return 1;
}

extern OSPiHandle*		nuPiSramHandle;
/*----------------------------------------------------------------------*/
/*	nuPiReadWriteSram  - DMA transfers data to and from SRAM.	*/
/*	The message queue is a local variable so it can be used         */
/*     between threads.                                                 */
/*	IN:	addr	      SRAM address. 				*/
/*		buf_ptr	RDRAM address. 					*/
/*		size	      Transfer size. 				*/
/*	RET:	None							*/
/*----------------------------------------------------------------------*/
int nuPiReadWriteSram(u32 addr, void* buf_ptr, u32 size, s32 flag) {  
    OSIoMesg	dmaIoMesgBuf;
    OSMesgQueue dmaMesgQ;
    OSMesg	dmaMesgBuf;

    /* Create the message queue. */
    osCreateMesgQueue(&dmaMesgQ, &dmaMesgBuf, 1);

    dmaIoMesgBuf.hdr.pri      = OS_MESG_PRI_NORMAL;
    dmaIoMesgBuf.hdr.retQueue = &dmaMesgQ;
    dmaIoMesgBuf.dramAddr     = buf_ptr;
    dmaIoMesgBuf.devAddr      = (u32)addr;
    dmaIoMesgBuf.size         = size;

    if(flag == OS_READ){
	/* Make CPU cache invalid */
	osInvalDCache((void*)buf_ptr, (s32)size);
    } else {
	/* Write back */
	osWritebackDCache((void*)buf_ptr, (s32)size);
    }
    osEPiStartDma(nuPiSramHandle, &dmaIoMesgBuf, flag);

    /* Wait for the end */
    (void)osRecvMesg(&dmaMesgQ, &dmaMesgBuf, OS_MESG_BLOCK);
    return 0;
}
