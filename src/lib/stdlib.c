# include <onix/stdlib.h>
// 延时
void delay(u32 count)
{
    while(count -- );
}
// 阻塞
void hang()
{
    while(true);
}
// 将 8位8421格式的bcd码转成无符号整数 0x25＝十进制25。用于时间表示
u8 bcd_to_bin(u8 value)
{
    return ((value & 0xf ) + ( value>>4 ) * 10);
}
// 将 二进制转化成bcd码，用于写入信息，用于时钟的数据转换
u8 bin_to_bcd(u8 value)
{
    return (value / 10) *0x10 +(value % 10);
}
//用于计算总页面大小和一个页面的大小，计算总页数，超过一页的任意一个大小都需要加一页
u32 div_round_up(u32 num, u32 size)
{
    return (num + size -1) / size;
}