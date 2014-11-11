#include <stdio.h>

#include <kernel/tty.h>
#include <stdarg.h>

int vprintf(const char *format, va_list args)
{
    char buf[256];
    vsprintf(buf, format, args);
    return tty_puts(buf, false);
}
