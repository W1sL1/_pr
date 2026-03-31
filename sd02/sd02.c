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
