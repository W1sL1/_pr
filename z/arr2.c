#include <stdio.h>

int main() {
    int arr1[10] = {5, 12, 3, 8, 21, 14, 7, 1, 9, 18};
    int arr2[15] = {11, 6, 25, 4, 13, 2, 17, 20, 15, 22, 0, 19, 16, 24, 10};
    
    int combined[25];
    
    for (int i = 0; i < 10; i++) {
        combined[i] = arr1[i];
    }
    for (int i = 0; i < 15; i++) {
        combined[10 + i] = arr2[i];
    }

    int num1, num2;
    printf("Введите два числа из массива: ");
    scanf("%d %d", &num1, &num2);

    int idx1 = -1, idx2 = -1;

    for (int i = 0; i < 25; i++) {
        if (combined[i] == num1) {
            idx1 = i;
        }
        if (combined[i] == num2) {
            idx2 = i;
        }
    }

    if (idx1 == -1 || idx2 == -1) {
        printf("Ошибка: одно или оба числа не найдены в массиве.\n");
        return 1;
    }

    int start = (idx1 < idx2) ? idx1 : idx2;
    int end = (idx1 < idx2) ? idx2 : idx1;

    if (end - start <= 1) {
        printf("Между этими числами нет других элементов.\n");
    } else {
        printf("Элементы между ними: ");
        for (int i = start + 1; i < end; i++) {
            printf("%d ", combined[i]);
        }
        printf("\n");
    }

    return 0;
}
