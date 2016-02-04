#ifndef __TREEOS_KERNEL_VMM_HEAP_H__
#define __TREEOS_KERNEL_VMM_HEAP_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdbool.h>
#include <stdint.h>

// Place the kernel heap at 3.25 GiB, 1 MiB in size
#define KHEAP_START    0xD0000000
#define KHEAP_MAX_SIZE 0x100000

struct heap
{
    uint32_t base;
    uint32_t size;
    uint8_t flags;
};

// Kernel heap init
void kernel_heap_init(void);

// Heap manipulation
struct heap *heap_alloc(struct heap *heap, uint32_t base, uint32_t size);
void *heap_grow(struct heap *heap, uint32_t pages);
void heap_shrink(struct heap *heap, void *ptr, uint32_t pages);

#endif
