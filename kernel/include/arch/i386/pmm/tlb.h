#ifndef __TREEOS_ARCH_I386_PMM_TLB_H__
#define __TREEOS_ARCH_I386_PMM_TLB_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (pmm/pmm.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

// TLB flushing
extern void flush_tlb(uint32_t virtualaddr);
extern void flush_tlb_full(void);

#endif
