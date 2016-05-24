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

// Load the global descriptor table
// Parameters:
//     uint32_t - pointer to GDT records
//
// Returns:
//     void
//
.global write_gdt
.type write_gdt, @function
write_gdt:
    pushl %ebp
    movl  %esp, %ebp

    // Install the new GDT
    movl 8(%ebp), %eax
    lgdt (%eax)

    // Reload segment registers
    movw $0x10, %ax
    movw %ax  , %ds
    movw %ax  , %es
    movw %ax  , %fs
    movw %ax  , %gs
    movw %ax  , %ss

    // Longjump to code segment
    ljmp $0x08, $.Lhere
.Lhere:
    movl %ebp, %esp
    popl %ebp
    ret

// Load the interrupt descriptor table
// Parameters:
//     uint32_t - pointer to IDT records
//
// Returns:
//     void
//
.global write_idt
.type write_idt, @function
write_idt:
    pushl %ebp
    movl  %esp, %ebp

    // This code should be fairly self-explanatory
    movl 8(%ebp), %eax
    lidt (%eax)

    movl %ebp, %esp
    popl %ebp
    ret

// Load the specified TSS into the task register
// Parameters:
//     uint16_t - index/offset of TSS entry in GDT
//
// Returns:
//     void
//
.global write_tss
.type write_tss, @function
write_tss:
    pushl %ebp
    movl  %esp, %ebp

    movw 8(%ebp), %ax
    ltr  %ax

    movl %ebp, %esp
    popl %ebp
    ret

// Read the value of the cr0 register
// Parameters:
//     none
//
// Returns:
//     uint32_t
//
.global read_cr0
.type read_cr0, @function
read_cr0:
    movl %cr0, %eax
    ret

// Write a value to the cr0 register
// Parameters:
//     uint32_t - new value
//
// Returns:
//     void
//
.global write_cr0
.type write_cr0, @function
write_cr0:
    pushl %ebp
    movl  %esp, %ebp

    movl 8(%ebp), %eax
    movl %eax, %cr0

    movl %ebp, %esp
    popl %ebp
    ret

// Read the value of the cr2 register
// Parameters:
//     none
//
// Returns:
//     uint32_t
//
.global read_cr2
.type read_cr2, @function
read_cr2:
    movl %cr2, %eax
    ret

// Read the value of the cr3 register
// Parameters:
//     none
//
// Returns:
//     uint32_t
//
.global read_cr3
.type read_cr3, @function
read_cr3:
    movl %cr3, %eax
    ret

// Write a value to the cr3 register
// Parameters:
//     uint32_t - new value
//
// Returns:
//     void
//
.global write_cr3
.type write_cr3, @function
write_cr3:
    pushl %ebp
    movl  %esp, %ebp

    movl 8(%ebp), %eax
    movl %eax, %cr3

    movl %ebp, %esp
    popl %ebp
    ret

// Read the value of the eflags register
// Parameters:
//     none
//
// Returns:
//     uint32_t
//
.global read_eflags
.type read_eflags, @function
read_eflags:
    pushfl
    popl %eax
    ret

// Read the value of the stack pointer, adjusted to its value before
// this function call
// Parameters:
//     none
//
// Returns:
//     uint32_t
//
.global read_esp
.type read_esp, @function
read_esp:
    leal 4(%esp), %eax
    ret
