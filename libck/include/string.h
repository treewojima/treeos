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
