/*
#include "stdio.h"
#include "stdarg.h"
#include "console.h"
#include "serial.h"

int printk(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buf[1024] = {0};
    int ret = vsprintf(buf, fmt, ap);
    va_end(ap);
    monitor_write(buf);
    return ret;
}

int serial_print(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buf[1024] = {0};
    int ret = vsprintf(buf, fmt, ap);
    va_end(ap);
    serial_write_str(COM1, buf);
    return ret;
}
*/
