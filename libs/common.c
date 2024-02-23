#include "common.h"
#include "console.h"

// 端口写一个字节
inline void outb(uint16_t port, uint8_t value)
{
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

// 端口读一个字节
inline uint8_t inb(uint16_t port)
{
	uint8_t ret;

	asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));

	return ret;
}

inline void outw(uint16_t port, uint16_t value)
{
	asm volatile ("outw %1, %0" : : "dN" (port), "a" (value));
}

inline void outl(uint16_t port, uint32_t value)
{
    asm volatile("outl %1, %0" : : "dN"(port), "a"(value)); // 相当于 out value, port
}

// 端口读一个字
inline uint16_t inw(uint16_t port)
{
	uint16_t ret;

	asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));

	return ret;
}

inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    asm volatile("inl %1, %0" : "=a"(ret) : "dN"(port)); // 相当于 in val, port; return val;
    return ret;
}

// 开启中断
void enable_intr()
{
    asm volatile ("sti");
}

// 关闭中断
void disable_intr()
{
    asm volatile ("cli" ::: "memory");
}
