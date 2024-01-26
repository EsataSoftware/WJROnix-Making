# include <onix/onix.h>
# include <onix/types.h>
# include <onix/io.h>
# include <onix/string.h>
// - CRT 地址寄存器 0x3D4
// - CRT 数据寄存器 0x3D5
// - CRT 光标位置 - 高位 0xE
// - CRT 光标位置 - 低位 0xF

// #define CRT_ADDR_REG 0x3D4
// #define CRT_DATA_REG 0x3D5

// #define CRT_CURSOR_H 0xe
// #define CRT_CURSOR_L 0xf

char message[]="hello onix!!!";
char buf[1024];

void kernel_init()
{
    // outb(CRT_ADDR_REG,CRT_CURSOR_H);//向地址寄存器端口输入高八位
    // u16 pos = inb (CRT_DATA_REG) << 8;//将数据寄存器的数据输出到pos并左移八位
    // outb(CRT_ADDR_REG,CRT_CURSOR_L);//向地址寄存器中端口输入低八位
    // pos |= inb(CRT_DATA_REG);//将两次的pos值进行位或操作
    

    // outb(CRT_ADDR_REG,CRT_CURSOR_H);
    // outb(CRT_DATA_REG,0);
    // outb(CRT_ADDR_REG,CRT_CURSOR_L);
    // outb(CRT_DATA_REG,0);
    int res;
    res=strcmp(buf,message);
    strcpy(buf,message);//copy
    res=strcmp(buf,message);
    strcat(buf,message);//拼接起来
    res=strcmp(buf,message);
    res=strlen(message);//不包括\0
    res=sizeof(message);//包括\0
    
    char *ptr=strchr(message,'!');//找到！
    ptr=strrchr(message,'!');//找到最后一个！


    memset(buf,0,sizeof(message));
    res =memcmp(buf,message,sizeof(message));
    memcpy(buf,message ,sizeof(message));
    res =memcmp(buf,message,sizeof(message));
    ptr=memchr(buf,'!',sizeof(message));

    return ;
}

