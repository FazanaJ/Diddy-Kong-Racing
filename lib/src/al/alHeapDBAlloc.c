/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800C77F0 */

#include "audio_internal.h"
#include "macros.h"

void *alHeapDBAlloc(UNUSED u8 *file, UNUSED s32 line, ALHeap *hp, s32 num, s32 size) {
    
    u8 *ptr = (u8 *) allocate_from_main_pool((size * num) + 0xF, MEMP_AUDIO_POOL);
    if ((s32) ptr & 0xF) {
        ptr = (u8 *) (((s32) ptr + 0xF) & ~0xF);
    }

    return ptr;
    /*s32 bytes;
    u8 *ptr = 0;

    bytes = (num*size + AL_CACHE_ALIGN) & ~AL_CACHE_ALIGN;
    
    if ((hp->cur + bytes) <= (hp->base + hp->len)) {
        ptr = hp->cur;
        hp->cur += bytes;
    } else {
    }

    return ptr;*/
}
