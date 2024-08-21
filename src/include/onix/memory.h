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
#endif