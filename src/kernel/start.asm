[bits 32]

extern kernel_init;表明为外部函数
global _start
_start:
   call kernel_init

    jmp $