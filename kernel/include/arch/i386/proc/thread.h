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

#ifndef __TREEOS_ARCH_I386_PROC_THREAD_H__
#define __TREEOS_ARCH_I386_PROC_THREAD_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (proc/thread.h) in non-i386 build
#endif

// This needs to be kept up to date with the size/members of struct thread_context
#define THREAD_CONTEXT_OFFSET 28

#ifndef TREEOS_EXPORT_ASM

#include <stdint.h>

// Thread context of execution
struct thread_context
{
    // Pushed manually
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    // Pushed by pusha
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    // From the interrupt stack frame
    uint32_t int_num;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t user_esp; // these only contain useful values
    uint32_t user_ss;  // if the process runs in user mode
} PACKED;

struct process;

// TODO: a lot
struct thread
{
    struct thread_context *context;

    struct process *owner;    

    struct thread *next;
};

#if 0
void thread_create(struct thread *thread,
                   entrypoint entry,
                   struct page_dir_entry *pgdir,
                   uint8_t flags);
#endif

char *strcat_registers(char *buf, const struct thread_context *const context);

#endif

#endif
