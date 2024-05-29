#include <onix/console.h>
#include <onix/io.h>
#include  <onix/string.h>

//常量定义
#define CRT_ADDR_REG 0x3D4//地址寄存器 端口
#define CRT_DATA_REG 0x3D5//数据寄存器 
//进行IO操作的基本
#define CRT_START_ADDR_H 0xC//起始内存位置 -高位
#define CRT_START_ADDR_L 0xD//起始内存位置 -低位
#define CRT_CURSOR_H 0xE
#define CRT_CURSOR_L 0xF
//显卡基本属性
#define MEM_BASE 0xB8000            // 显卡内存起始位置
#define MEM_SIZE 0x4000             // 显卡内存大小32个字节
#define MEM_END (MEM_BASE+MEM_SIZE) // 显卡终止位置
#define WIDTH 80                    // 屏幕文本列数（宽度）
#define HEIGHT 25                   // 屏幕文本行数(高度)
#define ROW_SIZE (WIDTH*2)          // 每行的字节数
#define SCR_SIZE (ROW_SIZE*HEIGHT)  // 整个屏幕的字节数
//ASCII值的定义
#define ASCII_NUL 0x00
#define ASCII_ENQ 0x05
#define ASCII_BEL 0x07 //  \a：响铃（发出声音或者使终端闪烁）。
#define ASCII_BS  0x08 //  \b：退格（向左移动光标一个位置）。
#define ASCII_HT  0x09 //  \t：水平制表符（将光标移动到下一个水平制表符位置）。
#define ASCII_LF  0x0A //  \n：换行（移动光标到下一行的起始位置）。
#define ASCII_VT  0x0B //  \v：垂直制表符（将光标移动到下一个垂直制表符位置）。
#define ASCII_FF  0x0C //  \f：换页（类似于换行，但是可能会触发打印机的换页操作）。
#define ASCII_CR  0x0D //  \r：回车（移动光标到当前行的起始位置）。
#define ASCII_DEL 0x7F 
//公共变量定义
static u32 screen ;//记录当前显示器开始的内存位置，单位都是字节

static u32 pos;//当前光标的内存位置,单位是字节

static u32 x,y;//当前光标的字符坐标

static u8 attr = 7;//字符样式

static u16 erase = 0x0720;// 空格

// 获得当前显示器的开始位置
static void get_screen() //java 中的get 函数             
{
    outb(CRT_ADDR_REG,CRT_CURSOR_H);  // 开始位置高地址给地址寄存器，
    screen = inb (CRT_DATA_REG)<<8;   // 获取此时地址寄存器的数据传递给数据寄存器，左移8位后再返回到变量screen里
    outb(CRT_ADDR_REG,CRT_CURSOR_L);  
    screen |= inb(CRT_DATA_REG);      // 将低8位的值获取出来与之前的直接进行合并
    screen <<=1;                      //  screen*=2 将字符转化成字节,得到显示器的相对位置
    screen +=  MEM_BASE;              //  转化成绝对位置

}
// 设置当前显示器开始位置
static void set_screen()
{
     outb(CRT_ADDR_REG,CRT_START_ADDR_H);//  起始内存高位 
     outb(CRT_DATA_REG,((screen - MEM_BASE)>>9) & 0xff);//  进行 get_screen 的逆操作 ，将字符转化成字节以及再右移 8+1 位，转化到原始获得的高位数据
     outb(CRT_ADDR_REG,CRT_START_ADDR_L);  //起始内存低位      
     outb(CRT_DATA_REG,((screen - MEM_BASE)>>1) & 0xff);//  将得到的低位地址输入到数据寄存器上，通过地址寄存器进行对端口进行设置

}
// 获得当前光标位置
static void get_cursor()
{   
    outb(CRT_ADDR_REG,CRT_CURSOR_H);  // 开始位置高地址给地址寄存器，再
    pos = inb (CRT_DATA_REG)<<8;   // 获取此时地址寄存器的数据传递给数据寄存器，左移8位后再返回到变量screen里
    outb(CRT_ADDR_REG,CRT_CURSOR_L);  
    pos |= inb(CRT_DATA_REG);

    get_screen();//获取此时的screen绝对位置
    pos <<=1;// pos *= 2转化成字节
    pos += MEM_BASE;//求绝对位置
    u32 delta =(pos - screen)>>1;//求出光标位置与屏幕位置的差值再转化字节字符
    x = delta % WIDTH;//因为每一行只有 WIDTH 个字节
    y = delta / WIDTH;//求出有多少列
}
// 设置光标位置
static void set_cursor()
{
    outb(CRT_ADDR_REG,CRT_CURSOR_H);//  光标高地址 
    outb(CRT_DATA_REG,((pos - MEM_BASE)>>9) & 0xff);//  进行 get_cursor 的逆操作 ，求相对位置，再右移 9 位
    outb(CRT_ADDR_REG,CRT_CURSOR_L);  // 光标低地址                 //  将得到的高位输入到数据寄存器中，再通过地址寄存器进行设置
    outb(CRT_DATA_REG,((pos - MEM_BASE)>>1) & 0xff);//  将得到的低位地址输入到数据寄存器上，通过地址寄存器进行对端口进行设置

}
void console_clear()
{
    screen = MEM_BASE;//从显卡起始位置开始
    pos = MEM_BASE;
    x = 0;
    y = 0;
    set_cursor();//将变量设置到对应的位置
    set_screen();

    u16 * ptr= (u16 *)MEM_BASE;
    while (ptr <= (u16 * )MEM_END)//判断是否达到显卡的末位置
    {
        *ptr = erase; //清空操作
        ptr++;
    }
    
}
static void scroll_up()//上滚一行
{
    if (screen  +SCR_SIZE +ROW_SIZE>=MEM_END)//还大于显卡终止位置，还有空间上滚 目前的内存位置+一个页面的字节数+下一行的字节数和与显卡终止位置比较
    {
        memcpy((void *)MEM_BASE, (void * )screen,SCR_SIZE);
        pos -= (screen - MEM_BASE);
        screen = MEM_BASE;
    }
        u32 *ptr =(u32 *)(screen + SCR_SIZE);//申请一个新的一行
        for (size_t i = 0; i < WIDTH; i++)
        {
            *ptr ++=erase;//清空
        }
        //光标和屏幕都加一行
        screen += ROW_SIZE;
        pos += ROW_SIZE;
        set_screen();
}
static void command_lf()//换行
{   //列未超过规定的列行
    if(y+1<HEIGHT)
    {
        y ++;
        pos +=ROW_SIZE;//光标叠加一行
        return ;
    }
    scroll_up();
}
static void command_cr()//回车
{
   pos -=(x << 1);// pos 往前移动 x 个字符所占的字节数。
   x =0;//将x赋为0
}
static void command_bs()//退格
{
    if(x)//判断此时需不需要退行
    {
        x--;
        pos -= 2;
        * (u16 *) pos=erase;
    }
}
static void command_del()
{
        * (u16 *) pos=erase;//清除
}
extern void start_beep();
 void console_write(char * buf,u32 count)//向显示屏上写东西
{                  //字符串     长度
    char ch;//当前输出的字符
    char *ptr = (char *)pos;
    while (count --)
    {
        ch = *buf++;
        switch (ch)
        {
        case ASCII_NUL :
          
            break;

        case ASCII_ENQ :
          
            break;
        case ASCII_BEL :
          //.todo./a
          start_beep();
            break;
         case  ASCII_BS :
            command_bs();
             break;
        case ASCII_HT :
          
            break;
        case ASCII_LF  :
          command_lf();
          command_cr();
            break;
        case ASCII_VT   :
          
            break;
        case ASCII_FF :
          command_lf();
            break;
        case ASCII_CR    :
          command_cr();
            break;
        case ASCII_DEL:
          command_del();
            break;
        default:
        if(x>=WIDTH)
        {
            x-=WIDTH;
            pos -=ROW_SIZE;
           command_lf();//换行
        }
           *((char *)pos) =ch;    //设置字符ASCII 
           pos++;
           *((char *)pos) =attr;  // 设置字符属性
            pos++;
           
           x++;

            break;
        }
    }
    set_cursor();
}
void console_init()//初始化
{
    console_clear();//清空屏幕
}