[bits 32]
extern console_init
extern memory_init
extern kernel_init;表明为外部函数
global _start
_start:
   ;call kernel_init
   push ebx; args_init
   push eax; magic
   call console_init; 初始化控制台
   call memory_init;  内存初始化
    jmp $