#include <onix/onix.h>
#include <onix/types.h>
#include <onix/io.h>
#include <onix/string.h>
#include <onix/console.h>
#include <onix/stdarg.h>
#include <onix/printk.h>
#include <onix/assert.h>
#include <onix/debug.h>
#include <onix/global.h>
#include <onix/interrupt.h>
#include <onix/stdlib.h>
#include <onix/task.h>
#include <onix/time.h>
#include <onix/rtc.h>
#include <onix/memory.h>

extern void console_init();
extern void gdt_init();
extern void interrupt_init();
extern void clock_init();
extern void hang();
extern void time_init();
extern void rtc_init();
extern void memory_map_init();
extern void mapping_init();
void kernel_init()
{  
    memory_map_init();
    mapping_init();
    interrupt_init();
    clock_init();
    // time_init();
    // rtc_init();
    // set_alarm(2);
    BMB;
    char * ptr = (char *)(0x100000 *20);
    ptr[0] = 'a';
    // asm volatile("sti");
    hang();

}

