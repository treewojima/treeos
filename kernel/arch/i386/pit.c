#include <kernel/timer.h>

#include <arch/i386/cpu.h>
#include <arch/i386/ioport.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PUBLIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Initialize the PIT
 * Parameters:
 *     frequency - desired frequency
 *
 * Returns:
 *     void
 */
void pit_init(uint32_t frequency)
{
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
}
