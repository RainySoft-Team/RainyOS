#include "common.h"
#include "stdarg.h"
#include "stdio.h"
#include "console.h"

static void itoa(uint32_t value, char **buf_ptr_addr, uint8_t base)
{
    uint32_t m = value % base;
    uint32_t i = value / base;
    if (i) itoa(i, buf_ptr_addr, base);
    if (m < 10) {
        *((*buf_ptr_addr)++) = m + '0';
    } else {
        *((*buf_ptr_addr)++) = m - 10 + 'A';
    }
}

int vsprintf(char *str, const char *format, va_list ap)
{
    char *buf_ptr = str;
    const char *index_ptr = format;
    char index_char = *index_ptr;
    int32_t arg_int;
    char *arg_str;
    while (index_char) {
        if (index_char != '%') {
            *(buf_ptr++) = index_char;
            index_char = *(++index_ptr);
            continue;
        }
        index_char = *(++index_ptr);
        switch (index_char) {
            case 's':
                arg_str = va_arg(ap, char*);
                strcpy(buf_ptr, arg_str);
                buf_ptr += strlen(arg_str);
                index_char = *(++index_ptr);
                break;
            case 'c':
                *(buf_ptr++) = va_arg(ap, char);
                index_char = *(++index_ptr);
                break;
            case 'd':
                arg_int = va_arg(ap, int);
                if (arg_int < 0) {
                    arg_int = -arg_int;
                    *buf_ptr++ = '-';
                }
                itoa(arg_int, &buf_ptr, 10);
                index_char = *(++index_ptr);
                break;
            case 'x':
                arg_int = va_arg(ap, int);
                itoa(arg_int, &buf_ptr, 16);
                index_char = *(++index_ptr);
                break;
        }
    }
    return strlen(str);
}

int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int retval;
    va_start(args, fmt);
    retval = vsprintf(buf, fmt, args);
    va_end(args);
    return retval;
}

int vprintf(const char *fmt, va_list args)
{
    char buf[1024] = {0};
    int ret = vsprintf(buf, fmt, args);
    console_write(buf);
    return ret;
}

int printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = vprintf(fmt, args);
    va_end(args);
    return ret;
}
