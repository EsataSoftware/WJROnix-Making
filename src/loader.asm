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
      xchg bx,bx

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

