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


void kernel_init()
{
    console_init();
    gdt_init();
    interrupt_init();
    
    
    asm volatile(
        "sti\n"
        "movl %eax , %eax\n");
    u32 counter = 0;
    while(true)
    {
        DEBUGK("looping in kernel init %d...\n",counter++);
        delay(100000000);
    }
    return ;
}

