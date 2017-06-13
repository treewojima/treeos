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

//#include <arch/i386/proc/thread.h>

.section .text

.global scheduler_bootstrap_thread
.type scheduler_bootstrap_thread, @function
scheduler_bootstrap_thread:
    // Disable interrupts (will re-enable automatically)
    cli

    // This function will never return properly, so may as well wipe part of the
    // call stack frame to get to the function arguments
    addl $4  , %esp

    // Set g_current_process to the passed pointer
    movl (%esp)            , %eax
    movl %eax              , g_current_process

    // Grab a pointer to the thread context
    movl (%eax), %edx
    movl (%edx), %ebp

    // Since we're bootstrapping a kernel thread, iret won't pop ss/esp off the
    // stack - so just move the stack pointer to where the new stack will be
    movl 28(%ebp), %esp

    // Twiddle eflags to re-enable interrupts after iret
    pushfl
    popl   %eax
    orl    $0x200, %eax
    pushl  %eax

    pushl 60(%ebp) // cs
    pushl 56(%ebp) // eip

    // Jump into the thread
    iret

.global scheduler_advance_stage2
.type scheduler_advance_stage2, @function
scheduler_advance_stage2:
    // Set the stack pointer to the new process's stack
    movl g_current_process, %eax
    movl (%eax)           , %edx
    movl (%edx)           , %esp

    movl $0, g_switch_processes

    jmp int_handler_stub_resume
