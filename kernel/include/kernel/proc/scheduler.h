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

#ifndef __TREEOS_KERNEL_PROC_SCHEDULER_H__
#define __TREEOS_KERNEL_PROC_SCHEDULER_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#ifdef TREEOS_I386
#   include <arch/i386/proc/scheduler.h>
#endif

#include <kernel/proc/process.h>
#include <stdbool.h>

extern struct process *g_current_process;

// Initialize the scheduling system
void scheduler_init(void);

// Add a process to the queue
bool scheduler_add_process(struct process *p);

// Find the next best process to switch to
struct process *scheduler_find_next_best(void);

// Only ever called from the interrupt handler
void scheduler_advance_stage1(const struct thread_context *const context);

#endif
