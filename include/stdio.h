#ifndef _STDIO_H_
#define _STDIO_H_

#include "stdarg.h"

int open(const char *pathname, uint8_t flags);
int close(int fd);
int read(int fd, void *buf, int count);
int write(int fd, void *buf, int count);
int unlink(int fd);
int putchar(int ch);
int puts(const char *s);

// printf.c
int vsprintf(char *str, const char *format, va_list ap);
int sprintf(char *buf, const char *fmt, ...);
int vprintf(const char *fmt, va_list args);
int printf(const char *fmt, ...);

// kstdio.c
int printk(const char *fmt, ...);
int serial_print(const char *fmt, ...);

#endif