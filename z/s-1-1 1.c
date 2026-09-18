#include <stdio.h>

union U {
    int x;
    int y;
    unsigned int z;
};

int main() {
    union U u;
    u.x = -1; // Все члены делят одну память, поэтому u.y и u.z тоже изменятся

    printf("x = %d\n", u.x);
    printf("y = %d\n", u.y);
    printf("z = %u\n", u.z); // Исполльзуем %u для беззнакового вывода

    return 0;
}
// дал мне пример кода си, там: union u [int x int y insigned int z]    union.u[1] = - 1   3 prinf(s s s x y z)
// тип там беззнаковый инт есть , ответ - 1 - 1 1
