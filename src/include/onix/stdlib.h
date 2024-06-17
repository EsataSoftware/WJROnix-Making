#ifndef ONIX_STDLIB_H
#define ONIX_STDLIB_H
#include <onix/types.h>
void delay(u32 count);
void hang();
u8 bcd_to_bin(u8 value); //  bcd 码转化成二进制
u8 bin_to_bcd(u8 valur); // 二进制转化成bcd
#endif