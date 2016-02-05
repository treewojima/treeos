#ifndef __TREEOS_ARCH_I386_PMM_ALLOCATOR_H__
#define __TREEOS_ARCH_I386_PMM_ALLOCATOR_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (pmm/allocator.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdint.h>

// Paging structure allocator - allocates PDEs or PTEs in blocks of 1024
// NOTE: Because a full PDE/PTE is 1 page in size, this acts similarly
//       to a raw page allocator
void paging_allocator_init(void);
void *paging_structure_alloc(void);

#endif
