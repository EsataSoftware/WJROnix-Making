#ifndef ONIX_INTERRUPT_H
#define ONIX_INTERRUPT_H

#include <onix/types.h>

#define IDT_SIZE 256

typedef struct gate_t
{
    u16 offset0;    // 段内偏移 0 ~ 15 位
    u16 selector;   // 代码段选择字
    u8 reserved;    // 保留不用
    u8 type :4;     // 任务门/中断门/陷阱门
    u8 segnment :1; // segment = 0 表示系统段
    u8 DPL : 2;     // 使用 int 指令访问的最低权限
    u8 present : 1; // 是否在内存中，也就是是否有效
    u16 offset1;    // 段内偏移 16 ~ 31 位

}_packed gate_t;

void interrupt_init();

#endif