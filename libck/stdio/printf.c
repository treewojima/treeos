#include <stdio.h>

#include <stdarg.h>

int printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    
    int ret = vprintf(format, args);

    va_end(args);

    return ret;
}
