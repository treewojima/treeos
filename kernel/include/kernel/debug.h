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

#ifndef __TREEOS_KERNEL_DEBUG_H__
#define __TREEOS_KERNEL_DEBUG_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <arch/i386/debug.h>

#ifndef TREEOS_DEBUG
#   define KASSERT(cond)
#   define KASSERT_MSG(cond, msg)
#   define KASSERT_WORRY(cond)
#   define KASSERT_WORRY_MSG(cond, msg)
#else

#include <kernel/panic.h>

#define KASSERT(cond) \
    PANIC_IF(!(cond), "KASSERT failed: " # cond)

#define KASSERT_MSG(cond, msg) \
    PANIC_IF(!(cond), "KASSERT failed: " # msg)

#define KASSERT_WORRY(cond) \
    WORRY_IF(!(cond), "KASSERT failed: " # cond)

#define KASSERT_WORRY_MSG(cond, msg) \
    WORRY_IF(!(cond), "KASSERT failed: " # msg)

#endif

#endif
