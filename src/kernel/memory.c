#include <onix/memory.h>
#include <onix/types.h>
#include <onix/debug.h>
#include <onix/assert.h>
#include <onix/stdlib.h>
#include <onix/string.h>
#include <onix/bitmap.h>

#define LOGK(fmt,args...) DEBUGK (fmt, ##args)

#define ZONE_VALID 1    //ards 可用区域
#define ZONE_RESERVED 2 // ards 不可用区域

#define IDX(addr) ((u32)addr >> 12) //获取addr 的内存页索引
#define DIDX(addr)(((u32)addr >> 22)& 0x3ff)//获取addr的页目录索引
#define TIDX(addr)(((u32)addr >>12) &0x3ff)// 获取addr的页表索引
#define PAGE(idx) ((u32)idx << 12) // 获取页索引 idx 对应的页开始的位置
#define ASSERT_PAGE(addr)  assert((addr & 0xfff) == 0)

// 内核页目录索引
#define KERNEL_PAGE_DIR 0x1000

// 内核页表索引
static u32 KERNEL_PAGE_TABLE[] =
{
    0x2000,
    0x3000,
};

#define KERNEL_MAP_BITS 0x4000      //位示图放在内存0x4000位置
#define KERNEL_MEMORY_SIZE (0x100000*sizeof(KERNEL_PAGE_TABLE))//可用的总空间大小

bitmap_t kernel_map;//位图数据结构

////描述从物理内存到实际进程可以使用的内存空间以及系统占用的空间
typedef struct ards_t
{
    u64 base;  // 基地址
    u64 size;  // 内存长度
    u32 type;  // 类型
}_packed ards_t;//不做硬件强行对齐，会导致内存图紊乱

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

    // 检测内存
    if(memory_size < KERNEL_MEMORY_SIZE)
    {
        panic("System memory is %dM too small, at least %dM needed/n",
        memory_size / MEMORY_BASE,KERNEL_MEMORY_SIZE / MEMORY_BASE );
    }
}
static u32 start_page = 0;  // 可分配物理内存起始位置
static u8 *memory_map;      // 物理内存数组,物理页引用数量，只有保证所有引用都不存在才能释放此物理内存
static u32 memory_map_pages;// 物理内存数组占用的页数
//  跟踪每一段内存的使用情况以及对每一段内存进行处理，进入loader之后对物理内存进行划分和映射，并位图初始化
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


        //初始化内核虚拟内存位图，保证8位对齐

        u32 length = (IDX(KERNEL_MEMORY_SIZE) - IDX(MEMORY_BASE))/8;//起
        bitmap_init(&kernel_map,(u8 *)KERNEL_MAP_BITS,length,IDX(MEMORY_BASE));
        bitmap_scan(&kernel_map,memory_map_pages);


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
    
    u32 idx = IDX(addr);// 获取内存页索引

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
static _inline void enable_page()
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


// 初始化内存映射
void mapping_init()
{
    // page  directory entry
    page_entry_t *pde = (page_entry_t *) KERNEL_PAGE_DIR;// 初始化页目录
    memset(pde,0,PAGE_SIZE);//将pde数组全部置为0
    
    idx_t index = 0;

    for(idx_t didx = 0; didx <(sizeof(KERNEL_PAGE_TABLE)/4); didx++)
    {
        page_entry_t * pte = (page_entry_t *)KERNEL_PAGE_TABLE[didx];
        memset(pte,0,PAGE_SIZE);

        page_entry_t * dentry = & pde[didx];
        entry_init(dentry, IDX((u32)pte));

        for(idx_t tidx = 0; tidx <1024; tidx++,index++)
        {
                //第0页不映射，造成空指针访问，缺页异常，便于排错
                if(index ==0)
                continue;

                page_entry_t * tentry = &pte[tidx];
                entry_init(tentry,index);
                memory_map[index] = 1; //设置物理内存数据，该页被占用

        }
    }

    //  将最后一个页表指向页目录自己，方便修改
    page_entry_t *entry = &pde[1023];
    entry_init(entry,IDX(KERNEL_PAGE_DIR));

    //设置cr3寄存器
    set_cr3((u32)pde);

 
    //分页有效
    enable_page();
    

}
// 页目录
static page_entry_t *get_pde()
{
    return (page_entry_t *) (0xfffff000);// 得到 0x1000
    // 返回页目录的地址去直接修改页目录的值
}
// 页表
static page_entry_t *get_pte(u32 vaddr) // 虚拟地址
{
    // 
    return (page_entry_t *)(0xffc00000 |(DIDX(vaddr) << 12));

}

//刷新虚拟地址的vaddr的快表 TLB
static void flush_tlb(u32 vaddr)
{
    asm volatile("invlpg (%0)" ::"r"(vaddr)
    : "memory");
}

// 从位图里面扫描 count 个连续的页
// 设置static 去实现memory.h未声明的函数，是推荐的
static u32 scan_page(bitmap_t*map,u32 count)
{
    assert (count > 0);
    int32 index = bitmap_scan(map,count);

    if(index ==EOF)// 说明找不到count连续的页
    {
        panic("Scan page fail!!!");
    }
    
    u32 addr = PAGE(index);//获取index的低20位
    LOGK("Scan page 0x%p count %d\n",addr,count);

    return addr;

}


//与scan_page 相对，重置相应的页
// 设置static 去实现memory.h未声明的函数，是推荐的
static void reset_page(bitmap_t *map, u32 addr,u32 count)
{
    ASSERT_PAGE(addr);// 不懂

    assert(count >0);
    u32 index = IDX(addr);

    for(size_t i = 0; i < count; i++)
    {
        assert(bitmap_test(map,index + i));
        bitmap_set(map,index+1,0);

    }

}
//分配内存空间
u32 alloc_kpage(u32 count)
{
    assert(count >0);
    u32 vaddr = scan_page(&kernel_map, count);

    LOGK("ALLOC kernel pages 0x%p count %d\n",vaddr,count);

    return vaddr;
}

//释放地址
void free_kpage(u32 vaddr,u32 count)
{
    ASSERT_PAGE(vaddr);

    assert(count);

    reset_page(&kernel_map,vaddr,count);
    LOGK("FREE kernel pages 0x%p,count 0x%d\n",vaddr,count);
}
void memory_test()
{
    u32 * pages = (u32 *) (0x200000);//2M位置
    u32 count = 0x6ff;  //一共0x7000个页，但是位图占用两个，所以减去2个页
    for(size_t i = 0; i < count; i++)
    {
        pages[i] =alloc_kpage(1);
        LOGK("0x%x\n",i);
    }
    for (size_t i = 0; i < count; i++)
    {
         free_kpage(pages[i],1);
        
    }
    
}