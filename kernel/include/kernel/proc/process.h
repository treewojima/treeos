#ifndef __TREEOS_KERNEL_PROC_PROCESS_H__
#define __TREEOS_KERNEL_PROC_PROCESS_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdint.h>

// Architecture-specific
struct thread;
struct meminfo;

struct process
{
    struct thread *thread;
    struct meminfo *meminfo;
    uint8_t flags;
};

// Process flags
#define PROC_FREE (1 << 0)

struct process *process_create(struct process *process);

#ifdef TREEOS_I386
#   include <arch/i386/proc/meminfo.h>
#   include <arch/i386/proc/thread.h>
#endif

#endif
