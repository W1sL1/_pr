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
    int Size = 0;
    if (Block == 8) Size = 12;
    if (Block == 12) Size = 17;
    if (Block == 16)Size = 21;
    if (Block == 24) Size = 29;
    if (Block == 32) Size = 38;
    if (Block == 48) Size = 54;
    if (Block == 64) Size = 71;
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
    FILE* BinaryFile = fopen("Binary.txt", "rb");
    FILE* ControlFile = fopen("Control.txt", "wb");
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
    FILE* UseFile = fopen("Binary.txt", "wb");
    FILE* File = fopen("Control.txt", "rb");
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
    if (TextToBit(NameFile, "Binary.txt")) return EXIT_FAILURE;
    if (AddControlBits(Block))return EXIT_FAILURE;
    WriteToFile("Coder.txt", "Control.txt");
    // remove("Binary.txt");
    // remove("Control.txt");
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
    if (TextToBit(NameFile, "Control.txt")) return EXIT_FAILURE;
    if (DeleteConrolBits(Block)) return EXIT_FAILURE;
    WriteToFile("Output.txt", "Binary.txt");
    remove("Binary.txt");
    remove("Control.txt");
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
        else printf("ошибка ввода, попробуйте еще раз\n");
    }
}

int main(void) {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    Menu();
    return EXIT_SUCCESS;
}

































// arg1 arg2 64 64 
#pragma warning(disable:4996)
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
//#include <Windows.h>
#include <locale.h>
char file_name[] = "input.txt";
char coding_file[]="coding.txt";

int razmer(int blok) {
	int size = 0;
	if (blok == 8) size = 12;
	if (blok == 12) size = 17;
	if (blok == 16)size = 21;
	if (blok == 24) size = 29;
	if (blok == 32) size = 38;
	if (blok == 48) size = 54;
	if (blok == 64) size = 71;
	return size;
}

int transfer_to_bit(char* file_name, char* name) {
	char buk = 0;
	int bukva = 0;
	FILE* input_file = fopen(file_name, "rb");
	FILE* useful_file = fopen(name, "wb");
	if (input_file == NULL){
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	if (useful_file == NULL) {
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	fseek(useful_file, 7, 0);
	while (!feof(input_file)) {
		bukva = fgetc(input_file);
		if (bukva < 0) {
			break;
		}
		for (int i = 0; i <= 7; i++) {
			buk = (bukva % 2) + '0';
			fputc(buk, useful_file);
			bukva = bukva / 2;
			if (i != 7) fseek(useful_file, -2, 1);
			else fseek(useful_file, -1, 1);
		}
		fseek(useful_file, 15, 1);
	}
	fclose(useful_file);
	fclose(input_file);
	return 0;
}
int transfer_to_symb(int nbits[]) {
	int numb = 0;
	if (nbits[0] == 1)numb += 128;
	if (nbits[1] == 1)numb += 64;
	if (nbits[2] == 1)numb += 32;
	if (nbits[3] == 1)numb += 16;
	if (nbits[4] == 1)numb += 8;
	if (nbits[5] == 1)numb += 4;
	if (nbits[6] == 1)numb += 2;
	if (nbits[7] == 1)numb += 1;
	return numb;
}
int file_write(char* file_name, char* Name) {
	int nbits[8] = { 0 }, numb;
	char bukva = 0;
	FILE* out_file = fopen(file_name, "wb");
	FILE* useful_file = fopen(Name, "rb");
	if (out_file == NULL) {
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	if (useful_file == NULL) {
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
int adding_cbits(int blok) {
	char bukva = 0;
	int summa = 0, cbits = 0, counter = 0, nbits[71];
	int size = razmer(blok);
	memset(nbits, 0, sizeof(nbits));
	FILE* binar_file = fopen("binary.txt", "rb");
	FILE* contr_file = fopen("control.txt", "wb");
	if (binar_file == NULL) {
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	if (contr_file == NULL) {
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	while (!feof(binar_file)) {
		for (int i = 0; i < size; i++) {
			if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) nbits[i] = 0;
			else {
				bukva = (char)fgetc(binar_file);
				if (!feof(binar_file)) nbits[i] = (int)bukva - '0';
				else nbits[i] = 0;
			}
		}
		for (int i = 0; i < size; i++) {
			if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) {
				cbits = i + 1;
				for (int j = 0; j < size; j++) {
					if (j < i) continue;
					if (counter != cbits) {
						counter++;
						summa = summa + nbits[j];
					}
					if (counter == cbits) {
						j += cbits;
						counter = 0;
					}
				}
				if (summa % 2 != 0)nbits[i] = 1;
				summa = 0;
				counter = 0;
			}
		}
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
			if (summa % 2 != 0 && nbits[i] != 1) {
				error += cbits;
			}
			else if (summa % 2 == 0 && nbits[i] != 0) {
				error += cbits;
			}
			summa = 0;
			counter = 0;
		}
	}
	if (error != 0) {
		if (nbits[error] == 0) nbits[error] = 1;
		else nbits[error] = 0;
	}
	return 0;
}
int delete_cbits(int blok) {
	char bukva = 0;
	int nbits[71] = { 0 };
	int size = razmer(blok);
	FILE* useful_file = fopen("binary.txt", "wb");
	FILE* file = fopen("control.txt", "rb");
	if (useful_file == NULL) {
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	if (file == NULL) {
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	while (!feof(file)) {
		for (int i = 0; i < size; i++) {
			if (!feof(file))bukva = (char)fgetc(file);
			if (!feof(file))nbits[i] = (int)bukva - '0';
			else nbits[i] = 0;
		}
		check_bits(nbits, size);
		for (int j = 0; j < size; j++) {
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
int coding(int blok) {
	//int blok = 0;
	int size = 0;
	//printf("Введите размер текстового блока(8,12,16,24,32,48,64): ");
	//scanf_s("%d", &blok);
	if ((size = razmer(blok)) == 0) {
		printf("Неверный размер!\n");
		return 1;
	}
	if (transfer_to_bit(file_name, "binary.txt")) return 1;
	if (adding_cbits(blok))return 1;
	file_write("coding.txt", "control.txt");
	remove("binary.txt");
	remove("control.txt");
	return 0;
}
int decoding(int blok) {
	//int blok = 0;
	int size = 0;
	//printf("Введите размер текстового блока(8,12,16,24,32,48,64): ");
	//scanf_s("%d", &blok);
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

int to_int (char* str){
	int num = 0;
	int i = 0;
	while(str[i] != '\0'){
		int sym;
		if (str[i] == '0') sym = 0;
		else if (str[i] == '1') sym = 1;
		else if (str[i] == '2') sym = 2;
		else if (str[i] == '3') sym = 3;
		else if (str[i] == '4') sym = 4;
		else if (str[i] == '5') sym = 5;
		else if (str[i] == '6') sym = 6;
		else if (str[i] == '7') sym = 7;
		else if (str[i] == '8') sym = 8;
		else if (str[i] == '9') sym = 9;
		num = num * 10 + sym;
		i++;
	}
	return num;
}

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, ".UTF8");
	int action = 0;
	int arg1 = to_int(argv[1]);
	int arg2 = to_int(argv[2]);

	while (action!=3) {
		printf("Введите действие:\n1: Кодирование\n2: Декодирование\n3: Выход из программы\n");
		scanf("%d", &action);
		if (action == 1 || action == 2 || action == 3) {
			if (action == 1) {
				coding(arg1);
				//printf("%s ", argv[1]);
				//printf("%d\n", arg1);
			}
			if (action == 2) {
				decoding(arg2);
				//printf("%s ", argv[2]);
				//printf("%d\n", arg2);
			}
		}
		else printf("Такого действия нет\n");
	}

	return 0;
}







































