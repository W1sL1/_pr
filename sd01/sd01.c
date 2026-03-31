#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define C 50 //количество слов в массивах s2 и s3
#define W 50 //размер слова
//paste args:  f1.txt f2.txt f3.txt f4.txt
void change_words(char* s1, char s2[C][W], char s3[C][W], char* argv) {
    //если есть слово s1 в s2, меняем на слово из s3
    for (int i = 0; i < C; i++) { 
        //удаляем последний перенос 
        if (s2[i][strlen(s2[i]) - 1] == '\n') {  
            s2[i][strlen(s2[i]) - 1] = 0;
        }
        if (strcmp(s1, s2[i]) == 0) {
            strcpy(s1, s3[i]);
            //удаляем последний перенос
            if (s3[i][strlen(s3[i]) - 1] == '\n') {  
                s1[strlen(s3[i]) - 1] = 0;
            }
            break;
        }
    }
    FILE* file4;
    file4 = fopen(argv, "a");
    fputs(s1, file4);
}
int main(int argc, char* argv[]) {
    FILE* file1, * file2, * file3, *file4;
    file1 = fopen(argv[1], "r");
    file2 = fopen(argv[2], "r");
    file3 = fopen(argv[3], "r");
    file4 = fopen(argv[4], "w");
    fclose(file4);
    char s1[W] = { 0 }; //хранение форм. слова
    char s2[C][W]; //в каждой ячеке хранится слово, которое нужно заменить
    char s3[C][W];  //в каждой ячеке хранится слово, на которое нужно заменить
    int i2 = 0;
    while (fgets(s2[i2], W, file2)) i2++;
    int i3 = 0;
    while (fgets(s3[i3], W, file3)) i3++;
    int i = 0;
    char symb = 0;
    while ((symb = fgetc(file1)) != EOF) {
        while (((symb >= -64) && (symb <= -1)) || ((symb >= 65) && (symb <= 90)) || ((symb >= 97) && (symb <= 122))) { //
            s1[i] = symb;
            i++;
            if (feof(file1) == 0) { //если есть еще строка
                symb = fgetc(file1);
            }
        } //осталось слово s1 и символ symb после него, который не является буквой
        change_words(s1, s2, s3, argv[4]);
        for (int j = 0; j < W; j++) s1[j] = 0;
        i = 0;
        file4 = fopen(argv[4], "a");
        fputc(symb, file4);
    }
}
