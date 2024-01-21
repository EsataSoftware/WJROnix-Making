#ifndef ONIX_TYPES_H
#define ONIX_TYPES_H
//一些常量
#define EOF -1// 判断错误的情况

#define NULL 0//空指针

#define bool _Bool//C++的bool
#define  true 1
#define false 0

#define _packed __attribute__((packed)) // 用于定义特殊的结构体
//定义无符号数
typedef unsigned int size_t;
//定义基本数据类型 int8指的是8位字节
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
//定义无符号数
typedef unsigned char u8;  //  u8: 该别名表示一个无符号8位整数（字节）。u8 可以存储的范围是 0 到 255。
typedef unsigned short u16;//u16: 该别名表示一个无符号16位整数。u16 可以存储的范围是 0 到 65,535。
typedef unsigned int u32;//u32: 该别名表示一个无符号32位整数。u32 可以存储的范围是 0 到 4,294,967,295。
typedef unsigned long long u64;//u64: 该别名表示一个无符号64位整数。u64 可以存储的范围是 0 到 18,446,744,073,709,551,615。

#endif

