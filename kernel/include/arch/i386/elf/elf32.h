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

#ifndef __TREEOS_KERNEL_ARCH_I386_ELF32_H__
#define __TREEOS_KERNEL_ARCH_I386_ELF32_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (elf/elf32.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdbool.h>

struct Elf32_Ehdr;

bool elf32_validate_platform(struct Elf32_Ehdr *hdr);

#endif
