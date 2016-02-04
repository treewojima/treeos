#ifndef __TREEOS_KERNEL_ARCH_PMM_PMM_H__
#define __TREEOS_KERNEL_ARCH_PMM_PMM_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (pmm/pmm.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdint.h>

// Don't remap page tables, just use the bootstrap table
#define USE_BOOTSTRAP_PAGING

// Memory map and paging initialization
void paging_init(void);

#endif
