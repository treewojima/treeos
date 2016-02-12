#include <kernel/timer.h>

#include <arch/i386/pit.h>
#include <kernel/interrupt.h>
#include <stdio.h>
#include "kernel_test.h"

static uint32_t system_ticks, quantum;
static void timer_callback(struct registers *registers UNUSED);

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
    int_register_irq_handler(IRQ_PIT, &timer_callback);
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
static void timer_callback(struct registers *registers UNUSED)
{
    system_ticks++;
    quantum++;

    if (quantum >= 100)
    {
        quantum = 0;
        printf("hai2u\n");
    }
}
