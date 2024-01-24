# qenu +vmware

## qemu

安装 qemu

   sudo pacman -S qemu
   sudo pacman -S qemu-arch-extra

## vmware

转换硬盘格式

qemu-img convert -pO vmdk $< $@

## samba

在vmware里访问文件
