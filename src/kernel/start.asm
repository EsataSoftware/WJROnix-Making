[bits 32]

extern kernel_init;表明为外部函数
global _start
_start:
    xchg bx,bx
    call kernel_init
    ;mov byte [0xb8000],'K';进入内核
    int 0x80;调用中断函数
    xchg bx,bx
    jmp $