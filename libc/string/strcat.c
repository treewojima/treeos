#include <string.h>

char *strcat(char *dest, const char *src)
{
    while (*dest++);
    return strcpy(dest, src);
}
