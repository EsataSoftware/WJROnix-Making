#ifndef ONIX_STDIO_H
#define ONIX_STDIO_H

#include <onix/stdarg.h>
int  vsprintf(char  * buf,const char * fmt,va_list args);//结果字符串，原字符串，参数指针
int sprintf(char * buf, const char*fmt,...);
#endif