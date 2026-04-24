#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <Windows.h>
#include <locale.h>
// Определение констант для пунктов меню
#define CODER 1
#define DECODER 2
#define EXIT 3

// Функция возвращает общий размер блока с учётом контрольных битов Хэмминга
int FindOutSize(int Block) {
    int Size = 0; if (Block == 8) Size = 12; if (Block == 12) Size = 17; if (Block == 16)Size = 21;
    if (Block == 24) Size = 29; if (Block == 32) Size = 38; if (Block == 48) Size = 54; if (Block == 64) Size = 71;
    return Size;
}

// Проверка успешности открытия файла, вывод ошибки при неудаче
int CheckFiles(FILE* File) {
    if (File == NULL) {
        printf("ошибка при открытии файла\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// Преобразование текстового файла в битовый поток (каждый байт раскладывается на 8 бит и записывается как символы '0'/'1')
int TextToBit(char* NameFile, char* Name) {
    char Sym = 0;
    int Symbol = 0;
    FILE* InputFile = fopen(NameFile, "rb");
    FILE* UseFile = fopen(Name, "wb");
    if (CheckFiles(UseFile)) return EXIT_FAILURE;
    if (CheckFiles(InputFile)) return EXIT_FAILURE;
    fseek(UseFile, 7, SEEK_SET); // Смещение на 7 байт от начала выходного файла
    while (!feof(InputFile)) {
        Symbol = fgetc(InputFile);
        if (Symbol < 0) {
            break;
        }
        // Преобразование одного байта в 8 бит, начиная с младшего
        for (int i = 0; i <= 7; i++) {
            Sym = (Symbol % 2) + '0'; // Получение младшего бита и преобразование в символ
            fputc(Sym, UseFile);
            Symbol = Symbol / 2; // Сдвиг вправо
            if (i != 7) fseek(UseFile, -2, SEEK_CUR); // Возврат на позицию назад для перезаписи
            else fseek(UseFile, -1, SEEK_CUR);
        }
        fseek(UseFile, 15, SEEK_CUR); // Пропуск 15 байт между байтами
    }
    fclose(UseFile);
    fclose(InputFile);
    return EXIT_SUCCESS;
}

// Преобразование массива из 8 бит в целое число (сборка байта)
int BitsToSymbol(int NumberBits[]) {
    int Number = 0;
    if (NumberBits[0] == 1)Number += 128; // Старший бит
    if (NumberBits[1] == 1)Number += 64;
    if (NumberBits[2] == 1)Number += 32;
    if (NumberBits[3] == 1)Number += 16;
    if (NumberBits[4] == 1)Number += 8;
    if (NumberBits[5] == 1)Number += 4;
    if (NumberBits[6] == 1)Number += 2;
    if (NumberBits[7] == 1)Number += 1; // Младший бит
    return Number;
}

// Чтение битового файла и запись восстановленных байтов в выходной файл
int WriteToFile(char* NameFile, char* Name) {
    int NumberBits[8] = { 0 }, Number;
    char Symbol = 0;
    FILE* OutputFile = fopen(NameFile, "wb");
    FILE* UseFile = fopen(Name, "rb");
    if (CheckFiles(UseFile)) return EXIT_FAILURE;
    if (CheckFiles(OutputFile)) return EXIT_FAILURE;
    while (!feof(UseFile)) {
        memset(NumberBits, 0, sizeof(NumberBits)); // Обнуление массива битов
        for (int i = 0; i < 8; i++) {
            if (!feof(UseFile)) Symbol = (char)fgetc(UseFile); // Чтение символа '0' или '1'
            if (!feof(UseFile)) NumberBits[i] = (int)Symbol - '0'; // Преобразование в число 0/1
        }
        Number = BitsToSymbol(NumberBits); // Сборка байта
        Symbol = (char)Number;
        fputc(Symbol, OutputFile); // Запись байта
    }
    fclose(OutputFile);
    fclose(UseFile);
    return EXIT_SUCCESS;
}

// Добавление контрольных битов Хэмминга к битовому потоку
int AddControlBits(int Block) {
    char Symbol = 0;
    int Sum = 0, ControlBit = 0, Count = 0, NumberBits[71];
    int Size = FindOutSize(Block); // Получение размера блока с контрольными битами
    memset(NumberBits, 0, sizeof(NumberBits));
    FILE* BinaryFile = fopen("bin.txt", "rb");
    FILE* ControlFile = fopen("con.txt", "wb");
    if (CheckFiles(BinaryFile)) return EXIT_FAILURE;
    if (CheckFiles(ControlFile)) return EXIT_FAILURE;
    while (!feof(BinaryFile)) {
        for (int i = 0; i < Size; i++) {
            // Позиции контрольных битов: 0,1,3,7,15,31,63 (степени двойки минус 1)
            if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) NumberBits[i] = 0;
            else {
                Symbol = (char)fgetc(BinaryFile);
                if (!feof(BinaryFile)) NumberBits[i] = (int)Symbol - '0';
                else NumberBits[i] = 0;
            }
        }
        for (int i = 0; i < Size; i++) {
            if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) {
                ControlBit = i + 1; // Номер контрольного бита (1,2,4,8,16,32,64)
                for (int j = 0; j < Size; j++) {
                    if (j < i) continue; // Пропуск позиций до контрольного бита
                    if (Count != ControlBit) {
                        Count++;
                        Sum = Sum + NumberBits[j]; // Суммирование битов, контролируемых данным битом
                    }
                    if (Count == ControlBit) {
                        j += ControlBit; // Пропуск следующей группы битов
                        Count = 0;
                    }
                }
                if (Sum % 2 != 0)NumberBits[i] = 1; // Установка бита чётности
                Sum = 0;
                Count = 0;
            }
        }
        for (int c = 0; c < Size; c++) {
            Symbol = (char)NumberBits[c] + '0'; // Преобразование 0/1 в символ
            fputc(Symbol, ControlFile);
        }
        memset(NumberBits, 0, sizeof(NumberBits)); // Очистка массива для следующего блока
    }
    fclose(BinaryFile);
    fclose(ControlFile);
    return EXIT_SUCCESS;
}

// Проверка и исправление однократной ошибки в блоке по коду Хэмминга
int CheckCorrectness(int NumberBits[], int Size) {
    int ControlBit = 0, Count = 0, Sum = 0, Error = 0;
    for (int i = 0; i < Size; i++) {
        if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) {
            ControlBit = i + 1;
            for (int j = 0; j < Size; j++) {
                if (j < i) continue;
                if (Count != ControlBit) {
                    Count++;
                    if (j != i) Sum = Sum + NumberBits[j]; // Сумма без самого контрольного бита
                }
                if (Count == ControlBit) {
                    j += ControlBit;
                    Count = 0;
                }
            }
            // Вычисление синдрома ошибки: если сумма нечётная, а бит равен 0, или сумма чётная, а бит равен 1
            if (Sum % 2 != 0 && NumberBits[i] != 1) {
                Error += ControlBit;
            }
            else if (Sum % 2 == 0 && NumberBits[i] != 0) {
                Error += ControlBit;
            }
            Sum = 0;
            Count = 0;
        }
    }
    if (Error != 0) {
        // Исправление ошибки: инвертирование бита в позиции Error-1
        if (NumberBits[Error] == 0) NumberBits[Error] = 1;
        else NumberBits[Error] = 0;
    }
    return EXIT_SUCCESS;
}

// Удаление контрольных битов из блока после проверки и исправления ошибок
int DeleteConrolBits(int Block) {
    char Symbol = 0;
    int NumberBits[71] = { 0 };
    int Size = FindOutSize(Block);
    FILE* UseFile = fopen("bin.txt", "wb");
    FILE* File = fopen("con.txt", "rb");
    if (CheckFiles(UseFile)) return EXIT_FAILURE;
    if (CheckFiles(File)) return EXIT_FAILURE;
    while (!feof(File)) {
        for (int i = 0; i < Size; i++) {
            if (!feof(File))Symbol = (char)fgetc(File);
            if (!feof(File))NumberBits[i] = (int)Symbol - '0';
            else NumberBits[i] = 0;
        }
        CheckCorrectness(NumberBits, Size); // Проверка и исправление ошибок
        for (int j = 0; j < Size; j++) {
            if (j == 0 || j == 1 || j == 3 || j == 7 || j == 15 || j == 31 || j == 63) continue; // Пропуск контрольных битов
            else {
                Symbol = (char)NumberBits[j] + '0';
                fputc(Symbol, UseFile); // Запись только информационных битов
            }
        }
    }
    fclose(UseFile);
    fclose(File);
    return EXIT_SUCCESS;
}

// Функция кодирования: запрос размера блока, имени файла и выполнение цепочки преобразований
int Coder(void) {
    int Block = 0;
    char NameFile[50] = { 0 };
    printf("введите размер текстового блока:");
    if (scanf("%d", &Block) == 0)return EXIT_FAILURE;
    while ((getchar()) != '\n'); // Очистка буфера ввода
    if (Block != 8 && Block != 16 && Block != 32 && Block != 64 && Block != 12 && Block != 48 && Block != 24) {
        printf("введен неправильный размер текстового блока\n");
        return EXIT_FAILURE;
    }
    printf("введите имя исходного файла:");
    if (scanf("%s", &NameFile) == 0)return EXIT_FAILURE;
    if (TextToBit(NameFile, "bin.txt")) return EXIT_FAILURE; // Текст -> биты
    if (AddControlBits(Block))return EXIT_FAILURE; // Добавление контрольных битов
    WriteToFile("cod.txt", "con.txt"); // Запись закодированного файла
    return EXIT_SUCCESS;
}

// Функция декодирования: запрос размера блока, имени файла и выполнение обратных преобразований
int Decoder(void) {
    int Block = 0;
    char NameFile[50] = { 0 };
    printf("введите размер текстового блока:");
    if (scanf("%d", &Block) == 0)return EXIT_FAILURE;
    while ((getchar()) != '\n');
    if (Block != 8 && Block != 16 && Block != 32 && Block != 64 && Block != 12 && Block != 48 && Block != 24) {
        printf("введен неправильный размер текстового блока\n");
        return EXIT_FAILURE;
    }
    printf("введите имя файла, который хотите декодировать:");
    if (scanf("%s", &NameFile) == 0)return EXIT_FAILURE;
    if (TextToBit(NameFile, "con.txt")) return EXIT_FAILURE; // Файл -> биты
    if (DeleteConrolBits(Block)) return EXIT_FAILURE; // Удаление контрольных битов с исправлением ошибок
    WriteToFile("out.txt", "bin.txt"); // Запись декодированного файла
    remove("bin.txt"); // Удаление временных файлов
    remove("con.txt");
    return EXIT_SUCCESS;
}

// Главное меню программы: циклический выбор кодирования, декодирования или выхода
void Menu(void) {
    while (1) {
        int Choice = 0;
        printf("введите действие:\n1: кодирование\n2: декодирование\n3: выход из программы\n");
        if (scanf("%d", &Choice) == 0)return;
        while ((getchar()) != '\n');
        if (Choice == CODER || Choice == DECODER || Choice == EXIT) {
            if (Choice == EXIT) break;
            if (Choice == CODER) {
                Coder();
                continue;
            }
            if (Choice == DECODER) {
                Decoder();
                continue;
            }
        }
        else printf("ошибка ввода, еще раз\n");
    }
}

int main(void) {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    Menu();
    return EXIT_SUCCESS;
}
