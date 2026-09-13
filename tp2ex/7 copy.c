#include <stdio.h>   // стандартный ввод/вывод: scanf, printf
#include <stdlib.h>  // malloc, free
#include <limits.h>  // LLONG_MAX — максимум для long long

int main(void) {  // точка входа; void = аргументов нет
    // читаем n; scanf вернёт 1 при успехе; &n — адрес переменной
    // если ввод плохой или n <= 0 — сразу выходим
    int n; if (scanf("%d", &n) != 1 || n <= 0) return 0;

    // Выделяем память под массивы: a - стоимости клеток, minCost - минимальная стоимость пути 
    // ways - количество способов достичь клетки с минимальной стоимостью
    // (long long *) — приведение типа указателя; n+1 — индексы с 1
    long long *a = (long long *)malloc((n + 1) * sizeof(long long));
    long long *minCost = (long long *)malloc((n + 1) * sizeof(long long));
    long long *ways = (long long *)malloc((n + 1) * sizeof(long long));
    // !a — true, если malloc вернул NULL (память не выделилась)
    if (!a || !minCost || !ways) {
        free(a); free(minCost); free(ways); return 0;}  // free(NULL) безопасен
    
    // Заполняем массив стоимостей клеток (индексация с 1)
    // %lld — формат для long long; &a[i] — адрес i-го элемента
    for (int i = 1; i <= n; i++) scanf("%lld", &a[i]);
    
    // Задаем условную бесконечность (чтобы избежать переполнения)
    // const — константа; / 4 — запас, чтобы minCost[p] + a[i] не переполнило long long
    const long long INF = LLONG_MAX / 4;
    
    // Инициализируем массивы: стоимость пути бесконечность, количество способов 0
    for (int i = 1; i <= n; i++) {
        minCost[i] = INF; ways[i] = 0;}
    
    // Старт: уже в клетке 1, стоимость пути 0, один способ
    minCost[1] = 0; ways[1] = 1;
    // динамика: для каждой клетки i считаем лучший путь с шагом 1..3
    for (int i = 2; i <= n; i++) {
        if (a[i] == -1) continue;  // -1 = клетка запрещена, пропускаем
        // Пробуем прыгнуть в i из клеток i-3, i-2, i-1 (шаг d от 1 до 3)
        for (int d = 1; d <= 3; d++) {
            int p = i - d; // Номер предыдущей клетки
            if (p < 1) continue; // Не выходим за левую границу
            if (minCost[p] == INF) continue;  // в p ещё нельзя попасть
            // Считаем стоимость пути до i через p
            long long candidate = minCost[p] + a[i];
            // Если нашли более дешевый путь - обновляем стоимость и сбрасываем счетчик способов
            // ways[i] = ways[p] — все лучшие пути в i идут через этот p
            if (candidate < minCost[i]) {
                minCost[i] = candidate; ways[i] = ways[p];} 
            // Если стоимость такая же - добавляем способы
            // += — прибавить к текущему числу способов
            else if (candidate == minCost[i]) ways[i] += ways[p];
            
        }
    }
    // INF значит до клетки n пути нет
    if (minCost[n] == INF) printf("-1\n");
    else {
        // Выводим минимальную стоимость и количество способов её достичь
        // \n — перевод строки
        printf("%lld\n", minCost[n]); printf("%lld\n", ways[n]);
    }
    // освобождаем кучу и завершаем программу с кодом 0 (успех)
    free(a); free(minCost); free(ways); return 0;
}
