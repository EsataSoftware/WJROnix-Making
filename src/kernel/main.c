# include <onix/onix.h>
# include <onix/types.h>
# include <onix/io.h>
# include <onix/string.h>
# include <onix/console.h>
# include <onix/stdarg.h>
# include <onix/printk.h>


char message[]="WJR onix!!!\n";
char buf[1024];

void text_stdarg(int cnt,...)//...的意思是不限制参数个数
{
    va_list args;//valist指针
    va_start(args,cnt);//将args指针指向下一个参数
    int arg;
    while(cnt --)
    {
        arg=va_arg(args,int);
    }
    va_end(args);

}
void kernel_init()
{
    console_init();
    //text_stdarg(5,1,0xaa,5,0x55,10);
    int cnt =30;
    while(cnt --)
    {
        printk("hello onix %#010x\n",cnt);
    //console_write(message,strlen(message));
    
    }
    return ;
}

