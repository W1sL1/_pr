#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int str_leng(const char* s) {
    int leng = 0; while (s[leng] != '\0') leng++;
    return leng;
}
int main(void) {
    //       buf output    curr leng in buf              
    static char out[333]; int out_leng = 0; int n;
    if (scanf("%d", &n) != 1) return 0;
    for (int i = 0; i < n; i++) {
        //  perekrit
        int overlap = 0; char sec[66]; char fir[66];
        if (scanf("%65s %65s", fir, sec) != 2) return 0;
        int leng_sec = str_leng(sec); int leng_fir = str_leng(fir);
        int max_overlap = leng_fir < leng_sec ? leng_fir : leng_sec;
        for (int j = max_overlap; j > 0; j--) {
            int flag = 1;
            for (int k = 0; k < j; k++) {
                if (fir[leng_fir - j + k] != sec[k]) {flag = 0; break;}
            }
            if (flag) {overlap = j; break;}
        }
        // in buf: first + ost. second + '\n'     wr->+1    this chr
        for (int i = 0; i < leng_fir; i++) out[out_leng++] = fir[i];
        for (int i = overlap; i < leng_sec; i++) out[out_leng++] = sec[i];
        out[out_leng++] = '\n';
    }
//multi pr(buf,1byte for 1elem,count elem,standart)   
    fwrite(out, 1, out_leng, stdout);
    return 0;
}
