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

#include <kernel/proc/process.h>

#include <kernel/interrupt.h>
#include <kernel/proc/scheduler.h>
#include <stdlib.h>

struct process *process_create(struct process *process)
{
    if (!process)
    {
        process = kcalloc(1, sizeof(*process));
        process->flags |= PROC_FREE;
    }

    return process;
}

void process_yield(void)
{
    bool lock = int_lock_region();
    g_current_process->flags |= PROC_DEBUG_YIELD;
    int_unlock_region(lock);
}
