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

#include <arch/i386/proc/task.h>

#include <arch/i386/cpu.h>
#include <kernel/const.h>
#include <stdio.h>
#include <string.h>

struct tss_entry g_kernel_tss;

void tss_init(void)
{
    memset(&g_kernel_tss, 0, sizeof(g_kernel_tss));

    // Set the default kernel stack pointer
    g_kernel_tss.esp0 = KERN_STACK_TOP;

    // Set the default kernel stack segment
    g_kernel_tss.ss0 = KERN_DATA_SELECTOR;

    // Load the TSS using the offset in the GDT (0x28), plus some control bits
    write_tss(0x2B);

    printf("[cpu] initialized task register\n");
}
