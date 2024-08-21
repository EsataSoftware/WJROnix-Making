#ifndef ONIX_STDLIB_H
#define ONIX_STDLIB_H
#include <onix/types.h>
void delay(u32 count);
void hang();
#define MAX(a, b) (a < b ? b : a)
#define MIN(a, b) (a < b ? a : b)

u8 bcd_to_bin(u8 value); //  bcd 码转化成二进制
u8 bin_to_bcd(u8 value); // 二进制转化成bcd

u32 div_round_up(u32 num, u32 size);
#endif