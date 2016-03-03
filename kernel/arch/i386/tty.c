#include <kernel/tty.h>

#include <arch/i386/ioport.h>
#include <kernel/debug.h>
#include <kernel/interrupt.h>
#include <kernel/pmm.h>
#include <stdbool.h>
#include <string.h>

// Phsyical address ranges of text buffer
#define VGA_TEXT_BUFFER_START 0xB8000
#define VGA_TEXT_BUFFER_END   0xBFFFF

// High address offset
#define IO_MMAP_HIGH_OFFSET 0xF0000000

// Maximum screen size
static const size_t VGA_WIDTH = 80, VGA_HEIGHT = 24;

// Pointer to video memory
static volatile uint16_t *tty_buf =
    (volatile uint16_t *)VGA_TEXT_BUFFER_START;

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

/* Remap memory-mapped IO addresses to high kernel memory
 * Parameters:
 *     none
 *
 * Returns:
 *     void
 */
void tty_remap(void)
{
    const uint32_t low_tty_buf = (uint32_t)tty_buf;
    const uint32_t high_tty_buf = low_tty_buf + IO_MMAP_HIGH_OFFSET;

    uint32_t pages = VGA_TEXT_BUFFER_END - VGA_TEXT_BUFFER_START;
    pages = PAGE_ALIGN_UP(pages);
    pages >>= PAGE_SIZE_SHIFT;

    // First, map the high address pages
    for (uint32_t page = 0; page < pages; page++)
    {
        page_map(low_tty_buf + (page << PAGE_SIZE_SHIFT),
                 high_tty_buf + (page << PAGE_SIZE_SHIFT),
                 true, false);
    }

    // Set the text buffer pointer to the new high address
    tty_buf = (uint16_t *)high_tty_buf;

    // Next, unmap low pages - this needs to be done outside the mapping loop
    // in case something goes wrong, so that any error messages are printed
    for (uint32_t page = 0; page < pages; page++)
    {
        page_unmap(low_tty_buf + (page << PAGE_SIZE_SHIFT));
    }
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
    bool lock = int_lock_region();

    tty_row = tty_col = 0;
    update_cursor();

    // Loop over every "letter" of the screen and "blank" it
    const uint16_t clear_char = (uint16_t)' ' | (uint16_t)(TTY_COLOR_SCREEN << 8);
    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            tty_buf[y * VGA_WIDTH + x] = clear_char;
        }
    }

    int_unlock_region(lock);
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
    return tty_putchar_color(c, TTY_COLOR_SCREEN);
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
    bool lock = int_lock_region();

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
    int_unlock_region(lock);
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
    return tty_puts_color(str, TTY_COLOR_SCREEN, newline);
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
    bool lock = int_lock_region();

    KASSERT(str != NULL);

    const char *counter;
    for (counter = str; *counter; counter++)
    {
        tty_putchar_color(*counter, color);
    }

    if (newline)
        tty_putchar_color('\n', color);
    
    int ret = counter - str + (newline ? 1 : 0);

    int_unlock_region(lock);
    return ret;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LOCAL FUNCTIONS                                                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Update the blinking cursor underscore
static void update_cursor(void)
{
    bool lock = int_lock_region();

    const uint16_t current_location = tty_row * VGA_WIDTH + tty_col;
    ioport_outb(0x3D4, 14);
    ioport_outb(0x3D5, current_location >> 8);
    ioport_outb(0x3D4, 15);
    ioport_outb(0x3D5, current_location);

    int_unlock_region(lock);
}

// Scroll the screen by one line
static void scroll(void)
{
    bool lock = int_lock_region();

    const uint16_t clear_char = (uint16_t)' ' | (uint16_t)(TTY_COLOR_SCREEN << 8);

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

    int_unlock_region(lock);
}
