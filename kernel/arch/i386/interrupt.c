#include <arch/i386/cpu.h>
#include <arch/i386/interrupt.h>
#include <arch/i386/ioport.h>
#include <kernel/debug.h>
#include <kernel/panic.h>
#include <kernel/reboot.h>
#include <kernel/tty.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// i8259A PIC definitions
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
#define FIRST_IRQ_INT 32
#define NUM_IRQS  16
#define VALID_IRQ(irq) ((irq) >= 0 && (irq) < NUM_IRQS)

// Global table of interrupt handler functions
int_handler_t *g_int_handler_table[INT_NUM_INTERRUPTS];

// Private table of IRQ handlers
int_handler_t *irq_handler_table[NUM_IRQS];

// Static prototypes
static void remap_pic(void);
static void unexpected_int_handler(struct registers *registers);
static void irq_int_handler(struct registers *registers);
static void syscall_int_handler(struct registers *registers);

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
    memset(irq_handler_table, 0, sizeof(int_handler_t) * NUM_IRQS);

    // Initialize the default interrupt handlers
    for (int i = 0; i < INT_NUM_INTERRUPTS; i++)
    {
        if (i >= FIRST_IRQ_INT &&
            i < FIRST_IRQ_INT + NUM_IRQS)
        {
            g_int_handler_table[i] = &irq_int_handler;
        }
        else if (i == INT_SYSCALL_INTERRUPT)
        {
            g_int_handler_table[i] = &syscall_int_handler;
        }
        else
        {
            g_int_handler_table[i] = &unexpected_int_handler;
        }
    }

    //int_set_irqmask(0);

    //int_end_atomic(iflag);
    //_int_enable();
    printf("[cpu] initialized interrupt and IRQ handlers\n");
}

void int_register_handler(int interrupt, int_handler_t handler)
{
    KASSERT(interrupt >= 0 && interrupt < INT_NUM_INTERRUPTS);

    // Make sure we're not trying to change any special interrupts
    PANIC_IF(interrupt == INT_SYSCALL_INTERRUPT,
             "tried to change syscall interrupt handler");
    PANIC_IF(interrupt >= FIRST_IRQ_INT && interrupt < FIRST_IRQ_INT + NUM_IRQS,
             "tried to change IRQ-aware interrupt handler");

    // If handler is null, reset the handler function to the default
    if (!handler)
    {
        g_int_handler_table[interrupt] = &unexpected_int_handler;
    }
    else
    {
        g_int_handler_table[interrupt] = handler;
    }
}

void int_register_irq_handler(int irq, int_handler_t handler)
{
    KASSERT(VALID_IRQ(irq));
    //KASSERT(handler != NULL);

    // Because our IRQ handler code checks for NULL handler references, don't
    // bother worrying about it here
    irq_handler_table[irq] = handler;
}

#ifndef INT_CALL_HANDLER_DIRECTLY
void int_handler(struct registers *registers)
{
    g_int_handler_table[registers->int_num](registers);
}
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  STATIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Initialize/remap the Intel 8259 PIC
static void remap_pic(void)
{
    ioport_outb(0x20, ICW1);        // ICW1 to master
    ioport_outb(0xA0, ICW1);        // ICW1 to slave
    ioport_outb(0x21, ICW2_MASTER); // ICW2 to master
    ioport_outb(0xA1, ICW2_SLAVE);  // ICW2 to slave
    ioport_outb(0x21, ICW3_MASTER); // ICW3 to master
    ioport_outb(0xA1, ICW3_SLAVE);  // ICW3 to slave
    ioport_outb(0x21, ICW4);        // ICW4 to master
    ioport_outb(0xA1, ICW4);        // ICW4 to slave
    ioport_outb(0x21, 0);           // slave int masks
    ioport_outb(0xA1, 0);           // master int masks
}

static void unexpected_int_handler(struct registers *registers)
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
    /* 14 */ "page fault"
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
    if (registers->int_num <= 20)
    {
        sprintf(msg_buf, "unhandled interrupt %d: %s",
                 registers->int_num, error_msg[registers->int_num]);
    }
    else if (registers->int_num < 32)
    {
        // All these are Intel-reserved, so just reuse the int15 string
        sprintf(msg_buf, "unhandled interrupt %d: %s",
                 registers->int_num, error_msg[15]);
    }
    else
    {
        sprintf(msg_buf, "unhandled interrupt %d", registers->int_num);
    }

    // We ain't coming back from this one.
    panic_r(msg_buf, registers);
}

static void irq_int_handler(struct registers *registers)
{
    const int irq = registers->int_num - FIRST_IRQ_INT;
    //const uint8_t command = 0x60 | (irq & 0x7);

    KASSERT(VALID_IRQ(irq));

    // Invoke the handler if there is one
    if (!irq_handler_table[irq])
    {
        char buf[64];
        sprintf(buf, "\nunhandled but active IRQ %d; ignoring", irq);
        tty_puts_color(buf, TERM_COLOR_WORRY, true);
        //panic_r(buf, registers);
    }
    else
    {
        irq_handler_table[irq](registers);
    }

    // Send an EOI signal to the master controller
    // (and slave controller if necessary)
    if (irq >= 8)
    {
        ioport_outb(0xA0, 0x20);
    }
    ioport_outb(0x20, 0x20);
}

static void syscall_int_handler(struct registers *registers)
{
    printf("\nreceived syscall\n");
    //halt();
}
