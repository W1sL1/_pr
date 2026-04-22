#include <stdio.h>
#include <limits.h>

#define MAX_N 100000  // максимальный размер при необходимости, можно увеличить

int main() {
    int n;
    scanf("%d", &n);
    
    long long a[MAX_N + 1];
    for (int i = 1; i <= n; ++i) {
        scanf("%lld", &a[i]);
    }
    
    // Если стартовая клетка запрещена, путь невозможен
    if (a[1] == -1) {
        printf("-1\n");
        return 0;
    }
    
    // dp[i] - минимальная стоимость добраться до клетки i
    // ways[i] - количество способов достичь эту стоимость
    long long dp[MAX_N + 1];
    long long ways[MAX_N + 1];
    
    const long long INF = LLONG_MAX / 2;  // достаточно большое число, чтобы избежать переполнения
    
    for (int i = 1; i <= n; ++i) {
        dp[i] = INF;
        ways[i] = 0;
    }
    
    dp[1] = a[1];
    ways[1] = 1;
    
    for (int i = 2; i <= n; ++i) {
        if (a[i] == -1) continue;  // клетка запрещена
        
        for (int jump = 1; jump <= 3; ++jump) {
            int prev = i - jump;
            if (prev >= 1 && dp[prev] != INF) {
                long long cost = dp[prev] + a[i];
                if (cost < dp[i]) {
                    dp[i] = cost;
                    ways[i] = ways[prev];
                } else if (cost == dp[i]) {
                    ways[i] += ways[prev];
                }
            }
        }
    }
    
    if (dp[n] == INF) {
        printf("-1\n");
    } else {
        printf("%lld\n", dp[n]);
        printf("%lld\n", ways[n]);
    }
    
    return 0;
}