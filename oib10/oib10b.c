#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <Windows.h>
#include <locale.h>
#define CODER 1
#define DECODER 2
#define EXIT 3

int FindOutSize(int Block) {
    int Size = 0; if (Block == 8) Size = 12; if (Block == 12) Size = 17; if (Block == 16)Size = 21;
    if (Block == 24) Size = 29; if (Block == 32) Size = 38; if (Block == 48) Size = 54; if (Block == 64) Size = 71;
    return Size;
}

int CheckFiles(FILE* File) {
    if (File == NULL) {
        printf("ошибка при открытии файла\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int TextToBit(char* NameFile, char* Name) {
    char Sym = 0;
    int Symbol = 0;
    FILE* InputFile = fopen(NameFile, "rb");
    FILE* UseFile = fopen(Name, "wb");
    if (CheckFiles(UseFile)) return EXIT_FAILURE;
    if (CheckFiles(InputFile)) return EXIT_FAILURE;
    fseek(UseFile, 7, SEEK_SET);
    while (!feof(InputFile)) {
        Symbol = fgetc(InputFile);
        if (Symbol < 0) {
            break;
        }
        for (int i = 0; i <= 7; i++) {
            Sym = (Symbol % 2) + '0';
            fputc(Sym, UseFile);
            Symbol = Symbol / 2;
            if (i != 7) fseek(UseFile, -2, SEEK_CUR);
            else fseek(UseFile, -1, SEEK_CUR);
        }
        fseek(UseFile, 15, SEEK_CUR);
    }
    fclose(UseFile);
    fclose(InputFile);
    return EXIT_SUCCESS;
}

int BitsToSymbol(int NumberBits[]) {
    int Number = 0;
    if (NumberBits[0] == 1)Number += 128;
    if (NumberBits[1] == 1)Number += 64;
    if (NumberBits[2] == 1)Number += 32;
    if (NumberBits[3] == 1)Number += 16;
    if (NumberBits[4] == 1)Number += 8;
    if (NumberBits[5] == 1)Number += 4;
    if (NumberBits[6] == 1)Number += 2;
    if (NumberBits[7] == 1)Number += 1;
    return Number;
}

int WriteToFile(char* NameFile, char* Name) {
    int NumberBits[8] = { 0 }, Number;
    char Symbol = 0;
    FILE* OutputFile = fopen(NameFile, "wb");
    FILE* UseFile = fopen(Name, "rb");
    if (CheckFiles(UseFile)) return EXIT_FAILURE;
    if (CheckFiles(OutputFile)) return EXIT_FAILURE;
    while (!feof(UseFile)) {
        memset(NumberBits, 0, sizeof(NumberBits));
        for (int i = 0; i < 8; i++) {
            if (!feof(UseFile)) Symbol = (char)fgetc(UseFile);
            if (!feof(UseFile)) NumberBits[i] = (int)Symbol - '0';
        }
        Number = BitsToSymbol(NumberBits);
        Symbol = (char)Number;
        fputc(Symbol, OutputFile);
    }
    fclose(OutputFile);
    fclose(UseFile);
    return EXIT_SUCCESS;
}

int AddControlBits(int Block) {
    char Symbol = 0;
    int Sum = 0, ControlBit = 0, Count = 0, NumberBits[71];
    int Size = FindOutSize(Block);
    memset(NumberBits, 0, sizeof(NumberBits));
    FILE* BinaryFile = fopen("bin.txt", "rb");
    FILE* ControlFile = fopen("con.txt", "wb");
    if (CheckFiles(BinaryFile)) return EXIT_FAILURE;
    if (CheckFiles(ControlFile)) return EXIT_FAILURE;
    while (!feof(BinaryFile)) {
        for (int i = 0; i < Size; i++) {
            if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) NumberBits[i] = 0;
            else {
                Symbol = (char)fgetc(BinaryFile);
                if (!feof(BinaryFile)) NumberBits[i] = (int)Symbol - '0';
                else NumberBits[i] = 0;
            }
        }
        for (int i = 0; i < Size; i++) {
            if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) {
                ControlBit = i + 1;
                for (int j = 0; j < Size; j++) {
                    if (j < i) continue;
                    if (Count != ControlBit) {
                        Count++;
                        Sum = Sum + NumberBits[j];
                    }
                    if (Count == ControlBit) {
                        j += ControlBit;
                        Count = 0;
                    }
                }
                if (Sum % 2 != 0)NumberBits[i] = 1;
                Sum = 0;
                Count = 0;
            }
        }
        for (int c = 0; c < Size; c++) {
            Symbol = (char)NumberBits[c] + '0';
            fputc(Symbol, ControlFile);
        }
        memset(NumberBits, 0, sizeof(NumberBits));
    }
    fclose(BinaryFile);
    fclose(ControlFile);
    return EXIT_SUCCESS;
}

int CheckCorrectness(int NumberBits[], int Size) {
    int ControlBit = 0, Count = 0, Sum = 0, Error = 0;
    for (int i = 0; i < Size; i++) {
        if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) {
            ControlBit = i + 1;
            for (int j = 0; j < Size; j++) {
                if (j < i) continue;
                if (Count != ControlBit) {
                    Count++;
                    if (j != i) Sum = Sum + NumberBits[j];
                }
                if (Count == ControlBit) {
                    j += ControlBit;
                    Count = 0;
                }
            }
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
        if (NumberBits[Error] == 0) NumberBits[Error] = 1;
        else NumberBits[Error] = 0;
    }
    return EXIT_SUCCESS;
}

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
        CheckCorrectness(NumberBits, Size);
        for (int j = 0; j < Size; j++) {
            if (j == 0 || j == 1 || j == 3 || j == 7 || j == 15 || j == 31 || j == 63) continue;
            else {
                Symbol = (char)NumberBits[j] + '0';
                fputc(Symbol, UseFile);
            }
        }
    }
    fclose(UseFile);
    fclose(File);
    return EXIT_SUCCESS;
}

int Coder(void) {
    int Block = 0;
    char NameFile[50] = { 0 };
    printf("введите размер текстового блока:");
    if (scanf("%d", &Block) == 0)return EXIT_FAILURE;
    while ((getchar()) != '\n');
    if (Block != 8 && Block != 16 && Block != 32 && Block != 64 && Block != 12 && Block != 48 && Block != 24) {
        printf("введен неправильный размер текстового блока\n");
        return EXIT_FAILURE;
    }
    printf("введите имя исходного файла:");
    if (scanf("%s", &NameFile) == 0)return EXIT_FAILURE;
    if (TextToBit(NameFile, "bin.txt")) return EXIT_FAILURE;
    if (AddControlBits(Block))return EXIT_FAILURE;
    WriteToFile("cod.txt", "con.txt");
    return EXIT_SUCCESS;
}

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
    if (TextToBit(NameFile, "con.txt")) return EXIT_FAILURE;
    if (DeleteConrolBits(Block)) return EXIT_FAILURE;
    WriteToFile("out.txt", "bin.txt");
    remove("bin.txt");
    remove("con.txt");
    return EXIT_SUCCESS;
}

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
