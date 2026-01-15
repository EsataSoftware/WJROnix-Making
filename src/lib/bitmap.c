#include <onix/bitmap.h>
#include <onix/string.h>
#include <onix/onix.h>
#include <onix/assert.h>

//构造位图
void  bitmap_make(bitmap_t * map, char * bits, u32 length, u32 offset)
{
    map->bits = bits;
    map->length = length;
    map->offset = offset;
}

// 初始化位图
void  bitmap_init(bitmap_t * map, char * bits, u32 length, u32 start )
{
    //清空数据
    memset(bits,0,length);
    bitmap_make(map,bits,length,start);

}

//测试某一位是否为1
bool bitmap_test(bitmap_t *map, u32 index)
{
    assert(index >= map->offset);//错误处理

    idx_t  idx =  index - map->offset;//从起始处到index处共有页面数目

    u32 bytes = idx / 8;// 一共占用多少B

    u8 bits = idx % 8;//在bytes之后的第几位是对应的位图

    // 是否小于map ->length总字节数
    assert(bytes < map ->length);

    return (map->bits[bytes] & (1 <<bits));// 不太理解，返回那一位
}

void bitmap_set(bitmap_t * map, u32 index, bool value)
{
     
    // value 代表该内存页面是否有效
    assert (value ==0 || value == 1);

    // 索引是否大于起始值
    assert (index  >= map->offset);

    idx_t  idx = index - map->offset;

    u32 bytes = idx /8;
    
    u8 bits = idx%8;
    
    if(value )
    {
        // 置为1
        map->bits[bytes] |=(1<<bits);
    }
    else
    {

        //置为0
        map->bits[bytes] &= ~(1<< bits);
    }
}
//扫描位图，实现内存的首次适应算法，搜索位图，寻找count大小的空闲页数，然后分配
int bitmap_scan(bitmap_t * map, u32 count)
{
    int start = EOF;                // 标记目标开始的位置 
    u32 bits_left = map->length *8; // 剩余的位数
    u32 next_bit = 0;               // 下一个位
    u32 counter = 0;                //计数器

    //从起始位置开始，扫描位图，找到连续的且页数大小为count的空闲页数
    while(bits_left-- >0)
    {

        if(!bitmap_test(map,next_bit +map->offset))     //判断是否空闲
        {
              counter ++;           //该页面满足++
        }
        else
        {
            counter =0;             //不满足，置0
        }

        next_bit ++;

        if(counter == count)        //观察是否找到了count数目的连续空闲页面
        {
            start = next_bit - count;
            break;
        }
    }
    if(start ==EOF)                 //说明没有空闲页面
        return EOF;

    bits_left = count;              //  循环次数
    next_bit = start;               //  页面起始值
    while(bits_left --)             //  循环
    {
        bitmap_set(map,map->offset + next_bit,true);    //占用该页面
        next_bit++;                 
    }

    return start + map->offset;         //返回占用的起始位置


}
