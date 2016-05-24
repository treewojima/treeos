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

#ifndef __TREEOS_KERNEL_ARCH_INTERRUPT_H__
#define __TREEOS_KERNEL_ARCH_INTERRUPT_H__

#include "defines.h"
#ifndef TREEOS_I386
#   error included i386-specific header (interrupt.h) in non-i386 build
#endif

#define INT_NUM_INTERRUPTS    49
#define INT_HANDLER_STUB_LEN  16
#define INT_SYSCALL_INTERRUPT 48

#define IRQ_PIT 0
#define IRQ_KBD 1

#ifndef TREEOS_EXPORT_ASM

#include <arch/i386/cpu.h>
#include <kernel/debug.h>

#define int_enabled() (read_eflags() & (1 << 9))

#define int_enable()                  \
    do                                \
    {                                 \
        KASSERT(!int_enabled());      \
        __asm__ __volatile__ ("sti"); \
    } while (0);

#define int_disable()                 \
    do                                \
    {                                 \
        KASSERT(int_enabled());       \
        __asm__ __volatile__ ("cli"); \
    } while (0);

#endif

#endif
