# include <onix/onix.h>

int magic= ONIX_MAGIC;

char message[]="Hello Onix!!!";//初始化的数据放在data中
char buf[1024];//未初始化的数据放在bss中

void kernel_init()
{
    char * vedeo=(char *)0xb8000;//指向文本显示框
    for (int  i = 0; i < sizeof(message); i++)
    {
        vedeo [i*2]=message[i];
    }
    
}

