#include <stdio.h>
#include <stdlib.h>

#define MAX 20

int n = 0; // Количество узлов
int adj[MAX][MAX]; // Матрица смежности
int visited[MAX]; // Массив посещенных узлов
int path[MAX]; // Массив для хранения текущего пути
int path_index = 0;
int found_any = 0; // Флаг: найден ли хотя бы один путь

// Функция поиска всех путей (DFS)
void findAllPaths(int u, int destination) {
    // Помечаем текущий узел как посещенный и добавляем в путь
    visited[u] = 1;
    path[path_index] = u + 1; // +1 для вывода в 1-индексации
    path_index++;

    // Если достигли целевого узла
    if (u == destination) {
        found_any = 1;
        for (int i = 0; i < path_index; i++) {
            printf("%d%s", path[i], (i == path_index - 1 ? "" : ","));
        }
        printf("\n");
    } else {
        // Рекурсивно идем по всем соседям
        for (int v = 0; v < n; v++) {
            if (adj[u][v] == 1 && !visited[v]) {
                findAllPaths(v, destination);
            }
        }
    }

    // Обратный ход (backtracking): снимаем пометку и уменьшаем длину пути
    path_index--;
    visited[u] = 0;
}

int main() {
    printf("Введите количество узлов (max %d): ", MAX);
    if (scanf("%d", &n) != 1) return 1;

    printf("Введите матрицу смежности (строка за строкой, можно с запятыми):\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // Читаем число и пропускаем символ-разделитель (запятую или пробел)
            if (scanf("%d%*c", &adj[i][j]) == 0) break;
        }
    }

    int start_node, end_node;
    printf("Введите начальный и конечный узлы: ");
    scanf("%d %d", &start_node, &end_node);

    // Сброс массивов перед поиском
    for (int i = 0; i < n; i++) visited[i] = 0;

    // Запуск поиска (уменьшаем индексы на 1 для работы с массивом 0..n-1)
    findAllPaths(start_node - 1, end_node - 1);

    if (!found_any) {
        printf("0\n");
    }

    return 0;
}