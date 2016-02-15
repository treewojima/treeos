#ifndef __TREEOS_KERNEL_PROC_SCHEDULER_H__
#define __TREEOS_KERNEL_PROC_SCHEDULER_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#ifdef TREEOS_I386
#   include <arch/i386/proc/scheduler.h>
#endif

#include <stdbool.h>

struct process;

// Initialize the scheduling system
void scheduler_init(void);

// Add a process to the queue
bool scheduler_add_process(struct process *p);

// Advance the scheduler (i.e., jump to the next process)
struct process *scheduler_advance(void);

#endif
