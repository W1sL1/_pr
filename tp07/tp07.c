#include <stdio.h>
#include <stdlib.h>

int main() {
    int n;
    
    // Считываем количество клеток
    if (scanf("%d", &n) != 1) {
        return 1;
    }

    // Выделяем память под массивы (индексация с 1 до n)
    long long *a = (long long *)malloc((n + 1) * sizeof(long long));
    long long *min_cost = (long long *)malloc((n + 1) * sizeof(long long));
    long long *ways = (long long *)malloc((n + 1) * sizeof(long long));

    // Считываем стоимости
    for (int i = 1; i <= n; i++) {
        scanf("%lld", &a[i]);
        min_cost[i] = -1; // -1 будет означать, что клетка пока недостижима
        ways[i] = 0;
    }

    // Базовый случай: старт из клетки 1
    if (a[1] != -1) {
        min_cost[1] = a[1];
        ways[1] = 1;
    }

    // Заполняем массивы для оставшихся клеток
    for (int i = 2; i <= n; i++) {
        // Если клетка запрещена, пропускаем её
        if (a[i] == -1) {
            continue;
        }

        long long best_cost = -1;
        long long best_ways = 0;

        // Проверяем 3 предыдущие возможные позиции (прыжки на 1, 2 или 3)
        for (int step = 1; step <= 3; step++) {
            int prev = i - step;
            
            // Если предыдущая клетка существует и достижима
            if (prev >= 1 && min_cost[prev] != -1) {
                long long current_cost = min_cost[prev] + a[i];

                // Если нашли более дешёвый путь или это первый найденный путь
                if (best_cost == -1 || current_cost < best_cost) {
                    best_cost = current_cost;
                    best_ways = ways[prev];
                } 
                // Если нашли путь с такой же минимальной стоимостью
                else if (current_cost == best_cost) {
                    best_ways += ways[prev];
                }
            }
        }

        min_cost[i] = best_cost;
        ways[i] = best_ways;
    }

    // Вывод результатов
    if (min_cost[n] == -1 || ways[n] == 0) {
        printf("-1\n");
    } else {
        printf("%lld\n", min_cost[n]);
        printf("%lld\n", ways[n]);
    }

    // Освобождаем память
    free(a);
    free(min_cost);
    free(ways);

    return 0;
}