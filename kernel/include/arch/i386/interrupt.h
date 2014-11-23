#ifndef __TREEOS_KERNEL_ARCH_INTERRUPT_H__
#define __TREEOS_KERNEL_ARCH_INTERRUPT_H__

#include "defines.h"
#ifndef __TREEOS_I386
#   error included i386-specific header (interrupt.h) in non-i386 build
#endif

#define INT_NUM_INTERRUPTS    49
#define INT_HANDLER_STUB_LEN  16
#define INT_SYSCALL_INTERRUPT 48

#define INT_CALL_HANDLER_DIRECTLY

#ifndef __TREEOS_EXPORT_ASM

#define int_enable()  __asm__ __volatile__ ("sti")
#define int_disable() __asm__ __volatile__ ("cli")

#endif

#endif
