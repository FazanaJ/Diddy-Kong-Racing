/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800D2480 */

#include "types.h"
#include "macros.h"
#include "libultra_internal.h"

#define PI_Q_BUF_LEN 1
extern u32 __osPiAccessQueueEnabled;
extern OSMesg piAccessBuf[PI_Q_BUF_LEN];
extern OSMesgQueue __osPiAccessQueue;
void __osPiCreateAccessQueue(void) {
    __osPiAccessQueueEnabled = 1;
    osCreateMesgQueue(&__osPiAccessQueue, piAccessBuf, PI_Q_BUF_LEN);
    osSendMesg(&__osPiAccessQueue, NULL, OS_MESG_NOBLOCK);
}

void __osPiGetAccess(void) {
    OSMesg dummyMesg;
    if (!__osPiAccessQueueEnabled)
        __osPiCreateAccessQueue();
    osRecvMesg(&__osPiAccessQueue, &dummyMesg, OS_MESG_BLOCK);
}

void __osPiRelAccess(void) {
    osSendMesg(&__osPiAccessQueue, NULL, OS_MESG_NOBLOCK);
}

// Used with _Litob
char ldigs[] = "0123456789abcdef"; // lowercase hex digits
char udigs[] = "0123456789ABCDEF"; // uppercase hex digits

// Powers for _Ldtob
const double pows[] = {10e0L, 10e1L, 10e3L, 10e7L, 10e15L, 10e31L, 10e63L, 10e127L, 10e255L};

const char Ldtob_NaN[] = "NaN";
const char Ldtob_Inf[] = "Inf";

//_Genld
const char _Genld_0_string[] = "0";

//_Ldtob
const double _Ldtob_1e8[] = {1.0e8};

