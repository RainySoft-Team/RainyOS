//#include "syscall.h"
#include "common.h"

/*
int read(int fd, void *buf, int count)
{
    return api_read(fd, buf, count);
}

int write(int fd, void *buf, int count)
{
    return api_write(fd, buf, count);
}

int open(const char *pathname, uint8_t flags)
{
    return api_open(pathname, flags);
}

int close(int fd)
{
    return api_close(fd);
}

int unlink(int fd)
{
    return api_unlink(fd);
}

int putchar(int ch)
{
    char s[2] = {0, 0};
    s[0] = ch;
    write(1, (void *) s, 2);
    return ch;
}

int puts(const char *s)
{
    write(1, (void *) s, strlen(s));
    putchar('\n');
    return strlen(s) + 1;
}
*/
