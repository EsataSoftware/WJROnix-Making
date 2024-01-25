[bits 32]

section .text;代码段

global inb ;声明全局函数，在任何位置都可以访问
inb :
    push ebp;相当于是将main函数的栈底保存起来，将栈顶的值赋给栈底，相当于在原来的栈顶基础上扩充
    mov ebp ,esp ;保存帧

    xor eax,eax;将eax清0
    mov edx,[ebp + 8];edx是port
    in al,dx;将端口号的 dx 的 8 bit 输入到 ax
    jmp $+2
    jmp $+2
    jmp $+2

    leave ;恢复栈帧
    ret

global outb ;声明全局函数，在任何位置都可以访问
outb :
    push ebp;相当于是将main函数的栈底保存起来，将栈顶的值赋给栈底，相当于在原来的栈顶基础上扩充
    mov ebp ,esp ;保存帧

    
    mov edx,[ebp + 8];port  找到要输出的端口号
    mov eax,[ebp + 12];value  输出的数据
    out  dx,al;将端口号的 al 的 8 bit 输入到 端口号 dx
    
    jmp $+2
    jmp $+2
    jmp $+2

    leave ;恢复栈帧
    ret

global inw ;声明全局函数，在任何位置都可以访问
inw :
    push ebp;相当于是将main函数的栈底保存起来，将栈顶的值赋给栈底，相当于在原来的栈顶基础上扩充
    mov ebp ,esp ;保存帧

    xor eax,eax;将eax清0
    mov edx,[ebp + 8];edx是port
    in ax,dx;将端口号的 dx 的 8 bit 输入到 ax
    jmp $+2
    jmp $+2
    jmp $+2

    leave ;恢复栈帧
    ret
    
global outw ;声明全局函数，在任何位置都可以访问
outw :
    push ebp;相当于是将main函数的栈底保存起来，将栈顶的值赋给栈底，相当于在原来的栈顶基础上扩充
    mov ebp ,esp ;保存帧

    
    mov edx,[ebp + 8];port  找到要输出的端口号
    mov eax,[ebp + 12];value  输出的数据
    out  dx,ax;将端口号的 al 的 8 bit 输入到 端口号 dx
    
    jmp $+2
    jmp $+2
    jmp $+2

    leave ;恢复栈帧
    ret
