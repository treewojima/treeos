#ifndef __TREEOS_KERNEL_INTERRUPT_H__
#define __TREEOS_KERNEL_INTERRUPT_H__

#include "defines.h"

#ifdef TREEOS_I386
#   include <arch/i386/interrupt.h>
#endif

struct registers;

typedef void (int_handler_t)(struct registers *);

void int_init(void);
void int_register_handler(int interrupt, int_handler_t handler);
void int_register_irq_handler(int irq, int_handler_t handler);

#ifndef INT_CALL_HANDLER_DIRECTLY
void int_handler(struct registers *registers);
#endif

#endif
