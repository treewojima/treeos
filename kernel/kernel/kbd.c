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
