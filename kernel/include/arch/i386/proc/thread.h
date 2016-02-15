#ifndef __TREEOS_ARCH_I386_PROC_THREAD_H__
#define __TREEOS_ARCH_I386_PROC_THREAD_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (proc/thread.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <arch/i386/pmm/paging.h>
#include <stdint.h>

// Thread context of execution
struct thread_context
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;

    // The pushal instruction pushes esp before ebp, but when used as a
    // thread context, the thread's esp is already on the stack - and pushal
    // will actually push the current stack pointer in the kernel interrupt
    // handler, which is useless
    uint32_t ebp, :32;

    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t int_num;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} PACKED;

struct process;

// TODO: a lot
struct thread
{
    struct thread_context context;

    struct process *owner;    

    struct thread *next;
};

#if 0
void thread_create(struct thread *thread,
                   entrypoint entry,
                   struct page_dir_entry *pgdir,
                   uint8_t flags);
#endif

char *strcat_registers(char *buf, const struct thread_context *const context);

#endif
