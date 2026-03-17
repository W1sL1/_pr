//ProzorovPA_5151004/50002
#include <stdio.h>
#define RWS 11
#define CLS 13
int main() {
    int mtrx[RWS][CLS];
    for (int i = 0; i < RWS; i++) {
        for (int j = 0; j < CLS; j++) {
            scanf("%d", &mtrx[i][j]);
        }
    }
    int max_L = 1;
    for (int i = 0; i < RWS; i++) {
        for (int j = 0; j < CLS; j++) {
            int max_poss_L = 1;
            //Максимальная длина по границам
            int max_L_lt_up = (i < j) ? i + 1 : j + 1;
            int max_L_rt_up = (i < (CLS - 1 - j)) ? i + 1 : (CLS - j);
            int max_L_dw = RWS - i;
            int max_bounds = max_L_lt_up;
            if (max_L_rt_up < max_bounds) max_bounds = max_L_rt_up;
            if (max_L_dw < max_bounds) max_bounds = max_L_dw;
            for (int L = 2; L <= max_bounds; L++) {
                int val = 1;
                //Влево-вверх: центр (i,j) -> (i-1,j-1) -> (i-2,j-2) и тд
                for (int k = 1; k < L; k++) {
                    if (mtrx[i - k][j - k] < mtrx[i - (k - 1)][j - (k - 1)]) {
                        val = 0;
                        break;
                    }
                }
                if (!val) continue;
                //Вправо-вверх: центр (i,j) -> (i-1,j+1) -> (i-2,j+2) и тд
                for (int k = 1; k < L; k++) {
                    if (mtrx[i - k][j + k] < mtrx[i - (k - 1)][j + (k - 1)]) {
                        val = 0;
                        break;
                    }
                }
                if (!val) continue;
                //Вниз: центр (i,j) -> (i+1,j) -> (i+2,j) и тд
                for (int k = 1; k < L; k++) {
                    if (mtrx[i + k][j] < mtrx[i + (k - 1)][j]) {
                        val = 0;
                        break;
                    }
                }
                if (!val) continue;
                max_poss_L = L;
            }
            if (max_poss_L > max_L) {
                max_L = max_poss_L;
            }
        }
    }
    printf("%d\n", max_L);
    return 0;
}
