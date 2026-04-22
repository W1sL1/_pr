#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 10
#define MAX_PATHS 100

int adj[MAX_NODES][MAX_NODES];
int visited[MAX_NODES];
int n = 4; // Размер матрицы

// Структура для хранения найденных путей
typedef struct {
    int nodes[MAX_NODES];
    int length;
} Path;

Path all_paths[MAX_PATHS];
int path_count = 0;

// DFS для поиска всех путей
void findPaths(int u, int target, int current_path[], int depth) {
    visited[u] = 1;
    current_path[depth] = u;

    if (u == target) {
        // Сохраняем найденный путь в массив
        all_paths[path_count].length = depth + 1;
        for (int i = 0; i <= depth; i++) {
            all_paths[path_count].nodes[i] = current_path[i];
        }
        path_count++;
    } else {
        for (int v = 0; v < n; v++) {
            if (adj[u][v] == 1 && !visited[v]) {
                findPaths(v, target, current_path, depth + 1);
            }
        }
    }
    visited[u] = 0; // Backtracking
}

// Функция для сортировки путей (сначала по длине, потом по составу)
int comparePaths(const void *a, const void *b) {
    Path *pathA = (Path *)a;
    Path *pathB = (Path *)b;
    
    if (pathA->length != pathB->length) {
        return pathA->length - pathB->length;
    }
    for (int i = 0; i < pathA->length; i++) {
        if (pathA->nodes[i] != pathB->nodes[i]) {
            return pathA->nodes[i] - pathB->nodes[i];
        }
    }
    return 0;
}

int main() {
    // Ввод матрицы 4x4
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%d", &adj[i][j]);
            if (getchar() == ',') ; // Пропуск запятой
        }
    }

    int start, end;
    scanf("%d %d", &start, &end);

    int current_path[MAX_NODES];
    findPaths(start - 1, end - 1, current_path, 0);

    if (path_count == 0) {
        printf("0\n");
    } else {
        // Сортируем, чтобы короткие пути (1,2,4) были выше длинных (1,2,3,4)
        qsort(all_paths, path_count, sizeof(Path), comparePaths);

        for (int i = 0; i < path_count; i++) {
            for (int j = 0; j < all_paths[i].length; j++) {
                printf("%d%s", all_paths[i].nodes[j] + 1, (j == all_paths[i].length - 1) ? "" : ",");
            }
            printf("\n");
        }
    }

    return 0;
}
