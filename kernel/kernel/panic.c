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

#include <kernel/panic.h>

#include <arch/i386/proc/thread.h> // For struct thread_context and strcat_registers
#include <arch/i386/serio.h>
#include <kernel/reboot.h>
#include <kernel/tty.h>
#include <stdio.h>
#include <string.h>

static char msg_buf[MAX_PANIC_BUF];

void _panic(const char *file, int line, const char *msg)
{
    memset(msg_buf, 0, MAX_PANIC_BUF);
    sprintf(msg_buf, "panic (%s:%d): %s", file, line, msg);
    serio_puts(SERIO_COM1, msg_buf, true);
    tty_puts_color(msg_buf, TTY_COLOR_PANIC, true);
    halt();
}

void _worry(const char *file, int line, const char *msg)
{
    memset(msg_buf, 0, MAX_PANIC_BUF);
    sprintf(msg_buf, "worry (%s:%d): %s\n", file, line, msg);
    serio_puts(SERIO_COM1, msg_buf, true);
    tty_puts_color(msg_buf, TTY_COLOR_WORRY, true);
}

void _panic_r(const char *file,
              int line,
              const char *msg,
              const struct thread_context *const context)
{
    memset(msg_buf, 0, MAX_PANIC_BUF);
    sprintf(msg_buf, "panic (%s:%d): %s\n", file, line, msg);
    strcat_registers(msg_buf, context);
    serio_puts(SERIO_COM1, msg_buf, true);
    tty_puts_color(msg_buf, TTY_COLOR_PANIC, true);
    halt();
}
