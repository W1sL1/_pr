// Язык си. 
// Лабораторная работа "4. Использование динамической памяти при обработке массивов"
// Необходимо реализовать программу, демонстрирующую использование 
// динамического выделения и освобождения памяти при обработке данных. 
// На вход программе подается сначала количество отсортированных массивов 
// целых чисел N, затем построчно размер массива и после его элементы. 
// Для каждого массива необходимо считать его длину и выделить 
// динамическую память ровно под то количество элементов, 
// которое содержится в массиве. 
// Программа должна считать входные массивы, 
// обработать их в соответствии с номером варианта 
// и вывести результат выполнения операции. 
// При формировании итогового массива программа должна использовать 
// только необходимый объем памяти, при необходимости изменяя размер 
// выделенной памяти. После завершения работы программы вся динамически 
// выделенная память должна быть корректно освобождена.
// Информация к вашему варианту задания:
// Объединить все входные массивы в один отсортированный массив, сохранив все элементы, включая повторяющиеся:
// Ввод
// 3
// 3 1 3 5
// 2 2 4
// 4 1 2 6 7
// Вывод
// 1 1 2 2 3 4 5 6 7

#include <stdio.h>
#include <stdlib.h>
int main() {
    int n; if (scanf("%d", &n) != 1) return 0;
    //  final array     current size of final array 
    int *result = NULL; int total_size = 0; 
    // 
    for (int i = 0; i < n; i++) {
        int m; scanf("%d", &m);
        //temp memory for current array
        int *current_arr = (int *)malloc(m * sizeof(int));
        //
        for (int j = 0; j < m; j++) scanf("%d", &current_arr[j]);
        //expanding resulting array to accommodate new elements
        int *temp = (int *)realloc(result, (total_size + m) * sizeof(int));
        //
        if (temp == NULL) {
            free(result);
            free(current_arr);
            return 1;
        }
        result = temp;
        //copy elements into the newly selected part 
        for (int j = 0; j < m; j++) {
            result[total_size + j] = current_arr[j];
        }
        total_size += m;
        free(current_arr);
    }
    //buble sort for final array 
    for (int i = 0; i < total_size - 1; i++) {
        for (int j = 0; j < total_size - i - 1; j++) {
            if (result[j] > result[j + 1]) {
                int tmp = result[j];
                result[j] = result[j + 1];
                result[j + 1] = tmp;
            }
        }
    }
    for (int i = 0; i < total_size; i++) {
        printf("%d%c", result[i], (i == total_size - 1 ? '\n' : ' '));
    }
    free(result); return 0;
}
