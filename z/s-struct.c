// он мне сказал написать структуру с 2 интовыми переменными, выделить под неё память, задать значения переменным и вывести адреса их (указатель?я хз) 

#include <stdio.h>

struct Person {
    int age;      
    double height; 
};

int main() {
    struct Person user;
    user.age = 25;
    user.height = 178.5;

    struct Person *ptrToStruct = &user;
    int *ptrToAge = &user.age;
    double *ptrToHeight = &user.height;

    printf("--- Информация о структуре и памяти ---\n");
    printf("Адрес начала структуры в памяти:  %p\n\n", (void*)ptrToStruct);
    printf("Переменная age:    Значение = %d,   Адрес = %p\n", user.age, (void*)ptrToAge);
    printf("Переменная height: Значение = %.1f, Адрес = %p\n\n", user.height, (void*)ptrToHeight);
    printf("Размер всей структуры в байтах: %zu\n", sizeof(user));
    return 0;
}
