#ifndef __TREEOS_KERNEL_TTY_H__
#define __TREEOS_KERNEL_TTY_H__

#include <defines.h>

#ifdef TREEOS_EXPORT_ASM
#   error this include is not meant to be used for assembly source files
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Common terminal color constants
enum
{
    TTY_COLOR_SCREEN = 0x1F,
    TTY_COLOR_PANIC  = 0x4F,
    TTY_COLOR_WORRY  = 0xE8
};

// Terminal driver initialization/clearing
void tty_init(void);
void tty_clear(void);

// Printing functions
int tty_putchar(int c);
int tty_putchar_color(int c, uint8_t color);
int tty_puts(const char *str, bool newline);
int tty_puts_color(const char *str, uint8_t color, bool newline);

#endif
