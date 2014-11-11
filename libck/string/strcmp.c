#include <string.h>

int strcmp(const char *str1, const char *str2)
{
    for (; *str1 && (*str1 == *str2); str1++, str2++);
    return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}
