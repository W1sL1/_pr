//Лабораторная работа "3. Найти n - ную подходящую дробь по значению цепной дроби."
//Найти n - ную подходящую дробь по значению цепной дроби.
//В стандартный поток ввода(stdin) через пробел подаются заранее не известное количество 
//элементов цепной дроби и n(ввод оканчивается EOF).
//Данные элементы необходимо сохранить в динамический массив.
//Далее на основе этих значений вычислить n - ную подходящую дробь.
//n - ной подходящей дробью для цепной дроби[a0; a1, a2, a3, …, an], 
//является значение некоторого рационального числа pn / qn
//Эйлер вывел рекуррентные формулы для вычисления числителей и знаменателей подходящих дробей :
//p - 1 = 1,
//p0 = a0,
//pn = an * pn - 1 + pn - 2
//q - 1 = 0,
//q0 = 1,
//qn = an * qn - 1 + qn - 2.
//вход: элементы цепной дроби и n.
//пример : 5 3 7 15 1 292 1 1
//выход : числитель и знаменатель n - ной подходящей дробь.
//пример : 104348 33215

#include <stdio.h>
#include <stdlib.h>
int main() {
    int* a = NULL; int count = 0; int num;
    while (scanf("%d", &num) == 1) {
        a = (int*)realloc(a, (count + 1) * sizeof(int));
        if (a == NULL) return 1; a[count++] = num;
    }
    if (count < 2) {free(a);return 1;}
    //last numb is N
    int n = a[count - 1];
    count--; // now count is count elem of fraction 
    if (n >= count) {free(a);return 1;}
    long long p_prev2 = 1, p_prev1 = a[0];
    long long q_prev2 = 0, q_prev1 = 1;
    long long p_curr = p_prev1, q_curr = q_prev1;
    for (int i = 1; i <= n; i++) {
        p_curr = a[i] * p_prev1 + p_prev2;
        q_curr = a[i] * q_prev1 + q_prev2;
        p_prev2 = p_prev1; q_prev2 = q_prev1;
        p_prev1 = p_curr; q_prev1 = q_curr;
    }
    printf("%lld %lld\n", p_curr, q_curr);
    free(a);return 0;
}