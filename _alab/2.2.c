//ProzorovPA_5151004/50002
#include <stdio.h>
#include <string.h>
int main() {
    int a1, a2, b1, b2;
    char inp[20] = { 0 }; //0 initialization
    if (scanf("%19s", inp) != 1) {
        return 1;
    }
    //str end=0
    char* minus_pos = strchr(inp, '-');
    if (!minus_pos) {
        return 1;
    }
    sscanf(inp, "%d.%3d-%d.%d", &a1, &a2, &b1, &b2);
    //len fractional(drobn) part of second(2) number
    int b2_dig = 0;
    char* dot_pos = strchr(minus_pos, '.');
    if (dot_pos) {
        char* p = dot_pos + 1;
        while (*p >= '0' && *p <= '9') {
            b2_dig++;
            p++;
        }
    }
    //b2 correction
    int b2_new = b2;
    if (b2_dig == 1) b2_new *= 100;
    else if (b2_dig == 2) b2_new *= 10;
    int tot1 = a1 * 1000 + a2;
    int tot2 = b1 * 1000 + b2_new;
    int res = tot1 - tot2;
    if (res < 0) {
        printf("-");
        res = -res;
    }
    printf("%d", res / 1000);
    int decimal = res % 1000;
    if (decimal > 0) {
        printf(".");
        if (decimal < 10) printf("00");
        else if (decimal < 100) printf("0");
        int tmp = decimal;
        while (tmp % 10 == 0) tmp /= 10;
        printf("%d", tmp);
    }
    return 0;
}
