#include "PR/os_internal.h"
#include "PRinternal/controller.h"
#include "PRinternal/siint.h"

s32 __osPfsGetStatus(OSMesgQueue *queue, int channel) {
    s32 ret;
    OSMesg dummy;
    u8 pattern;
    OSContStatus data[MAXCONTROLLERS];

    if (__osBbIsBb) {
        if (__osBbPakAddress[channel] != 0) {
        return 0;
        }
        return PFS_ERR_NOPACK;
    }
	
	ret = 0;
    __osPfsRequestData(0);
    
	ret = __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
    osRecvMesg(queue, &dummy, OS_MESG_BLOCK);
    
	ret = __osSiRawStartDma(OS_READ, &__osPfsPifRam);
    osRecvMesg(queue, &dummy, OS_MESG_BLOCK);
    
	__osPfsGetInitData(&pattern, data);

    if (((data[channel].status & CONT_CARD_ON) != 0) && ((data[channel].status & CONT_CARD_PULL) != 0)) {
        return PFS_ERR_NEW_PACK;
    } else if ((data[channel].errno != 0) || ((data[channel].status & CONT_CARD_ON) == 0)) {
        return PFS_ERR_NOPACK;
    } else if ((data[channel].status & CONT_ADDR_CRC_ER) != 0) {
        return PFS_ERR_CONTRFAIL;
    }

    return ret;
}
