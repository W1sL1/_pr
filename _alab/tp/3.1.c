//3.1
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int main() {
    //                80+ \0          tekush
    int leng = 0; char stri[81]; char curr_char;
    while (1) {
        int res = scanf("%c", &curr_char);
        if (res != 1 || leng >= 80 || curr_char == '\n' ) {
            break;
        }
        stri[leng] = curr_char; leng++;
    }
    //mass for all ASCII symb; end symb=\0
    int array[256] = { 0 }; stri[leng] = '\0';
    // chastot of each symb 
    for (int i = 0; i < leng; i++) {
        unsigned char c = stri[i];
        array[c]++;
    }
    // for min to max (on code of symb)
    for (int i = 0; i < 256; i++) {
        if (array[i] > 0) {
            printf("%c=%d\n", (char)i, array[i]);
        }
    }
    return 0;
}
