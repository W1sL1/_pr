#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// Глобальные имена файлов
char file_name[] = "input.txt";
char coding_file[] = "coding.txt";

// Определяет общий размер блока (данные + контрольные биты)
int razmer(int blok) {
    int size = 0; 
    if (blok == 8) size = 12; 
    if (blok == 12) size = 17;
    if (blok == 16) size = 21; 
    if (blok == 24) size = 29; 
    if (blok == 32) size = 38;
    if (blok == 48) size = 54; 
    if (blok == 64) size = 71; 
    return size;
}

// Переводит байты файла в текстовые символы '0' и '1'
int transfer_to_bit(char* file_name, char* name) {
    char buk = 0;
    int bukva = 0;
    FILE* input_file = fopen(file_name, "rb");
    FILE* useful_file = fopen(name, "wb");
    
    if (input_file == NULL || useful_file == NULL){
        printf("Ошибка при открытии файла\n");
        return 1;
    }

    fseek(useful_file, 7, 0); // Смещение для корректного порядка записи битов
    while (!feof(input_file)) {
        bukva = fgetc(input_file);
        if (bukva < 0) break;

        for (int i = 0; i <= 7; i++) {
            buk = (bukva % 2) + '0'; // Получаем младший бит
            fputc(buk, useful_file);
            bukva = bukva / 2;
            // Хитрая навигация курсором для записи битов в нужном порядке
            if (i != 7) fseek(useful_file, -2, 1);
            else fseek(useful_file, -1, 1);
        }
        fseek(useful_file, 15, 1);
    }
    fclose(useful_file);
    fclose(input_file);
    return 0;
}

// Превращает массив из 8 "битов" (символов 0/1) обратно в целое число (байт)
int transfer_to_symb(int nbits[]) {
    int numb = 0;
    if (nbits[0] == 1) numb += 128;
    if (nbits[1] == 1) numb += 64;
    if (nbits[2] == 1) numb += 32;
    if (nbits[3] == 1) numb += 16;
    if (nbits[4] == 1) numb += 8;
    if (nbits[5] == 1) numb += 4;
    if (nbits[6] == 1) numb += 2;
    if (nbits[7] == 1) numb += 1;
    return numb;
}

// Читает текстовые '0'/'1' и записывает их как реальные байты в файл
int file_write(char* file_name, char* Name) {
    int nbits[8] = { 0 }, numb;
    char bukva = 0;
    FILE* out_file = fopen(file_name, "wb");
    FILE* useful_file = fopen(Name, "rb");
    
    if (out_file == NULL || useful_file == NULL) {
        printf("Ошибка при открытии файла\n");
        return 1;
    }
    
    while (!feof(useful_file)) {
        memset(nbits, 0, sizeof(nbits));
        for (int i = 0; i < 8; i++) {
            if (!feof(useful_file)) bukva = (char)fgetc(useful_file);
            if (!feof(useful_file)) nbits[i] = (int)bukva - '0';
        }
        numb = transfer_to_symb(nbits);
        bukva = (char)numb;
        fputc(bukva, out_file);
    }
    fclose(out_file);
    fclose(useful_file);
    return 0;
}

// Добавление контрольных бит Хэмминга
int adding_cbits(int blok) {
    char bukva = 0;
    int summa = 0, cbits = 0, counter = 0, nbits[71];
    int size = razmer(blok);
    memset(nbits, 0, sizeof(nbits));

    FILE* binar_file = fopen("binary.txt", "rb");
    FILE* contr_file = fopen("control.txt", "wb");
    
    if (binar_file == NULL || contr_file == NULL) return 1;

    while (!feof(binar_file)) {
        // 1. Расстановка информационных бит и пропуск позиций для контрольных (степени 2)
        for (int i = 0; i < size; i++) {
            if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) {
                nbits[i] = 0;
            } else {
                bukva = (char)fgetc(binar_file);
                if (!feof(binar_file)) nbits[i] = (int)bukva - '0';
                else nbits[i] = 0;
            }
        }

        // 2. Расчет значений контрольных бит
        for (int i = 0; i < size; i++) {
            if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) {
                cbits = i + 1; // Шаг проверки для конкретного бита
                for (int j = 0; j < size; j++) {
                    if (j < i) continue;
                    if (counter != cbits) {
                        counter++;
                        summa = summa + nbits[j];
                    }
                    if (counter == cbits) {
                        j += cbits; // Пропускаем блоки согласно алгоритму
                        counter = 0;
                    }
                }
                if (summa % 2 != 0) nbits[i] = 1; // Установка бита четности
                summa = 0;
                counter = 0;
            }
        }

        // Запись готового блока в файл
        for (int c = 0; c < size; c++) {
            bukva = (char)nbits[c] + '0';
            fputc(bukva, contr_file);
        }
        memset(nbits, 0, sizeof(nbits));
    }
    fclose(binar_file);
    fclose(contr_file);
    return 0;
}

// Проверка и исправление ошибок в блоке
int check_bits(int nbits[], int size) {
    int cbits = 0, counter = 0, summa = 0, error = 0;
    for (int i = 0; i < size; i++) {
        if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) {
            cbits = i + 1;
            for (int j = 0; j < size; j++) {
                if (j < i) continue;
                if (counter != cbits) {
                    counter++;
                    if (j != i) summa = summa + nbits[j];
                }
                if (counter == cbits) {
                    j += cbits;
                    counter = 0;
                }
            }
            // Если сумма не совпала с контрольным битом - накапливаем позицию ошибки
            if (summa % 2 != 0 && nbits[i] != 1) error += cbits;
            else if (summa % 2 == 0 && nbits[i] != 0) error += cbits;
            summa = 0;
            counter = 0;
        }
    }
    // Если найдена ошибка (error > 0), инвертируем бит (индекс error-1)
    if (error != 0) {
        if (nbits[error - 1] == 0) nbits[error - 1] = 1;
        else nbits[error - 1] = 0;
    }
    return 0;
}

// Удаление контрольных бит при декодировании
int delete_cbits(int blok) {
    char bukva = 0;
    int nbits[71] = { 0 };
    int size = razmer(blok);
    FILE* useful_file = fopen("binary.txt", "wb");
    FILE* file = fopen("control.txt", "rb");
    
    if (useful_file == NULL || file == NULL) return 1;

    while (!feof(file)) {
        for (int i = 0; i < size; i++) {
            if (!feof(file)) bukva = (char)fgetc(file);
            if (!feof(file)) nbits[i] = (int)bukva - '0';
            else nbits[i] = 0;
        }
        
        check_bits(nbits, size); // Исправляем ошибку перед удалением бит

        for (int j = 0; j < size; j++) {
            // Записываем только информационные биты (пропускаем степени двойки)
            if (j == 0 || j == 1 || j == 3 || j == 7 || j == 15 || j == 31 || j == 63) continue;
            else {
                bukva = (char)nbits[j] + '0';
                fputc(bukva, useful_file);
            }
        }
    }
    fclose(useful_file);
    fclose(file);
    return 0;
}

// Процесс кодирования
int coding(int blok) {
    int size = 0;
    if ((size = razmer(blok)) == 0) {
        printf("Неверный размер!\n");
        return 1;
    }
    if (transfer_to_bit(file_name, "binary.txt")) return 1;
    if (adding_cbits(blok)) return 1;
    file_write("coding.txt", "control.txt");
    remove("binary.txt");
    remove("control.txt");
    return 0;
}

// Процесс декодирования
int decoding(int blok) {
    int size = 0;
    if ((size = razmer(blok)) == 0) {
        printf("Неверный размер!\n");
        return 1;
    }
    if (transfer_to_bit(coding_file, "control.txt")) return 1;
    if (delete_cbits(blok)) return 1;
    file_write("decoding.txt", "binary.txt");
    remove("binary.txt");
    remove("control.txt");
    return 0;
}

// Аналог atoi: перевод строки в число
int to_int (char* str){
    int num = 0;
    int i = 0;
    while(str[i] != '\0'){
        int sym = str[i] - '0';
        if (sym >= 0 && sym <= 9) {
            num = num * 10 + sym;
        }
        i++;
    }
    return num;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, ".UTF8");
    
    // Ожидается два аргумента командной строки (размеры блоков)
    if (argc < 3) {
        printf("Использование: %s <размер_кодирования> <размер_декодирования>\n", argv[0]);
        return 1;
    }

    int action = 0;
    int arg1 = to_int(argv[1]);
    int arg2 = to_int(argv[2]);

    while (action != 3) {
        printf("\nВведите действие:\n1: Кодирование\n2: Декодирование\n3: Выход из программы\n");
        if (scanf("%d", &action) != 1) break;

        if (action == 1) {
            if (coding(arg1) == 0) printf("Успешно закодировано в coding.txt\n");
        } else if (action == 2) {
            if (decoding(arg2) == 0) printf("Успешно декодировано в decoding.txt\n");
        } else if (action != 3) {
            printf("Такого действия нет\n");
        }
    }
    return 0;
}
