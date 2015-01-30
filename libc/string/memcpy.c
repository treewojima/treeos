#include <stdint.h>
#include <string.h>

void *memcpy(void *dest, const void *src, size_t num)
{
    const uint8_t *srcc = (const unsigned char *)src;
    uint8_t *destc = (unsigned char *)dest;

    for (; num > 0; num--)
    {
        *destc++ = *srcc++;
    }

    return dest;
}
