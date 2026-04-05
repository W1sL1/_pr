#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// gcc sd01.c -o sd01
// ./sd01 f1.txt f2.txt f3.txt f4.txt

// Структура для хранения пары "что заменяем -> на что заменяем"
typedef struct {
    char *target;       // исходное слово (что ищем)
    char *replacement;  // слово-замена (на что меняем)
} ReplacementPair;

// Контейнер для словаря замен (массив пар)
typedef struct {
    ReplacementPair *items;  // динамический массив пар
    int count;               // количество пар в словаре
} Dictionary;

// Освобождение памяти, выделенной под словарь
void free_dictionary(Dictionary *dict) {
    for (int i = 0; i < dict->count; i++) {
        free(dict->items[i].target);      // освобождаем исходное слово
        free(dict->items[i].replacement); // освобождаем слово-замену
    }
    free(dict->items); // освобождаем массив пар
}

// Чтение списка слов из файла (по одному слову на строку)
// Возвращает массив строк и их количество через параметр count
char** read_words(const char *filename, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) return NULL; // если файл не открылся — возвращаем NULL

    char **words = NULL;
    char buffer[1024];
    *count = 0;

    // Читаем файл построчно
    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\r\n")] = 0; // удаляем символы перевода строки (\n, \r)
        
        // Увеличиваем массив на один элемент
        words = realloc(words, (*count + 1) * sizeof(char*));
        words[*count] = strdup(buffer); // копируем строку в динамическую память
        (*count)++;
    }
    
    fclose(file);
    return words;
}

// Проверка, является ли символ частью слова
// Считаем буквами (включая кириллицу) и цифры
int is_word_char(char c) {
    return isalnum((unsigned char)c) || (unsigned char)c > 127; // >127 — символы кириллицы
}

int main(int argc, char *argv[]) {
    // Проверка количества аргументов командной строки
    if (argc < 5) {
        printf("Использование: %s f1.txt f2.txt f3.txt f4.txt\n", argv[0]);
        return 1;
    }

    int count2 = 0, count3 = 0;
    char **words2 = read_words(argv[2], &count2); // читаем слова для замены
    char **words3 = read_words(argv[3], &count3); // читаем слова-замены

    if (!words2 || !words3) {
        printf("Ошибка чтения файлов словаря.\n");
        return 1;
    }

    // Берём минимальное количество пар (если файлы разной длины)
    int min_count = count2 < count3 ? count2 : count3;
    
    // Выделяем память под словарь замен
    Dictionary dict = { malloc(min_count * sizeof(ReplacementPair)), min_count };

    // Заполняем словарь парами (target → replacement)
    for (int i = 0; i < min_count; i++) {
        dict.items[i].target = words2[i];       // что ищем
        dict.items[i].replacement = words3[i];  // на что заменяем
    }
    
    // Освобождаем временные массивы (сами строки уже в словаре)
    free(words2);
    free(words3);

    // Открываем входной и выходной файлы
    FILE *fin = fopen(argv[1], "r");
    FILE *fout = fopen(argv[4], "w");

    if (!fin || !fout) {
        printf("Ошибка открытия файлов текста.\n");
        return 1;
    }

    char c;
    char word_buf[1024]; // буфер для накопления символов текущего слова
    int idx = 0;         // текущая позиция в буфере

    // Читаем файл посимвольно
    while (1) {
        c = fgetc(fin);
        
        // Если символ — часть слова (буква/цифра/кириллица) и не конец файла
        if (is_word_char(c) && c != EOF) {
            word_buf[idx++] = c;           // добавляем в буфер слова
        } else {
            // Если в буфере есть накопленное слово
            if (idx > 0) {
                word_buf[idx] = '\0';      // завершаем строку нулём
                
                char *to_print = word_buf; // по умолчанию печатаем исходное слово
                
                // Ищем слово в словаре замен
                for (int i = 0; i < dict.count; i++) {
                    if (strcmp(word_buf, dict.items[i].target) == 0) {
                        to_print = dict.items[i].replacement; // нашли замену
                        break;
                    }
                }
                
                fprintf(fout, "%s", to_print); // печатаем (исходное или замену)
                idx = 0; // сбрасываем буфер для следующего слова
            }
            
            // Если достигнут конец файла — выходим из цикла
            if (c == EOF) break;
            
            // Печатаем текущий символ (не буквенный: пробел, запятая, точка и т.д.)
            fputc(c, fout);
        }
    }

    // Закрываем файлы и освобождаем память словаря
    fclose(fin);
    fclose(fout);
    free_dictionary(&dict);

    return 0;
}