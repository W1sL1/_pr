//ProzorovPA_5151004/50002
#include <stdio.h>
#include <stdlib.h>
double my_abs(double x) { //Функция для вычисления модуля double
    return (x < 0) ? -x : x; //Тернарный оп. ?if true :if false
}
int main() {
    int num;
    scanf("%d", &num);
    int* ar = (int*)malloc(num * sizeof(int)); //Выделение памяти под массив
    //
    if (ar == NULL) return 1; //Проверка успеха выделения памяти
    double sum = 0;
    for (int indx = 0; indx < num; indx++) { //Находим сумму
        scanf("%d", &ar[indx]);
        sum += ar[indx];
    }
    double avg = sum / num; //Среднее арифм.
    int answ = ar[0];
    double min_dff = my_abs(ar[0] - avg); //Начальная разница
    for (int indx = 1; indx < num; indx++) {
        double dff = my_abs(ar[indx] - avg); //Разница для сравнения
        if (dff < min_dff) { //Если разница меньше минимальной
            min_dff = dff; //Обновление минимальной разницы
            answ = ar[indx]; //Обновление ответа
        }
    }
    printf("%d\n", answ);
    free(ar); //Освобождение памяти
    return 0;
}
