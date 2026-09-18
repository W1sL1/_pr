#include <stdio.h>
#include <stdlib.h>
struct person{
    int age;
    double he;
};
void main(){
    struct person user;
    user.age=10;
    user.he=180.1;
    printf("%d ",user.age);
    printf("%f",user.he);
}