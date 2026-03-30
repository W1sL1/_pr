// Условия лабораторной работы №2 Калькулятор.
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
// 2+2
// a * b
// sin(a)^2 + cos(b)^2
// sqrt(2^((a + b) ^ (c + d * sin(cos(x))))! - 17)

































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
#include <locale.h>

typedef struct {
    char *from; // что ищем
    char *to;   // на что заменяем
} Rule;

typedef enum {
    TOKEN_NONE = 0,
    TOKEN_WORD = 1,
    TOKEN_DELIM = 2
} TokenType;

static void *xrealloc(void *p, size_t n) {
    void *q = realloc(p, n);
    if (!q) {
        perror("realloc");
        exit(1);
    }
    return q;
}

static char *read_line(FILE *f) {
    // Читает строку до '\n' (без '\n'), возвращает malloc'd строку или NULL на EOF.
    size_t cap = 64, len = 0;
    char *buf = (char*)malloc(cap);
    if (!buf) {
        perror("malloc");
        exit(1);
    }

    int ch;
    while ((ch = fgetc(f)) != EOF) {
        if (ch == '\n') break;
        if (ch == '\r') {
            // Windows CRLF: откусываем '\r' в конце
            continue;
        }
        if (len + 1 >= cap) {
            cap *= 2;
            buf = (char*)xrealloc(buf, cap);
        }
        buf[len++] = (char)ch;
    }

    if (ch == EOF && len == 0) {
        free(buf);
        return NULL;
    }

    buf[len] = '\0';
    return buf;
}

static int is_letter_char(int c) {
    // По условию: английские и русские буквы.
    // isalpha работает корректно при подходящей локали (setlocale для LC_CTYPE).
    return isalpha((unsigned char)c) != 0;
}

static void flush_word(FILE *out, Rule *rules, size_t rule_count,
                        char *word, size_t *wlen) {
    if (*wlen == 0) return;

    word[*wlen] = '\0';

    // Замена с учетом регистра: strcmp.
    for (size_t i = 0; i < rule_count; i++) {
        if (strcmp(word, rules[i].from) == 0) {
            fputs(rules[i].to, out);
            *wlen = 0;
            return;
        }
    }

    // Не нашли — выводим как есть
    fwrite(word, 1, *wlen, out);
    *wlen = 0;
}

static void free_rules(Rule *rules, size_t n) {
    if (!rules) return;
    for (size_t i = 0; i < n; i++) {
        free(rules[i].from);
        free(rules[i].to);
    }
    free(rules);
}

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr,
            "Usage: %s file1.txt file2.txt file3.txt file4.txt\n", argv[0]);
        return 1;
    }

    // Локаль важна для isalpha по кириллице (обычно Windows-1251 + системная локаль).
    setlocale(LC_CTYPE, "");

    const char *file1 = argv[1];
    const char *file2 = argv[2];
    const char *file3 = argv[3];
    const char *file4 = argv[4];

    FILE *in1 = fopen(file1, "rb");
    FILE *in2 = fopen(file2, "r");
    FILE *in3 = fopen(file3, "r");
    FILE *out = fopen(file4, "wb");

    if (!in1 || !in2 || !in3 || !out) {
        perror("fopen");
        if (in1) fclose(in1);
        if (in2) fclose(in2);
        if (in3) fclose(in3);
        if (out) fclose(out);
        return 1;
    }

    // Читаем правила построчно: file2[i] -> file3[i]
    size_t rule_count = 0, rule_cap = 16;
    Rule *rules = (Rule*)malloc(rule_cap * sizeof(Rule));
    if (!rules) {
        perror("malloc");
        return 1;
    }

    while (1) {
        char *a = read_line(in2);
        char *b = read_line(in3);

        if (!a && !b) break;              // обе EOF
        if (!a || !b) {
            fprintf(stderr, "Error: file2 and file3 have different number of lines.\n");
            free(a);
            free(b);
            free_rules(rules, rule_count);
            fclose(in1); fclose(in2); fclose(in3); fclose(out);
            return 1;
        }

        if (rule_count == rule_cap) {
            rule_cap *= 2;
            rules = (Rule*)xrealloc(rules, rule_cap * sizeof(Rule));
        }

        rules[rule_count].from = a;
        rules[rule_count].to = b;
        rule_count++;
    }

    // Проходим file1 и делаем замену “на границах слов”
    char *word = (char*)malloc(64);
    if (!word) {
        perror("malloc");
        free_rules(rules, rule_count);
        fclose(in1); fclose(in2); fclose(in3); fclose(out);
        return 1;
    }

    size_t wcap = 64, wlen = 0;

    int ch;
    TokenType t;

    while ((ch = fgetc(in1)) != EOF) {
        if (is_letter_char(ch)) {
            t = TOKEN_WORD;
            if (wlen + 1 >= wcap) {
                wcap *= 2;
                word = (char*)xrealloc(word, wcap);
            }
            word[wlen++] = (char)ch;
        } else {
            t = TOKEN_DELIM;
            (void)t; // чтобы ясно показать логику: слово не набираем, разделитель выводим
            flush_word(out, rules, rule_count, word, &wlen);
            fputc(ch, out); // сохраняем все знаки препинания/пробелы как есть
        }
    }

    // добиваем последнее слово
    flush_word(out, rules, rule_count, word, &wlen);

    free(word);
    free_rules(rules, rule_count);

    fclose(in1);
    fclose(in2);
    fclose(in3);
    fclose(out);

    return 0;
}