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

#ifndef __TREEOS_KERNEL_ARCH_IOPORT_H__
#define __TREEOS_KERNEL_ARCH_IOPORT_H__

#include <defines.h>
#include <stdint.h>

#ifndef TREEOS_I386
#   error included i386-specific header (ioport.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#define USE_ASM_IOPORT

#ifdef USE_ASM_IOPORT
#   define ioport_inb  asm_ioport_inb
#   define ioport_outb asm_ioport_outb
#   define ioport_inw  asm_ioport_inw
#   define ioport_outw asm_ioport_outw
#   define ioport_inl  asm_ioport_inl
#   define ioport_outl asm_ioport_outl
#else
#   define ioport_inb  c_ioport_inb
#   define ioport_outb c_ioport_outb
#   define ioport_inw  c_ioport_inw
#   define ioport_outw c_ioport_outw
#   define ioport_inl  c_ioport_inl
#   define ioport_outl c_ioport_outl
#endif

uint8_t ioport_inb(uint16_t port);
void ioport_outb(uint16_t port, uint8_t value);

uint16_t ioport_inw(uint16_t port);
void ioport_outw(uint16_t port, uint16_t value);

uint32_t ioport_inl(uint16_t port);
void ioport_outl(uint16_t port, uint32_t value);

#endif
