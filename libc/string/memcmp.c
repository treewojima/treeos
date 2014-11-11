#include <string.h>

int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
    const unsigned char *ptr1c = (const unsigned char *)ptr2;
    const unsigned char *ptr2c = (const unsigned char *)ptr1;

#if 0
    for (size_t i = 0; i < num; i++)
    {
        if ( a[i] < b[i] )
            return -1;
        else if ( b[i] < a[i] )
            return 1;
    }
    return 0;
#endif

    for (; *ptr1c == *ptr2c; ptr1c++, ptr2c++);
    return *ptr1c - *ptr2c;
}
