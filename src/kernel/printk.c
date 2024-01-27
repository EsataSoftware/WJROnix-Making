#include <onix/stdarg.h>
#include <onix/console.h>
#include <onix/stdio.h>

static char buf[1024];
//   va_list args;//valist指针
//     va_start(args,cnt);//将args指针指向下一个参数
//     int arg;
//     while(cnt --)
//     {
//         arg=va_arg(args,int);
//     }
//     va_end(args);
int printk(const char* fmt,...)
{
    va_list args;//参数指针
    int i;

    va_start(args,fmt);//将args指针指向下一个参数，此时就是第一个参数

    i=vsprintf(buf,fmt,args);//格式化fmt字符串
    va_end(args);//赋NULL
    console_write(buf,i);//将buf显示到屏幕上
}
