#include <stdio.h>
#include <stdlib.h>

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;

    int *result = NULL; // Итоговый массив
    int total_size = 0; // Текущий размер итогового массива

    for (int i = 0; i < n; i++) {
        int m;
        scanf("%d", &m);

        // Выделяем временную память под текущий массив
        int *current_arr = (int *)malloc(m * sizeof(int));
        for (int j = 0; j < m; j++) {
            scanf("%d", &current_arr[j]);
        }

        // Расширяем итоговый массив под новые элементы
        int *temp = (int *)realloc(result, (total_size + m) * sizeof(int));
        if (temp == NULL) {
            free(result);
            free(current_arr);
            return 1;
        }
        result = temp;

        // Копируем элементы во вновь выделенную часть
        for (int j = 0; j < m; j++) {
            result[total_size + j] = current_arr[j];
        }
        total_size += m;

        // Освобождаем память текущего массива
        free(current_arr);
    }

    // Сортировка итогового массива (простой метод вставками или пузырьком)
    for (int i = 0; i < total_size - 1; i++) {
        for (int j = 0; j < total_size - i - 1; j++) {
            if (result[j] > result[j + 1]) {
                int tmp = result[j];
                result[j] = result[j + 1];
                result[j + 1] = tmp;
            }
        }
    }

    // Вывод результата
    for (int i = 0; i < total_size; i++) {
        printf("%d%c", result[i], (i == total_size - 1 ? '\n' : ' '));
    }

    // Освобождение итогового массива
    free(result);

    return 0;
}
