#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXN 100 // Максимальное количество вершин в графе
#define MAX_PATHS 100000 // Максимальное количество сохраняемых путей

typedef struct {
    int nodes[MAXN]; // Вершины пути
    int len; // Длина пути
} Path;

int graph[MAXN][MAXN]; // Матрица смежности графа
int n; // Реальное количество вершин

Path paths[MAX_PATHS]; // Массив всех найденных путей
int pathCount = 0; // Счётчик найденных путей

int curPath[MAXN]; // Текущий путь при обходе DFS
int visited[MAXN]; // Массив посещённых вершин

/* Парсинг всех целых чисел из строки (игнорирует запятые, пробелы и др. символы) */
int parseInts(const char *line, int out[], int maxOut) {
    int cnt = 0; // Счётчик извлечённых чисел
    const char *p = line; // Указатель для обхода строки

    while (*p && cnt < maxOut) {
        while (*p && !isdigit((unsigned char)*p) && *p != '-') p++; // Пропускаем нецифровые символы, кроме минуса
        if (!*p) break; // Достигнут конец строки

        char *endPtr;
        long val = strtol(p, &endPtr, 10); // Парсим целое число
        if (p == endPtr) break; // Если парсинг не удался

        out[cnt++] = (int)val; // Сохраняем число
        p = endPtr; // Сдвигаем указатель за число
    }

    return cnt; // Возвращаем количество чисел
}

void savePath(int len) {
    if (pathCount >= MAX_PATHS) return; // Если массив заполнен, не сохраняем
    paths[pathCount].len = len; // Записываем длину пути
    for (int i = 0; i < len; i++) {
        paths[pathCount].nodes[i] = curPath[i]; // Копируем вершины
    }
    pathCount++; // Увеличиваем счётчик путей
}

void dfs(int u, int target, int depth) {
    visited[u] = 1; // Помечаем вершину как посещённую
    curPath[depth] = u; // Добавляем вершину в текущий путь

    if (u == target) {
        savePath(depth + 1); // Если достигли цели, сохраняем путь
    } else {
        for (int v = 0; v < n; v++) {
            if (graph[u][v] && !visited[v]) { // Если есть ребро и вершина не посещена
                dfs(v, target, depth + 1); // Рекурсивный вызов DFS
            }
        }
    }

    visited[u] = 0; // Снимаем пометку посещения (для других путей)
}

int pathCompare(const void *a, const void *b) { // Компаратор для сортировки путей
    const Path *p1 = (const Path *)a;
    const Path *p2 = (const Path *)b;

    /* Сначала по длине пути */
    if (p1->len != p2->len) return p1->len - p2->len;

    /* Затем лексикографически */
    int m = (p1->len < p2->len) ? p1->len : p2->len; // Берём минимальную длину
    for (int i = 0; i < m; i++) {
        if (p1->nodes[i] != p2->nodes[i]) return p1->nodes[i] - p2->nodes[i]; // Сравниваем вершины по порядку
    }
    return 0; // Пути считаются равными, если префиксы совпали
}

int main(void) {
    char line[2048]; // Буфер для чтения строк
    int temp[MAXN]; // Временный массив для чисел из строки

    /* 1) Читаем первую строку матрицы, определяем n */
    if (!fgets(line, sizeof(line), stdin)) {
        printf("0\n");
        return 0;
    }

    n = parseInts(line, temp, MAXN); // Определяем размер матрицы по первой строке
    if (n <= 0 || n > MAXN) {
        printf("0\n");
        return 0;
    }

    for (int j = 0; j < n; j++) graph[0][j] = temp[j]; // Заполняем первую строку графа

    /* 2) Читаем оставшиеся n-1 строк матрицы */
    for (int i = 1; i < n; i++) {
        if (!fgets(line, sizeof(line), stdin)) {
            printf("0\n");
            return 0;
        }

        int cnt = parseInts(line, temp, MAXN); // Парсим строку
        if (cnt < n) { // Проверяем, что чисел достаточно
            printf("0\n");
            return 0;
        }

        for (int j = 0; j < n; j++) {
            graph[i][j] = temp[j]; // Заполняем i-ю строку графа
        }
    }

    /* 3) Читаем строку с двумя вершинами (1..n) */
    if (!fgets(line, sizeof(line), stdin)) {
        printf("0\n");
        return 0;
    }

    int uv[2];
    int got = parseInts(line, uv, 2); // Парсим начальную и конечную вершины
    if (got < 2) {
        printf("0\n");
        return 0;
    }

    int start = uv[0] - 1;  /* перевод к 0-базовой индексации */
    int target = uv[1] - 1;

    if (start < 0 || start >= n || target < 0 || target >= n) { // Проверка валидности вершин
        printf("0\n");
        return 0;
    }

    memset(visited, 0, sizeof(visited)); // Инициализация массива посещений
    dfs(start, target, 0); // Запуск поиска в глубину

    if (pathCount == 0) { // Если путей не найдено
        printf("0\n");
        return 0;
    }

    qsort(paths, pathCount, sizeof(Path), pathCompare); // Сортировка путей

    for (int i = 0; i < pathCount; i++) { // Вывод всех путей
        for (int j = 0; j < paths[i].len; j++) {
            printf("%d", paths[i].nodes[j] + 1); /* обратно к 1..n */
            if (j + 1 < paths[i].len) printf(","); // Добавляем запятую, если не последняя вершина
        }
        printf("\n");
    }

    return 0;
}
