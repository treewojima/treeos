#include <kernel/interrupt.h>

#include <arch/i386/ioport.h>
#include <arch/i386/proc/thread.h>
#include <kernel/debug.h>
#include <kernel/panic.h>
#include <kernel/reboot.h>
#include <kernel/tty.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//extern inline bool int_lock_region(void);

// PIC I/O ports
#define PIC_MASTER_CMD  0x20
#define PIC_MASTER_DATA (PIC_MASTER_CMD + 1)
#define PIC_SLAVE_CMD   0xA0
#define PIC_SLAVE_DATA  (PIC_SLAVE_CMD + 1)

// Command definitions
#define ICW1         0x11  // ICW1 - ICW4 needed, cascade mode, interval = 8,
                           // edge triggered. (I think interval is irrelevant
                           // for x86.)
#define ICW2_MASTER  0x20  // put IRQs 0-7 at 0x20 (above Intel reserved ints)
#define ICW2_SLAVE   0x28  // put IRQs 8-15 at 0x28
#define ICW3_MASTER  0x04  // IR2 connected to slave
#define ICW3_SLAVE   0x02  // slave has ID 2
#define ICW4         0x01  // 8086 mode, no auto-EOI, non-buffered mode, not
                           // special fully nested mode

// Get the master and slave parts of an IRQ mask
#define MASTER(mask) ((mask) & 0xFF)
#define SLAVE(mask)  (((mask) >> 8) & 0xFF)

// External IRQs range from 32-47
#define FIRST_IRQ_INT  32
#define NUM_IRQS       16
#define VALID_IRQ(irq) ((irq) < NUM_IRQS)

// Global table of interrupt handler functions
int_handler_t g_int_handler_table[INT_NUM_INTERRUPTS];

// IRQ handler linked list
struct irq_handler
{
    int_handler_t handler;
    struct irq_handler *next;
};

// Each entry in this array is the head of that IRQ's linked list
static struct irq_handler *irq_handler_list[NUM_IRQS];
//static int_handler_t *irq_handler_table[NUM_IRQS];

// IRQ mask
static uint16_t irq_mask;
#define IRQ_MASK_MASTER(m) ((uint8_t)((m) & 0xFF))
#define IRQ_MASK_SLAVE(m)  ((uint8_t)(((m) >> 8) & 0xFF))

static void set_raw_mask(uint16_t mask);
static void remap_pic(void);

// Default handlers
static void unexpected_int_handler(const struct thread_context *const context);
static void irq_int_handler(const struct thread_context *const context);
static void syscall_int_handler(const struct thread_context *const context);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PUBLIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Initialize the interrupt system
 * Parameters:
 *     none
 *
 * Returns:
 *     void
 */
void int_init(void)
{
    //bool iflag = int_begin_atomic();
    //_int_disable();

    // Remap/initialize the 8259 PIC and load the IDT pointer
    idt_init();
    remap_pic();

    memset(g_int_handler_table, 0, sizeof(int_handler_t) * INT_NUM_INTERRUPTS);
    memset(irq_handler_list, 0, sizeof(struct irq_handler *) * NUM_IRQS);

    // Initialize the default interrupt handlers
    for (int i = 0; i < INT_NUM_INTERRUPTS; i++)
    {
        if (i >= FIRST_IRQ_INT &&
            i < FIRST_IRQ_INT + NUM_IRQS)
        {
            g_int_handler_table[i] = irq_int_handler;
        }
        else if (i == INT_SYSCALL_INTERRUPT)
        {
            g_int_handler_table[i] = syscall_int_handler;
        }
        else
        {
            g_int_handler_table[i] = unexpected_int_handler;
        }
    }

    printf("[cpu] initialized interrupt and IRQ handlers\n");
}

/* Set an interrupt handler routine
 * Parameters:
 *     interrupt - the interrupt number (NOT the IRQ number)
 *     handler - pointer to the handler function
 *
 * Returns:
 *     void
 */
void int_register_handler(uint8_t interrupt, int_handler_t handler)
{
    KASSERT(interrupt < INT_NUM_INTERRUPTS);

    // Make sure we're not trying to change any special interrupts
    PANIC_IF(interrupt == INT_SYSCALL_INTERRUPT,
             "tried to change syscall interrupt handler");
    PANIC_IF(interrupt >= FIRST_IRQ_INT && interrupt < FIRST_IRQ_INT + NUM_IRQS,
             "tried to change IRQ-aware interrupt handler");

    // If handler is null, reset the handler function to the default
    if (!handler)
    {
        g_int_handler_table[interrupt] = unexpected_int_handler;
    }
    else
    {
        g_int_handler_table[interrupt] = handler;
    }
}

/* Register an IRQ handler
 * Parameters:
 *     irq - the IRQ number (NOT the raw interrupt number)
 *     handler - pointer to the handler function
 *
 * Returns:
 *     void
 */
void int_register_irq_handler(uint8_t irq, int_handler_t handler)
{
    KASSERT(VALID_IRQ(irq));
    KASSERT(irq != 2);
    KASSERT(handler);

    // NOTE: This assumes that memory management is up and running by the time
    //       this function is called!
    struct irq_handler *new_node = kmalloc(sizeof(struct irq_handler));
    KASSERT(new_node);
    new_node->handler = handler;
    new_node->next = NULL;

    // Append the handler to the end of the list
    struct irq_handler **node = &irq_handler_list[irq];
    if (!*node)
    {
        *node = new_node;
    }
    else
    {
        while ((*node)->next)
            *node = (*node)->next;

        (*node)->next = new_node;
    }
}

/* Mask an IRQ
 * Parameters:
 *     irq - the IRQ number (NOT the raw interrupt number)
 *
 * Returns:
 *     true if IRQ was masked, false if it was already masked
 */
bool int_mask_irq(uint8_t irq)
{
    KASSERT(VALID_IRQ(irq));
    KASSERT(irq != 2);

    uint16_t irq_bit = 1 << irq;

    // Check if the IRQ is already masked
    if (irq_mask & irq_bit)
    {
        char buf[40];
        sprintf(buf, "masking already masked IRQ %u\n", irq);
        worry(buf);
        return false;
    }

    set_raw_mask(irq_mask | irq_bit);
    return true;
}

/* Unmask an IRQ
 * Parameters:
 *     irq - the IRQ number (NOT the raw interrupt number)
 *
 * Returns:
 *     true if IRQ was unmasked, false if it was already unmasked
 */
bool int_unmask_irq(uint8_t irq)
{
    KASSERT(VALID_IRQ(irq));
    KASSERT(irq != 2);

    uint16_t irq_bit = 1 << irq;

    // Check if the IRQ is already unmasked
    if (!(irq_mask & irq_bit))
    {
        char buf[40];
        sprintf(buf, "masking already masked IRQ %u\n", irq);
        worry(buf);
        return false;
    }

    set_raw_mask(irq_mask & ~irq_bit);
    return true;
}

/* Gets the mask status of an IRQ
 * Parameters:
 *     irq - the IRQ number (NOT the raw interrupt number)
 *
 * Returns:
 *     true if IRQ is unmasked, false otherwise
 */
bool int_irq_enabled(uint8_t irq)
{
    KASSERT(VALID_IRQ(irq));

    return !(irq_mask & (1 << irq));
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  STATIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Set the IRQ mask on the PIC
void set_raw_mask(uint16_t mask)
{
    if (irq_mask == mask) return;

    uint8_t master_mask = IRQ_MASK_MASTER(mask);
    uint8_t slave_mask = IRQ_MASK_SLAVE(mask);

    if (master_mask != IRQ_MASK_MASTER(irq_mask))
    {
        ioport_outb(PIC_MASTER_DATA, master_mask);
    }

    if (slave_mask != IRQ_MASK_SLAVE(irq_mask))
    {
        ioport_outb(PIC_SLAVE_DATA, slave_mask);
    }

    irq_mask = mask;
}

// Initialize/remap the PIC
static void remap_pic(void)
{
    ioport_outb(PIC_MASTER_CMD,  ICW1);        // ICW1 to master
    ioport_outb(PIC_SLAVE_CMD,   ICW1);        // ICW1 to slave
    ioport_outb(PIC_MASTER_DATA, ICW2_MASTER); // ICW2 to master
    ioport_outb(PIC_SLAVE_DATA,  ICW2_SLAVE);  // ICW2 to slave
    ioport_outb(PIC_MASTER_DATA, ICW3_MASTER); // ICW3 to master
    ioport_outb(PIC_SLAVE_DATA,  ICW3_SLAVE);  // ICW3 to slave
    ioport_outb(PIC_MASTER_DATA, ICW4);        // ICW4 to master
    ioport_outb(PIC_SLAVE_DATA,  ICW4);        // ICW4 to slave

    ioport_outb(PIC_MASTER_DATA, 0xFB); // mask all master IRQs except #2 (the cascade)
    ioport_outb(PIC_SLAVE_DATA,  0xFF); // mask all slave IRQs
    irq_mask = 0xFFFB;
}

static void unexpected_int_handler(const struct thread_context *const context)
{
    static const char *error_msg[] =
    {
    /* 0  */ "divide by zero",
    /* 1  */ "debugger (this shouldn't EVER be fired)",
    /* 2  */ "non-maskable interrupt",
    /* 3  */ "breakpoint",
    /* 4  */ "overflow",
    /* 5  */ "range exceeded for BOUND instruction",
    /* 6  */ "invalid/undefined opcode",
    /* 7  */ "device not available (no math coprocessor?)",
    /* 8  */ "double fault",
    /* 9  */ "coprocessor segment overflow (this shouldn't EVER be fired)",
    /* 10 */ "invalid TSS",
    /* 11 */ "segment not present",
    /* 12 */ "stack-segment fault",
    /* 13 */ "general protection fault",
    /* 14 */ "page fault",
    /* 15 */ "Intel reserved (this shouldn't EVER be fired)",
    /* 16 */ "x87 FPU floating-point error",
    /* 17 */ "alignment check fault",
    /* 18 */ "machine check fault",
    /* 19 */ "SIMD floating-point exception",
    /* 20 */ "virtualization/EPT exception"
    };

    // Make sure we have sane values
    //const int int_num = registers->int_num;
    //KASSERT(registers->int_num >= 0);
    //WORRY_IF(int_num >= FIRST_IRQ_INT && int_num < FIRST_IRQ_INT + NUM_IRQS,
    //         "calling unexpected_int_handler instead of so");

    char msg_buf[MAX_PANIC_BUF];
    if (context->int_num <= 20)
    {
        sprintf(msg_buf, "unhandled interrupt %d: %s",
                context->int_num, error_msg[context->int_num]);
    }
    else if (context->int_num < 32)
    {
        // All these are Intel-reserved, so just reuse the int15 string
        sprintf(msg_buf, "unhandled interrupt %d: %s",
                context->int_num, error_msg[15]);
    }
    else
    {
        sprintf(msg_buf, "unhandled interrupt %d", context->int_num);
    }

    // We ain't coming back from this one.
    panic_r(msg_buf, context);
}

static void irq_int_handler(const struct thread_context *const context)
{
    const uint8_t irq = context->int_num - FIRST_IRQ_INT;
    KASSERT(VALID_IRQ(irq));

    for (struct irq_handler *node = irq_handler_list[irq];
         node != NULL;
         node = node->next)
    {
        KASSERT(node->handler);
        node->handler(context);
    }

    // Send an EOI signal to the master controller
    // (and slave controller if necessary)
    if (irq >= 8) ioport_outb(PIC_SLAVE_CMD, 0x20);
    ioport_outb(PIC_MASTER_CMD, 0x20);
}

static void syscall_int_handler(const struct thread_context *const context)
{
    printf("\nreceived syscall %d\n", context->eax);
    switch (context->eax)
    {
    case 1:
        {
            char *str = (char *)context->ecx;
            tty_puts(str, true);
        }
        break;

    default: break;
    }
}
