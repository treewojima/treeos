#include <stdio.h>

#include <kernel/tty.h>

int putchar(int ic)
{
    return tty_putchar(ic);
}
