#ifndef __TREEOS_KERNEL_PMM_H__
#define __TREEOS_KERNEL_PMM_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#ifdef TREEOS_I386
#   include <arch/i386/pmm/paging.h>
#   include <arch/i386/pmm/pmm.h>
#   include <arch/i386/pmm/tlb.h>
#endif

#endif
