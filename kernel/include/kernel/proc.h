#ifndef __TREEOS_KERNEL_PROC_H__
#define __TREEOS_KERNEL_PROC_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#ifdef TREEOS_I386
#   include <arch/i386/proc/task.h>
#endif

#endif
