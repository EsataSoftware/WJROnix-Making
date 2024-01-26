# include <onix/onix.h>
# include <onix/types.h>
# include <onix/io.h>
# include <onix/string.h>
# include <onix/console.h>
// - CRT 地址寄存器 0x3D4
// - CRT 数据寄存器 0x3D5
// - CRT 光标位置 - 高位 0xE
// - CRT 光标位置 - 低位 0xF

// #define CRT_ADDR_REG 0x3D4
// #define CRT_DATA_REG 0x3D5

// #define CRT_CURSOR_H 0xe
// #define CRT_CURSOR_L 0xf

char message[]="hello onix!!!\n";
char buf[1024];

void kernel_init()
{
    console_init();
    
    while(true)
    {
    console_write(message,sizeof(message)-1);
    }
    return ;
}

