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

#include <arch/i386/proc/thread.h>

#include <stdio.h>

char *strcat_registers(char *buf, const struct thread_context *const context)
{
    int pos = 0;
    while (*(buf + pos) != '\0')
        pos++;

    pos += sprintf(buf + pos, "  eax=%x ebx=%x ecx=%x edx=%x\n",
                   context->eax, context->ebx, context->ecx,
                   context->edx);
    pos += sprintf(buf + pos, "  ebp=%x esi=%x edi=%x\n",
                   context->ebp, context->esi, context->edi);
    pos += sprintf(buf + pos, "  ds=%x es=%x fs=%x gs=%x\n",
                   context->ds, context->es, context->fs,
                   context->gs);
    pos += sprintf(buf + pos, "  cs:eip=%x:%x\n",
                   context->cs, context->eip);
    pos += sprintf(buf + pos, "  ss:esp=%x:%x\n",
                   context->user_ss, context->esp);
    pos += sprintf(buf + pos, "  eflags=%x\n",
                   context->eflags);
    pos += sprintf(buf + pos, "  int=%d err=%x\n",
                   context->int_num, context->error_code);


    return buf;
}
