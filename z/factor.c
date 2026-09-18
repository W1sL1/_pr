#include <stdio.h>
unsigned long long factorial(int n) {
    if (n <= 1) return 1; // baze 0! = 1, 1! = 1 
    return n * factorial(n - 1);
}
int main() {
    int number ;
    printf("input int pls\n");
    scanf("%d",&number);
    unsigned long long result = factorial(number);
    printf("Факториал %d равен %llu\n", number, result);
    return 0;
}
