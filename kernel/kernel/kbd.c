#include <kernel/kbd.h>

#include <kernel/interrupt.h>
#include <stdio.h>

//static void kbd_int_handler(struct registers *registers);
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
    int_register_irq_handler(1, kbd_int_handler);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LOCAL FUNCTIONS                                                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Keyboard IRQ callback
static void kbd_int_handler()
{
    uint8_t scancode = kbd_read_scancode();
    if (!(scancode & 0x80))
    {
        printf("detected keypress: ");
        putchar(scancode_to_ascii[scancode]);
        putchar('\n');
    }
}
