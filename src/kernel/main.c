# include <onix/onix.h>
# include <onix/types.h>
# include <onix/io.h>
# include <onix/string.h>
# include <onix/console.h>
# include <onix/stdarg.h>
# include <onix/printk.h>
# include <onix/assert.h>
# include <onix/debug.h>




void kernel_init()
{
    console_init();
    //text_stdarg(5,1,0xaa,5,0x55,10);
    // int cnt =30;
    // while(cnt --)
    // {
    //     printk("hello onix %#010x\n",cnt);
    // //console_write(message,strlen(message));
    
    // }
    
    BMB;

    DEBUGK("debug onix!!!\n");
    return ;
}

