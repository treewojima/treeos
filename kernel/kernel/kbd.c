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

#include <kernel/kbd.h>

#include <kernel/interrupt.h>
#include <stdio.h>

// Empty parameter list is intentional to avoid tripping a warning/error
static void kbd_int_handler();

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PUBLIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Initialize the keyboard driver
 * Parameters:
 *     none
 *
 * Returns:
 *     void
 */
void kbd_init(void)
{
    int_register_irq_handler(IRQ_KBD, kbd_int_handler);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LOCAL FUNCTIONS                                                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Keyboard IRQ callback
static void kbd_int_handler()
{
#ifdef KBD_DEBUG_KEYPRESSES
    uint8_t scancode = kbd_read_scancode();
    if (!(scancode & 0x80))
    {
        printf("detected keypress: ");
        putchar(scancode_to_ascii[scancode]);
        putchar('\n');
    }
#endif
}
