/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800CE170 */

#include "types.h"
#include "macros.h"
#include "config.h"

#ifdef PUPPYPRINT_DEBUG
u32 gMemUsed[16];
u8 gMemUsedCounter = 0;

unsigned int get_ra_register(void) {
    unsigned int ra_register;
    asm volatile (
        "move %0, $ra" 
        : "=r" (ra_register)
        : 
        :  
    );

    return ra_register;
}
#endif

void *memcpy(void *dst, const void *src, size_t size) {
#ifdef PUPPYPRINT_DEBUG
    if (gMemUsedCounter < 16) {
        gMemUsed[gMemUsedCounter++] = get_ra_register();
    }
#endif
    u8 *_dst = dst;
    const u8 *_src = src;
    while (size > 0) {
        *_dst++ = *_src++;
        size--;
    }
    return dst;
}

size_t strlen(const char *str) {
    const u8 *ptr = (const u8 *) str;
    while (*ptr) {
        ptr++;
    }
    return (const char *) ptr - str;
}

char *strchr(const char *str, s32 ch) {
    u8 c = ch;
    while (*(u8 *)str != c) {
        if (*(u8 *)str == 0) {
            return NULL;
        }
        str++;
    }
    return (char *) str;
}
