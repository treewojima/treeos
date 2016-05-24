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

#ifndef __TREEOS_KERNEL_ARCH_CPU_H__
#define __TREEOS_KERNEL_ARCH_CPU_H__

#include <defines.h>

#ifndef TREEOS_I386
#   error included i386-specific header (arch/i386/cpu.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdint.h>

void gdt_init(void);
void idt_init(void);
void tss_init(void);

/*
 * Functions implemented in assembly (src/arch/i386/cpu_asm.S)
 */
void write_gdt(uint32_t gdt_ptr);
void write_idt(uint32_t idt_ptr);
void write_tss(uint16_t index);

uint32_t read_cr0(void);
void write_cr0(uint32_t value);

uint32_t read_cr2(void);

uint32_t read_cr3(void);
void write_cr3(uint32_t value);

uint32_t read_eflags(void);
uint32_t read_esp(void);

void jump_usermode_c(void);

#endif
