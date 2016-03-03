// Architecture-independent constants, as well as the appropriate arch header

#ifndef __TREEOS_KERNEL_CONST_H__
#define __TREEOS_KERNEL_CONST_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdint.h>

#ifdef TREEOS_I386
#   include <arch/i386/const.h>
#endif

// Kernel addresses and boundaries
extern const uint8_t g_kernel_high_offset;
#define KERN_HIGH_OFFSET ((uint32_t)&g_kernel_high_offset)

extern const uint8_t g_kernel_boot_addr, g_kernel_high_addr;
#define KERN_BOOT_ADDR ((uint32_t)&g_kernel_boot_addr)
#define KERN_HIGH_ADDR (KERN_BOOT_ADDR + KERN_HIGH_OFFSET)

extern const uint8_t g_bss_start, g_bss_end;
#define BSS_START ((uint32_t)&g_bss_start)
#define BSS_END   ((uint32_t)&g_bss_end)

extern const uint8_t g_kernel_start, g_kernel_end;
#define KERN_START ((uint32_t)&g_kernel_start)
#define KERN_END   ((uint32_t)&g_kernel_end)

extern const uint8_t g_kernel_stack_bottom, g_kernel_stack_top;
#define KERN_STACK_BOTTOM ((uint32_t)&g_kernel_stack_bottom)
#define KERN_STACK_TOP    ((uint32_t)&g_kernel_stack_top)

#endif
