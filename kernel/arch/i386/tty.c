#include <arch/i386/ioport.h>
#include <kernel/debug.h>
#include <kernel/tty.h>
#include <stdbool.h>

// Maximum screen size
static const size_t VGA_WIDTH = 80, VGA_HEIGHT = 24;

// Physical address of VGA screen buffer
static volatile uint16_t *const tty_buf = (volatile uint16_t *const)0xB8000;

// Current position
static size_t tty_row, tty_col;

// Static prototypes
static void update_cursor(void);
static void scroll(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PUBLIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Initialize the VGA terminal driver
 * Parameters:
 *     none
 *
 * Returns:
 *     void
 */
void tty_init(void)
{
    static bool initialized = false;

    WORRY_IF(initialized, "tty driver is already initialized");
    tty_clear();
    initialized = true;
}

/* Clear the screen (by filling it with blank characters)
 * Parameters:
 *     none
 *
 * Returns:
 *     void
 */
void tty_clear(void)
{
    tty_row = tty_col = 0;
    update_cursor();

    // Loop over every "letter" of the screen and "blank" it
    const uint16_t clear_char = (uint16_t)' ' | (uint16_t)(TERM_COLOR_SCREEN << 8);
    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            tty_buf[y * VGA_WIDTH + x] = clear_char;
        }
    }
}

/* Write a char to the VGA console
 * Parameters:
 *     c - character to print
 *
 * Returns:
 *     character written
 */
int tty_putchar(int c)
{
    return tty_putchar_color(c, TERM_COLOR_SCREEN);
}

/* Write a char to the VGA console, specifying the fg and bg colors
 * Parameters:
 *     c     - character to print
 *     color - color fg/bg byte
 *
 * Returns:
 *     character written
 */
int tty_putchar_color(int c, uint8_t color)
{
    const uint16_t encoded_char = (uint16_t)c | (uint16_t)(color << 8);

    // Handle control characters
    switch (c)
    {
    case '\n':
        tty_col = 0;
        if (++tty_row == VGA_HEIGHT)
            scroll();
        break;

    case '\t':
        tty_col += 4;
        if (tty_col >= VGA_WIDTH)
        {
            tty_col = 0;
            if (++tty_row == VGA_HEIGHT)
                scroll();
        }
        break;

    default:
        tty_buf[tty_row * VGA_WIDTH + tty_col] = encoded_char;
        if (++tty_col == VGA_WIDTH)
        {
            tty_col = 0;
            if (++tty_row == VGA_HEIGHT)
                scroll();
        }
    }

    update_cursor();
    return c;
}

/* Write a string to the VGA console
 * Parameters:
 *     str - string to print
 *
 * Returns:
 *     number of characters written
 */
int tty_puts(const char *str, bool newline)
{
    return tty_puts_color(str, TERM_COLOR_SCREEN, newline);
}

/* Write a string to the VGA console, specifying the fg and bg colors
 * Parameters:
 *     str   - string to print
 *     color - color fg/bg byte
 *
 * Returns:
 *     number of characters written
 */
int tty_puts_color(const char *str, uint8_t color, bool newline)
{
    KASSERT(str != NULL);

    const char *counter;
    for (counter = str; *counter; counter++)
    {
        tty_putchar_color(*counter, color);
    }

    if (newline)
        tty_putchar_color('\n', color);
    
    return counter - str + (newline ? 1 : 0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  STATIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Update the blinking cursor underscore
static void update_cursor(void)
{
   const uint16_t current_location = tty_row * VGA_WIDTH + tty_col;
   ioport_outb(0x3D4, 14);
   ioport_outb(0x3D5, current_location >> 8);
   ioport_outb(0x3D4, 15);
   ioport_outb(0x3D5, current_location);
}

// Scroll the screen by one line
static void scroll(void)
{
    const uint16_t clear_char = (uint16_t)' ' | (uint16_t)(TERM_COLOR_SCREEN << 8);

    // Move everything in the screen up one line
    for (size_t line = 0; line < VGA_WIDTH * VGA_HEIGHT; line++)
    {
        tty_buf[line] = tty_buf[line + VGA_WIDTH];
    }

    for (size_t x = 0; x < VGA_WIDTH; x++)
    {
        tty_buf[VGA_WIDTH * (VGA_HEIGHT - 1) + x] = clear_char;
    }

    tty_row--;
}
