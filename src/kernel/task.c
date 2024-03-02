# include <onix/task.h>
# include <onix/printk.h>
# include <onix/debug.h>

//分页 页式存储
#define  PAGE_SIZE 0x1000


//定义了一个任务/进程
task_t  *a =(task_t * ) 0x1000;
task_t  *b = (task_t*) 0x2000;

extern void task_switch (task_t *next);//调用汇编函数

task_t * running_stack()
{
    //利用C语言框架来执行汇编语句
    asm volatile(
    "movl %esp, %eax\n"//保存栈顶指针的值，将栈顶指针的值传给eax
    "andl $0xffff000, %eax\n"//将0xffff000 与 eax 的值进行位与操作，相当于是清除栈顶指针的后12位
    );
    //不需要返回值的原因是eax本身就存储着函数返回值的功能，而这个最后也就是为了返回目前的栈顶
}

//简单的轮换调度，a,b,a,b
void schedule()
{
    task_t * current=running_stack();//获取到此时的栈顶
    task_t *next = current == a?b:a;// 如果此时 current 为 a 进程的话 执行 b 进程 ，
    task_switch(next);// 切换到 next 所指向的函数
    

}

//创建进程a 省略栈帧
u32 _ofp thread_a()
{
    BMB;
    asm volatile("sti\n");
   
    while(true)
    {   
        printk("A");
    }
}


u32 _ofp thread_b()
{
    asm volatile("sti\n");

    while(true)
    {
        printk("B");
      
    }

}

//创建任务
static void task_create(task_t * task , target_t target) //target 是一个函数指针，
{                                                        //指向一个不接受参数并返回无符号 32 位整数的函数。
    u32 stack = (u32)task +PAGE_SIZE;//设置任务 task 所在的栈顶
   
    stack -= sizeof(task_frame_t);// 减去需要用于保存任务的状态信息 寄存器信息
    task_frame_t * frame = (task_frame_t *) stack;//截取了栈的一部分内存，用来储存寄存器信息
    frame->ebx =0x11111111;
    frame->esi =0x22222222;
    frame->edi =0x33333333;
    frame->ebp =0x44444444;
    frame->eip=(void *)target;//下一个执行函数的跳转地址
    task->stack=(u32 *)stack;
}

void task_init()
{
    task_create(a,thread_a);
    task_create(b,thread_b);
   schedule();
}