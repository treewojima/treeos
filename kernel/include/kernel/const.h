// Architecture-independent constants, as well as the appropriate arch header

#ifndef __TREEOS_KERNEL_CONST_H__
#define __TREEOS_KERNEL_CONST_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#ifdef TREEOS_I386
#   include <arch/i386/const.h>
#endif

#endif
