#include <stdio.h>
#include <stdlib.h>

// Функция для слияния двух отсортированных массивов
int* merge_arrays(int* arr1, int size1, int* arr2, int size2, int* new_size) {
    int* result = (int*)malloc((size1 + size2) * sizeof(int));
    if (result == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(1);
    }
    
    int i = 0, j = 0, k = 0;
    
    // Слияние двух отсортированных массивов
    while (i < size1 && j < size2) {
        if (arr1[i] <= arr2[j]) {
            result[k++] = arr1[i++];
        } else {
            result[k++] = arr2[j++];
        }
    }
    
    // Копирование оставшихся элементов
    while (i < size1) {
        result[k++] = arr1[i++];
    }
    
    while (j < size2) {
        result[k++] = arr2[j++];
    }
    
    *new_size = size1 + size2;
    return result;
}

// Функция для вывода массива
void print_array(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int N; // Количество массивов
    int** arrays = NULL; // Массив указателей на динамические массивы
    int* sizes = NULL; // Массив размеров каждого массива
    
    // Чтение количества массивов
    printf("Введите количество массивов: ");
    if (scanf("%d", &N) != 1) {
        fprintf(stderr, "Ошибка чтения количества массивов\n");
        return 1;
    }
    
    // Выделение памяти для хранения указателей на массивы и их размеров
    arrays = (int**)malloc(N * sizeof(int*));
    sizes = (int*)malloc(N * sizeof(int));
    
    if (arrays == NULL || sizes == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        free(arrays);
        free(sizes);
        return 1;
    }
    
    // Чтение массивов
    for (int i = 0; i < N; i++) {
        int size;
        if (scanf("%d", &size) != 1) {
            fprintf(stderr, "Ошибка чтения размера массива %d\n", i + 1);
            // Освобождение ранее выделенной памяти
            for (int j = 0; j < i; j++) {
                free(arrays[j]);
            }
            free(arrays);
            free(sizes);
            return 1;
        }
        
        sizes[i] = size;
        
        // Выделение памяти под массив
        arrays[i] = (int*)malloc(size * sizeof(int));
        if (arrays[i] == NULL && size > 0) {
            fprintf(stderr, "Ошибка выделения памяти для массива %d\n", i + 1);
            // Освобождение ранее выделенной памяти
            for (int j = 0; j < i; j++) {
                free(arrays[j]);
            }
            free(arrays);
            free(sizes);
            return 1;
        }
        
        // Чтение элементов массива
        for (int j = 0; j < size; j++) {
            if (scanf("%d", &arrays[i][j]) != 1) {
                fprintf(stderr, "Ошибка чтения элемента массива %d\n", i + 1);
                // Освобождение памяти
                for (int k = 0; k <= i; k++) {
                    free(arrays[k]);
                }
                free(arrays);
                free(sizes);
                return 1;
            }
        }
    }
    
    
    // Объединение всех массивов в один отсортированный
    int* merged_array = NULL;
    int merged_size = 0;
    
    // Начинаем с первого массива
    if (N > 0) {
        merged_array = (int*)malloc(sizes[0] * sizeof(int));
        if (merged_array == NULL) {
            fprintf(stderr, "Ошибка выделения памяти для результирующего массива\n");
            // Освобождение памяти
            for (int i = 0; i < N; i++) {
                free(arrays[i]);
            }
            free(arrays);
            free(sizes);
            return 1;
        }
        
        // Копируем первый массив
        for (int i = 0; i < sizes[0]; i++) {
            merged_array[i] = arrays[0][i];
        }
        merged_size = sizes[0];
        
        // Последовательно сливаем остальные массивы
        for (int i = 1; i < N; i++) {
            int new_size;
            int* new_merged = merge_arrays(merged_array, merged_size, arrays[i], sizes[i], &new_size);
            
            // Освобождаем старый объединенный массив
            free(merged_array);
            
            merged_array = new_merged;
            merged_size = new_size;
        }
    }
    
    // Вывод результата
    if (merged_size > 0) {
        print_array(merged_array, merged_size);
    } else {
        printf("Нет элементов для вывода\n");
    }
    
    // Освобождение динамически выделенной памяти
    // Освобождаем все входные массивы
    for (int i = 0; i < N; i++) {
        free(arrays[i]);
    }
    
    // Освобождаем массив указателей и массив размеров
    free(arrays);
    free(sizes);
    
    // Освобождаем результирующий массив
    free(merged_array);
    
    return 0;
}