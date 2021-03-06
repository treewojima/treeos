/* treeos
 * Copyright (C) 2014-2016 Scott Bishop <treewojima@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

.section .text

// Interrupt handler stub function
.type int_handler_stub, @function
int_handler_stub:
    // Preserve the interrupted thread context
    pushal    // eax, ecx, edx, ebx, esp, ebp, esi, edi
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    // Switch to kernel data segment descriptor
    movw $0x10, %ax
    movw %ax  , %ds
    movw %ax  , %es
    movw %ax  , %fs
    movw %ax  , %gs

    // Find the beginning of the interrupt stack frame
    movl %esp, %ebp
    addl $48 , %ebp

    // Find the address of the interrupt handler function (in C)
    movl (%ebp), %esi               // grab the interrupt value out of the stack
    movl $g_int_handler_table, %eax
    movl (%eax, %esi, 4), %ebx      // grab the address of the C function
    pushl %esp                      // push the address of x86_registers struct
    call *%ebx
    addl $4, %esp

    // If preemption isn't enabled, there's no need to check if rescheduling
    // is necessary
    cmpl $0, g_preemption_enabled
    je int_handler_stub_resume

    // Switch processes
    cmpl $0, g_switch_processes
    je int_handler_stub_resume
    pushl %esp
    call scheduler_advance_stage1 // preserves thread context
    addl $4, %esp
    jmp scheduler_advance_stage2  // switches stack pointer to new process

    // Fall through

.global int_handler_stub_resume
.type int_handler_stub_resume, @function
int_handler_stub_resume:

    // Reload preserved registers from stack
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popal

    // Ignore interrupt number and error code
    addl $8, %esp

    // Jump back into the interrupted thread
    iret

// Macro-defined interrupt functions

.macro ASM_ISR_NOERRCODE inum
    .global isr\inum
    .align 8
    .type isr\inum, @function
    isr\inum:
        pushl $0xDEADBEEF
        pushl $(\inum)
        jmp int_handler_stub
.endm

.macro ASM_ISR_ERRCODE inum
    .global isr\inum
    .align 8
    .type isr\inum, @function
    isr\inum:
        pushl $(\inum)
        jmp int_handler_stub
.endm

.macro ASM_ISR_IRQ irqnum inum
    .global irq\irqnum
    .align 8
    .type irq\irqnum, @function
    irq\irqnum:
        pushl $0xDEADBEEF
        pushl $(\inum)
        jmp int_handler_stub
.endm

// First, our interrupt service routines
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

// Now, our IRQs
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

// Finally the syscall handler
ASM_ISR_NOERRCODE 48
