#ifndef __TREEOS_KERNEL_INTERRUPT_H__
#define __TREEOS_KERNEL_INTERRUPT_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#ifdef TREEOS_I386
#   include <arch/i386/interrupt.h>
#endif

#include <stdbool.h>
#include <stdint.h>

struct thread_context;

typedef void (*int_handler_t)(const struct thread_context *const);

void int_init(void);

// Interrupt and IRQ handlers
void int_register_handler(uint8_t interrupt, int_handler_t handler);
void int_register_irq_handler(uint8_t irq, int_handler_t handler);

// IRQ masking
bool int_mask_irq(uint8_t irq);
bool int_unmask_irq(uint8_t irq);
bool int_irq_enabled(uint8_t irq);

// Functions/macros that disable interrupts for critical regions
static inline bool int_lock_region(void) FORCE_INLINE;
static inline bool int_lock_region(void)
{
    //bool lock = int_enabled();
    //if (lock) int_disable();
    bool lock = int_irq_enabled(IRQ_PIT);
    if (lock) int_mask_irq(IRQ_PIT);
    return lock;
}

#if 0
#   define int_unlock_region(lock) do { if (lock) int_unmask_irq(IRQ_PIT); } while (0)
#else
static inline void int_unlock_region(bool lock) FORCE_INLINE;
static inline void int_unlock_region(bool lock)
{
    if (lock) int_unmask_irq(IRQ_PIT);
}
#endif

#endif
