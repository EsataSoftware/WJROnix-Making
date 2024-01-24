[bits 32]

test:
    ret
extern exit 
global main
main:
   
  call test
    push 0;
    call exit