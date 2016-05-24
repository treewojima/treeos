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

#ifndef __TREEOS_LIBC_STRING_H__
#define __TREEOS_LIBC_STRING_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void *ptr1, const void *ptr2, size_t num);
void *memcpy(void *dest, const void *src, size_t num);
void *memmove(void *dest, const void *src, size_t size);
void *memset(void *dest, int value, size_t num);

char *strcat(char *dest, const char *src);
int strcmp(const char *str1, const char *str2);
char *strcpy(char *dest, const char *src);
size_t strlen(const char *str);
char *strncpy(char *dest, const char *src, size_t num);

#ifdef __cplusplus
}
#endif

#endif
