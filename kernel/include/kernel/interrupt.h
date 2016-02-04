#ifndef __TREEOS_KERNEL_INTERRUPT_H__
#define __TREEOS_KERNEL_INTERRUPT_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdbool.h>
#include <stdint.h>

struct registers;

typedef void (int_handler_t)(struct registers *);

void int_init(void);

// Interrupt and IRQ handlers
void int_register_handler(uint8_t interrupt, int_handler_t handler);
void int_register_irq_handler(uint8_t irq, int_handler_t handler);

// IRQ masking
bool int_mask_irq(uint8_t irq);
bool int_unmask_irq(uint8_t irq);

#ifdef TREEOS_I386
#   include <arch/i386/interrupt.h>
#endif

#endif
