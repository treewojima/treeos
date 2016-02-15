#ifndef __TREEOS_KERNEL_TASKS_IDLE_H__
#define __TREEOS_KERNEL_TASKS_IDLE_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <kernel/proc/process.h>

extern struct process g_kernel_idle_task;

void kernel_idle_task(void);

#endif
