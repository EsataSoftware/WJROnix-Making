[org 0x1000]

  dw 0x55aa;来判断主引导扇区是否发生错误

xchg bx,bx;
  mov si ,loading
  call print  
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