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
struct thread_context;

// Initialize the scheduling system
void scheduler_init(void);

// Add a process to the queue
bool scheduler_add_process(struct process *p);

// Find the next best process to switch to
struct process *scheduler_find_next_best(void);

// Only ever called from the interrupt handler
void scheduler_advance_stage1(const struct thread_context *const context);

#endif
