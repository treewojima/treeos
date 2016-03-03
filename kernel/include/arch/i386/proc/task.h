#ifndef __TREEOS_ARCH_I386_PROC_TASK_H__
#define __TREEOS_ARCH_I386_PROC_TASK_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (proc/task.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdint.h>

// Task selector segment descriptor
struct tss_entry
{
    uint32_t prev_tss;   // previous TSS - if using hardware task switching,
                         //     this acts as a linked list
    uint32_t esp0;       // stack pointer
    uint32_t ss0;        // stack segment

    // Unless implementing hardware task switching, the rest of this structure
    // is unused
    uint32_t unused[23];
} PACKED;

extern struct tss_entry g_kernel_tss;

void tss_init(void);

#endif
