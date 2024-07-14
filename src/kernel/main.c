# include <onix/onix.h>
# include <onix/types.h>
# include <onix/io.h>
# include <onix/string.h>
# include <onix/console.h>
# include <onix/stdarg.h>
# include <onix/printk.h>
# include <onix/assert.h>
# include <onix/debug.h>
# include <onix/global.h>
# include <onix/interrupt.h>
# include <onix/stdlib.h>
# include <onix/task.h>
# include <onix/time.h>
# include <onix/rtc.h>


extern void console_init();
extern void gdt_init();
extern void interrupt_init();
extern void clock_init();
extern void hang();
extern void time_init();
extern void rtc_init();

void kernel_init()
{
    console_init(); //显卡驱动
    gdt_init();
    interrupt_init();
    clock_init();
    time_init();
    rtc_init();
    //set_alarm(2);
    asm volatile("sti");
    hang();

}

