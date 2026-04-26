#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

// Функция для определения количества контрольных бит (L) для K информационных бит
int get_control_bits_count(int k) {
    int l = 0;
    while ((1 << l) < (k + l + 1)) {
        l++;
    }
    return l;
}

// Проверка, является ли позиция степенью двойки
bool is_power_of_two(int n) {
    return (n > 0) && ((n & (n - 1)) == 0);
}

// Кодирование одного блока данных
void encode_block(int k, unsigned char *data, unsigned char *encoded, int n) {
    int data_idx = 0;
    // Размещение информационных бит на позициях, не являющихся степенями 2 [cite: 83, 85]
    for (int i = 1; i <= n; i++) {
        if (!is_power_of_two(i)) {
            encoded[i] = data[data_idx++];
        } else {
            encoded[i] = 0; 
        }
    }

    // Расчет контрольных бит как четности соответствующих групп 
    for (int l = 0; l < (n - k); l++) {
        int pos = (1 << l);
        int parity = 0;
        for (int i = 1; i <= n; i++) {
            if (i & pos) {
                if (i != pos) parity ^= encoded[i];
            }
        }
        encoded[pos] = parity;
    }
}

// Декодирование и исправление ошибок в блоке
void decode_block(int k, unsigned char *encoded, unsigned char *decoded, int n) {
    int error_pos = 0;

    // Определение позиции ошибки путем суммирования позиций несовпадающих контрольных бит 
    for (int l = 0; l < (n - k); l++) {
        int pos = (1 << l);
        int parity = 0;
        for (int i = 1; i <= n; i++) {
            if (i & pos) {
                if (i != pos) parity ^= encoded[i];
            }
        }
        if (parity != encoded[pos]) {
            error_pos += pos;
        }
    }

    // Исправление ошибки путем инверсии бита 
    if (error_pos > 0 && error_pos <= n) {
        encoded[error_pos] ^= 1;
    }

    // Извлечение информационных бит
    int data_idx = 0;
    for (int i = 1; i <= n; i++) {
        if (!is_power_of_two(i)) {
            decoded[data_idx++] = encoded[i];
        }
    }
}

// Вспомогательная функция для работы с битовыми потоками в файлах
void process_file(const char *input_name, const char *output_name, int k, bool encoding) {
    FILE *in = fopen(input_name, "rb");
    FILE *out = fopen(output_name, "wb");
    if (!in || !out) {
        printf("Ошибка открытия файла!\n");
        return;
    }

    int l = get_control_bits_count(k);
    int n = k + l;
    
    unsigned char *block_data = (unsigned char *)malloc(k);
    unsigned char *block_encoded = (unsigned char *)malloc(n + 1);

    if (encoding) {
        // Чтение по k бит, запись по n бит
        int bit;
        int count = 0;
        while ((bit = fgetc(in)) != EOF) {
            for (int b = 7; b >= 0; b--) {
                block_data[count++] = (bit >> b) & 1;
                if (count == k) {
                    encode_block(k, block_data, block_encoded, n);
                    // Запись закодированного блока
                    for (int i = 1; i <= n; i++) {
                        static int out_byte = 0, out_count = 0;
                        out_byte = (out_byte << 1) | block_encoded[i];
                        if (++out_count == 8) {
                            fputc(out_byte, out);
                            out_byte = 0; out_count = 0;
                        }
                    }
                    count = 0;
                }
            }
        }
    } else {
        // Чтение по n бит, запись по k бит
        int bit;
        int count = 0;
        while ((bit = fgetc(in)) != EOF) {
            for (int b = 7; b >= 0; b--) {
                block_encoded[++count] = (bit >> b) & 1;
                if (count == n) {
                    decode_block(k, block_encoded, block_data, n);
                    for (int i = 0; i < k; i++) {
                        static int out_byte = 0, out_count = 0;
                        out_byte = (out_byte << 1) | block_data[i];
                        if (++out_count == 8) {
                            fputc(out_byte, out);
                            out_byte = 0; out_count = 0;
                        }
                    }
                    count = 0;
                }
            }
        }
    }

    free(block_data);
    free(block_encoded);
    fclose(in);
    fclose(out);
    printf("Готово! Результат сохранен в %s\n", output_name);
}

int main() {
    int choice, k;
    char filename[256];

    while (1) {
        printf("введите действие:\n1: кодирование\n2: декодирование\n3: выход из программы\n");
        if (scanf("%d", &choice) != 1) break;
        if (choice == 3) break;

        printf("введите размер текстового блока (8,12,16,24,32,48,64):");
        scanf("%d", &k);

        if (choice == 1) {
            printf("введите имя исходного файла:");
            scanf("%s", filename);
            process_file(filename, "encoded.bin", k, true);
        } else if (choice == 2) {
            printf("введите имя файла, который хотите декодировать:");
            scanf("%s", filename);
            process_file(filename, "decoded.txt", k, false);
        }
    }
    return 0;
}
