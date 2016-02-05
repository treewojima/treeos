#ifndef __TREEOS_LIBC_STDLIB_H__
#define __TREEOS_LIBC_STDLIB_H__

#include <defines.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *kmalloc(size_t);
void *krealloc(void *, size_t);
void *kcalloc(size_t, size_t);
void kfree(void *);

#ifdef __cplusplus
}
#endif

#endif
