#ifndef __TREEOS_KERNEL_MM_H__
#define __TREEOS_KERNEL_MM_H__

#include <kernel/multiboot.h>
#include <stdint.h>

void mm_init(void);
void mm_init_heap(struct multiboot_info *mbi);

#include "defines.h"
#ifdef __TREEOS_I386
#   include <arch/i386/mm.h>
#endif

#endif
