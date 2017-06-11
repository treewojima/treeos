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

#include <kernel/tasks/idle.h>

#include <arch/i386/asm.h>
#include <kernel/interrupt.h>
#include <kernel/proc/process.h>
#include <stdio.h>

struct process g_kernel_idle_task;

void kernel_idle_task(void)
{
    for (;;)
    {
        printf("idle task: ");
        dbg_print_esp();
        process_yield();
        while (g_kernel_idle_task.flags & PROC_DEBUG_YIELD)
        {
            asm_hlt();
        }
    }
}
