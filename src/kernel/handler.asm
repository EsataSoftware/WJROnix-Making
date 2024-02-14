[bits 32]
; 中断处理函数入口

section .text;代码段
extern printk;引入printk函数

global interrupt_handler;全局函数
interrupt_handler:
    xchg bx,bx
    push message
    call printk 
    add esp, 4 ; 调整栈指针以清理栈（假设是32位栈指针）

    xchg bx, bx ; 
    iret ; 从中断返回，从栈中弹出标志、CS 和 EIP
section .data

message :
db"default interrupt", 10, 0