#ifndef __TREEOS_KERNEL_VMM_ADDR_H__
#define __TREEOS_KERNEL_VMM_ADDR_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#ifdef TREEOS_I386
#   include <arch/i386/vmm/addr.h>
#endif

#endif
