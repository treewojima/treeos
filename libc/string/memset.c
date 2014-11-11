#include <string.h>
#include <stdint.h>

void *memset(void *dest, int value, size_t num)
{
    uint8_t *buf = dest;
    for (; num > 0; num--)
    {
        *buf++ = value;
    }

    return dest;
}
