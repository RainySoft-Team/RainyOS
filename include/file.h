#ifndef _FILE_H_
#define _FILE_H_

#include "common.h"

enum oflags {
   O_RDONLY,
   O_WRONLY,
   O_RDWR,
   O_CREAT = 4
} oflags_t;

// OS暂不支持fseek
// 等后面再在fileinfo上抽象出一层再说

int sys_open(char *pathname, uint8_t flag);
int sys_close(int fd);
int sys_unlink(const char *pathname);
int sys_write(int fd, const void *buf, uint32_t cnt);
int sys_read(int fd, void *buf, uint32_t cnt);

#endif