#include <onix/memory.h>
#include <onix/types.h>
#include <onix/debug.h>
#include <onix/assert.h>
#include <onix/stdlib.h>
#include <onix/string.h>


#define LOGK(fmt,args...) DEBUGK (fmt, ##args)

#define ZONE_VALID 1    //ards 可用区域
#define ZONE_RESERVED 2 // ards 不可用区域
#define IDX(addr) ((u32)addr >> 12) //获取addr 的内存页索引
#define PAGE(idx) ((u32)idx << 12) // 获取页索引 idx 对应的页开始的位置
#define ASSERT_PAGE(addr)  assert((addr & 0xfff) == 0)

typedef struct ards_t
{
    u64 base;  // 基地址
    u64 size;  // 内存长度
    u32 type;  // 类型
}_packed ards_t;

static u32 memory_base = 0;    //可用内存基地址 = 1M
static u32 memory_size = 0;    //可用内存大小
static u32 total_pages = 0;    //所有内存页数
static u32 free_pages = 0;     //空闲内存页数

#define used_pages (total_pages - free_pages)
//  用来初始化内存，在操作系统启动时执行
void memory_init(u32 magic, u32 addr)
{
    u32 count;
    ards_t * ptr;
    // 如果是 onix loader 进入内核
    if(magic == ONIX_MAGIC)
    {
        count = *(u32 *)addr;
        ptr = (ards_t *)(addr +4);
    for(size_t i = 0; i < count; i++,ptr++)
    {
        LOGK("Memory base 0x%p Memory size 0x%p Memory type %d\n",(u32)ptr->base,(u32)ptr->size,(u32)ptr->type);
       

        if(ptr->type == ZONE_VALID && ptr->size > memory_size)
        {
            memory_base = (u32)ptr->base;
            memory_size = (u32)ptr->size;
        }
    }
    }
    else{
        panic("Memory init magic unknown 0x%p\n", magic);
    }
    LOGK("ARDS count %d\n",count);
    LOGK("Memory base 0x%p\n",(u32)memory_base);
    LOGK("Memory size 0x%p\n",(u32)memory_size);
    
    assert(memory_base == MEMORY_BASE); // 内存开始的位置 为1M
    assert((memory_size & 0xfff) == 0);//相当与一个if判断 按页对齐

    total_pages= IDX(memory_size) + IDX(MEMORY_BASE);
    free_pages = IDX(memory_size);

    LOGK("Total pages %d\n",total_pages);
    LOGK("Free pages:%d\n",free_pages);
}
static u32 start_page = 0;  // 可分配物理内存起始位置
static u8 *memory_map;      // 物理内存数组,物理页引用数量，只有保证所有引用都不存在才能释放此物理内存
static u32 memory_map_pages;// 物理内存数组占用的页数
//  跟踪每一段内存的使用情况以及对每一段内存进行处理
void memory_map_init()
{
        // 初始化物理内存数组
        memory_map = (u8 *)memory_base;
        // 计算物理内存数组占用的页数
        memory_map_pages = div_round_up(total_pages, PAGE_SIZE);

        free_pages -= memory_map_pages;
        

        // 清空物理内存数组
        memset((void * )memory_map, 0, memory_map_pages *PAGE_SIZE);
        // 前 1M 的内存位置 以及 物理内存数组已占用的页，已被占用
        start_page = IDX(MEMORY_BASE) + memory_map_pages;

        for(size_t i = 0; i < start_page; i++)
        {
            memory_map[i] = 1;// 默认是0，未被占用
        }
        LOGK("Total pages %d free pages %d\n", total_pages, free_pages);
} 
static u32 get_page()
{
    for (size_t i = start_page; i < total_pages; i++)
    {
        if(! memory_map[i])
        {
            memory_map[i] = 1;
            free_pages--;
            assert(free_pages >= 0);// 判断是否出错
            u32 page = ((u32)i) << 12;// 对 i 转为 u32 类型，并右移 12 位，获取页面映射对应的物理地址
            LOGK("GET page 0x%p\n",page);
            return page;
        }
    }
    panic("Out of Memory!!!");
}
// 释放物理内存
static u32 put_page(u32 addr)
{
    ASSERT_PAGE(addr); // 判断是不是页面边界
    
    u32 idx = IDX(addr);// 获取内存也索引

    // idx 大于 1M 小于 总页面数
    assert(idx >= start_page && idx < total_pages);
    // 保证只有一个引用
    assert(memory_map[idx] >= 1);
    
    //物理引用减一
    memory_map[idx]--;

    // 若为 0 ，则空闲页加一
    if(! memory_map[idx])
    {
        free_pages ++;
    }
    assert(free_pages > 0 && free_pages < total_pages);
    LOGK("PUT page 0x%p\n",addr);


}
void memory_test()
{
    u32 pages[10];
    for (size_t i = 0; i < 10; i++)
    {
        pages[i] = get_page();
    }
    for (size_t i = 0; i < 10; i++)
    {
        put_page(pages[i]);
    }
    
    
}
u32 get_cr3()
{
    asm volatile("movl %cr3, %eax\n");
}
void set_cr3(u32 pde)
{
    ASSERT_PAGE(pde);
    asm volatile("movl %%eax,%%cr3\n" :: "a"(pde));
    // 将pde的值移到eax寄存器中，在将寄存器中数据移入cr3寄存器中


}
static void enable_page()
{
    //0b1000_0000_0000_0000_0000_0000_0000_0000
    //0b8000_0000
    asm volatile(
        "movl %cr0, %eax\n"
        "orl $0x80000000, %eax\n"
        "movl %eax, %cr0\n");

}
static void entry_init(page_entry_t * entry,u32 index)
{
    *(u32 *)entry = 0;
    entry->present = 1;
    entry->write = 1;
    entry->user = 1;
    entry->index = index;
}
// 内核页目录
#define KERNEL_PAGE_DIR 0x200000
// 内核页表
#define KERNEL_PAGE_ENTRY 0x201000

// 初始化内存映射
void mapping_init()
{
    page_entry_t *pde = (page_entry_t *) KERNEL_PAGE_DIR;// 初始化页目录
    memset(pde,0,PAGE_SIZE);//将pde数组全部置为0
    entry_init(&pde[0],IDX(KERNEL_PAGE_ENTRY)); // 初始化第0个页
    page_entry_t *pte = (page_entry_t *) KERNEL_PAGE_ENTRY;// 初始化页表
    memset(pte,0,PAGE_SIZE);
    page_entry_t *entry;
    // 初始化一个页目录，
    for (size_t tide = 0; tide < 1024; tide++)
    {
        entry = &pte[tide];
        entry_init(entry,tide);
        memory_map[tide] = 1;//设置该页被占用
    }// 将1024 个页分别进行映射到页目录中
    BMB;
    set_cr3((u32)pde);
    BMB;
    enable_page();
    

}
