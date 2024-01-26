#include <onix/string.h>

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
char *strchr(const char *str,int ch)//查询字符串中的字母为ch
{
    char * ptr=(char *)str;
    while (true)
    {
        if(*ptr ==ch)
        {
            return ptr;
        }
        if(*ptr++==EOS)
        {
            return NULL;

        }
    }
}
char *strrchr(const char *str,int ch)
{
    char *last=NULL;
    char * ptr=(char*)(str);
    while(true)
    {
        if(*ptr==ch)
        {
            last=ptr;
        }
        if(*ptr++==EOS)
        {
            return last;
        }
    }
}

int memcmp(const void *lhs, const void *rhs, size_t count)
{
    char *lptr = (char *)lhs;
    char *rptr = (char *)rhs;
    while ((count > 0) && *lptr == *rptr)
    {
        lptr++;
        rptr++;
        count--;
    }
    if (count == 0)
        return 0;
    return *lptr < *rptr ? -1 : *lptr > *rptr;
}
void *memset(void *dest,int ch,size_t count)
{
    char *ptr=dest;
    while(count--)
    {
        *ptr++=ch;
    }
    return dest;
}
void *memcpy(void *dest,const void *src,size_t count)

{
    char * ptr= dest;
    while(count--)
    {
        *ptr++=*((char*)(src++));
    }
    return dest;

}
void *memchr(const void * str,int ch,size_t count)
{
    char *ptr=(char*)str;
    while(count --)
    {
        if(*ptr==ch)
        {
            return (void*)ptr;
        }
        ptr++;
    }
    }