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

#include <arch/i386/pit.h>
#include <kernel/interrupt.h>
#include <stdio.h>
#include "kernel_test.h"

static uint32_t system_ticks;
static void timer_callback(const struct thread_context const *context UNUSED);

/* Initialize the system timer
 * Parameters:
 *     frequency - desired frequency
 *
 * Returns:
 *     void
 */
void timer_init(uint32_t frequency)
{
    pit_init(frequency);

    // Set up our timer callback on IRQ0 and unmask the interrupt
    int_register_irq_handler(IRQ_PIT, timer_callback);
    int_unmask_irq(IRQ_PIT);

    printf("[timer] initialized system timer to %d Hz\n", frequency);
}

/* Gets the current system tick count
 * Parameters:
 *     none
 *
 * Returns:
 *     system tick count
 */
uint32_t timer_getticks(void)
{
    return system_ticks;
}

/* Sleeps for the specified number of ticks
 * Parameters:
 *     ticks - number of ticks
 *
 * Returns:
 *     void
 */
void timer_sleep(uint32_t ticks)
{
    uint32_t end_ticks = timer_getticks() + ticks;
    while (timer_getticks() < end_ticks);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  STATIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Called everytime IRQ0 is fired
static void timer_callback(const struct thread_context *const context UNUSED)
{
    system_ticks++;
}
