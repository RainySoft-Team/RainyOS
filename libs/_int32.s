; int32 --- Real Mode Interrupt Functionality Under Protected Mode
; Original version by Napalm

[bits 32]

section .data

section .text

global _int32

struc regs16_t
    .di resw 1
    .si resw 1
    .bp resw 1
    .sp resw 1
    .bx resw 1
    .dx resw 1
    .cx resw 1
    .ax resw 1
    .gs resw 1
    .fs resw 1
    .es resw 1
    .ds resw 1
    .ef resw 1 ; eflags缩写，代码对齐好看捏
endstruc ; nasm特有的结构体声明语法

%define TEMP_CODE_SEG_BASE 0 ; 需要进入临时保护模式代码段中转一下
%define CODE_ADDR 07c00h ; 引导被加载到的位置，现在已经在kernel了，这块地方没人管（喜
; int32中会将reloc标签以后的代码copy到CODE_OFF，需要REBASE计算新地址
; 由于copy过程中偏移不变，减旧start加新start即可
%define REBASE(x) (((x) - reloc) + CODE_ADDR)
; 32位数据段，直接用现存的
%define DATA32 2 * 8
%define CODE32 3 * 8 ; 临时代码段
%define DATA16 4 * 8 ; 16位数据段
%define CODE16 5 * 8 ; 16位代码段
%define STACK16 (CODE_ADDR - regs16_t_size) ; regs16_t是struc，nasm会提供regs16_t_size作为表示regs16_t常量的大小
; 栈不在多，够用就行，此处能装下所有寄存器就是够用

_int32:
    cli ; 关中断，免得保护模式的中断来烦人

    mov esi, TEMP_CODE_SEG_BASE + reloc ; ds:esi -> src，数据源
    mov edi, CODE_ADDR ; es:edi -> dest，数据应该放在哪
    mov ecx, (int32_end - reloc) ; ecx -> size，传送大小，这里把从代码结束到reloc（基准点）的代码全部copy
    cld ; 复制时edi自增
    rep movsb ; 将ds:esi的内容不断copy至es:edi

    ; 复制完成，执行一个farcall跳到CODE_ADDR去执行后续代码
    push CODE32 ; cs
    push CODE_ADDR ; eip
    call far [esp] ; 执行farcall
    add esp, 8 ; 把farcall时入栈的cs和eip再弹出去
    ret ; 调用完毕

reloc: ; farcall后进入下面的代码，只是不在这个地址
    ; 后面所有对内存地址的引用均要使用REBASE宏
    ; 进入16位保护模式之前，先存一些东西
    mov [REBASE(stack32_ptr)], esp ; 保存保护模式栈
    sidt [REBASE(idt32_ptr)] ; 保存保护模式IDT
    lea esi, [esp + 20] ; 相当于 mov esi, esp + 20，但汇编不支持这么写
    ; 此时的栈情况：
    ; [esp] farcall时eip
    ; [esp + 4] farcall时cs
    ; [esp + 8] 执行时eip
    ; [esp + 12] 执行时cs
    ; [esp + 16] 执行完int32后的返回地址
    ; [esp + 20] 第一个参数（中断号）
    ; [esp + 24] 第二个参数（regs16_t地址）
    lodsd ; mov eax, [esi] \n\t add esi, 4
    mov byte [REBASE(ib)], al ; 补全实模式代码段的INT指令
    ; 好了，现在的esi指向esp + 24
    mov esi, [esi] ; mov esi, [esp + 24]
    ; 现在esi存的是regs16_t地址，同时也是下一个movsb的数据源
    ; u1s1 Napalm这块写的是真乱
    mov edi, STACK16 ; 实模式堆栈地址
    mov ecx, regs16_t_size ; regs16_t大小，为什么能这么写见上
    mov esp, edi ; 现在把esp指向实模式栈，不知道为什么这么早指过去
    rep movsb ; 把esi指向的内存写入edi
    ; 在32位保护模式所做的最后工作完毕，执行远跳转进入16位保护模式！
    jmp dword CODE16:REBASE(p_mode16)
; ……长路归乡……（指初始时就在实模式）
[bits 16]
p_mode16:
    ; 把所有段初始化为16位数据段
    mov ax, DATA16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ; 关闭保护模式
    mov eax, cr0
    and al, ~0x01
    mov cr0, eax ; CR0.PE = 1
    ; 什么 你问为什么不关A20？因为没必要关
    ; 16位保护模式工作结束，跳转到实模式
    jmp word 0:REBASE(r_mode16) ; 怎么了 难道实模式的cs不是0？
r_mode16:
    ; 呼，终于回实模式了
    ; 将ds ss设为cs
    xor ax, ax ; mov ax, 0，因为cs == 0所以相当于mov ax, cs
    mov ds, ax
    mov ss, ax
    lidt [REBASE(idt16_ptr)] ; 加载IVT
    mov bx, 0x0870 ; bh: PIC0, bl: PIC1；此处是实模式下的设定
    call set_pic ; set_pic也是地址，为什么不用REBASE？因为near call用的是相对地址，其他情况用的都是绝对地址
    ; 此时的栈指向regs16_t，把寄存器全部pop出来
    popa
    pop gs
    pop fs
    pop es
    pop ds
    ; 栈里还有个flags，但说真的有pop出来的必要吗（恼
    ; 要INT了，把中断打开
    sti
    ; 由于INT号在保护模式中设定，不能直接INT，而是需要用机器码中转
    db 0xCD
ib: db 0x00 ; 这里就是保护模式写入的中断号位置
    ; 中断用完了，避免其他中断干扰，再次cli
    cli
    xor sp, sp ; sp清0
    mov ss, sp ; ss设为现在的sp，相当于mov ss, 0
    mov sp, CODE_ADDR ; 之前pop了一堆寄存器，把sp重新指回栈顶
    ; 把现在的寄存器状态push进去
    pushf ; 这里就需要了，VBE是需要判断FLAGS.CF来确认执行情况的
    push ds ; 怎么pop的就怎么push
    push es
    push fs
    push gs
    pusha
    ; 将PIC设置成保护模式的样子
    mov bx, 0x2028 ; bh:PIC0，bl:PIC1
    call set_pic
    ; 回到保护模式
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    ; 明明实模式到保护模式只需要jmp一次，保护模式到实模式却需要两次，真奇怪
    jmp dword CODE32:REBASE(p_mode32)

; ……奔向保护……
[bits 32]
p_mode32:
    ; 回到保护模式，重设各段
    mov ax, DATA32
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; 重设IDT
    lidt [REBASE(idt32_ptr)] ; 之前sidt存的
    ; 重设esp
    mov esp, [REBASE(stack32_ptr)] ; 之前mov存的
    mov esi, STACK16 ; 数据源，进保护之前push的一堆寄存器
    lea edi, [esp + 24] ; mov edi, esp + 24
    mov edi, [edi] ; mov edi, [esp + 24]，参数所指向的位置，数据目的地
    mov ecx, regs16_t_size ; 不用说了吧。。。
    cld ; 执行int过程中可能更改，再cld一遍
    rep movsb ; 将esi数据复制到edi
    ; 至此已完成一切工作
    ; 由于调用时中断不一定开着，这里不进行sti，开不开中断由主调决定
    ; 好啦，主体代码部分结束，前面是farcall的，现在retf回去咯
    retf
; 一些用到的函数和内存
set_pic: ; 按bx的设定重设PIC
    push ax ; 端口操作必然用到ax，存一下
; PIC这部分没什么可注释的，具体参见descriptor_tables.c中对PIC的初始化
    mov al, 0x11
    out 0x20, al
    out 0xA0, al
    mov al, bh ; PIC0起始中断号
    out 0x21, al
    mov al, bl ; PIC1起始中断号
    out 0xA1, al
    mov al, 0x04
    out 0x21, al
    shr al, 1
    out 0xA1, al ; 主片是4，从片是2，刚刚好
    shr al, 1 ; al现在是1
    out 0x21, al
    out 0xA1, al ; 也是硬性规定，参见descriptor_tables.c，L73，L74
; PIC设定完成，弹出ax返回
    pop ax
    ret

stack32_ptr:
    dd 0 ; 32位栈的位置，留出来

idt32_ptr:
    dw 0 ; limit
    dd 0 ; base
; 由于idt32先sidt再lidt，所以直接让它为0，啥也不用管即可

idt16_ptr:
    dw 0x3FF ; limit
    dd 0 ; base
; IVT位于0~0x3FF内存空间，这样设定完全合理

; 至此int32汇编程序全部结束
int32_end: