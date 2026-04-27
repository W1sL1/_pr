#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
int main(void) {
    int n;
    if (scanf("%d", &n) != 1 || n <= 0) return 0;
    // Выделяем память под массивы: a — стоимости клеток, minCost — минимальная стоимость пути,
    // ways — количество способов достичь клетки с минимальной стоимостью
    long long *a = (long long *)malloc((n + 1) * sizeof(long long));
    long long *minCost = (long long *)malloc((n + 1) * sizeof(long long));
    long long *ways = (long long *)malloc((n + 1) * sizeof(long long));
    if (!a || !minCost || !ways) {
        free(a); free(minCost); free(ways); return 0;
    }
    // Заполняем массив стоимостей клеток (индексация с 1)
    for (int i = 1; i <= n; i++) {
        scanf("%lld", &a[i]);
    }
    // Задаем условную бесконечность (достаточно большое число, чтобы избежать переполнения)
    const long long INF = LLONG_MAX / 4;
    // Инициализируем массивы: стоимость пути — бесконечность, количество способов — 0
    for (int i = 1; i <= n; i++) {
        minCost[i] = INF; ways[i] = 0;
    }
    // Старт: уже в клетке 1, стоимость пути 0, один способ
    minCost[1] = 0; ways[1] = 1;
    for (int i = 2; i <= n; i++) {
        if (a[i] == -1) continue;
        // Пробуем прыгнуть в i из клеток i-3, i-2, i-1 (шаг d от 1 до 3)
        for (int d = 1; d <= 3; d++) {
            int p = i - d; // Номер предыдущей клетки
            if (p < 1) continue; // Не выходим за левую границу
            if (minCost[p] == INF) continue; 
            // Считаем стоимость пути до i через p
            long long candidate = minCost[p] + a[i];
            // Если нашли более дешевый путь — обновляем стоимость и сбрасываем счетчик способов
            if (candidate < minCost[i]) {
                minCost[i] = candidate;
                ways[i] = ways[p];
            } 
            // Если стоимость такая же — добавляем способы
            else if (candidate == minCost[i]) {
                ways[i] += ways[p];
            }
        }
    }
    if (minCost[n] == INF) 
        printf("-1\n");
    else {
        // Выводим минимальную стоимость и количество способов её достичь
        printf("%lld\n", minCost[n]); printf("%lld\n", ways[n]);
    }
    free(a); free(minCost); free(ways);
    return 0;
}