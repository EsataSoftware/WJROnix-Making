
CFLAGS:= -m32
CFLAGS+= -Qn
CFLAGS+= -fno-pic
#CFLAGS+= -fomit-frame-pointer
CFLAGS+= -mpreferred-stack-boundary=2
CFLAGS+= -fno-asynchronous-unwind-tables
CFLAGS := $(strip $(CFLAGS)) # 删除多余的空格

.PHONY:pahams.s
pahams.s: pahams.c
	gcc $(CFLAGS) -S $< -O $@
