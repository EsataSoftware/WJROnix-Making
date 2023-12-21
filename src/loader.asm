[org 0x1000]

  dw 0x55aa;来判断主引导扇区是否发生错误


  mov si ,loading
  call print  
xchg bx,bx;

detect_memory:
   ;寄存器赋值
   xor ebx ,ebx;将ebx赋0
   ;确定缓冲区的大小
   mov ax,0;
   mov es,ax
   mov edi,ards_buffer;

   mov edx ,0x534d4150;固定签名

   .next
      ;子功能号
      mov eax,0xe820
      ;结构体的大小（字节）
      mov ecx ,20

  ;显示loading
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


   ards_count:;ards大小
      dw 0
   ards_buffer:;ards缓冲区，不固定值是因为不同的虚拟机的buffer不同

