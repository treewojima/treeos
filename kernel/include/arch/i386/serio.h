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

#ifndef __TREEOS_KERNEL_ARCH_SERIO_H__
#define __TREEOS_KERNEL_ARCH_SERIO_H__

#include "defines.h"
#ifndef TREEOS_I386
#   error included i386-specific header (serio.h) in non-i386 build
#endif

#define SERIO_COM1 0x3F8
#define SERIO_COM2 0x2F8
#define SERIO_COM3 0x3E8
#define SERIO_COM4 0x2E8

#ifndef TREEOS_EXPORT_ASM

#include <stdbool.h>

void serio_init(int com);
bool serio_data_incoming(int com);
char serio_readchar(int com);
bool serio_data_outgoing(int com);
void serio_writechar(int com, char c);

int serio_puts(int com, const char *str, bool newline);

#endif

#endif
