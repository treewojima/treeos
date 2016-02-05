#ifndef __TREEOS_KERNEL_ARCH_CPU_H__
#define __TREEOS_KERNEL_ARCH_CPU_H__

#include "defines.h"
#ifndef TREEOS_I386
#   error included i386-specific header (cpu.h) in non-i386 build
#endif

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdint.h>

// Thread context
struct registers
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t epi;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t int_num;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} PACKED;

void gdt_init(void);
void idt_init(void);
void tss_init(void);
//void clear_bss(void);

char *strcat_registers(char *buf, const struct registers *const registers);

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

void jump_usermode_c(void);

#endif
