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

int bitmap_scan(bitmap_t * map, u32 count)
{
    int start = EOF;
    u32 bits_left = map->length *8;
    u32 next_bit = 0;
    u32 counter = 0;

    while(bits_left-- >0)
    {

        if(!bitmap_test(map,next_bit +map->offset))
        {
              counter ++;
        }
        else
        {
            counter =0;
        }

        next_bit ++;

        if(counter == count)
        {
            start = next_bit - count;
            break;
        }
    }
    if(start ==EOF)
        return EOF;

    bits_left = count;
    next_bit = start;
    while(bits_left --)
    {
        bitmap_set(map,map->offset +next_bit,true);
        next_bit++;
    }

    return start + map->offset;


}
# include <onix/debug.h>

#define LOGK(fmt,args...) DEBUGK(fmt,## args)

#define LEN 2

u8 buf[LEN];
bitmap_t map;

void bitmap_tests()
{
    bitmap_init(&map, buf,LEN,0);
    for (size_t i = 0; i < 33; i++)
    {
        /* code */
        idx_t  idx = bitmap_scan(&map,1);
        if(idx ==EOF)
        {
            LOGK("TEST FINISH\n");
            break;
        }
        LOGK("%d\n",idx);
    }
}