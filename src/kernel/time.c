#include  <onix/time.h>
#include  <onix/debug.h>
#include  <onix/stdlib.h>
#include  <onix/io.h>
#include  <onix/rtc.h>
#define LOGK(fmt,args...) DEBUGK (fmt,##args)

#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

//  下面是 CMOS 信息的寄存器索引
#define CMOS_SECOND 0x00 // (0 ~ 59)
#define CMOS_MINUTE 0x02 // (0 ~ 59)
#define CMOS_HOUR 0x04 // (0 ~ 23)
#define CMOS_WEEKDAY 0x06 // (1 ~ 7)周末为1，周六为7
#define CMOS_DAY 0x07 // (1 ~ 31)
#define CMOS_MONTH 0x08 // (1 ~ 12)
#define CMOS_YEAR 0x09 // (0 ~ 99)
#define CMOS_CENTURY 0x32 // 可能不存在
#define CMOS_NMI 0x80 

#define MINUTE 60
#define HOUR (60 * MINUTE)
#define DAY (24 * HOUR)
#define YEAR (365 * HOUR)

// 每个月开始时的已经过去的天数

static int month[13] = 
{   
    0,//占位
    0,
    (31),
    (31 +29 ),
    (31 + 29 +  31),
    (31 + 29 +  31+ 30),
    (31 + 29 +  31+ 30+31),
    (31 + 29 +  31+ 30+31+30),
    (31 + 29 +  31+ 30+31+30+31),
    (31 + 29 +  31+ 30+31+30+31+31),
    (31 + 29 +  31+ 30+31+30+31+31+30),
    (31 + 29 +  31+ 30+31+30+31+31+30+31),
    (31 + 29 +  31+ 30+31+30+31+31+30+31+30),
    
};
time_t startup_time;
int century;

time_t mktime(tm *time)
{
    time_t res;
    int year;// 1970 年开始的年数
    // 从1900年开始计算
    if(time->tm_year >=70)
        year =time->tm_year-70;
    else 
        year = time->tm_year-70+100;
    // 这些年经过的秒数
    res =YEAR * year;
    //已经过去的闰年，每个加 1 天 
    res += DAY * ((year +1)/4);
    // 已经过去的月份的时间
    res += month[time->tm_mon]*DAY;
    // 如果 2 月已经过了，并且当前不是闰年，则减去一天
    if (time->tm_mon > 2 &&((year + 2) % 4))
        res -=DAY;
    // 这个月已经过去的天
    res += DAY * (time->tm_mday-1);

    // 今天过去的小时
    res += HOUR * time->tm_hour;
    
    // 这个分钟过去的秒
    res += MINUTE*time->tm_min;

    // 分钟过去的秒
    res += time->tm_sec;
    return res;
}

int get_yday(tm * time )
{
    int res = month[time->tm_mon];//已经过去的月的天数
    res += time->tm_mday;// 这个月过去的天数
    int year;
    if(time->tm_year>=70)
        year = time->tm_year-70;
    else 
        year = time->tm_year-70+100;
    if((year+2)%4&&time->tm_mon>2)
    {
        res -=2;
    }
    return res;
}

void time_read_bcd(tm * time)
{
    //CMOS 的访存速度很慢，为了减少时间吴超，读取下面循环中所有数值后
    //若此时 CMOS 中秒值发生变化，那么重新读取所有值。

    do 
    {

        time->tm_sec=cmos_read(CMOS_SECOND);// 从CMOS寄存器中获取时间
        time->tm_min= cmos_read(CMOS_MINUTE);// 获取时间
        time->tm_hour= cmos_read(CMOS_HOUR);
        time->tm_wday= cmos_read(CMOS_WEEKDAY);
        time->tm_mday=cmos_read(CMOS_DAY);
        time->tm_mon=cmos_read(CMOS_MONTH);
        time->tm_year= cmos_read(CMOS_YEAR);
        century=cmos_read(CMOS_CENTURY);
    }while(time->tm_sec!=cmos_read(CMOS_SECOND));
}
void time_read(tm *time)
{
    time_read_bcd(time);
    time->tm_sec=bcd_to_bin(time->tm_sec);
    time->tm_min=bcd_to_bin(time->tm_min);
    time->tm_hour=bcd_to_bin(time->tm_hour);
    time->tm_wday=bcd_to_bin(time->tm_wday);
    time->tm_mday=bcd_to_bin(time->tm_mday);
    time->tm_mon=bcd_to_bin(time->tm_mon);
    time->tm_year=bcd_to_bin(time->tm_year);
    time->tm_yday=get_yday(time);
    time->tm_isdst =-1;
    century=bcd_to_bin(century);
}
void time_init()
{
    tm time;
    time_read(&time);
    startup_time=mktime(&time);
    LOGK("startup time: %d%d-%02d-%02d %02d:%02d:%02d\n",
          century,
          time.tm_year,
          time.tm_mon,
          time.tm_mday,
          time.tm_hour,
          time.tm_min,
          time.tm_sec
          );
     //hang();
}



