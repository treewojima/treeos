#ifndef __TREEOS_KERNEL_PROC_PROCESS_H__
#define __TREEOS_KERNEL_PROC_PROCESS_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdint.h>

#ifdef TREEOS_I386
#   include <arch/i386/pmm/paging.h>
#   include <arch/i386/proc/thread.h>
#endif

struct process
{
    // Thread pointer must be the first member of this structure, or else
    // thread bootstrapping code will break!
    struct thread *thread;

    struct page_dir_entry *pgdir;
    uint8_t flags;

    uint32_t quantum;

    struct process *next;
};

// Process flags
#define PROC_FREE                (1 << 0)
#define PROC_DEBUG_YIELD         (1 << 1)
#define PROC_FREE_THREAD_CONTEXT (1 << 2)

#define DEFAULT_QUANTUM 100

struct process *process_create(struct process *process);

void process_yield(void);



#endif
