/* treeos
 * Copyright (C) 2014-2016 Scott Bishop <treewojima@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TREEOS_LIBC_STDIO_H__
#define __TREEOS_LIBC_STDIO_H__

#include <defines.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char *format, ...) FORMAT(1, 2);
int vprintf(const char *format, va_list args);
int sprintf(char *str, const char *format, ...) FORMAT(2, 3);
int vsprintf(char *str, const char *format, va_list args);

int puts(const char *str);
int putchar(int ic);

#ifdef __cplusplus
}
#endif

#endif
