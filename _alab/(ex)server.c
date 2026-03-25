//ProzorovPA_5151004/50002
#include <stdio.h>
int main() {
    int t; int res[111]; //result
    scanf("%d", &t); 
    for (int t_indx = 0; t_indx < t; t_indx++) {
        int c; int mx_len = 1; int ar[55]; //count, array (massive)  
        scanf("%d", &c); 
        for (int c_indx = 0; c_indx < c; c_indx++) scanf("%d", &ar[c_indx]);
        for (int start = 0; start < c; start++) {
            int first = ar[start], second = ar[start], current = 1;
            for (int next = start + 1; next < c; next++) {
                if (ar[next] == first || ar[next] == second) current++; //(||) or
                else if (first == second) { second = ar[next]; current++; }
                else break;
                if (current > mx_len) mx_len = current;
            }
        }
        res[t_indx] = mx_len;
    }
    for (int t_indx = 0; t_indx < t; t_indx++) printf("%d\n", res[t_indx]);
    return 0;
}
