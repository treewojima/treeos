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

#include <arch/i386/asm.h>
#include <kernel/interrupt.h>
#include <kernel/reboot.h>
#include <kernel/tty.h>

extern void liballoc_dump(void);

void reboot(void)
{
    tty_puts_color("rebooting", TTY_COLOR_PANIC, false);

    // Force a triple fault by loading an empty IDT and triggering an interrupt
    uint64_t fake_idt = 0;
    write_idt((uint32_t)&fake_idt);

    int_enable();
    asm_int(3);

    for (;;);
    __builtin_unreachable();
}

void halt(void)
{
    tty_puts_color("halted", TTY_COLOR_PANIC, false);

    liballoc_dump();

    asm_cli();
    asm_hlt();

    for (;;);
    __builtin_unreachable();
}
