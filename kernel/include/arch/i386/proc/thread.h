#ifndef __TREEOS_ARCH_I386_PROC_THREAD_H__
#define __TREEOS_ARCH_I386_PROC_THREAD_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (proc/thread.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <arch/i386/cpu.h>
#include <arch/i386/pmm/paging.h>
#include <stdint.h>

struct process;

// TODO: a lot
struct thread
{
    struct registers registers;

    struct process *owner;    

    struct thread *next;
};

typedef void (*entrypoint)();

#if 0
void thread_create(struct thread *thread,
                   entrypoint entry,
                   struct page_dir_entry *pgdir,
                   uint8_t flags);
#endif

#endif
