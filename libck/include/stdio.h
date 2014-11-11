#ifndef __TREEOS_LIBC_STDIO_H__
#define __TREEOS_LIBC_STDIO_H__

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char *format, ...);
int vprintf(const char *format, va_list args);
int sprintf(char *str, const char *format, ...);
int vsprintf(char *str, const char *format, va_list args);

int puts(const char *str);
int putchar(int ic);

#ifdef __cplusplus
}
#endif

#endif
