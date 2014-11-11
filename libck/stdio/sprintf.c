#include <stdio.h>

#include <stdarg.h>

int sprintf(char *str, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    int ret = vsprintf(str, format, args);

    va_end(args);

    return ret;
}
