#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(void) {
    int n;
    if (scanf("%d", &n) != 1 || n <= 0) {
        return 0;
    }

    long long *a = (long long *)malloc((n + 1) * sizeof(long long));
    long long *minCost = (long long *)malloc((n + 1) * sizeof(long long));
    long long *ways = (long long *)malloc((n + 1) * sizeof(long long));

    if (!a || !minCost || !ways) {
        free(a);
        free(minCost);
        free(ways);
        return 0;
    }

    for (int i = 1; i <= n; i++) {
        scanf("%lld", &a[i]);
    }

    const long long INF = LLONG_MAX / 4;

    for (int i = 1; i <= n; i++) {
        minCost[i] = INF;
        ways[i] = 0;
    }

    // Старт: уже в клетке 1
    minCost[1] = 0;
    ways[1] = 1;

    for (int i = 2; i <= n; i++) {
        if (a[i] == -1) {
            continue; // запрещенная клетка
        }

        for (int d = 1; d <= 3; d++) {
            int p = i - d;
            if (p < 1) continue;
            if (minCost[p] == INF) continue; // до p нельзя добраться

            long long candidate = minCost[p] + a[i];

            if (candidate < minCost[i]) {
                minCost[i] = candidate;
                ways[i] = ways[p];
            } else if (candidate == minCost[i]) {
                ways[i] += ways[p];
            }
        }
    }

    if (minCost[n] == INF) {
        printf("-1\n");
    } else {
        printf("%lld\n", minCost[n]);
        printf("%lld\n", ways[n]);
    }

    free(a);
    free(minCost);
    free(ways);
    return 0;
}
