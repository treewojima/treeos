#ifndef __TREEOS_ARCH_I386_THREAD_H__
#define __TREEOS_ARCH_I386_THREAD_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (proc/thread.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <arch/i386/cpu.h>
#include <kernel/mm.h>
#include <stdint.h>

// TODO: a lot
struct kthread
{
    struct page_dir_entry *pgdir;
    struct registers *registers;

    struct kthread *next;
} PACKED;

void kthread_create()

#endif
