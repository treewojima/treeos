#ifndef __TREEOS_KERNEL_PROC_SCHED_H__
#define __TREEOS_KERNEL_PROC_SCHED_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

void sched_init(void);

#endif
