#ifndef ONIX_STDARG_H
#define ONIX_STDARG_H

typedef char * va_list;
//宏定义类似于函数
#define va_start(ap,v)(ap =(va_list)&v + sizeof(char*))//找到下一个参数
#define va_arg(ap,t)(*(t *)((ap += sizeof(char*))-sizeof (char*)))//将任意变参数转化成t，
#define va_end(ap) (ap -(va_list)0)//空指针
#endif