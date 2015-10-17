#ifndef __TREEOS_KERNEL_MM_H__
#define __TREEOS_KERNEL_MM_H__

//#include <kernel/multiboot.h>
#include <stdbool.h>
#include <stdint.h>

struct heap
{
    uint32_t base;
    uint32_t size;
};

void mm_init(void);
void mm_init_placement_heap(void);
void mm_init_kernel_heap(void);

//void *mm_alloc_page(void);
//void *mm_alloc_pages(uint32_t num_pages);

void *mm_get_physaddr(void *virtualaddr);

void *mm_placement_alloc(uint32_t size, bool align);

#include "defines.h"
#ifdef __TREEOS_I386
#   include <arch/i386/mm.h>
#endif

#endif
