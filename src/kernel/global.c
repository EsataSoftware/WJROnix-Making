#include <onix/global.h>
#include <onix/string.h>
#include <onix/debug.h>

descriptor_t gdt[GDT_SIZE];//内核全局描述符表
pointer_t   gdt_ptr;//内核全局描述表指针

void gdt_init()
{
    
    DEBUGK("init gdt!!!\n");//输出init  gdt

    asm volatile ("sgdt gdt_ptr");//保存loader的全局描述符表

    memcpy(&gdt,(void *)gdt_ptr.base,gdt_ptr.limit+1);//将gdt_ptr的全局描述符表的数据存入gdt的位置上，大小为gdt_ptr.limit+1

    //更改结构体属性的值
    gdt_ptr.base = (u32)&gdt;
    gdt_ptr.limit= sizeof(gdt) - 1;
    //将初始化的指针在放回全局描述符寄存器

    asm volatile("lgdt gdt_ptr");
    
}
