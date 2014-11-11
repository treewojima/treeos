#ifndef __TREEOS_KERNEL_ARCH_CPU_H__
#define __TREEOS_KERNEL_ARCH_CPU_H__

#include <stdint.h>

struct registers
{
    // Saved processor registers
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

    // Interrupt number
    uint32_t int_num;

    // If the processor doesn't explicitly push an error code, the interrupt
    // handler pushes a dummy value so that the stack layout is the same for
    // all interrupts
    uint32_t error_code;

    // The processor pushes these values onto the stack on an interrupt
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
};

void gdt_init(void);
void idt_init(void);
void tss_init(void);
//void clear_bss(void);

uint32_t get_eflags(void);

char *strcat_registers(char *buf, const struct registers *const registers);

/*
 * Functions implemented in assembly (src/arch/i386/cpu_asm.S)
 */
extern void load_gdt(uint32_t gdt_ptr);
extern void load_idt(uint32_t idt_ptr);
extern void load_tss(void);
extern void jump_usermode(void);

#endif
