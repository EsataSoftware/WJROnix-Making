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

