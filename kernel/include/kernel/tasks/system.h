#ifndef __TREEOS_KERNEL_TASKS_SYSTEM_H__
#define __TREEOS_KERNEL_TASKS_SYSTEM_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <kernel/proc/process.h>

extern struct process g_kernel_system_task;

void kernel_system_task(void);

#endif
