# WJROnix-Making

基于Archlinux vmware和qemu和bochs实现简易操作系统

## 工具

    vscode用来调试和操作
    bochs 用于模拟真实的cpu和硬件结构的软件，便于观察和调试每一行汇编，寄存器做了什么操作。
    qemu   用于快速开发测试内核，不需要直接去把Linux部署到真实的开发板就可以实现编译运行，有模拟的io设备。

## boot部分

在实模式中未进入保护模式，使用BIOS中断服务(ROM中写好的服务)常见的服务。在一般的Linux中被驱动程序替代

- int 0x10

        显示字符/图形(视频服务)

- int 0x13

        读写硬盘(磁盘服务)

- int 0x16

        键盘输入(键盘服务)
- int 0x15

        系统配置(内存检测)

### boot.asm

操作系统引导程序(bootloader)

```asm
[org 0x7c00];告诉汇编器将这个装入0x7c00位置
```

初始化显示和寄存器

- 初始化显示和寄存器

      - 初始化显示
                
                使用BIOS中断0x10(视频服务),向ah写入0x0e，向al写[si]。si存的是要显示的字符。用print去显示
- 打印提示信息

        通过上述逻辑,向si写入数据，并将si数据写入al;
- 从硬盘读取扇区到内存

        调用BIOS的int 0x13

- 验证数据正确性

        比较0x55aa是否为主引导扇区的首数据

### loader.asm

```asm
[org 0x 1000] ;同上
```

主引导程序包括内存检测、加载保护模式、保护模式、读取硬盘信息放入内存、初始化内存信息、初始化gdt(全局描述符)

- 判断主引导扇区数据并打印

        主要做主引导扇区是否为55aa然后打印
- 物理机器内存检测(detect_memory)

        调用BIOS的ards系统调用去获取物理内存分布，以便于进入保护模式和加载内核去

- 加载保护模式(prepare_protected_mode)

        修改状态位，加载全局描述符gdt并jmp到保护模式
- 保护模式

        初始化各种通用寄存器(ax,ds,es,fs,gs,ss)。
        去修改esp(修改栈顶)
- 读取硬盘信息

        通过寄存器去加载内核，循环读取扇区，从0x1f0读取到EDI所指的位置
- 内存的相关参数

        memory_base(基地址),memory_limit(内存中可以存的页面数)

- gdt的相关参数

        gdt_base gdt_end通过汇编符号偏移量去以及每次运行时放入的绝对地址确定此时物理地址

## include/onix部分

    各种系统文件的头文件

## kernel

    各种文件的实现

## lib

    主要用于标准库输入输出

### #include<onix/stdlib.c>

- 基本工具

  ```cpp
  delay(u32 count);             //忙等待延时
  hang();                       //阻塞系统挂起
  bcd_to_bin(u8 value)          //bcd码二进制用于输入时间
  bin_to_bcd(u8 value)          //用于输出时间
  div_round_up(u32 num,u32 size)//用于计算页面的数目

        
