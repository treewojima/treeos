#ifndef __TREEOS_ARCH_I386_PROC_MEMINFO_H__
#define __TREEOS_ARCH_I386_PROC_MEMINFO_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (proc/meminfo.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <arch/i386/pmm/paging.h>
#include <kernel/vmm/heap.h>

struct meminfo
{
    struct heap *heap;
    struct page_dir_entry *pgdir;
};

#endif
