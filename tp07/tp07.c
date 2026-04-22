#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF LLONG_MAX

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;

    long long *a = (long long *)malloc(n * sizeof(long long));
    long long *min_cost = (long long *)malloc(n * sizeof(long long));
    long long *ways = (long long *)malloc(n * sizeof(long long));

    for (int i = 0; i < n; i++) {
        scanf("%lld", &a[i]);
        min_cost[i] = INF; // Изначально все клетки недостижимы
        ways[i] = 0;
    }

    // Начальная точка
    if (a[0] != -1) {
        min_cost[0] = a[0];
        ways[0] = 1;
    } else {
        // Если первая клетка запрещена, пути нет
        printf("-1\n");
        return 0;
    }

    for (int i = 1; i < n; i++) {
        if (a[i] == -1) continue; // Пропускаем запрещенные клетки

        long long best_prev_cost = INF;

        // Смотрим на 1, 2 и 3 клетки назад
        for (int j = 1; j <= 3; j++) {
            int prev = i - j;
            if (prev >= 0 && min_cost[prev] != INF) {
                if (min_cost[prev] < best_prev_cost) {
                    best_prev_cost = min_cost[prev];
                }
            }
        }

        // Если нашли хотя бы один путь до текущей клетки
        if (best_prev_cost != INF) {
            min_cost[i] = best_prev_cost + a[i];
            
            // Считаем количество путей, которые дают эту минимальную стоимость
            for (int j = 1; j <= 3; j++) {
                int prev = i - j;
                if (prev >= 0 && min_cost[prev] == best_prev_cost) {
                    ways[i] += ways[prev];
                }
            }
        }
    }

    // Вывод результата
    if (min_cost[n - 1] == INF) {
        printf("-1\n");
    } else {
        printf("%lld\n%lld\n", min_cost[n - 1], ways[n - 1]);
    }

    free(a);
    free(min_cost);
    free(ways);

    return 0;
}