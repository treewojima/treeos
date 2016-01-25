#include <stdio.h>

#include <kernel/tty.h>

int puts(const char *str)
{
    return tty_puts(str, true);
}
