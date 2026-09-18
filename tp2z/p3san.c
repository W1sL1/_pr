#include <stdio.h>

// 1. Объявляем структуру
struct Person {
    int age;       // Первая переменная (4 байта)
    double height; // Вторая переменная (8 байт)
};

int main() {
    // 2. Выделяем память под структуру (создаем переменную типа struct Person)
    struct Person user;

    // 3. Записываем значения в переменные структуры
    user.age = 25;
    user.height = 178.5;

    // 4. Получаем указатели на саму структуру и на её элементы
    struct Person *ptrToStruct = &user;
    int *ptrToAge = &user.age;
    double *ptrToHeight = &user.height;

    // 5. Выводим значения и адреса памяти
    printf("--- Информация о структуре и памяти ---\n");
    
    // %p используется для вывода адресов (указателей) в шестнадцатеричном формате
    printf("Адрес начала структуры в памяти:  %p\n\n", (void*)ptrToStruct);
    
    printf("Переменная age:    Значение = %d,   Адрес = %p\n", user.age, (void*)ptrToAge);
    printf("Переменная height: Значение = %.1f, Адрес = %p\n\n", user.height, (void*)ptrToHeight);

    // Демонстрация: адрес структуры совпадает с адресом её первого элемента
    printf("Размер всей структуры в байтах: %zu\n", sizeof(user));

    return 0;
}
