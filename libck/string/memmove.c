#include <string.h>

void *memmove(void *dest, const void *src, size_t size)
{
    unsigned char *destc = (unsigned char *)dest;
    const unsigned char *srcc = (const unsigned char *)src;

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
