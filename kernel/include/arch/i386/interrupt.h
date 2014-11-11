#ifndef __TREEOS_KERNEL_ARCH_INTERRUPT_H__
#define __TREEOS_KERNEL_ARCH_INTERRUPT_H__

#define INT_NUM_INTERRUPTS    49
#define INT_HANDLER_STUB_LEN  16
#define INT_SYSCALL_INTERRUPT 48

#define INT_CALL_HANDLER_DIRECTLY

#ifndef __treeos_export_asm

#include <kernel/debug.h>
#include <stdbool.h>
#include <stdint.h>

struct registers;

typedef void (int_handler_t)(struct registers *);

#define int_enable()  __asm__ __volatile__ ("sti")
#define int_disable() __asm__ __volatile__ ("cli")

void int_init(void);
void int_register_handler(int interrupt, int_handler_t handler);
void int_register_irq_handler(int irq, int_handler_t handler);

#ifndef INT_CALL_HANDLER_DIRECTLY
void int_handler(struct registers *registers);
#endif

#define int_begin_atomic() (false)
#define int_end_atomic(b)

#endif

#endif
