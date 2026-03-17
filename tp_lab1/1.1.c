//Prozorov PA 5151004/50002
#include <stdio.h>
int main() {
    short mn, mx, meter;
    meter = 1;
    while (meter > 0) {
        meter++;
    }
    mx = meter - 1;
    mn = meter;
    short i;
    for (i = mn; i < mx; i++) {
        printf("%hd,", i);
    }
    printf("%hd", mx);
    return 0;
}
