BUILD:=../build
SRC:=.

ENTRYPOINT:=0x10000

CFLAGS := -m32 # 指明程序的大小
CFLAGS += -fno-builtin # 不需要 gcc 的内置函数，否则会自动替换 gcc 库中的函数
CFLAGS += -nostdinc # 不需要标准头文件
CFLAGS += -fno-pic # 不需要位置无关的代码，position independent code 强制更改储存位置
CFLAGS += -fno-pie # 不需要位置无关的可执行程序
CFLAGS += -nostdlib # 不需要标准库
CFLAGS += -fno-stack-protector  # 不需要栈保护
CFLAGS := $(strip $(CFLAGS)) # 删除多余的空格


DEBUG:= -g# 添加调试信息
INCLUDE:=-I$(SRC)/include #定位到src的include文件下

$(BUILD)/boot/%.bin: $(SRC)/boot/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f bin $< -o $@

$(BUILD)/%.o: $(SRC)/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@


$(BUILD)/%.o: $(SRC)/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/kernel.bin: \
	$(BUILD)/kernel/start.o \
	$(BUILD)/kernel/main.o \
	$(BUILD)/kernel/console.o \
	$(BUILD)/kernel/io.o 	\
	$(BUILD)/kernel/printk.o 	\
	$(BUILD)/kernel/assert.o 	\
	$(BUILD)/kernel/debug.o 	\
	$(BUILD)/kernel/global.o 	\
	$(BUILD)/kernel/task.o 	\
	$(BUILD)/kernel/schedule.o 	\
	$(BUILD)/kernel/interrupt.o \
	$(BUILD)/kernel/handler.o 	\
	$(BUILD)/kernel/clock.o 	\
	$(BUILD)/kernel/time.o 	\
	$(BUILD)/kernel/rtc.o 	\
	$(BUILD)/kernel/memory.o 	\
	$(BUILD)/lib/stdlib.o 	\
	$(BUILD)/lib/vsprintf.o 	\
	$(BUILD)/lib/string.o 

	$(shell mkdir -p $(dir $@))  
	ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRYPOINT) 

$(BUILD)/system.bin: $(BUILD)/kernel.bin
	objcopy -O binary $< $@


$(BUILD)/system.map: $(BUILD)/kernel.bin
	nm $< | sort > $@


$(BUILD)/master.img: $(BUILD)/boot/boot.bin \
	$(BUILD)/boot/loader.bin  \
	$(BUILD)/system.bin  \
	$(BUILD)/system.map  \

#   创建一个 16M 的硬盘镜像
	yes | bximage  -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
#   将 boot.bin 写入主引导扇区
	dd if=$(BUILD)/boot/boot.bin of=$@ bs=512 count=1 conv=notrunc 
#   将 loader.bin 写入硬盘
	dd if=$(BUILD)/boot/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
#	测试 system.bin 小于 100k，否则就修改下面的 count
	test -n "$$(find $(BUILD)/system.bin -size -100k)"
#   将 system.bin 写入硬盘
	dd if=$(BUILD)/system.bin of=$@ bs=512 count=200 seek=10 conv=notrunc  


test: 	$(BUILD)/master.img
.PHONY: clean
clean:
	rm -rf $(BUILD)
	
.PHONY: bochs
bochs: $(BUILD)/master.img
	bochs -q -f ../bochs/bochsrc

.PHONY: bochsg
bochsg: $(BUILD)/master.img
	bochs-gdb -q -f ../bochs/bochsrc.gdb

# .PHONY: qemu
# qemu: $(BUILD)/master.img
# 	qemu-system-i386 \
# 	-m 32M \
# 	-boot c \
# 	-drive file=$<,if=ide,index=0,media=disk,format=raw \
# 	-audiodev pa,id=hda \
# 	-machine pcspk-audiodev=hda \
# 	-rtc base=localtime \
	
# .PHONY: qemug
# qemug: $(BUILD)/master.img
# 	qemu-system-i386 \
# 	-s -S  \
# 	-m 32M \
# 	-boot c \
# 	-drive file=$<,index=0,media=disk,format=raw \
# 	-audiodev pa.id=hda \
# 	-machine pcspk-audiodev=hda \

QEMU:= qemu-system-i386 \
	-m 32M \
	-boot c \
	-drive file=$(BUILD)/master.img,if=ide,index=0,media=disk,format=raw \
	-audiodev pa,id=hda \
	-machine pcspk-audiodev=hda \
	-rtc base=localtime \

.PHONY: qemu
qemu: $(BUILD)/master.img
	$(QEMU)

.PHONY: qemug
qemug: $(BUILD)/master.img
	$(QEMU) -s -S


$(BUILD)/master.vmdk: $(BUILD)/master.img
	qemu-img convert -O vmdk $< $@

.PHONY:vmdk
vmdk: $(BUILD)/master.vmdk