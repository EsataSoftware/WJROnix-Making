# include <onix/stdlib.h>

void delay(u32 count)
{
    while(count -- )
    ;
}
void hang()
{
    while(true)
    ;
}
// 将 bcd 码转成整数
u8 bcd_to_bin(u8 value)
{
    return ((value & 0xf ) + ( value>>4 ) * 10);
}
// 将 二进制转化成bcd码
u8 bin_to_bcd(u8 value)
{
    return (value / 10) *0x10 +(value % 10);
}