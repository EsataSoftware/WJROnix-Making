#include "onix/types.h"
#include <stdio.h>

typedef struct descriptor /* 共8位*/
    {
    unsigned short limit_low;//段界限0~15位s
    unsigned int base_low : 24 ;//基地址0~23位16M;
    unsigned char type : 4;//段类型
    unsigned char segment : 1;//1·表示代码段或数据段，0 表示系统段;
    unsigned char DPL : 2;//DescriptorPrivilege Level 描达符特权等级0 ~ 3
    unsigned char present : 1;//存在位，1在内存中，0 在磁盘上;
    unsigned char limit_high :4;//段界限16 ~ 19;
    unsigned char available : 1;//该安排的都安排了，送给操作系统吧
    unsigned char long_mode  :1;//64位扩展标志
    unsigned char big :1;//32位还是16位;
    unsigned char granularity :1;// 粒度 4KB 或1B
    unsigned  char base_high;//基地址 24~31 位
    }_packed descriptor ;
int main()
{
    printf("size of u8 %d\n",sizeof(u8));
    printf("size of u8 %d\n",sizeof(u16));
    printf("size of u8 %d\n",sizeof(u32));
    printf("size of u8 %d\n",sizeof(u64));
    return 0;
}
