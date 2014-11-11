.section .text

#include <arch/i386/interrupt.h>

.macro ASM_ISR_NOERRCODE inum
    .global isr\inum
    .align 8
    .type isr\inum, @function
    isr\inum:
        #cli
        pushl $0
        pushl $(\inum)
        jmp int_handler_stub
.endm

.macro ASM_ISR_ERRCODE inum
    .global isr\inum
    .align 8
    .type isr\inum, @function
    isr\inum:
        #cli
        pushl $(\inum)
        jmp int_handler_stub
.endm

.macro ASM_ISR_IRQ irqnum inum
    .global irq\irqnum
    .align 8
    .type irq\irqnum, @function
    irq\irqnum:
        #cli
        pushl $0
        pushl $(\inum)
        jmp int_handler_stub
.endm

# First, our interrupt service routines
ASM_ISR_NOERRCODE  0
ASM_ISR_NOERRCODE  1
ASM_ISR_NOERRCODE  2
ASM_ISR_NOERRCODE  3
ASM_ISR_NOERRCODE  4
ASM_ISR_NOERRCODE  5
ASM_ISR_NOERRCODE  6
ASM_ISR_NOERRCODE  7
ASM_ISR_ERRCODE    8
ASM_ISR_NOERRCODE  9
ASM_ISR_ERRCODE   10
ASM_ISR_ERRCODE   11
ASM_ISR_ERRCODE   12
ASM_ISR_ERRCODE   13
ASM_ISR_ERRCODE   14
ASM_ISR_NOERRCODE 15
ASM_ISR_NOERRCODE 16
ASM_ISR_NOERRCODE 17
ASM_ISR_NOERRCODE 18
ASM_ISR_NOERRCODE 19
ASM_ISR_NOERRCODE 20
ASM_ISR_NOERRCODE 21
ASM_ISR_NOERRCODE 22
ASM_ISR_NOERRCODE 23
ASM_ISR_NOERRCODE 24
ASM_ISR_NOERRCODE 25
ASM_ISR_NOERRCODE 26
ASM_ISR_NOERRCODE 27
ASM_ISR_NOERRCODE 28
ASM_ISR_NOERRCODE 29
ASM_ISR_NOERRCODE 30
ASM_ISR_NOERRCODE 31

# Now, our IRQs
ASM_ISR_IRQ  0, 32
ASM_ISR_IRQ  1, 33
ASM_ISR_IRQ  2, 34
ASM_ISR_IRQ  3, 35
ASM_ISR_IRQ  4, 36
ASM_ISR_IRQ  5, 37
ASM_ISR_IRQ  6, 38
ASM_ISR_IRQ  7, 39
ASM_ISR_IRQ  8, 40
ASM_ISR_IRQ  9, 41
ASM_ISR_IRQ 10, 42
ASM_ISR_IRQ 11, 43
ASM_ISR_IRQ 12, 44
ASM_ISR_IRQ 13, 45
ASM_ISR_IRQ 14, 46
ASM_ISR_IRQ 15, 47

# Finally the syscall handler
ASM_ISR_NOERRCODE 48

# Interrupt handler stub function
.align 8
.type int_handler_stub, @function
int_handler_stub:
    # Preserve some registers (as a registers struct)
    pushal
    pushl   %ds
    pushl   %es
    pushl   %fs
    pushl   %gs

    # Switch to kernel data segment descriptor
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

#ifdef INT_CALL_HANDLER_DIRECTLY
    # Find the address of the interrupt handler function (in C)
    movl 48(%esp), %esi             # grab the interrupt value out of the stack
    movl $g_int_handler_table, %eax
    movl (%eax, %esi, 4), %ebx      # grab the address of the C function
    pushl %esp                      # push the address of x86_registers struct
    call *%ebx
    add $4, %esp
#else
    pushl %esp
    call int_handler
    addl $4, %esp
#endif

    # Reload original registers, clean up stack and return
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popal
    addl $8, %esp
    #sti
    iret

# Enable interrupts
#.global _int_enable
#.align 8
#.type _int_enable, @function
#_int_enable:
#    sti
#    ret

# Disable interrupts
#.global _int_disable
#.align 8
#.type _int_disable, @function
#_int_disable:
#    cli
#    ret
