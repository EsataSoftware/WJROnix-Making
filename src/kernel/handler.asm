[bits 32]
; 中断处理函数入口

extern handler_table; 外部函数

section .text; 代码段

%macro INTERRUPT_HANDLER 2
; 这一行定义了一个宏，名为INTERRUPT_HANDLER，
; 接受两个参数。第一个参数是中断号，第二个参数是一个布尔值，用于指示是否需要保护现场
interrupt_handler_%1:
; 这一行定义了一个标签，以interrupt_handler_开头，
; 后接中断号（第一个参数%1）作为后缀。这个标签是中断处理程序的入口点。
%ifn %2
; 这是一个条件判断，用于检查第二个参数是否为假（即非零）。
; 如果第二个参数为真，则执行下一行；如果为假，则跳过下一行。
    xchg bx,bx
    push 0x20222202
    ; 如果%2第二个参数为假（即不需要保护现场），
    ; 则压入一个特定的值（0x20222202）。这个值通常用于调试目的，用来标记中断处理程序的入口。简化结构
%endif
    push %1; 压入中断向量，跳转到中断入口
    jmp interrupt_entry
%endmacro

interrupt_entry:

    ; 保存现场也就是上文的寄存器的信息
    ; 段寄存器和通用寄存器
    push ds
    push es
    push fs
    push gs
    pusha

    ; 找到前面 push %1 压入的中断向量
    mov eax,[esp + 12 * 4] ; 跳过之前的寄存器的信息，找到对应的中断向量
   
    ; 向中断处理函数传递参数
    push eax
    ; 求得中断处理函数的地址，调用中断处理函数，handler_table 中存储了中断处理函数的指针
    call [handler_table + eax * 4]
    ; 对应 push eax, 调用结束恢复栈,根据偏移地址和基地址求得中断函数的地址
    add esp,4
   
   ; 恢复下文寄存器的信息
    popa
    pop gs
    pop fs
    pop es 
    pop ds

    ; 对应push %1
    ; 对应 error code 或 push magic
    add esp, 8
    iret 


INTERRUPT_HANDLER 0x00,0; divide by zero           除0异常
INTERRUPT_HANDLER 0x01,0; debug                    单步调试
INTERRUPT_HANDLER 0x02,0; non maskable interrupt   不可屏蔽中断，原语也不能屏蔽，一般用于最严重的硬件故障
INTERRUPT_HANDLER 0x03,0; breakpoint               单步调试

INTERRUPT_HANDLER 0x04,0; overflow                 eflags的溢出标志位of 
INTERRUPT_HANDLER 0x05,0; bound range exceeded    寻址到有效地址外

INTERRUPT_HANDLER 0x06,0; invalid opcode          CPU发现无效指令码  
INTERRUPT_HANDLER 0x07,0; device not avilable     设备不存在

INTERRUPT_HANDLER 0x08,1; double fault            双故障出错，多重故障发生
INTERRUPT_HANDLER 0x09,1; coprocessor segment overrun 协处理器段超出
INTERRUPT_HANDLER 0x0a,1; invalid TSS               任务状态段无效，存储任务在执行时的寄存器状态、内存段映射、特权级别等。
INTERRUPT_HANDLER 0x0b,1; segment not present       描述符所指的段不存在

INTERRUPT_HANDLER 0x0c,1; stack segment fault       堆栈段不存在或寻址堆栈段越界
INTERRUPT_HANDLER 0x0d,1; general protection fault  没有符合特权级的操作引起，越级使用权限
INTERRUPT_HANDLER 0x0e,1; page fault                页不存在

INTERRUPT_HANDLER 0x0f,0; reserved

INTERRUPT_HANDLER 0x10,0; x87 floating point execption  协处理器发出的错误信号引起
INTERRUPT_HANDLER 0x11,1; alignment check           对齐检测只在 CPL 3 执行，于 486 引入
INTERRUPT_HANDLER 0x12,0; machine check             与模型相关，奔腾处理机引入
INTERRUPT_HANDLER 0x13,0; SIMD Floating - Point Exception 与浮点操作有关

INTERRUPT_HANDLER 0x14,0; Virtualization Exception
INTERRUPT_HANDLER 0x15,1; Control Protection Exception   ret 与 iret 误用
INTERRUPT_HANDLER 0x16,0; reserved
INTERRUPT_HANDLER 0x17,0; reserved

INTERRUPT_HANDLER 0x18,0; reserved
INTERRUPT_HANDLER 0x19,0; reserved
INTERRUPT_HANDLER 0x1a,0; reserved
INTERRUPT_HANDLER 0x1b,0; reserved

INTERRUPT_HANDLER 0x1c,0; reserved
INTERRUPT_HANDLER 0x1d,0; reserved
INTERRUPT_HANDLER 0x1e,0; reserved
INTERRUPT_HANDLER 0x1f,0; reserved

INTERRUPT_HANDLER 0x20,0; clock 时钟中断
INTERRUPT_HANDLER 0x21,0
INTERRUPT_HANDLER 0x22,0
INTERRUPT_HANDLER 0x23,0
INTERRUPT_HANDLER 0x24,0
INTERRUPT_HANDLER 0x25,0
INTERRUPT_HANDLER 0x26,0
INTERRUPT_HANDLER 0x27,0
INTERRUPT_HANDLER 0x28,0; rtc 实时时钟
INTERRUPT_HANDLER 0x29,0
INTERRUPT_HANDLER 0x2a,0
INTERRUPT_HANDLER 0x2b,0
INTERRUPT_HANDLER 0x2c,0
INTERRUPT_HANDLER 0x2d,0
INTERRUPT_HANDLER 0x2e,0
INTERRUPT_HANDLER 0x2f,0
; 下面的数组记录了每个中断函数入口的指针
; 这个数据段的空格花了我半个小时哈哈哈被自己蠢笑了
section .data
global handler_entry_table
handler_entry_table:
    dd interrupt_handler_0x00
    dd interrupt_handler_0x01
    dd interrupt_handler_0x02
    dd interrupt_handler_0x03
    dd interrupt_handler_0x04
    dd interrupt_handler_0x05
    dd interrupt_handler_0x06
    dd interrupt_handler_0x07
    dd interrupt_handler_0x08
    dd interrupt_handler_0x09
    dd interrupt_handler_0x0a
    dd interrupt_handler_0x0b
    dd interrupt_handler_0x0c
    dd interrupt_handler_0x0d
    dd interrupt_handler_0x0e
    dd interrupt_handler_0x0f
    dd interrupt_handler_0x10
    dd interrupt_handler_0x11
    dd interrupt_handler_0x12
    dd interrupt_handler_0x13
    dd interrupt_handler_0x14
    dd interrupt_handler_0x15
    dd interrupt_handler_0x16
    dd interrupt_handler_0x17
    dd interrupt_handler_0x18
    dd interrupt_handler_0x19
    dd interrupt_handler_0x1a
    dd interrupt_handler_0x1b
    dd interrupt_handler_0x1c
    dd interrupt_handler_0x1d
    dd interrupt_handler_0x1e
    dd interrupt_handler_0x1f
    dd interrupt_handler_0x20
    dd interrupt_handler_0x21
    dd interrupt_handler_0x22
    dd interrupt_handler_0x23
    dd interrupt_handler_0x24
    dd interrupt_handler_0x25
    dd interrupt_handler_0x26
    dd interrupt_handler_0x27
    dd interrupt_handler_0x28
    dd interrupt_handler_0x29
    dd interrupt_handler_0x2a
    dd interrupt_handler_0x2b
    dd interrupt_handler_0x2c
    dd interrupt_handler_0x2d
    dd interrupt_handler_0x2e
    dd interrupt_handler_0x2f
   










