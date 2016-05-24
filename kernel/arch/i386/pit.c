/* treeos
 * Copyright (C) 2014-2016 Scott Bishop <treewojima@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
