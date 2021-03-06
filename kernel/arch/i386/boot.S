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

// Source: http://wiki.osdev.org/User:Mduft/HigherHalf_Kernel_with_32-bit_Paging
// TODO: comment the code!

// This section needs to be at the very beginning of the binary for the kernel
// to be recognized as a multiboot-compatible file
.section .multiboot

#include <kernel/multiboot.h>

// Declare constants used for creating a multiboot header
#define MULTIBOOT_FLAGS (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)

// The multiboot header itself
.align 0x100
    .long MULTIBOOT_HEADER_MAGIC
    .long MULTIBOOT_FLAGS
    .long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_FLAGS)

// Data section - includes the initial page directories/tables
.section .data

// Reserve space for the bootstrap kernel page directory and table
.align 0x1000
.global g_real_kernel_page_dir
g_real_kernel_page_dir:
    .space 0x1000, 0x00
.global g_real_kernel_page_table
g_real_kernel_page_table:
    .space 0x1000, 0x00
//.global g_kernel_low_page_table
//g_kernel_low_page_table:
//    .space 0x1000, 0x00

// The kernel bootstrap stack
//.section .bootstrap_stack, "aw", @nobits
.global g_kernel_stack_bottom
g_kernel_stack_bottom:
    //.skip 0x4000 // 16 KiB
    .space 0x4000, 0x00
.global g_kernel_stack_top
g_kernel_stack_top:

msg_panic:
    .asciz "PANIC!"

msg_kmain_returned:
    .asciz "unexpected return from kmain()"

msg_function_name:
    .asciz "higher_half_start"

.section .text
.code32

// For debug purposes
.set VIDEO_RAM   , 0xB8000
.set VIDEO_DWORDS, 0x3E8

// Kernel entry point; called after control is handed over from the bootloader
.global _start
.type _start, @function
_start:
    // Set the stack/base pointers to account for the offset into the higher half
    movl $g_kernel_stack_top  , %esp
    subl $g_kernel_high_offset, %esp

    // Initialize the bootstrap paging mechanism
    call init_bootstrap_paging

    // Adjust stack/base pointers to the proper virtual address
    addl $g_kernel_high_offset, %esp
    movl %esp              , %ebp

    jmp higher_half_start

.global higher_half_start
.type higher_half_start, @function
higher_half_start:
    // Clear the screen
    movl  $VIDEO_RAM   , %edi
    movl  $VIDEO_DWORDS, %ecx
    pushl %eax
    movl  $0x07200720  , %eax
    rep stosl
    popl  %eax

    // Initialize core kernel functionality and push the multiboot arguments
    pushl %ebx
    pushl %eax
    call  kinit

    // Call global constructors
    call _init

    // Call the main kernel function
    call kmain

    // kmain shouldn't have returned...
    pushl $msg_kmain_returned
    pushl $112
    pushl $msg_function_name
    call _panic
    jmp the_end2

the_end:
    movl $msg_panic, %eax
    call boot_print_msg
the_end2:
    cli
    hlt
.Lhang:
    jmp .Lhang

.type boot_print_msg, @function
boot_print_msg:
    pushl %edx
    pushl %ebx

    movl $VIDEO_RAM, %edx

    print_loop:
        movb (%eax) , %bl
        xorb %bh    , %bh
        cmpb $0x0   , %bl
        je   end_print
        orw  $0x4F00, %bx
        movw %bx    , (%edx)
        add  $0x2   , %edx
        inc  %eax
        jmp  print_loop
    end_print:

    popl %ebx
    popl %edx
    ret

// Initialize bootstrap paging
.type init_bootstrap_paging, @function
init_bootstrap_paging:
    pushl %eax
    pushl %ebx
    pushl %edx
    pushl %ecx

    movl  $g_real_kernel_page_dir   , %eax
    subl  $g_kernel_high_offset, %eax

    movl $g_real_kernel_page_table , %ebx
    subl $g_kernel_high_offset, %ebx
    orl  $0x7                 , %ebx
    movl %ebx                 , (%eax)

    pushl %eax
    movl  $g_kernel_high_offset, %eax
    shrl  $22                  , %eax
    movl  $4                   , %ecx
    mull  %ecx
    movl  %eax                 , %edx
    popl  %eax

    pushl %eax
    addl  %edx, %eax

    movl $g_real_kernel_page_table , %ebx
    subl $g_kernel_high_offset, %ebx
    orl  $0x7                 , %ebx
    movl %ebx                 , (%eax)

    popl %ebx
    movl $0x100000, %ecx
    idmap_first_mb_loop:
        movl %ecx   , %edx
        call map_page
        subl $0x1000, %ecx
        jnz  idmap_first_mb_loop

    movl $g_kernel_boot_addr  , %ecx
    movl $g_kernel_end        , %eax
    subl $g_kernel_high_offset, %eax

    map_kernel:
        movl %ecx, %edx
        call map_page

        pushl %ecx
        addl  $g_kernel_high_offset, %ecx
        call  map_page
        popl  %ecx

        addl $0x1000, %ecx
        cmpl %eax   , %ecx
        jle  map_kernel

    // Map the last entry of the PDE to itself
    movl %ebx       , %eax
    addl $4092      , %eax   // page dir index 1023 * 4 bytes in size
    movl %ebx       , %ecx
    andl $0xFFFFF000, %ecx   // strip off the last 12 flag bits
    orl  $0x7       , %ecx
    movl %ecx       , (%eax)

    movl %ebx      , %cr3
    movl %cr0      , %eax
    orl  $(1 << 31), %eax
    movl %eax      , %cr0

    popl %ecx
    popl %edx
    popl %ebx
    popl %eax
    ret

.type map_page, @function
map_page:
    pushl %eax
    pushl %ebx
    pushl %edx
    pushl %ecx

    pushl %edx
    pushl %ecx

    movl %ecx  , %eax
    shrl $22   , %eax
    movl $4    , %ecx
    mull %ecx
    addl %eax  , %ebx
    movl (%ebx), %eax

    movl %eax, %ecx
    andl $0x1, %ecx
    cmpl $0x0, %ecx
    je   the_end

    andl $0xFFFFF000, %eax

    popl  %edx
    pushl %eax
    movl  %edx  , %eax
    shrl  $0xC  , %eax
    andl  $0x3FF, %eax
    movl  $0x4  , %ecx
    mull  %ecx
    movl  %eax  , %ebx
    popl  %eax

    popl %edx
    addl %ebx, %eax
    orl  $0x7, %edx
    movl %edx, (%eax)

    popl %edx
    popl %ecx
    popl %ebx
    popl %eax
    ret
