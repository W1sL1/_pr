//ProzorovPA_5151004/50002
//#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
//16x functions
char* hxshort(short num) {
    //save in memo out of func
    static char buf[22]; unsigned short n = (num < 0) ? -num : num; 
    //      to                           from
    sprintf(buf, "%s%x", (num < 0) ? "-" : "", n);return buf;}
char* hxint(int num) {
    static char buf[22]; unsigned int n = (num < 0) ? -num : num;
    sprintf(buf, "%s%x", (num < 0) ? "-" : "", n);return buf;}
char* hxlong(long num) {
    static char buf[22]; unsigned long n = (num < 0) ? -num : num;
    sprintf(buf, "%s%lx", (num < 0) ? "-" : "", n);return buf;}
//Convert.
char* conv_base(long long num, int base) {
    static char buf[65]; char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i = 0, negative = 0; long long n = num;
    if (n == 0) {
        buf[0] = '0'; buf[1] = '\0';
        return buf;
    }
    //(-)nums
    if (n < 0) {
        negative = 1;
        n = -n;
    }
    //convert. num
    while (n != 0) {
        long long rem = n % base; //reminder
        buf[i++] = dig[rem];
        n = n / base;
    }
    //add (-)
    if (negative) buf[i++] = '-';
    buf[i] = '\0';
    //str reverse
    for (int j = 0; j < i / 2; j++) {
        char tmp = buf[j];
        buf[j] = buf[i - 1 - j];
        buf[i - 1 - j] = tmp;
    }
    return buf;}
//main 
int main() { 
    //0x- signature for compilator (next 16x, not int)
    short vshort = 0xf55e; //-2722 in 16x 
    printf("short;%s=%s;34\n", hxshort(vshort), conv_base(vshort, 34));
    int vint = 0xde54f7fe; //-564856834 in 16x  
    printf("int;%s=%s;6\n", hxint(vint), conv_base(vint, 6));
    unsigned int vunint = 0x5f04c788; //1594148744 in 16x
    printf("unsigned int;%x=%s;31\n", vunint, conv_base(vunint, 31));
    long vlong = 0x3bcea81e; //1003399198 in 16x
    printf("long;%s=%s;7\n", hxlong(vlong), conv_base(vlong, 7));
    unsigned long vunlong = 0x843cbb50; //2218572624 in 16x
    printf("unsigned long;%lx=%s;15\n", vunlong, conv_base(vunlong, 15));
    return 0;}
