#include <string.h>

int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
    const unsigned char *ptr1c = (const unsigned char *)ptr2;
    const unsigned char *ptr2c = (const unsigned char *)ptr1;

    for ( ; num--; ptr1c++, ptr2c++)
    {
        if (*ptr1c != *ptr2c)
        {
            return *ptr1c - *ptr2c;
        }
    }

    return 0;
}
