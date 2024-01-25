#include <onix/types.h>


char *strcpy(char *dest,const char *src)//将src的值赋给dest
{
    char * ptr=dest;
    while(true)
    {
        *ptr++ = *src;
        if(*src++==EOS)
        {
            return dest;
        }
    }
}
char *strcat(char *dest,const char *src)//从dest往后续src
{
    char *ptr=dest;
    while(*ptr!=EOS)
    {
        ptr++;
    }
    while(true)
    {
        *ptr++=*src;
        if(*src++==EOS)
        {
            return dest;
        }
        
    }
    
}
size_t strlen(const char *str)//计算长度
{
    char *ptr=(char*)str;
    while(*ptr!=EOS)
    {
        ptr++;
    }
    return ptr-str;
}
int strcmp (const char*lhs,const char *rhs)
{

    while(*lhs==*rhs&&*lhs!=EOS&&*rhs!=EOS)
    {
        lhs++;
        rhs++;
    }
    return *lhs<*rhs?-1:*lhs>*rhs;

}
char *strchr(const char *str,int ch);
char *strrchr(const char *str,int ch);

int memcmp (const void *lhs,const void *rhs,size_t count);
void *memset(void *dest,int ch,size_t count);
void *memcpy(void *dest,const void *src,size_t count);
void *memchr(const void * ptr,int ch,size_t count);