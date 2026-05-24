#include <stdio.h> // stdin/stdout, fgets, printf
#include <stdlib.h> // strtol, qsort
#include <string.h> // memset
#include <ctype.h> // isdigit для разбора чисел
#define MAXN 100 // верхняя граница n (вершин)
#define MAX_PATHS 100000 // лимит найденных путей в массиве paths
// Path — один найденный маршрут: вершины и длина
typedef struct {
    int nodes[MAXN]; // номера вершин пути (0-базовые)
    int len; // число вершин в пути
} Path;

int graph[MAXN][MAXN]; int n; // матрица смежности n×n и размер графа
Path paths[MAX_PATHS]; int pathCount = 0; // все пути и их количество
int curPath[MAXN]; int visited[MAXN]; // текущий DFS-путь и метки посещения

// Парсинг всех целых чисел из строки (игнорирует запятые, пробелы и др. символы) 
int parseInts(const char *line, int out[], int maxOut) { // извлекает числа в out[], возвращает их число
    int cnt = 0; const char *p = line; // счётчик и указатель по строке

    while (*p && cnt < maxOut) { // пока есть символы и место в out
        while (*p && !isdigit((unsigned char)*p) && *p != '-') p++; // пропуск не-цифр (кроме '-')
        if (!*p) break; // конец строки — выход

        char *endPtr; long val = strtol(p, &endPtr, 10); // читаем очередное целое
        if (p == endPtr) break; // не число — прекращаем разбор

        out[cnt++] = (int)val; p = endPtr; // сохраняем и сдвигаем p за число
    }

    return cnt; // сколько чисел записали
}

void savePath(int len) { // сохраняет curPath[0..len-1] в paths[pathCount]
    if (pathCount >= MAX_PATHS) return; // переполнение — тихо не сохраняем
    paths[pathCount].len = len; // длина копируемого пути
    for (int i = 0; i < len; i++) {
        paths[pathCount].nodes[i] = curPath[i]; // копия вершин из текущего пути
    }
    pathCount++; // следующий свободный слот
}

void dfs(int u, int target, int depth) { // DFS с откатом: все простые пути из u в target
    visited[u] = 1; curPath[depth] = u; // помечаем u и кладём в curPath

    if (u == target) {
        savePath(depth + 1); // дошли — сохраняем путь длины depth+1
    } else {
        for (int v = 0; v < n; v++) {
            if (graph[u][v] && !visited[v]) { // ребро u→v и v ещё не в пути
                dfs(v, target, depth + 1); // рекурсия в соседа
            }
        }
    }

    visited[u] = 0; // откат: u снова свободна для других веток
}

int pathCompare(const void *a, const void *b) { // компаратор qsort для Path
    const Path *p1 = (const Path *)a; // приведение void* к Path*
    const Path *p2 = (const Path *)b;

    // Сначала по длине пути 
    if (p1->len != p2->len) return p1->len - p2->len; // короче — раньше

    // Затем лексикографически 
    int m = (p1->len < p2->len) ? p1->len : p2->len; // здесь len равны, m = len
    for (int i = 0; i < m; i++) {
        if (p1->nodes[i] != p2->nodes[i]) return p1->nodes[i] - p2->nodes[i]; // первая разница
    }
    return 0; // пути совпадают
}

int main(void) { // ввод графа, поиск путей, сортировка и печать
    char line[2048]; int temp[MAXN]; // буфер строки и временный массив чисел

    // 1) Читаем первую строку матрицы, определяем n 
    if (!fgets(line, sizeof(line), stdin)) {
        printf("0\n"); return 0;} // нет ввода — «путей нет»

    n = parseInts(line, temp, MAXN); // n = число элементов в первой строке
    if (n <= 0 || n > MAXN) {
        printf("0\n"); return 0;} // некорректный размер

    for (int j = 0; j < n; j++) graph[0][j] = temp[j]; // первая строка матрицы

    // 2) Читаем оставшиеся n-1 строк матрицы 
    for (int i = 1; i < n; i++) {
        if (!fgets(line, sizeof(line), stdin)) {
            printf("0\n"); return 0;} // обрыв ввода матрицы

        int cnt = parseInts(line, temp, MAXN); // числа i-й строки
        if (cnt < n) {
            printf("0\n"); return 0;} // строка короче n

        for (int j = 0; j < n; j++) graph[i][j] = temp[j]; // заполняем i-ю строку
        
    }

    // 3) Читаем строку с двумя вершинами (1..n) 
    if (!fgets(line, sizeof(line), stdin)) {
        printf("0\n"); return 0;} // нет пары start/target

    int uv[2]; int got = parseInts(line, uv, 2); // start и target в 1..n
    if (got < 2) {
        printf("0\n"); return 0;} // меньше двух чисел

    int start = uv[0] - 1;  // перевод к 0-базовой индексации 
    int target = uv[1] - 1; // конечная вершина (0-базовая)

    if (start < 0 || start >= n || target < 0 || target >= n) {
        printf("0\n"); return 0;} // вершины вне [1..n]

    memset(visited, 0, sizeof(visited)); // visited[] = 0 перед DFS
    dfs(start, target, 0); // перебор всех простых путей start→target

    if (pathCount == 0) {
        printf("0\n"); return 0;} // путей не нашли

    qsort(paths, pathCount, sizeof(Path), pathCompare); // сортировка по длине, затем лексикографически

    for (int i = 0; i < pathCount; i++) { // печать каждого пути
        for (int j = 0; j < paths[i].len; j++) {
            printf("%d", paths[i].nodes[j] + 1); // обратно к 1..n 
            if (j + 1 < paths[i].len) printf(","); // запятая между вершинами
        }
        printf("\n"); // конец строки пути
    }

    return 0;
}
