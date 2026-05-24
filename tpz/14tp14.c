#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXN 100
#define MAX_PATHS 100000

typedef struct {
    int nodes[MAXN];
    int len;
} Path;

int graph[MAXN][MAXN]; int n;
Path paths[MAX_PATHS]; int pathCount = 0;
int curPath[MAXN]; int visited[MAXN];

// Парсинг всех целых чисел из строки (игнорирует запятые, пробелы и др. символы) 
int parseInts(const char *line, int out[], int maxOut) {
    int cnt = 0; const char *p = line;

    while (*p && cnt < maxOut) {
        while (*p && !isdigit((unsigned char)*p) && *p != '-') p++;
        if (!*p) break;

        char *endPtr; long val = strtol(p, &endPtr, 10);
        if (p == endPtr) break;

        out[cnt++] = (int)val; p = endPtr;
    }

    return cnt;
}

void savePath(int len) { // сохраняет curPath[0..len-1] в paths[pathCount]
    if (pathCount >= MAX_PATHS) return;
    paths[pathCount].len = len;
    for (int i = 0; i < len; i++) {
        paths[pathCount].nodes[i] = curPath[i];
    }
    pathCount++;
}

void dfs(int u, int target, int depth) { // DFS с откатом: все простые пути из u в target
    visited[u] = 1; curPath[depth] = u;

    if (u == target) {
        savePath(depth + 1);
    } else {
        for (int v = 0; v < n; v++) {
            if (graph[u][v] && !visited[v]) {
                dfs(v, target, depth + 1);
            }
        }
    }

    visited[u] = 0;
}

int pathCompare(const void *a, const void *b) { // компаратор qsort для Path
    const Path *p1 = (const Path *)a;
    const Path *p2 = (const Path *)b;

    // Сначала по длине пути 
    if (p1->len != p2->len) return p1->len - p2->len;

    // Затем лексикографически 
    int m = (p1->len < p2->len) ? p1->len : p2->len;
    for (int i = 0; i < m; i++) {
        if (p1->nodes[i] != p2->nodes[i]) return p1->nodes[i] - p2->nodes[i];
    }
    return 0;
}

int main(void) { // ввод графа, поиск путей, сортировка и печать
    char line[2048]; int temp[MAXN];

    // 1) Читаем первую строку матрицы, определяем n 
    if (!fgets(line, sizeof(line), stdin)) {
        printf("0\n"); return 0;}

    n = parseInts(line, temp, MAXN);
    if (n <= 0 || n > MAXN) {
        printf("0\n"); return 0;}

    for (int j = 0; j < n; j++) graph[0][j] = temp[j];

    // 2) Читаем оставшиеся n-1 строк матрицы 
    for (int i = 1; i < n; i++) {
        if (!fgets(line, sizeof(line), stdin)) {
            printf("0\n"); return 0;}

        int cnt = parseInts(line, temp, MAXN);
        if (cnt < n) {
            printf("0\n"); return 0;}

        for (int j = 0; j < n; j++) graph[i][j] = temp[j];
        
    }

    // 3) Читаем строку с двумя вершинами (1..n) 
    if (!fgets(line, sizeof(line), stdin)) {
        printf("0\n"); return 0;}

    int uv[2]; int got = parseInts(line, uv, 2);
    if (got < 2) {
        printf("0\n"); return 0;}

    int start = uv[0] - 1;  // перевод к 0-базовой индексации 
    int target = uv[1] - 1;

    if (start < 0 || start >= n || target < 0 || target >= n) {
        printf("0\n"); return 0;}

    memset(visited, 0, sizeof(visited));
    dfs(start, target, 0);

    if (pathCount == 0) {
        printf("0\n"); return 0;}

    qsort(paths, pathCount, sizeof(Path), pathCompare);

    for (int i = 0; i < pathCount; i++) {
        for (int j = 0; j < paths[i].len; j++) {
            printf("%d", paths[i].nodes[j] + 1); // обратно к 1..n 
            if (j + 1 < paths[i].len) printf(",");
        }
        printf("\n");
    }

    return 0;
}
