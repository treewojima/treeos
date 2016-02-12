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
    struct thread *thread;
    struct page_dir_entry *pgdir;
    uint8_t flags;

    struct process *next;
};

// Process flags
#define PROC_FREE (1 << 0)

struct process *process_create(struct process *process);



#endif
