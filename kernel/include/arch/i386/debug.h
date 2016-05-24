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

#ifndef __TREEOS_ARCH_I386_DEBUG_H__
#define __TREEOS_ARCH_I386_DEBUG_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (arch/i386/debug.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#ifndef TREEOS_DEBUG
#   define KASSERT(cond)
#   define KASSERT_MSG(cond, msg)
#   define KASSERT_WORRY(cond)
#   define KASSERT_WORRY_MSG(cond, msg)
#else

#include <arch/i386/cpu.h>
#include <stdio.h>

#define dbg_print_esp() printf("esp = %p\n", read_esp())

#endif

#endif
