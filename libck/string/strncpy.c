#include <string.h>

char *strncpy(char *dest, const char *src, size_t num)
{
    for (; num > 0; num--)
    {
        *dest++ = *src++;
    }

    return dest;
}
