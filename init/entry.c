/*

	entry.c

	RainyOS system kernel entry program
	
	RainySoftTeam & RainyOSTeam RainyOS 2022~2024

*/

#include "common.h"
#include "console.h"
#include "string.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"
#include "task.h"
#include "sched.h"
#include "keyboard.h"
#include "shell.h"
#include "multiboot.h"
#include "acpi.h"
#include "int32.h"

// 内核初始化函数
void kern_init();

// 开启分页机制之后的 Multiboot 数据指针
multiboot_t *glb_mboot_ptr;

// 开启分页机制之后的内核栈
char kern_stack[STACK_SIZE]  __attribute__ ((aligned(16)));

// 内核栈的栈顶
uint32_t kern_stack_top;

// 内核使用的临时页表和页目录
// 该地址必须是页对齐的地址，内存 0-640KB 肯定是空闲的
__attribute__((section(".init.data"))) pgd_t *pgd_tmp  = (pgd_t *)0x1000;
__attribute__((section(".init.data"))) pgd_t *pte_low  = (pgd_t *)0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_hign = (pgd_t *)0x3000;

// 内核入口函数
__attribute__((section(".init.text"))) void kern_entry()
{
	pgd_tmp[0] = (uint32_t)pte_low | PAGE_PRESENT | PAGE_WRITE;
	pgd_tmp[PGD_INDEX(PAGE_OFFSET)] = (uint32_t)pte_hign | PAGE_PRESENT | PAGE_WRITE;

	// 映射内核虚拟地址 4MB 到物理地址的前 4MB
	int i;
	for (i = 0; i < 1024; i++) {
		pte_low[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}

	// 映射 0x00000000-0x00400000 的物理地址到虚拟地址 0xC0000000-0xC0400000
	for (i = 0; i < 1024; i++) {
		pte_hign[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}
	
	// 设置临时页表
	asm volatile ("mov %0, %%cr3" : : "r" (pgd_tmp));

	uint32_t cr0;

	// 启用分页，将 cr0 寄存器的分页位置为 1 就好
	asm volatile ("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0" : : "r" (cr0));
	
	// 切换内核栈
	kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE);
	asm volatile ("mov %0, %%esp\n\t"
			"xor %%ebp, %%ebp" : : "r" (kern_stack_top));

	// 更新全局 multiboot_t 指针
	glb_mboot_ptr = mboot_ptr_tmp + PAGE_OFFSET;

	// 调用内核初始化函数
	kern_init();
}

int flag = 0;

int thread(void *arg)
{
	asm volatile ("hlt");
	asm volatile ("hlt");
	asm volatile ("hlt");
	asm volatile ("hlt");

	///*
	console_clear();

	console_write_color("RainyOS v0.58 (Alpha, Feb 14 2024, 21:40:17) [GCC 12.2.0] on Machine\n", rc_black, rc_green);
	console_write_color("RainyShell v0.32 (Alpha, Feb 14 2024, 22:40:17) [GCC 12.2.0] on RainyOS\n", rc_black, rc_green);
	console_write_color("Type 'help' to see the help list table.\n", rc_black, rc_green);
	print_warn("You should type 'bugs' to see the bugs in this version. Be careful!\n");
	console_write_color("Now you are in the shell.\n\n", rc_black, rc_light_cyan);


	shell();
	//*/

	return 0;
}

void kern_init()
{
	console_write("Loading");

	for (int i = 0; i < 69; i++){
		init_timer(200);
		console_write(">");
	}

	printk_color(rc_black, rc_light_cyan, "DONE");

	print_succ("Boot RainyOS");

	init_debug();
	init_gdt();
	init_idt();

	print_succ("Init debug");
	print_succ("Init GDT");
	print_succ("Init IDT");

	//console_clear();
	print_succ("Load RainyOS Kernel");
	print_succ("Load Entry Program");
	printk_color(rc_white, rc_black, "RainyOS RainySoftTeam & RainyOSTeam 2022~2024\n");
	printk_color(rc_white, rc_red, "RainyOS Alpha Version 0.58 Build 17\n");

	init_timer(200);

	//printk("kernel in memory start: 0x%08X\n", kern_start);
	//printk("kernel in memory end:   0x%08X\n", kern_end);
	//printk("kernel in memory used:   %d KB\n\n", (kern_end - kern_start) / 1024);
	
	// show_memory_map();
	init_pmm();
	init_vmm();
	init_heap();

	//printk_color(rc_black, rc_red, "\nThe Count of Physical Memory Page is: %u\n\n", phy_page_count);

	test_heap();

	init_sched();

	init_keyboard();

	//init_acpi();
	#define HAVE_BUGS

	// 开启中断
	enable_intr();

	/*
	regs16_t regs;
    regs.ax = 0x4f02;
    regs.bx = 0x4105;
    _int32(0x10, &regs);

	*Nooooooooooooooooooooo!!!!! Why it damage the memory page!!
    *Ahhhhhhhh NOOOO!!! F**king d**n s**tty Memory Page, I hate you!!!

    */

	//Angry code *Laugh*

	/*
	 *
	 *Ummmm...Here is some about int32.
	 *Well we know. int32 will damage our memory page.
	 *So when it return to Protect Mode, we can try to open it again..
	 *(Need it close the Memory Page first? I don't know...)
	 *
	 */
	
	kernel_thread(thread, NULL);

	/*
	while (1) {
        if (fifo_status(&decoded_key) > 0) {
			char c = fifo_get(&decoded_key);
            console_putc_color(c, rc_black, rc_white);
        }
    }
	*/

	//readline();

	while (1) {
		asm volatile ("hlt");
	}

	//Here is done
}

