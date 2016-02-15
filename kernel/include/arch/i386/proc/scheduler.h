#ifndef __TREEOS_ARCH_I386_PROC_SCHEDULER_H__
#define __TREEOS_ARCH_I386_PROC_SCHEDULER_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (proc/scheduler.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdint.h>

struct process;

void scheduler_bootstrap_thread(struct process *p);

#endif
