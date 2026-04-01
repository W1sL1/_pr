// Условия лабораторной работы №2 Калькулятор на языке си, табуляция 4 пробела.
// Необходимо написать программу:
// - входные данные: выражение в инфиксной нотации  
// - выходные данные: значение введенного выражения. 
// Возможности: 
// - поддержка арифметических операций (+, -, *, /, ^, !, корень числа (sqrt),  
// - поддержка скобок (в том числе вложенных, без ограничения на максимальную вложенность)
// - поддержка тригонометрических операций (sin, cos,tg, ctg, arcsin, … ),  
// - введение пользовательских переменных
// При вводе выражения, содержащего переменные, программа должна спросить у пользователя значения этих переменных и с этими значениями вычислять результат выражения.
// Программа должна быть реализована на основе алгоритма подсчета значения в постфиксной нотации и алгоритма сортировочной станции с использованием структур данных стек и очередь.
// Стек и очередь должны быть реализованы на основе линейных списков.
// Примеры входных выражений, которые программа должна поддерживать:
// 
// 
// выведи txt со значениями аргументов и ответами (тесты си) в формате выражение - количество переменных - (их значение через пробел) - ответ
// Примеры входных выражений, которые программа должна поддерживать:
// 2+2
// a * b
// S(a)^2 + C(b)^2
// Q(2^((a + b) ^ (c + d * S(C(x))))! - 17)
//
// мои операции { '(',')','-', '+', '/', '*', '^','!','Q'(sqrt),'S'(sin),'C'(cos),'T'(tg),'G'(ctg),'A'(arcsin),'R'(arccos) }



















// Условия лабораторной работы №1 по Структурам данных. 
// Тема: "Работа с файлами". Язык: Си.
// Необходимо реализовать программу, осуществляющую замену слов в тексте и вывод полученного текста в выходной файл.
//  Входные данные:
// - путь к файлу1 с текстом (имя file1.txt)
// - путь к файлу2 со словами, которые требуется заменить (имя file2.txt)
// - путь к файлу3 со словами, на которые требуется заменить (имя file3.txt)
// - путь к выходному файлу4, в который будет записан результирующий текст (имя file4.txt).
// Слова в файлах 2 и 3 разделяются переносом строки. Слово на первой строке файла 2 должно заменяться на слово первой строки файла 3, слово на второй строке - на соответствующее слово второй строки и т.д. 
// Требования:
// - текст может включать английские и русские буквы, различные знаки препинания и специальные символы (в т.ч. пробелы, табуляции, и т.д.)
// - результирующий текст должен повторять все знаки препинания и специальные символы оригинального текста.
// - программа должна эффективно управлять памятью       (применяем динамическую память, используем каждый байт оптимально).
// - входные данные в программу должны передаваться с использованием аргументов командной строки и входных параметров main (argv, argc). 
// - в программе необходимо найти применение и грамотно заиспользовать структуры (или объединения или ENUM'ы).
//
// - замена целых слов 
// - учитывать регистр 
//
// Пример входных данных
// file1.txt
// Hello, world! Привет мир.
// WORLD world World. 
// file2.txt
// world
// мир
// file3.txt
// WORLD2
// МИР2
// file4.txt
// (файл-приемник, изначально можно пустой)
// Ожидаемый выход (file4.txt)
// Hello, WORLD2! Привет МИР2.
// WORLD WORLD2 World.
















#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Структура для хранения пар слов (что меняем -> на что меняем)
typedef struct {
    char *target;
    char *replacement;
} ReplacementPair;

// Контейнер для всех пар
typedef struct {
    ReplacementPair *items;
    int count;
} Dictionary;

// Функция для очистки памяти
void free_dictionary(Dictionary *dict) {
    for (int i = 0; i < dict->count; i++) {
        free(dict->items[i].target);
        free(dict->items[i].replacement);
    }
    free(dict->items);
}

// Чтение слов из файла в массив строк
char** read_words(const char *filename, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;

    char **words = NULL;
    char buffer[1024];
    *count = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\r\n")] = 0; // Удаление переноса строки
        words = realloc(words, (*count + 1) * sizeof(char*));
        words[*count] = strdup(buffer);
        (*count)++;
    }
    fclose(file);
    return words;
}

// Проверка, является ли символ частью слова (буквы и цифры)
int is_word_char(char c) {
    return isalnum((unsigned char)c) || (unsigned char)c > 127; // Учет кириллицы
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Использование: %s f1.txt f2.txt f3.txt f4.txt\n", argv[0]);
        return 1;
    }

    int count2 = 0, count3 = 0;
    char **words2 = read_words(argv[2], &count2);
    char **words3 = read_words(argv[3], &count3);

    if (!words2 || !words3) {
        printf("Ошибка чтения файлов словаря.\n");
        return 1;
    }

    int min_count = count2 < count3 ? count2 : count3;
    Dictionary dict = { malloc(min_count * sizeof(ReplacementPair)), min_count };

    for (int i = 0; i < min_count; i++) {
        dict.items[i].target = words2[i];
        dict.items[i].replacement = words3[i];
    }
    free(words2); free(words3);

    FILE *fin = fopen(argv[1], "r");
    FILE *fout = fopen(argv[4], "w");

    if (!fin || !fout) {
        printf("Ошибка открытия файлов текста.\n");
        return 1;
    }

    char c;
    char word_buf[1024];
    int idx = 0;

    while (1) {
        c = fgetc(fin);
        if (is_word_char(c) && c != EOF) {
            word_buf[idx++] = c;
        } else {
            if (idx > 0) {
                word_buf[idx] = '\0';
                char *to_print = word_buf;
                for (int i = 0; i < dict.count; i++) {
                    if (strcmp(word_buf, dict.items[i].target) == 0) {
                        to_print = dict.items[i].replacement;
                        break;
                    }
                }
                fprintf(fout, "%s", to_print);
                idx = 0;
            }
            if (c == EOF) break;
            fputc(c, fout);
        }
    }

    fclose(fin);
    fclose(fout);
    free_dictionary(&dict);

    return 0;
}
