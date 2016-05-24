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

#ifndef __TREEOS_KERNEL_PROC_PROCESS_H__
#define __TREEOS_KERNEL_PROC_PROCESS_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdint.h>

#ifdef TREEOS_I386
#   include <arch/i386/pmm/paging.h>
#   include <arch/i386/proc/thread.h>
#endif

struct process
{
    // Thread pointer must be the first member of this structure, or else
    // thread bootstrapping code will break!
    struct thread *thread;

    struct page_dir_entry *pgdir;
    uint8_t flags;

    uint32_t quantum;

    struct process *next;
};

// Process flags
#define PROC_FREE                (1 << 0)
#define PROC_DEBUG_YIELD         (1 << 1)
#define PROC_FREE_THREAD_CONTEXT (1 << 2)

#define DEFAULT_QUANTUM 100

struct process *process_create(struct process *process);

void process_yield(void);



#endif
