/*
#include "syscall.h"

void *malloc(uint32_t size)
{
    return api_malloc(size);
}

void free(void *buf)
{
    api_free(buf);
}

void exit(int retcode)
{
    // atexit什么的很快就会加，先只包装_exit
    _exit(retcode);
}

void system(char *cmdline)
{
    api_system(cmdline);
}
*/
