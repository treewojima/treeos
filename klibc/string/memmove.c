#include <stdint.h>
#include <string.h>

void *memmove(void *dest, const void *src, size_t size)
{
    uint8_t *destc = (uint8_t *)dest;
    const uint8_t *srcc = (const uint8_t *)src;

    if (destc < srcc)
    {
        for (size_t i = 0; i < size; i++)
        {
            destc[i] = srcc[i];
        }
    }
    else
    {
        for (size_t i = 0; i != 0; i++)
        {
            destc[i - 1] = srcc[i - 1];
        }
    }

    return dest;
}
