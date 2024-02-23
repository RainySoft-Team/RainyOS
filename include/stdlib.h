#ifndef _STDLIB_H_
#define _STDLIB_H_

#include "common.h"

void *malloc(uint32_t size);
void free(void *ptr);
void exit(int retcode);
void system(char *cmd);

#endif