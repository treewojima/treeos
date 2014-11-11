#include <string.h>

size_t strlen(const char *str)
{
    const char *tmp;
    for (tmp = str; *tmp != '\0'; tmp++);
    return tmp - str;
}
