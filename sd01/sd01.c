#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Структура для хранения пары слов
typedef struct {
    char *old_word;
    char *new_word;
} WordPair;

// Функция для чтения слов из файла
char **read_words_from_file(const char *filename, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Ошибка открытия файла");
        exit(EXIT_FAILURE);
    }

    char **words = NULL;
    char buffer[1024];
    *count = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        // Удаляем символ новой строки
        buffer[strcspn(buffer, "\n")] = '\0';
        words = realloc(words, (*count + 1) * sizeof(char *));
        words[*count] = strdup(buffer);
        (*count)++;
    }

    fclose(file);
    return words;
}

// Функция для замены слов в тексте
void replace_words_in_text(char *text, WordPair *pairs, int pair_count) {
    char *token = strtok(text, " \t\n.,!?;:\"'()[]{}");
    while (token != NULL) {
        for (int i = 0; i < pair_count; i++) {
            if (strcmp(token, pairs[i].old_word) == 0) {
                // Заменяем слово
                strcpy(token, pairs[i].new_word);
                break;
            }
        }
        token = strtok(NULL, " \t\n.,!?;:\"'()[]{}");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Использование: %s <файл1> <файл2> <файл3> <файл4>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file = argv[1];
    const char *old_words_file = argv[2];
    const char *new_words_file = argv[3];
    const char *output_file = argv[4];

    // Чтение слов для замены и новых слов
    int old_count, new_count;
    char **old_words = read_words_from_file(old_words_file, &old_count);
    char **new_words = read_words_from_file(new_words_file, &new_count);

    if (old_count != new_count) {
        fprintf(stderr, "Количество слов в файлах %s и %s не совпадает\n", old_words_file, new_words_file);
        return EXIT_FAILURE;
    }

    // Создание массива пар слов
    WordPair *pairs = malloc(old_count * sizeof(WordPair));
    for (int i = 0; i < old_count; i++) {
        pairs[i].old_word = old_words[i];
        pairs[i].new_word = new_words[i];
    }

    // Чтение текста из входного файла
    FILE *input = fopen(input_file, "r");
    if (!input) {
        perror("Ошибка открытия входного файла");
        return EXIT_FAILURE;
    }

    fseek(input, 0, SEEK_END);
    long file_size = ftell(input);
    fseek(input, 0, SEEK_SET);

    char *text = malloc(file_size + 1);
    fread(text, 1, file_size, input);
    text[file_size] = '\0';
    fclose(input);

    // Замена слов в тексте
    replace_words_in_text(text, pairs, old_count);

    // Запись результата в выходной файл
    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("Ошибка открытия выходного файла");
        return EXIT_FAILURE;
    }

    fwrite(text, 1, strlen(text), output);
    fclose(output);

    // Освобождение памяти
    for (int i = 0; i < old_count; i++) {
        free(old_words[i]);
        free(new_words[i]);
    }
    free(old_words);
    free(new_words);
    free(pairs);
    free(text);

    return EXIT_SUCCESS;
}