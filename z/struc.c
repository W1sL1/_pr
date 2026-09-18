#include <stdio.h>
#include <stdlib.h>

struct st{
    int a;
    int b;
};

void main(){
    struct st me;
    me.a=10;
    me.b=12;
    printf("%d %d",me.a,me.b);
    struct st *ptr = &me;
    printf("\n%p",ptr);
}