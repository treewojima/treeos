#include <stdio.h>

#include <kernel/tty.h>
#include <stdarg.h>

#define BUF_LEN 512

int vprintf(const char *format, va_list args)
{
    char buf[BUF_LEN];
    vsprintf(buf, format, args);
    return tty_puts(buf, false);
}
