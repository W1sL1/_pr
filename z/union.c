#include <stdio.h>
#include <stdlib.h>
union un{
    char ch1;
    char ch2;
};
void main(){
    union un my;
    my.ch1='A';
    my.ch2='B';
    printf("%c %c",my.ch1,my.ch2);
    char *ptr1=&my.ch1; char *ptr2=&my.ch2;
    printf("\n%p %p",ptr1,ptr2);
    union un *pt=&my;
    printf("\n%p",pt);
}