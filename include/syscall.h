#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "common.h"

// 系统调用结构：
// 库函数putchar -> 用户级API api_putchar -> int 80h -> syscall_handler -> syscall_manager -> sys_putchar

void syscall_handler();

// 真正的系统调用函数，转发系统调用
int *syscall_manager(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);

// sys_系列函数
#include "file.h" // sys_read, sys_write
#include "memory.h" // sys_malloc, sys_free
#include "shell.h"

// api_系列函数
int api_read(int fd, void *buf, int count);
int api_write(int fd, void *buf, int count);
int api_open(const char *pathname, uint8_t flags);
int api_close(int fd);
int api_unlink(int fd);
void *api_malloc(uint32_t size);
void api_free(void *buf);
void api_system(char *cmdline);

void clear();
void ls();
int exec(char *progname);
void _exit(int retval);
void lspci();
uint8_t *datetime();
void lscpu();
void shutdown();
void reboot();

#endif