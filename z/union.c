#include <stdio.h>
#include <stdlib.h>
union un{
    int i;
    char ch;
};
void main(){
    union un my;
    my.i=12;
    my.ch='A';
    printf("%d ",my.i);
    printf("%c",my.ch);
}