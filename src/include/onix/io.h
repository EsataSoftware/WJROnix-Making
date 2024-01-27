#ifndef ONIX_IO_H
#define ONIX_IO_H

#include <onix/types.h>

extern u8 inb (u16 port);//输入一个字节//从port端口获取一个u8变量的值
extern u16 inw(u16 port);//输入一个字

extern  void outb(u16 port,u8 value);//输出一个字节从右往左压入//赋值操作，将后面的value的值赋给port
extern  void outw(u16 port, u16 value);//输出一个字


#endif