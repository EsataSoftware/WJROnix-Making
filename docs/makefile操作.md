# 将生成的.bin放入build文件中

 BUILD:=../build
SRC:=.



$(BUILD)/boot/%.bin: $(SRC)/boot/%.asm
$(shell mkdir -p $(dir $@))
nasm -f bin $< -o $@

$(BUILD)/master.img: $(BUILD)/boot/boot.bin \
$(BUILD)/boot/loader.bin

yes | bximage  -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
dd if=$(BUILD)/boot/boot.bin of=$@ bs=512 count=1 conv=notrunc 
dd if=$(BUILD)/boot/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
.PHOMY: usb
usb: $(BUILD)/boot/boot.bin /dev/sdb
sudo dd if=/dev/sdb of=tmp.bin bs=512 count=1 conv=notrunc 
cp tmp.bin usb.bin
sudo rm tmp.bin
dd if=boot.bin of=usb.bin bs=446 count=1 conv=notrunc 
sudo dd if=usb.bin of=/dev/sdb bs=512 count=1 conv=notrunc 
rm usb.bin

test: $(BUILD)/master.img
.PHOMY: clean
clean:
rm -rf $(BUILD)

.PHOMY: bochs
bochs: $(BUILD)/master.img
bochs -q

 $(CFLAGE) $(DEBUG) $(INCLUDE)

INCLUDE 表明你要编译头文件所在的位置

- 使用前面定义的变量
- -c: 这个选项告诉编译器只进行编译而不进行链接，生成目标文件。
- $<: 这是自动变量，表示规则中的第一个依赖项，即源文件的文件名。
- -o $@: 这个选项告诉编译器生成的目标文件的名称，即规则中的目标文件名。

(BUILD)/kernel.bin: $(BUILD)/kernel/start.o \
     $(BUILD)/kernel/main.o
    $(shell mkdir -p $(dir $@))  
    ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRYPOINT)

- 需要下载m32的库在 Arch Linux 上启用 multilib 存储库需要编辑 /etc/pacman.conf 文件。以下是具体的步骤：
打开 /etc/pacman.conf 文件：
sudo nano /etc/pacman.conf
你可以使用其他文本编辑器，例如 vim 或 gedit，取决于你的个人偏好。
查找 [multilib] 部分：
在打开的 pacman.conf 文件中，找到包含 [multilib] 的部分。通常，这部分看起来像：
[multilib]
Include = /etc/pacman.d/mirrorlist
运行以下命令以更新软件包数据库：
sudo pacman -Sy
-Sy 选项会强制更新软件包数据库，确保它包含 multilib 存储库的信息。
现在，你已经成功启用了 multilib 存储库。这使得你可以安装并使用与 32 位架构兼容的软件包。