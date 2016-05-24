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

#ifndef __TREEOS_KERNEL_TTY_H__
#define __TREEOS_KERNEL_TTY_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Common terminal color constants
enum
{
    TTY_COLOR_SCREEN = 0x1F,
    TTY_COLOR_PANIC  = 0x4F,
    TTY_COLOR_WORRY  = 0xE8
};

// Terminal driver initialization/clearing
void tty_init(void);
void tty_remap(void);
void tty_clear(void);

// Printing functions
int tty_putchar(int c);
int tty_putchar_color(int c, uint8_t color);
int tty_puts(const char *str, bool newline);
int tty_puts_color(const char *str, uint8_t color, bool newline);

#endif
