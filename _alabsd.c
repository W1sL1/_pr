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
// Необходимо реализовать программу, осуществляющую замену слов в тексте и вывод полученного 
// текста в выходной файл.
//  Входные данные:
// - путь к файлу1 с текстом
// - путь к файлу2 со словами, которые требуется заменить
// - путь к файлу3 со словами, на которые требуется заменить
// - путь к выходному файлу4, в который будет записан результирующий текст.
// Слова в файлах 2 и 3 разделяются переносом строки. Слово на первой строке файла 2 должно 
// заменяться на слово первой строки файла 3, слово на второй строке - на соответствующее 
// слово второй строки и т.д.
// Требования:
// - текст может включать английские буквы, различные знаки препинания и специальные символы 
// (в т.ч. пробелы, табуляции, и т.д.)
// - результирующий текст должен повторять все знаки препинания и специальные символы оригинального текста.
// - программа должна эффективно управлять памятью 
// (применяем динамическую память, используем каждый байт оптимально).
// - входные данные в программу должны передаваться с использованием аргументов 
// командной строки и входных параметров main (argv, argc). 
// - в программе необходимо найти применение и грамотно заиспользовать структуры 
// (или объединения или ENUM'ы).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* Коды завершения / ошибок */
typedef enum {
    ERR_OK = 0,
    ERR_USAGE,
    ERR_OPEN,
    ERR_READ,
    ERR_MISMATCH,  /* разное число строк в file2 и file3 */
    ERR_ALLOC
} AppError;

/* Одна пара замены (применение структуры по заданию) */
typedef struct {
    char *from;
    char *to;
} Replacement;

static int is_eng_letter(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/* Читает одну строку без '\n'; возвращает 0 при EOF до данных, 1 при успехе */
static int read_line(FILE *fp, char **buf, size_t *cap, size_t *len_out) {
    size_t len = 0;
    int c;

    if (*buf == NULL) {
        *cap = 32;
        *buf = malloc(*cap);
        if (!*buf)
            return -1;
    }

    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n')
            break;
        if (len + 1 >= *cap) {
            size_t ncap = *cap * 2;
            char *nb = realloc(*buf, ncap);
            if (!nb)
                return -1;
            *buf = nb;
            *cap = ncap;
        }
        (*buf)[len++] = (char)c;
    }
    if (len + 1 >= *cap) {
        size_t ncap = len + 2;
        char *nb = realloc(*buf, ncap);
        if (!nb)
            return -1;
        *buf = nb;
        *cap = ncap;
    }
    (*buf)[len] = '\0';
    *len_out = len;

    if (c == EOF && len == 0)
        return 0; /* конец файла, пустая «строка» не считается */
    return 1;
}

static void free_map(Replacement *map, size_t n) {
    for (size_t i = 0; i < n; i++) {
        free(map[i].from);
        free(map[i].to);
    }
    free(map);
}

/* Загрузка пар из двух файлов; *out_map должен быть NULL */
static AppError load_replacements(const char *path2, const char *path3,
                                  Replacement **out_map, size_t *out_n) {
    FILE *f2 = fopen(path2, "rb");
    FILE *f3 = fopen(path3, "rb");
    if (!f2 || !f3) {
        if (f2) fclose(f2);
        if (f3) fclose(f3);
        return ERR_OPEN;
    }

    Replacement *map = NULL;
    size_t n = 0;
    size_t cap = 0;

    char *line2 = NULL, *line3 = NULL;
    size_t cap2 = 0, cap3 = 0;
    size_t len2, len3;

    for (;;) {
        int r2 = read_line(f2, &line2, &cap2, &len2);
        int r3 = read_line(f3, &line3, &cap3, &len3);

        if (r2 < 0 || r3 < 0) {
            free(line2);
            free(line3);
            fclose(f2);
            fclose(f3);
            free_map(map, n);
            return ERR_ALLOC;
        }
        if (r2 == 0 && r3 == 0)
            break;
        if (r2 != r3) {
            free(line2);
            free(line3);
            fclose(f2);
            fclose(f3);
            free_map(map, n);
            return ERR_MISMATCH;
        }

        if (n == cap) {
            size_t ncap = cap ? cap * 2 : 8;
            Replacement *nm = realloc(map, ncap * sizeof *map);
            if (!nm) {
                free(line2);
                free(line3);
                fclose(f2);
                fclose(f3);
                free_map(map, n);
                return ERR_ALLOC;
            }
            map = nm;
            cap = ncap;
        }

        char *from = malloc(len2 + 1);
        char *to = malloc(len3 + 1);
        if (!from || !to) {
            free(from);
            free(to);
            free(line2);
            free(line3);
            fclose(f2);
            fclose(f3);
            free_map(map, n);
            return ERR_ALLOC;
        }
        memcpy(from, line2, len2 + 1);
        memcpy(to, line3, len3 + 1);

        map[n].from = from;
        map[n].to = to;
        n++;
    }

    free(line2);
    free(line3);
    fclose(f2);
    fclose(f3);

    *out_map = map;
    *out_n = n;
    return ERR_OK;
}

/* Первое совпадение по порядку строк в file2 */
static const char *lookup(const Replacement *map, size_t n,
                          const char *word, size_t wlen) {
    for (size_t i = 0; i < n; i++) {
        if (strlen(map[i].from) == wlen && memcmp(map[i].from, word, wlen) == 0)
            return map[i].to;
    }
    return NULL;
}

static AppError process(const char *path_in, const char *path_out,
                        const Replacement *map, size_t n) {
    FILE *in = fopen(path_in, "rb");
    FILE *out = fopen(path_out, "wb");
    if (!in || !out) {
        if (in) fclose(in);
        if (out) fclose(out);
        return ERR_OPEN;
    }

    char *wbuf = NULL;
    size_t wcap = 0, wlen = 0;
    int c;

    while ((c = fgetc(in)) != EOF) {
        if (is_eng_letter(c)) {
            if (wlen + 2 > wcap) {
                size_t ncap = wcap ? wcap * 2 : 32;
                char *nw = realloc(wbuf, ncap);
                if (!nw) {
                    free(wbuf);
                    fclose(in);
                    fclose(out);
                    return ERR_ALLOC;
                }
                wbuf = nw;
                wcap = ncap;
            }
            wbuf[wlen++] = (char)c;
        } else {
            if (wlen > 0) {
                wbuf[wlen] = '\0';
                const char *rep = lookup(map, n, wbuf, wlen);
                if (rep)
                    fputs(rep, out);
                else
                    fwrite(wbuf, 1, wlen, out);
                wlen = 0;
            }
            fputc(c, out);
        }
    }

    if (wlen > 0) {
        wbuf[wlen] = '\0';
        const char *rep = lookup(map, n, wbuf, wlen);
        if (rep)
            fputs(rep, out);
        else
            fwrite(wbuf, 1, wlen, out);
    }

    free(wbuf);
    if (fclose(in) != 0 || fclose(out) != 0)
        return ERR_READ;
    return ERR_OK;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr,
                "Usage: %s <text_file> <old_words_file> <new_words_file> <output_file>\n",
                argv[0]);
        return ERR_USAGE;
    }

    Replacement *map = NULL;
    size_t n = 0;

    AppError e = load_replacements(argv[2], argv[3], &map, &n);
    if (e != ERR_OK) {
        if (e == ERR_MISMATCH)
            fprintf(stderr, "Mismatch: different number of lines in file2 and file3.\n");
        else if (e == ERR_OPEN)
            perror("open");
        else
            fprintf(stderr, "Memory error loading mappings.\n");
        return (int)e;
    }

    e = process(argv[1], argv[4], map, n);
    free_map(map, n);

    if (e != ERR_OK) {
        if (e == ERR_OPEN)
            perror("open");
        else
            fprintf(stderr, "Error during processing.\n");
        return (int)e;
    }

    return ERR_OK;
}
















#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LEN 256
#define INITIAL_CAPACITY 10
#define GROWTH_FACTOR 2

// Структура для хранения пары слов (оригинал -> замена)
typedef struct {
    char *original;
    char *replacement;
} WordPair;

// Структура для хранения словаря замен
typedef struct {
    WordPair *pairs;
    int count;
    int capacity;
} Dictionary;

// Инициализация словаря
Dictionary* init_dictionary() {
    Dictionary *dict = (Dictionary*)malloc(sizeof(Dictionary));
    if (!dict) {
        fprintf(stderr, "Ошибка выделения памяти для словаря\n");
        exit(EXIT_FAILURE);
    }
    
    dict->count = 0;
    dict->capacity = INITIAL_CAPACITY;
    dict->pairs = (WordPair*)malloc(dict->capacity * sizeof(WordPair));
    
    if (!dict->pairs) {
        fprintf(stderr, "Ошибка выделения памяти для пар слов\n");
        free(dict);
        exit(EXIT_FAILURE);
    }
    
    return dict;
}

// Добавление пары слов в словарь
void add_pair(Dictionary *dict, const char *original, const char *replacement) {
    if (dict->count >= dict->capacity) {
        dict->capacity *= GROWTH_FACTOR;
        dict->pairs = (WordPair*)realloc(dict->pairs, dict->capacity * sizeof(WordPair));
        if (!dict->pairs) {
            fprintf(stderr, "Ошибка перевыделения памяти для словаря\n");
            exit(EXIT_FAILURE);
        }
    }
    
    // Выделяем память для строк и копируем их
    dict->pairs[dict->count].original = (char*)malloc(strlen(original) + 1);
    dict->pairs[dict->count].replacement = (char*)malloc(strlen(replacement) + 1);
    
    if (!dict->pairs[dict->count].original || !dict->pairs[dict->count].replacement) {
        fprintf(stderr, "Ошибка выделения памяти для строки\n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(dict->pairs[dict->count].original, original);
    strcpy(dict->pairs[dict->count].replacement, replacement);
    
    dict->count++;
}

// Загрузка словаря из файлов
Dictionary* load_dictionary(const char *original_file, const char *replacement_file) {
    FILE *f_orig = fopen(original_file, "r");
    FILE *f_repl = fopen(replacement_file, "r");
    
    if (!f_orig || !f_repl) {
        if (f_orig) fclose(f_orig);
        if (f_repl) fclose(f_repl);
        fprintf(stderr, "Ошибка открытия файлов словаря\n");
        exit(EXIT_FAILURE);
    }
    
    Dictionary *dict = init_dictionary();
    char orig_line[MAX_WORD_LEN];
    char repl_line[MAX_WORD_LEN];
    
    while (fgets(orig_line, sizeof(orig_line), f_orig) && 
           fgets(repl_line, sizeof(repl_line), f_repl)) {
        // Удаляем символ новой строки
        orig_line[strcspn(orig_line, "\n")] = '\0';
        repl_line[strcspn(repl_line, "\n")] = '\0';
        
        if (strlen(orig_line) > 0) {  // Игнорируем пустые строки
            add_pair(dict, orig_line, repl_line);
        }
    }
    
    fclose(f_orig);
    fclose(f_repl);
    return dict;
}

// Освобождение памяти словаря
void free_dictionary(Dictionary *dict) {
    for (int i = 0; i < dict->count; i++) {
        free(dict->pairs[i].original);
        free(dict->pairs[i].replacement);
    }
    free(dict->pairs);
    free(dict);
}

// Проверка, является ли символ разделителем (не буквой)
int is_delimiter(char c) {
    // Буквы английского алфавита считаем частью слова
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        return 0;
    }
    return 1;
}

// Поиск и замена слова в тексте
char* replace_word(const char *text, const Dictionary *dict, size_t *new_len) {
    if (!text || !dict || dict->count == 0) {
        *new_len = strlen(text);
        char *result = (char*)malloc(*new_len + 1);
        if (result) strcpy(result, text);
        return result;
    }
    
    // Начальная оценка размера буфера
    size_t capacity = strlen(text) + 1;
    char *result = (char*)malloc(capacity);
    if (!result) {
        fprintf(stderr, "Ошибка выделения памяти для результата\n");
        exit(EXIT_FAILURE);
    }
    result[0] = '\0';
    
    size_t pos = 0;
    size_t text_len = strlen(text);
    
    while (pos < text_len) {
        // Находим начало слова
        while (pos < text_len && is_delimiter(text[pos])) {
            // Копируем разделители как есть
            char ch[2] = {text[pos], '\0'};
            size_t current_len = strlen(result);
            if (current_len + 2 > capacity) {
                capacity *= GROWTH_FACTOR;
                result = (char*)realloc(result, capacity);
                if (!result) {
                    fprintf(stderr, "Ошибка перевыделения памяти\n");
                    exit(EXIT_FAILURE);
                }
            }
            strcat(result, ch);
            pos++;
        }
        
        if (pos >= text_len) break;
        
        // Нашли начало слова
        size_t word_start = pos;
        while (pos < text_len && !is_delimiter(text[pos])) {
            pos++;
        }
        size_t word_len = pos - word_start;
        
        // Извлекаем слово
        char *word = (char*)malloc(word_len + 1);
        if (!word) {
            fprintf(stderr, "Ошибка выделения памяти для слова\n");
            exit(EXIT_FAILURE);
        }
        strncpy(word, text + word_start, word_len);
        word[word_len] = '\0';
        
        // Ищем замену
        const char *replacement = NULL;
        for (int i = 0; i < dict->count; i++) {
            if (strcmp(word, dict->pairs[i].original) == 0) {
                replacement = dict->pairs[i].replacement;
                break;
            }
        }
        
        // Добавляем слово или замену в результат
        const char *to_add = replacement ? replacement : word;
        size_t add_len = strlen(to_add);
        size_t current_len = strlen(result);
        
        if (current_len + add_len + 1 > capacity) {
            while (current_len + add_len + 1 > capacity) {
                capacity *= GROWTH_FACTOR;
            }
            result = (char*)realloc(result, capacity);
            if (!result) {
                fprintf(stderr, "Ошибка перевыделения памяти\n");
                free(word);
                exit(EXIT_FAILURE);
            }
        }
        
        strcat(result, to_add);
        free(word);
    }
    
    *new_len = strlen(result);
    return result;
}

// Основная функция обработки файла
void process_file(const char *input_file, const char *output_file, const Dictionary *dict) {
    FILE *fin = fopen(input_file, "r");
    FILE *fout = fopen(output_file, "w");
    
    if (!fin || !fout) {
        if (fin) fclose(fin);
        if (fout) fclose(fout);
        fprintf(stderr, "Ошибка открытия файлов для обработки\n");
        exit(EXIT_FAILURE);
    }
    
    // Читаем весь файл в память
    fseek(fin, 0, SEEK_END);
    long file_size = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    
    char *buffer = (char*)malloc(file_size + 1);
    if (!buffer) {
        fprintf(stderr, "Ошибка выделения памяти для буфера файла\n");
        fclose(fin);
        fclose(fout);
        exit(EXIT_FAILURE);
    }
    
    size_t bytes_read = fread(buffer, 1, file_size, fin);
    buffer[bytes_read] = '\0';
    fclose(fin);
    
    // Обрабатываем текст
    size_t result_len;
    char *result = replace_word(buffer, dict, &result_len);
    
    // Записываем результат
    fwrite(result, 1, result_len, fout);
    
    // Освобождаем память
    free(buffer);
    free(result);
    fclose(fout);
}

// Функция для отображения справки
void print_usage(const char *program_name) {
    fprintf(stderr, "Использование: %s <файл_текста> <файл_оригиналов> <файл_замен> <выходной_файл>\n", 
            program_name);
    fprintf(stderr, "  файл_текста     - исходный текстовый файл\n");
    fprintf(stderr, "  файл_оригиналов - файл со словами для замены (по одному на строку)\n");
    fprintf(stderr, "  файл_замен      - файл со словами-заменами (по одному на строку)\n");
    fprintf(stderr, "  выходной_файл   - файл для сохранения результата\n");
}

int main(int argc, char *argv[]) {
    // Проверка аргументов командной строки
    if (argc != 5) {
        fprintf(stderr, "Ошибка: неверное количество аргументов\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    const char *text_file = argv[1];
    const char *original_words_file = argv[2];
    const char *replacement_words_file = argv[3];
    const char *output_file = argv[4];
    
    // Загружаем словарь замен
    Dictionary *dict = load_dictionary(original_words_file, replacement_words_file);
    
    if (dict->count == 0) {
        fprintf(stderr, "Предупреждение: словарь замен пуст\n");
    } else {
        printf("Загружено %d пар замен\n", dict->count);
    }
    
    // Обрабатываем файл
    process_file(text_file, output_file, dict);
    
    printf("Обработка завершена. Результат сохранен в файл: %s\n", output_file);
    
    // Освобождаем память
    free_dictionary(dict);
    
    return EXIT_SUCCESS;
}