#include <string.h>

void *memcpy(void *dest, const void *src, size_t num)
{
    const unsigned char *srcc = (const unsigned char *)src;
    unsigned char *destc = (unsigned char *)dest;

    for (; num > 0; num--)
    {
        *destc++ = *srcc++;
    }

    return dest;
}
