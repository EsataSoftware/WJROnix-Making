[org 0x1000]

  dw 0x55aa;来判断主引导扇区是否发生错误


  mov si ,loading
  call print  
;xchg bx,bx;

detect_memory:
   ;寄存器赋值
   xor ebx ,ebx;将ebx赋0
   ;确定缓冲区的大小
   mov ax,0;
   mov es,ax
   mov edi,ards_buffer;

   mov edx ,0x534d4150;固定签名

.next:
      ;子功能号
      mov eax,0xe820
      ;结构体的大小（字节）
      mov ecx ,20;设置字节大小
      ;系统调用
      int 0x15;BIOS 的系统调用
      ;检测标志位
      jc error;条件跳转
      
      ;没问题的话就接着遍历 di ARDS 缓冲区
      add di ,cx;cx存的是20，特殊的寄存器需要间接存

      inc word [ards_count];自增ards的数量

      cmp ebx,0;比较后续值，判断还有没有ards
      jnz .next
      
      ;检测结束
      mov si,detecting
      call print

      ; xchg bx,bx;
      ; mov byte [0xb8000],'P'

      jmp prepare_protected_mode

   ;    mov cx,[ards_count];将ards的数量放入到cx通过loop来循环
   ;    ;结构体指针
   ;    mov si,0
   ; .show
   ;    mov eax,[ards_buffer+si]
   ;    mov ebx,[ards_buffer+si+8]
   ;    mov edx,[ards_buffer+si+16]
   ;    add si,20;
   ;    xchg bx,bx
   ;    loop .show

prepare_protected_mode:;加载保护模式
   ;xchg bx,bx;

   cli;关闭中断

   ;打开A20线
   in al,0x92;从0x92读取一个字节
   or al,0b10;修改第二位置为1
   out 0x92,al;输出al的数据

   lgdt [gdt_ptr];加载 gdt 把全局描述符的地址放到gdt寄存器上

   ;启动保护模式
   mov eax,cr0
   or eax,1;将第0位置为1
   mov cr0,eax
   ;用跳转来刷新缓存，并启用保护模式
   jmp dword code_selector:protect_mode
   ;跳转目标地址的大小为32位执行code_selector然后跳转到protected_mode
 



print:
   mov ah, 0x0e
.next:
   mov  al,[si]
   cmp al, 0
   jz .done
   int 0x10
   inc si
   jmp .next
.done:
   ret

loading:
    db "Loading Onix...", 10, 13, 0;\n \r
detecting:
    db "Detecting Memory Success...", 10, 13, 0;\n \r
error:
   mov si,.msg
   call print
   hlt;cpu停止
   jmp $
   .msg db "Loading Error...", 10, 13, 0;\n \r   




[ bits 32]
protect_mode:
 
   ;xchg bx,bx  直接进入内核
   mov ax,data_selector
   mov ds,ax
   mov es,ax
   mov fs,ax
   mov gs,ax
   mov ss,ax;初始化段寄存器

   mov esp,0x10000;修改栈顶；就是操作系统存放的位置

   ;直接对内存修改，不用再借助寄存器
   
   mov edi, 0x10000;输出到硬盘的哪个位置
   mov ecx, 10;起始扇区ecx寄存器中
   mov bl,200;读入几个扇区
   call read_disk

   jmp dword code_selector:0x10000;跳转到代码段，并找到代码段0x10000位置的地方执行
   ud2;表示出错
jmp $;阻塞

read_disk:
   ;初始化寄存器的值
      ;设置第二个端口读取扇区的数量
      mov dx,0x1f2;将扇区数量的值传入端口寄存器
      mov al,bl;将读取扇区数量的值bl赋给al
      out dx,al;向0x1f2端口写入al的值

      inc dx;自增
      mov al,cl;将前八位放入al中
      out dx,al;向0x1f3输出al的值

      inc dx;自增
      shr ecx,8;将ecx右移8位
      mov al,cl;将中八位放入al中
      out dx,al;向0x1f4输出al的值

      inc dx;自增
      shr ecx,8;将ecx右移8位
      mov al,cl;将后八位放入al中
      out dx,al;向0x1f5输出al的值

      inc dx;自增0x1f6
      shr ecx,8;将ecx右移8位
      and cl,0b1111;进行与运算，相当于保留后四位，将前四位赋0
      mov al, 0b1110_0000;配置0x1f6的值
      or al,cl;将cl与al合并
      out dx,al;

      inc dx;自增
      mov al,0x20;读硬盘
      out dx,al;输出
      
      xor ecx,ecx;清空ecx
      mov cl,bl;得到读写扇区数量
      .read:
        push cx;保存cx的值进入栈中得到副本数据
         call .waits;等待数据准备完毕进行检查
         call .reads;读取操作
        pop cx;将副本数据恢复
         loop .read

         ret
      .waits: 
      mov dx,0x1f7
      .check:
         in al,dx
         jmp $+2;延迟
         jmp $+2
         jmp $+2
         and al,0b1000_1000;保留第三位和第七位，其他全赋0
         cmp al,0b0000_1000;比较这两个字符，会把结果存入ZF标志位
         jnz .check;起到了一个循环判断的作用
         ret

      .reads:
         mov dx,0x1f0;把读的数据放入dx中
          mov cx,256;设置一个扇区256个字改变了cx的值
      .readw:
         in ax,dx
         jmp $+2;延迟
         jmp $+2
         jmp $+2
         mov [edi] , ax
         add edi,2;往下递增地址 
         loop .readw
         ret
      

code_selector equ (1 << 3);索引往右移3位
data_selector equ (2 << 3)
;常数
memory_base equ 0;基地址
;界限=（4G/4K）-1
memory_limit equ ((1024 * 1024 * 1024 * 4)/(4*1024))-1;4G/4KB  内存长度-1



gdt_ptr:
  ;末地址-初地址-1
  dw (gdt_end-gdt_base)-1
  dd gdt_base
gdt_base:;全局描述符表的开始位置
  dd 0 ,0 ; NULL 描述符//用0来填充4个字节
gdt_code:;代码段
   dw memory_limit & 0xffff ;将界限进行位运算来进行截断得到0 ~ 15 位的段界限
   dw memory_base & 0xffff;由于只有实模式只有16位，所以和上边类似，也需要截断得到 0
   ;~ 15 位的基地址
   db (memory_base >>16) & 0xff;先右移再进行位运算进行截断，从而来得到基地址的最后8位
   ;    存在  特权等级2位 段类型是代码段还是数据段  type 共4位
   db 0b_1_00_1_1_0_1_0
  ;  粒度 位数 是否扩展 赠送的  段界限
   db 0b1_1_0_0_0000 |(memory_limit >>16) & 0xf
   db (memory_base>>24) & 0xff;基地址 24 ~ 31
gdt_data:;数据段 
   dw memory_limit & 0xffff ;将界限进行位运算来进行截断得到0 ~ 15 位的段界限
   dw memory_base & 0xffff;由于只有实模式只有16位，所以和上边类似，也需要截断得到 0
   ;~ 15 位的基地址
   db (memory_base >>16) & 0xff;先右移再进行位运算进行截断，从而来得到基地址的最后8位
   ;    存在  特权等级2位 段类型是代码段还是数据段  type 共4位 向上 -可写- 没有被读过
   db 0b_1_00_1_0_0_1_0
  ;  粒度 位数 是否扩展 赠送的  段界限
   db 0b1_1_0_0_0000 |(memory_limit >>16) & 0xf
   db (memory_base>>24) & 0xff;基地址 24 ~ 31
gdt_end:

ards_count:; ards 大小
      dw 0
ards_buffer:;ards缓冲区，不固定值是因为不同的虚拟机的buffer不同

