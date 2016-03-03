#ifndef __TREEOS_ARCH_I386_VMM_ADDR_H__
#define __TREEOS_ARCH_I386_VMM_ADDR_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (vmm/addr.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

// Address translation
void *virt_to_phys(void *vaddr);

#endif
