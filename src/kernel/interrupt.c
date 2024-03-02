#include <onix/interrupt.h>
#include <onix/global.h>
#include <onix/printk.h>
#include <onix/debug.h>
#include <onix/stdlib.h>
#include <onix/io.h>


#define ENTRY_SIZE 0x30
#define LOGK(fmt,args...) DEBUGK(fmt,##args)
#define PIC_M_CTRL 0x20 // 主片的控制端口
#define PIC_M_DATA 0x21 // 主片的数据端口
#define PIC_S_CTRL 0xa0 // 从片的控制端口
#define PIC_S_DATA 0xa1 // 从片的数据端口
#define PIC_EOI 0x20    // 通知中断控制器中断结束


gate_t idt[IDT_SIZE];
pointer_t idt_ptr;

handler_t handler_table[IDT_SIZE];
extern handler_t handler_entry_table[ENTRY_SIZE];

static char *messages[] = {
    "#DE Divide Error\0",
    "#DB RESERVED\0",
    "--  NMI Interrupt\0",
    "#BP Breakpoint\0",
    "#OF Overflow\0",
    "#BR BOUND Range Exceeded\0",
    "#UD Invalid Opcode (Undefined Opcode)\0",
    "#NM Device Not Available (No Math Coprocessor)\0",
    "#DF Double Fault\0",
    "    Coprocessor Segment Overrun (reserved)\0",
    "#TS Invalid TSS\0",
    "#NP Segment Not Present\0",
    "#SS Stack-Segment Fault\0",
    "#GP General Protection\0",
    "#PF Page Fault\0",
    "--  (Intel reserved. Do not use.)\0",
    "#MF x87 FPU Floating-Point Error (Math Fault)\0",
    "#AC Alignment Check\0",
    "#MC Machine Check\0",
    "#XF SIMD Floating-Point Exception\0",
    "#VE Virtualization Exception\0",
    "#CP Control Protection Exception\0",
};

// 通知中断控制器，中断处理结束
void send_eoi(int vector)
{
    if(vector >=0x20 && vector <0x28)
    {
        outb(PIC_M_CTRL,PIC_EOI);
    }
    if(vector >= 0x28 && vector < 0x30)
    {
        outb(PIC_M_CTRL,PIC_EOI);
        outb(PIC_S_CTRL,PIC_EOI);
    }
}

extern void schedule();// 调度函数

void default_handler(int vector)
{
    send_eoi(vector);
    schedule();
}
// 增加了中断上下文的一些寄存器的信息
void exception_handler(
    int vector,
    u32 edi, u32 esi, u32 edp, u32 esp,
    u32 ebx, u32 edx, u32 ecx, u32 eax,
    u32 gs, u32 fs, u32 es, u32 ds,
    u32 vector0, u32 error,u32 eip, u32 cs, u32 eflags)
{
    char * message =NULL;
    if(vector <22)
    {
        message=messages[vector];
    }
    else
    {
        message = messages[15];    
    }
    printk("\nException : %s \n",messages[vector]);
    printk("   VECTOR : 0x%02X\n", vector);
    printk("    ERROR : 0x%08X\n", error);
    printk("   EFLAGS : 0x%08X\n", eflags);
    printk("       CS : 0x%02X\n", cs);
    printk("      EIP : 0x%08X\n", eip);
    printk("      ESP : 0x%08X\n", esp);
    // 阻塞
    hang();
}
void pic_init()
{
    outb(PIC_M_CTRL,0b00010001); // ICM1:边沿触发，级联 8259 ，需要 ICW4,
    outb(PIC_M_DATA,0x20);       // ICM2:起始中断向量号 0x20
    outb(PIC_M_DATA,0b00000100); // ICW3:IR2 接从片
    outb(PIC_M_DATA,0b00000001); // ICW4: 8086模式， 正常EOI

    outb(PIC_S_CTRL,0b00010001); // ICM1:边沿触发，级联 8259 ，需要 ICW4,
    outb(PIC_S_DATA,0x28);       // ICM2:起始端口号 0x28
    outb(PIC_S_DATA,2);          // S设置从片连接到主片的 IR2 引脚
    outb(PIC_S_DATA,0b00000001); // ICW4: 8086模式， 正常EOI

    outb(PIC_M_DATA,0b11111110); // 关闭所有中断
    outb(PIC_S_DATA,0b11111111); // 关闭所有中断
    
}
void idt_init()
{
    for (size_t i = 0; i < ENTRY_SIZE; i++)
    {
        gate_t *gate = &idt[i];
        handler_t handler=handler_entry_table[i];
        gate->offset0 = (u32) handler &0xffff;
        gate->offset1 = ((u32) handler >> 16) & 0xffff;
        gate->selector = 1 <<3;//代码段
       gate->reserved = 0;//保留不用
        gate->type = 0b1110;//中断门
        gate->segnment = 0;  // 系统段
        gate->DPL = 0; //内核态
        gate->present = 1;//有效
    }
    //建立中断程序处理表 
      for (size_t i = 0; i < 0x20; i++)
    {
        handler_table[i] = exception_handler; // 处理异常情况
    }
     for(size_t i = 20;i<ENTRY_SIZE;i++)
     {        handler_table[i] =default_handler;    // 处理默认情况
     }

    idt_ptr.base = (u32) idt;//基地址
    idt_ptr.limit = sizeof(idt)-1;//限制
    
    asm volatile("lidt idt_ptr\n");
}
void interrupt_init()
{
    pic_init();
    idt_init();
}