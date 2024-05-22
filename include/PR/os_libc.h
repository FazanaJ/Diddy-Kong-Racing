#ifndef _OS_LIBC_H_
#define _OS_LIBC_H_

#include "ultratypes.h"
// Old deprecated functions from strings.h, replaced by memcpy/memset.
extern void bcopy(const void *src, void *dst, size_t size);
extern void wcopy(const void *src, void *dst, size_t size);
extern void bzero(void *dst, size_t size);

#endif /* !_OS_LIBC_H_ */
