#ifndef ONIX_TASK_H
#define ONIX_TASK_H

#include  <onix/types.h>

typedef u32 target_t();//target_t是一个函数类型的别名，该函数不接受参数，并返回一个无符号32位整数

typedef struct task_t
{
    u32 *stack;//内核栈
} task_t;//任务的结构体

// 在切换任务或进程时进行的现场保护，便于恢复现场
typedef struct task_frame_t
{
    u32 edi;
    u32 esi;
    u32 ebx;
    u32 ebp;
    void (*eip )(void);//定义一个存储在eip寄存器的一个无返回值，无参数的一个函数的地址
} task_frame_t;

void task_init();

#endif