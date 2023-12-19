[org 0x7c00]

;设置屏幕模式为文本模式：清除屏幕
mov ax, 3
int 0x10
;初始化段寄存器
mov ax, 0
mov ds, ax
mov es, ax
mov ss,ax
mov sp, 0x7c00

mov si,booting 
call print
;确定输出的基本信息

xchg bx,bx;bochs 的魔术断点
mov edi, 0x1000;读到硬盘的哪个位置
mov ecx, 0;将要输入的扇区的数据放入ecx寄存器中
mov bl,1;读入几个扇区
call read_disk

xchg bx,bx;bochs 的魔术断点
mov edi, 0x1000;写到硬盘的哪个位置
mov ecx, 2;起始扇区
mov bl,1;读入几个扇区
call write_disk

xchg bx,bx;bochs 的魔术断点
;阻塞
   jmp $
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
      


write_disk:
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
      mov al,0x30;读硬盘
      out dx,al;输出
      
      xor ecx,ecx;清空ecx
      mov cl,bl;得到读写扇区数量
      .write:
        push cx;保存cx的值进入栈中得到副本数据
       call .writes;读取操作
      call .waits;等待数据准备完毕进行检查
        pop cx;将副本数据恢复
         loop .write

         ret
      .waits: 
      mov dx,0x1f7
      .check:
         in al,dx
         jmp $+2;延迟
         jmp $+2
         jmp $+2
         and al,0b1000_0000;保留第三位和第七位，其他全赋0
         cmp al,0b0000_0000;比较这两个字符，会把结果存入ZF标志位
         jnz .check;起到了一个循环判断的作用
         ret

      .writes:
         mov dx,0x1f0;把读的数据放入dx中
          mov cx,256;设置一个扇区256个字改变了cx的值
      .writew:
      mov  ax, [edi]
        out dx,ax
         jmp $+2;延迟
         jmp $+2
         jmp $+2
         
         add edi,2;往下递增地址 
         loop .writew
         ret
      
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

booting:
    db "Booting Onix...", 10, 13, 0;\n \r
;填充
 times 510-($ - $$) db 0
;主引导扇区规定最后两个字节必须是0x55，0xaa
 db 0x55, 0xaa
