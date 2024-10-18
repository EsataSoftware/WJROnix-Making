#ifndef ONIX_MEMORY_H
#define ONIX_MEMORY_H
#define PAGE_SIZE 0x1000  //一页的大小 4k
#define MEMORY_BASE 0X100000 //可用内存大小 
#include <onix/types.h>
void memory_init(u32 magic, u32 addr);
void memory_map_init();
static u32 get_page();
static u32 put_page(u32 addr);
void memory_test();


typedef struct page_entry_t
{
    u8 present : 1;// 是否在内存中
    u8 write : 1;// 0 可读，1 可读可写
    u8 user :1 ;// 1 所有人 0  超级用户 DPL < 3
    u8 pwt : 1;// page write through 1 直写模式，0 回写模式  快表：将页表缓存到了块表之中，在发生缺页中断时，可以更快的找到页表。
    // 直写模式 写入源数据和cache 中，回写模式 只写入cache
    u8 pcd: 1;//page cache disable 禁止此页缓冲，意味着只能从内存中读取
    u8 accessed: 1;//是否被访问过，用于统计使用频率，在发生缺页中断时参与选择页去加载到内存
    u8 dirty :1;//脏页，表示该页缓冲被写过
    u8 pat: 1; // page attribute table 页大小 4M/4K
    u8 global :1;// 全局，所有进程都用到，该页不刷新缓冲
    u8 ignored : 3;//没有什么用处
    u32 index: 20;// 索引
}_packed page_entry_t;

u32 get_cr3();
void set_cr3(u32 pde);
#endif