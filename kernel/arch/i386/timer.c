#include <kernel/timer.h>

#include <arch/i386/cpu.h>
#include <arch/i386/interrupt.h>
#include <arch/i386/ioport.h>
#include <stdio.h>

static uint32_t ticks;

static void timer_callback(struct registers *registers);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PUBLIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Initialize the system timer (uses the PIT)
 * Parameters:
 *     frequency - desired frequency
 *
 * Returns:
 *     void
 */
void timer_init(uint32_t frequency)
{
    ticks = 0;

    // Set up our timer callback on IRQ0
    int_register_irq_handler(0, &timer_callback);

    // The value sent to the PIT is the value to divide its input clock
    // (1193180 Hz) by, to get the required frequency.
    // NOTE: the divisor must fit into 16 bits.
    uint16_t divisor = 1193180 / frequency;

    // Send command byte
    ioport_outb(0x43, 0x36);

    // Split divisor into upper/lower bytes
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)(divisor >> 8);

    // Send divisor
    ioport_outb(0x40, l);
    ioport_outb(0x40, h);

    printf("[timer] initialized system timer to %d Hz\n", frequency);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  STATIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Called everytime IRQ0 is fired
static void timer_callback(struct registers *registers)
{
    ticks++;

    if (!(ticks % 100))
    {
        puts("one second has passed");
    }

    //kprintf("tick: %d\n", ticks);
    //proc_switch();
}
