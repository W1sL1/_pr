#include <stdio.h>
struct Point {
    int x;
    int y;
};
int main() {
    struct Point myPoint; // Сама структура
    struct Point *ptr;    // Указатель на структуру
    ptr = &myPoint;       // Записали адрес структуры в указатель
    // 1. Работаем напрямую через точку
    myPoint.x = 10; 
    // 2. Работаем через указатель и стрелку
    ptr->y = 20;          // То же самое, что (*ptr).y = 20;
    printf("Координаты: %d, %d\n", myPoint.x, myPoint.y); // Выведет: 10, 20
    return 0;
}
