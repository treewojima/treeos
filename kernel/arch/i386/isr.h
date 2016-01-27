#ifndef __TREEOS_ARCH_I386_ISR_H__
#define __TREEOS_ARCH_I386_ISR_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (cpu.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM

#if 0
// These don't work because C preprocessor macros cannot insert newlines,
// and we need to add in .global specifiers for these to be visible to
// the rest of the kernel
#define ISR_NOERRCODE(inum) \
    isr##inum: \
        pushl $0; \
        pushl $inum; \
        jmp int_handler_stub

#define ISR_ERRCODE(inum) \
    isr##inum: \
        pushl $inum; \
        jmp int_handler_stub

#define ISR_IRQ(irqnum, inum) \
    irq##irqnum: \
        pushl $0; \
        pushl $inum; \
        jmp int_handler_stub
#endif

#else

// Interrupt service routines
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

// IRQ service routines
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

// Syscall interrupt service routine
extern void isr48(void);

#endif

#endif
