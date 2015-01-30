#ifndef __TREEOS_KERNEL_ARCH_MM_H__
#define __TREEOS_KERNEL_ARCH_MM_H__

#include "defines.h"
#ifndef __TREEOS_I386
#   error included i386-specific header (mm.h) in non-i386 build
#endif

#define PAGE_SIZE 4096

#ifndef __TREEOS_EXPORT_ASM

#include <stdbool.h>
#include <stdint.h>

/* Paging structures */
struct page_dir_entry
{
   uint32_t present        : 1;  // present
   uint32_t rw             : 1;  // read-only if clear, read/write if set
   uint32_t user           : 1;  // supervisor-only if clear
   uint32_t writethrough   : 1;  // write-back if clear, write-through if set
   uint32_t cache_disabled : 1;  // instructs to "do not cache" if set
   uint32_t accessed       : 1,  // set if the page has been accessed/written to
                           : 1;  // hidden always-zero field
   uint32_t page_size      : 1,  // 4 KiB if clear, 4 MiB if set
                           : 1;  // hidden unused field
   uint32_t os_flags       : 3;  // flags that can be used by the OS
   uint32_t address        : 20; // aligned address of page table
} __attribute__((packed));

struct page_table_entry // aka a page
{
    uint32_t present        : 1;  // present
    uint32_t rw             : 1;  // read-only if clear, read/write if set
    uint32_t user           : 1,  // supervisor-only if clear
                            : 2;  // hidden reserved fields
    uint32_t accessed       : 1;  // set if the page has been accessed/written to
    uint32_t dirty          : 1,  // set if the page has been written to
                            : 2;  // hidden reserved fields
    uint32_t os_flags       : 3;  // flags that can be used by the OS
    uint32_t address        : 20; // physical address of page
} __attribute__((packed));

struct page_table_entry *mm_alloc_frame(struct page_table_entry *frame);
void mm_free_frame(struct page_table_entry *frame);

uint32_t mm_init_frame_bitmap(uint32_t mem_size);
void mm_init_region(uint32_t base, uint32_t size);
void mm_deinit_region(uint32_t base, uint32_t size);

extern void mm_flush_tlb(uint32_t virtualaddr);
extern void mm_flush_tlb_full(void);

#endif

#endif
