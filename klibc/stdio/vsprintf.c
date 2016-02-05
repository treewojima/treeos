#include <stdio.h>

#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#define BUF_LEN 512

static const char *ltoa(int32_t value, char *buf);
static const char *ultoa(uint32_t value, char *buf);
static const char *ultox(uint32_t value, char *buf);

int vsprintf(char *str, const char *format, va_list args)
{
    const char *str_orig = str;
    char buf[BUF_LEN];

    for ( ; *format; format++)
    {
        if (*format == '%')
        {
            // Read this condition carefully!
            if (!*(++format)) break;

            switch (*format)
            {
            case 's':
                {
                    char *s = va_arg(args, char *);
                    strcpy(str, s);
                    str += strlen(s);
                }
                break;

            case 'c':
                {
                    int c = va_arg(args, int);
                    *str++ = (char)c;
                }
                break;

            case 'd':
            case 'i':
                {
                    memset(buf, 0, BUF_LEN);
                    ltoa(va_arg(args, int32_t), buf);
                    strcpy(str, buf);
                    str += strlen(buf);
                }
                break;

            case 'u':
                {
                    memset(buf, 0, BUF_LEN);
                    ultoa(va_arg(args, uint32_t), buf);
                    strcpy(str, buf);
                    str += strlen(buf);
                }
                break;

            case 'x':
            case 'p':
                {
                    memset(buf, 0, BUF_LEN);
                    ultox(va_arg(args, uint32_t), buf);
                    strcpy(str, buf);
                    str += strlen(buf);
                }
                break;

            case 'b':
                {
                    memset(buf, 0, BUF_LEN);
                    int b = va_arg(args, int);
                    strcpy(str, (b ? "true" : "false"));
                    str += (b ? 4 : 5);
                }
                break;

            default:
                *str++ = *format;
            }
        }
        else
        {
            *str++ = *format;
        }
    }

    *str = '\0';
    return str - str_orig;
}

static const char *ltoa(int32_t value, char *buf)
{
    uint32_t raw;
    if (value < 0)
    {
        *buf++ = '-';
        raw = (uint32_t)-value;
    }
    else
    {
        raw = (uint32_t)value;
    }

    return ultoa(raw, buf);
}

static const char *ultoa(uint32_t value, char *buf)
{
    int num_digits = 0;
    do
    {
        buf[num_digits++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    int swap = 0;
    char tmp = 0;
    for (int i = 0; i < num_digits / 2; i++)
    {
        swap = num_digits - (i + 1);
        tmp = buf[i];
        buf[i] = buf[swap];
        buf[swap] = tmp;
    }

    buf[num_digits] = 0;

    return buf;
}

static const char *ultox(uint32_t value, char *buf)
{
    static const char hex_array[] = "0123456789ABCDEF";

    buf[0] = '0';
    buf[1] = 'x';
    buf += 2;

    int num_digits = 0;
    do
    {
        buf[num_digits++] = hex_array[value % 16];
        value /= 16;
    } while (value > 0);

    int swap = 0;
    char tmp = 0;
    for (int i = 0; i < num_digits / 2; i++)
    {
        swap = num_digits - (i + 1);
        tmp = buf[i];
        buf[i] = buf[swap];
        buf[swap] = tmp;
    }

    buf[num_digits] = 0;

    return buf;
}
