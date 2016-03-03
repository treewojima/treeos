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
