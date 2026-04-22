#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 20

int adj[MAX_NODES][MAX_NODES]; // Матрица смежности
int visited[MAX_NODES];       // Массив посещенных вершин
int path[MAX_NODES];          // Текущий путь
int n = 0;                    // Общее количество узлов
int found_any = 0;            // Флаг: найден ли хотя бы один путь

// Функция для вывода найденного пути
void print_path(int length) {
    for (int i = 0; i < length; i++) {
        printf("%d%s", path[i] + 1, (i == length - 1) ? "" : ",");
    }
    printf("\n");
}

// Рекурсивный поиск всех путей (DFS + Backtracking)
void find_all_paths(int u, int destination, int path_index) {
    visited[u] = 1;
    path[path_index] = u;
    path_index++;

    if (u == destination) {
        print_path(path_index);
        found_any = 1;
    } else {
        for (int v = 0; v < n; v++) {
            if (adj[u][v] == 1 && !visited[v]) {
                find_all_paths(v, destination, path_index);
            }
        }
    }

    // Обратный ход (backtracking): снимаем пометку
    path_index--;
    visited[u] = 0;
}

int main() {
    char line[1024];
    int matrix[MAX_NODES][MAX_NODES];
    
    // Считывание матрицы смежности
    // Мы читаем строки, пока не встретим строку без запятых (где будут номера узлов)
    int r = 0;
    while (fgets(line, sizeof(line), stdin)) {
        if (strchr(line, ',') == NULL) {
            // Если в строке нет запятой, значит это целевые узлы (конец матрицы)
            int start_node, end_node;
            if (sscanf(line, "%d %d", &start_node, &end_node) == 2) {
                n = r;
                // Запускаем поиск (учитываем, что ввод от 1, а в массиве от 0)
                find_all_paths(start_node - 1, end_node - 1, 0);
                break;
            }
        } else {
            // Парсинг строки матрицы
            int c = 0;
            char *token = strtok(line, ", \n");
            while (token != NULL) {
                adj[r][c++] = atoi(token);
                token = strtok(NULL, ", \n");
            }
            r++;
        }
    }

    if (!found_any) {
        printf("0\n");
    }

    return 0;
}