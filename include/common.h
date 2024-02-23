#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include "types.h"
#include "string.h"

#define true 1
#define false 0

void outb(uint16_t port, uint8_t value);// 端口写一个字节
uint8_t inb(uint16_t port);// 端口读一个字节
void outw(uint16_t port, uint16_t value);
uint16_t inw(uint16_t port);// 端口读一个字
void outl(uint16_t port, uint32_t value);
uint32_t inl(uint16_t port);

void enable_intr();// 开启中断
void disable_intr();// 关闭中断

#define NULL ((void *) 0)

#endif // INCLUDE_COMMON_H_
