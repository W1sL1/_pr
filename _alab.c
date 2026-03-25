// Условия лабораторной работы №1 по Структурам данных. 
// Тема: "Работа с файлами". Язык: Си.
// Необходимо реализовать программу, осуществляющую замену слов в тексте и вывод полученного текста в выходной файл.
//  Входные данные:
// - путь к файлу1 с текстом
// - путь к файлу2 со словами, которые требуется заменить
// - путь к файлу3 со словами, на которые требуется заменить
// - путь к выходному файлу4, в который будет записан результирующий текст.
// Слова в файлах 2 и 3 разделяются переносом строки. Слово на первой строке файла 2 должно заменяться на слово первой строки файла 3, слово на второй строке - на соответствующее слово второй строки и т.д.
// Требования:
// - текст может включать английские буквы, различные знаки препинания и специальные символы (в т.ч. пробелы, табуляции, и т.д.)
// - результирующий текст должен повторять все знаки препинания и специальные символы оригинального текста.
// - программа должна эффективно управлять памятью (применяем динамическую память, используем каждый байт оптимально).
// - входные данные в программу должны передаваться с использованием аргументов командной строки и входных параметров main (argv, argc). 
// - в программе необходимо найти применение и грамотно заиспользовать структуры (или объединения или ENUM'ы).

























// Язык си. 
// Лабораторная работа "4. Использование динамической памяти при обработке массивов"
// Необходимо реализовать программу, демонстрирующую использование 
// динамического выделения и освобождения памяти при обработке данных. 
// На вход программе подается сначала количество отсортированных массивов 
// целых чисел N, затем построчно размер массива и после его элементы. 
// Для каждого массива необходимо считать его длину и выделить 
// динамическую память ровно под то количество элементов, 
// которое содержится в массиве. 
// Программа должна считать входные массивы, 
// обработать их в соответствии с номером варианта 
// и вывести результат выполнения операции. 
// При формировании итогового массива программа должна использовать 
// только необходимый объем памяти, при необходимости изменяя размер 
// выделенной памяти. После завершения работы программы вся динамически 
// выделенная память должна быть корректно освобождена.
// Информация к вашему варианту задания:
// Объединить все входные массивы в один отсортированный массив, сохранив все элементы, включая повторяющиеся:
// Ввод
// 3
// 3 1 3 5
// 2 2 4
// 4 1 2 6 7	
// Вывод
// 1 1 2 2 3 4 5 6 7 





























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









//Найти n - ную подходящую дробь по значению цепной дроби.
//В стандартный поток ввода(stdin) через пробел подаются заранее не известное количество 
//элементов цепной дроби и n(ввод оканчивается EOF).
//Данные элементы необходимо сохранить в динамический массив.
//Далее на основе этих значений вычислить n - ную подходящую дробь.
//n - ной подходящей дробью для цепной дроби[a0; a1, a2, a3, …, an], 
//является значение некоторого рационального числа pn / qn
//Эйлер вывел рекуррентные формулы для вычисления числителей и знаменателей подходящих дробей :
//p - 1 = 1,
//p0 = a0,
//pn = an * pn - 1 + pn - 2
//q - 1 = 0,
//q0 = 1,
//qn = an * qn - 1 + qn - 2.
//вход: элементы цепной дроби и n.
//пример : 5 3 7 15 1 292 1 1
//выход : числитель и знаменатель n - ной подходящей дробь.
//пример : 104348 33215






#include <stdio.h>
#include <stdlib.h>

int main() {
   int* a = NULL;
   int count = 0;
   int num;

   // Читаем все числа
   while (scanf("%d", &num) == 1) {
       a = (int*)realloc(a, (count + 1) * sizeof(int));
       if (a == NULL) {
           return 1;
       }
       a[count++] = num;
   }

   if (count < 2) {
       free(a);
       return 1;
   }

   // Последнее число - это n
   int n = a[count - 1];
   count--; // Теперь count - количество элементов цепной дроби

   if (n >= count) {
       free(a);
       return 1;
   }

   long long p_prev2 = 1, p_prev1 = a[0];
   long long q_prev2 = 0, q_prev1 = 1;
   long long p_curr = p_prev1, q_curr = q_prev1;

   for (int i = 1; i <= n; i++) {
       p_curr = a[i] * p_prev1 + p_prev2;
       q_curr = a[i] * q_prev1 + q_prev2;

       p_prev2 = p_prev1;
       q_prev2 = q_prev1;
       p_prev1 = p_curr;
       q_prev1 = q_curr;
   }

   printf("%lld %lld\n", p_curr, q_curr);

   free(a);
   return 0;
}






//Язык "Си"
//Лабораторная работа "02. Модель процессора x86"
//Пожелания: избегать ошибки Run - Time Check Failure #2 - Stack around the variable 'cmd' was corrupted.
//Реализовать структуру "процессор", которая содержит 32 - х битные регистры EAX, ECX, EDX, EIP,
//для реализации данных регистров(кроме EIP) использовать объединения,
//так что - бы возможно было обращаться к младшим 16 битам регистра(AX, CX, DX) 
//и к страшим и младшим 8 битами регистров AX, CX, DX(AH, AL, CH, CL, DH, DL).
//Рекомендуется использовать типы данных uint_32_t, uint_16_t и uint_8_t.
//Регистры EAX, ECX, EDX - целочисленные регистры общего назначения.
//Регистр EIP - счётчик инструкция, содержит номер следующей инструкции.
//Также необходимо реализовать набор арифметических и отладочных команд :
//1. mov - копирует значение.
//2. add - суммирует значения.
//3. sub - вычитает значения.
//4. shw - выводит в консоль значение регистра или константы в шестнадцатеричном виде.
//команды mov, add, sub работают со всеми регистрами кроме EIP и константами, но константы только во втором аргументе.
//Примеры использования команд с описанием :
//mov EAX, 0xf35d # копирование значения 0xf35d в регистре EAX(EAX = 0xf35d)
//mov CL, AH # копирование значения из регистра AH в регистре CL(CL = AH)
//add DX, 0x2 # суммирование значениия регистра DX и занчения 0x2 (DX = DX + 0x2)
//add EDX, DX # суммирование значениия регистра EDX и занчения DX(EDX = EDX + DX)
//sub CX, 0xff # вычитание значениия 0xff из регистра занчения CX(CX = CX - 0xff)
//sub EAX, ECX # вычитание значениия регистра ECX из регистра занчения EAX(EAX = EAX - ECX)
//shw EIP # выводит в консоль значение регистра EIP
//shw DX # выводит в консоль значение регистра DX
//Программа читает данные команды из файла commands.txt и выполняет на модели процессора,
//в стандартный поток вывода(stdout) выводит значения команды shw.
//Примеры ввода(текст в файле commands.txt) и вывода :
//Ввод:
//shw EIP
//mov EAX 0xffffffff
//shw EAX
//add AH 0x1
//shw EAX
//mov EDX 0x1234
//sub EDX EDX
//shw EDX
//mov DX EAX
//shw EDX
//shw EIP
//Вывод :
//0x1
//0xffffffff
//0xffff00ff
//0x0
//0xff
//0xb




#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef union {
   uint32_t dword;   // EAX/ECX/EDX целиком (32 бита)
   struct {
       uint16_t lo;  // младшие 16 бит (AX/CX/DX)
       uint16_t hi;  // старшие 16 бит
   } w;
   struct {
       uint8_t lo;   // младшие 8 бит (AL/CL/DL)
       uint8_t hi;   // старшие 8 бит (AH/CH/DH)
       uint16_t hi16;
   } b;
} reg32_u;

typedef struct {
   reg32_u eax;
   reg32_u ecx;
   reg32_u edx;
   uint32_t eip;
} cpu_t;

typedef enum {
   OP_NONE,
   OP_REG8,
   OP_REG16,
   OP_REG32,
   OP_IMM
} op_type_t;

typedef struct {
   op_type_t type;
   uint8_t* r8;
   uint16_t* r16;
   uint32_t* r32;
   uint32_t imm;
} operand_t;

static uint32_t read_operand(const operand_t* op) {
   switch (op->type) {
   case OP_REG8:  return (uint32_t)(*op->r8);
   case OP_REG16: return (uint32_t)(*op->r16);
   case OP_REG32: return *op->r32;
   case OP_IMM:   return op->imm;
   default:       return 0;
   }
}

static void write_operand(operand_t* op, uint32_t value) {
   switch (op->type) {
   case OP_REG8:  *op->r8 = (uint8_t)value;  break;
   case OP_REG16: *op->r16 = (uint16_t)value; break;
   case OP_REG32: *op->r32 = value;           break;
   default: break;
   }
}

static int parse_register(const char* name, cpu_t* cpu, operand_t* op, int allow_eip) {
   char buf[8];
   size_t len = strlen(name);
   size_t i;

   if (len == 0 || len >= sizeof(buf))
       return -1;

   for (i = 0; i <= len; ++i)
       buf[i] = (char)toupper((unsigned char)name[i]);

   op->r8 = NULL;
   op->r16 = NULL;
   op->r32 = NULL;
   op->type = OP_NONE;

   if (strcmp(buf, "EAX") == 0) {
       op->type = OP_REG32;
       op->r32 = &cpu->eax.dword;
   }
   else if (strcmp(buf, "ECX") == 0) {
       op->type = OP_REG32;
       op->r32 = &cpu->ecx.dword;
   }
   else if (strcmp(buf, "EDX") == 0) {
       op->type = OP_REG32;
       op->r32 = &cpu->edx.dword;
   }
   else if (strcmp(buf, "EIP") == 0) {
       if (!allow_eip)
           return -1;
       op->type = OP_REG32;
       op->r32 = &cpu->eip;
   }
   else if (strcmp(buf, "AX") == 0) {
       op->type = OP_REG16;
       op->r16 = &cpu->eax.w.lo;
   }
   else if (strcmp(buf, "CX") == 0) {
       op->type = OP_REG16;
       op->r16 = &cpu->ecx.w.lo;
   }
   else if (strcmp(buf, "DX") == 0) {
       op->type = OP_REG16;
       op->r16 = &cpu->edx.w.lo;
   }
   else if (strcmp(buf, "AL") == 0) {
       op->type = OP_REG8;
       op->r8 = &cpu->eax.b.lo;
   }
   else if (strcmp(buf, "AH") == 0) {
       op->type = OP_REG8;
       op->r8 = &cpu->eax.b.hi;
   }
   else if (strcmp(buf, "CL") == 0) {
       op->type = OP_REG8;
       op->r8 = &cpu->ecx.b.lo;
   }
   else if (strcmp(buf, "CH") == 0) {
       op->type = OP_REG8;
       op->r8 = &cpu->ecx.b.hi;
   }
   else if (strcmp(buf, "DL") == 0) {
       op->type = OP_REG8;
       op->r8 = &cpu->edx.b.lo;
   }
   else if (strcmp(buf, "DH") == 0) {
       op->type = OP_REG8;
       op->r8 = &cpu->edx.b.hi;
   }
   else {
       return -1;
   }

   return 0;
}

static int parse_imm(const char* s, uint32_t* out) {
   char* endptr;
   unsigned long val = strtoul(s, &endptr, 0);
   if (s == endptr)
       return -1;
   *out = (uint32_t)val;
   return 0;
}

int main(void) {
   cpu_t cpu;
   FILE* f;
   char line[128];

   memset(&cpu, 0, sizeof(cpu));

   f = fopen("commands.txt", "r");
   if (!f) {
       fprintf(stderr, "Cannot open commands.txt\n");
       return 1;
   }

   while (fgets(line, sizeof(line), f)) {
       char* p = line;
       char* tok;
       char* op_str;
       char* arg1_str;
       char* arg2_str;
       operand_t dst, src;
       uint32_t val;

       /* убираем перевод строки */
       line[strcspn(line, "\r\n")] = '\0';

       /* пропуск начальных пробелов */
       while (isspace((unsigned char)*p))
           ++p;

       if (*p == '\0' || *p == '#')
           continue;

       /* увеличиваем EIP перед выполнением каждой инструкции */
       cpu.eip++;

       /* разбор: op arg1 arg2 (запятая отделяется как разделитель) */
       op_str = strtok(p, " \t");
       if (!op_str)
           continue;

       arg1_str = strtok(NULL, " \t,");
       arg2_str = strtok(NULL, " \t,");

       /* если пошёл комментарий после аргументов, игнорируем его */
       if (arg1_str && arg1_str[0] == '#')
           arg1_str = NULL;
       if (arg2_str && arg2_str[0] == '#')
           arg2_str = NULL;

       /* приводим команду к нижнему регистру */
       for (tok = op_str; *tok; ++tok)
           *tok = (char)tolower((unsigned char)*tok);

       if (strcmp(op_str, "shw") == 0) {
           if (!arg1_str)
               continue;

           if (parse_register(arg1_str, &cpu, &dst, 1) == 0) {
               val = read_operand(&dst);
           }
           else {
               if (parse_imm(arg1_str, &val) != 0)
                   continue;
           }
           printf("0x%x\n", val);
       }
       else if (strcmp(op_str, "mov") == 0 ||
           strcmp(op_str, "add") == 0 ||
           strcmp(op_str, "sub") == 0) {

           int is_add = (strcmp(op_str, "add") == 0);
           int is_sub = (strcmp(op_str, "sub") == 0);

           if (!arg1_str || !arg2_str)
               continue;

           /* EIP недопустим в mov/add/sub */
           if (parse_register(arg1_str, &cpu, &dst, 0) != 0)
               continue;

           if (parse_register(arg2_str, &cpu, &src, 0) != 0) {
               /* второй аргумент может быть константой */
               if (parse_imm(arg2_str, &val) != 0)
                   continue;
               src.type = OP_IMM;
               src.imm = val;
           }

           if (!is_add && !is_sub) { /* mov */
               val = read_operand(&src);
               write_operand(&dst, val);
           }
           else { /* add/sub */
               uint32_t a = read_operand(&dst);
               uint32_t b = read_operand(&src);
               uint32_t res = is_add ? (a + b) : (a - b);
               write_operand(&dst, res);
           }
       }
       /* неизвестные команды просто игнорируем */
   }

   fclose(f);
   return 0;
}








//Язык "Си"
//Тема "4. Рекурсия"
//Лабораторная работа "4.1. Расстановка шахматных фигур"
//Пожелания: исправить 4.1.c(5) : fatal error C1083 : 
//Cannot open include file : 'stdbool.h' : No such file or directory.
//Дана квадратная шахматная доска размером N x N.На доске уже размещено K фигур.Фигуры размещены так, 
//что находятся не под боем друг друга.
//Необходимо расставить на доске еще L фигур так, 
//чтобы никакая из фигур на доске не находилась под боем любой другой фигуры.Необходимо найти все возможные решения.
//Входные данные : файл input.txt.На первой строке файла записаны три числа : N L K(через пробел).
//Далее следует K строк, содержащих числа x и y(через пробел) - координаты уже стоящей на доске фигуры.
//Координаты отсчитываются от 0 до N - 1. 1 <= N <= 24.
//Выходные данные : файл output.txt.На каждое найденное решение необходимо записать в файл одну строку.
//Строка состоит из пар(x, y) - координаты фигур на доске.В решение следует вывести координаты всех фигур, 
//находящихся на доске.Каждую фигуру необходимо записать в виде пары координат, разделенных запятой и обрамленных скобками.
//Координаты отсчитываются от 0 до N - 1. Порядок, в котором фигуры перечислены в решении, не имеет значения.
//Порядок перечисления решений не имеет значения.Выводимые решения не должны содержать повторы, 
//т.е.каждое найденное решение необходимо вывести только один раз.
//Если не было найдено ни одного решения, в файл необходимо записать no solutions.
//Ваша фигура - магараджа.Фигура объединяет в себе возможности ферзя и коня, 
//т.е.под боем оказываются все вертикали, горизонтали, диагонали и клетки(+-1, +-2), (+-2, +-1) от фигуры.
//Пример 1 :
//input.txt :
//1 1 0
//output.txt :
//(0, 0)
//Пример 2 :
//input.txt :
//2 1 0
//output.txt :
//(0, 0)
//(1, 0)
//(0, 1)
//(1, 1)
//Пример 3 :
//input.txt :
//3 1 0
//output.txt :
//(0, 0)
//(1, 0)
//(2, 0)
//(0, 1)
//(0, 2)
//(1, 1)
//(1, 2)
//(2, 1)
//(2, 2)
//Пример 4:
//input.txt :
//4 2 0
//output.txt :
//(0, 0) (1, 3)
//(0, 0) (2, 3)
//(0, 0) (3, 1)
//(0, 0) (3, 2)
//(0, 3) (1, 0)
//(1, 0) (2, 3)
//(1, 0) (3, 3)
//(0, 3) (2, 0)
//(1, 3) (2, 0)
//(2, 0) (3, 3)
//(0, 1) (3, 0)
//(0, 2) (3, 0)
//(1, 3) (3, 0)
//(2, 3) (3, 0)
//(0, 1) (3, 2)
//(0, 1) (3, 3)
//(0, 2) (3, 3)
//(0, 2) (3, 1)
//(0, 3) (3, 1)
//(0, 3) (3, 2)
//Пример 5:
//input.txt :
//5 3 1
//1 0
//output.txt :
//(0, 3) (1, 0) (3, 4) (4, 1)
