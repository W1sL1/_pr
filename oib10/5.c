#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
char file_name[] = "input.txt";
char coding_file[]="coding.txt";

// Определяет общий размер блока (данные + контрольные биты)
int razmer(int blok) {
	int size = 0;
	if (blok == 8) size = 12;   // 8 бит данных -> 12 бит всего
	if (blok == 12) size = 17;
	if (blok == 16)size = 21;
	if (blok == 24) size = 29;
	if (blok == 32) size = 38;
	if (blok == 48) size = 54;
	if (blok == 64) size = 71;
	return size;
}

// Переводит байты файла в текстовые символы '0' и '1' . file_name - исходный бинарный файл, name - выходной текстовый файл с битами
int transfer_to_bit(char* file_name, char* name) {
	char buk = 0;
	int bukva = 0;
	FILE* input_file = fopen(file_name, "rb");   // Открываем исходный файл для чтения (в бинарном режиме)
	FILE* useful_file = fopen(name, "wb");       // Открываем выходной файл для записи
	if (input_file == NULL){
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	if (useful_file == NULL) {
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	fseek(useful_file, 7, 0); // Смещение для корректного порядка записи битов (запись с конца байта)
	while (!feof(input_file)) {
		bukva = fgetc(input_file);  // Читаем очередной байт
		if (bukva < 0) {
			break;
		}
		// Проходим по 8 битам каждого байта
		for (int i = 0; i <= 7; i++) {
			buk = (bukva % 2) + '0'; // Получаем младший бит (0 или 1) и преобразуем в символ
			fputc(buk, useful_file); // Записываем символ в файл
			bukva = bukva / 2;       // Сдвигаем число вправо для получения следующего бита
			if (i != 7) fseek(useful_file, -2, 1);  // Возвращаемся на позицию назад для записи следующего бита
			else fseek(useful_file, -1, 1);         // Для последнего бита смещаемся на -1
		}
		fseek(useful_file, 15, 1);  // Переходим к следующему байту (смещение на 8 позиций вперёд + 7 уже занятых)
	}
	fclose(useful_file);
	fclose(input_file);
	return 0;
}

// Превращает массив из 8 "битов" (символов 0/1) обратно в целое число (байт)
int transfer_to_symb(int nbits[]) {
	int numb = 0;
	// Восстанавливаем байт по битам (старший бит - 128, младший - 1)
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

// Читает текстовые '0'/'1' и записывает их как реальные байты в файл
int file_write(char* file_name, char* Name) {
	int nbits[8] = { 0 }, numb;
	char bukva = 0;
	FILE* out_file = fopen(file_name, "wb");    // Файл для записи байтов
	FILE* useful_file = fopen(Name, "rb");      // Файл с текстовыми битами
	if (out_file == NULL) {
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	if (useful_file == NULL) {
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	while (!feof(useful_file)) {
		memset(nbits, 0, sizeof(nbits));        // Обнуляем массив битов
		// Читаем 8 символов (битов)
		for (int i = 0; i < 8; i++) {
			if (!feof(useful_file)) bukva = (char)fgetc(useful_file);
			if (!feof(useful_file)) nbits[i] = (int)bukva - '0';  // Преобразуем символ в число 0 или 1
		}
		numb = transfer_to_symb(nbits);  // Конвертируем 8 бит в число
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
	int summa = 0, cbits = 0, counter = 0, nbits[71];  // Массив до 71 элемента (максимальный размер блока)
	int size = razmer(blok);  // Получаем общий размер блока с контрольными битами
	memset(nbits, 0, sizeof(nbits));  // Обнуляем массив
	FILE* binar_file = fopen("binary.txt", "rb");     // Файл с исходными данными в виде битов
	FILE* contr_file = fopen("control.txt", "wb");    // Файл для записи данных с контрольными битами
	if (binar_file == NULL) {
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	if (contr_file == NULL) {
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	while (!feof(binar_file)) {
		// 1. Расстановка информационных бит и пропуск позиций для контрольных (степени 2)
		for (int i = 0; i < size; i++) {
			// Позиции 0, 1, 3, 7, 15, 31, 63 - это степени двойки (места для контрольных битов)
			if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) nbits[i] = 0;  // Пока записываем 0, позже рассчитаем значение
			else {
				bukva = (char)fgetc(binar_file);  // Читаем информационный бит
				if (!feof(binar_file)) nbits[i] = (int)bukva - '0';
				else nbits[i] = 0;  // Если файл закончился, дополняем нулями
			}
		}
		// 2. Расчет значений контрольных бит
		for (int i = 0; i < size; i++) {
			// Обрабатываем только позиции контрольных битов
			if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) {
				cbits = i + 1; // Шаг проверки для конкретного бита (1, 2, 4, 8, 16, 32, 64)
				// Проходим по всем битам, начиная с текущей позиции
				for (int j = 0; j < size; j++) {
					if (j < i) continue;  // Пропускаем биты до контрольного
					if (counter != cbits) {
						counter++;
						summa = summa + nbits[j];  // Суммируем проверяемые биты
					}
					if (counter == cbits) {
						j += cbits;  // Пропускаем блоки согласно алгоритму Хэмминга
						counter = 0;
					}
				}
				if (summa % 2 != 0)nbits[i] = 1; // Установка бита четности (если сумма нечетная)
				summa = 0;
				counter = 0;
			}
		}
		// Запись готового блока в файл
		for (int c = 0; c < size; c++) {
			bukva = (char)nbits[c] + '0';  // Преобразуем число в символ
			fputc(bukva, contr_file);
		}
		memset(nbits, 0, sizeof(nbits));  // Очищаем массив для следующего блока
	}
	fclose(binar_file);
	fclose(contr_file);
	return 0;
}

// Проверка и исправление ошибок в блоке (декодирование Хэмминга)
int check_bits(int nbits[], int size) {
	int cbits = 0, counter = 0, summa = 0, error = 0;
	// Проходим по всем контрольным битам
	for (int i = 0; i < size; i++) {
		if (i == 0 || i == 1 || i == 3 || i == 7 || i == 15 || i == 31 || i == 63) {
			cbits = i + 1;  // Размер шага проверки
			// Проверяем биты, за которые отвечает данный контрольный бит
			for (int j = 0; j < size; j++) {
				if (j < i) continue;
				if (counter != cbits) {
					counter++;
					if (j != i) summa = summa + nbits[j];  // Суммируем все биты, кроме самого контрольного
				}
				if (counter == cbits) {
					j += cbits;  // Пропускаем cbits битов
					counter = 0;
				}
			}
			// Если сумма не совпала с контрольным битом - накапливаем позицию ошибки
			// ошибки: если ожидалась 1, а получили 0 -> error += cbits
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
	// Если найдена ошибка (error > 0), инвертируем бит (индекс error-1) . error содержит позицию ошибочного бита (начиная с 1)
	if (error != 0) {
		if (nbits[error] == 0) nbits[error] = 1;  // Инвертируем ошибочный бит
		else nbits[error] = 0;
	}
	return 0;
}

// Удаление контрольных бит при декодировании
int delete_cbits(int blok) {
	char bukva = 0;
	int nbits[71] = { 0 };
	int size = razmer(blok);  // Размер блока с контрольными битами
	FILE* useful_file = fopen("binary.txt", "wb");   // Файл для чистых данных (без контрольных битов)
	FILE* file = fopen("control.txt", "rb");         // Файл с данными + контрольными битами
	if (useful_file == NULL) {
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	if (file == NULL) {
		printf("Ошибка при открытии файла\n");
		return 1;
	}
	while (!feof(file)) {
		// Читаем очередной блок (размером size)
		for (int i = 0; i < size; i++) {
			if (!feof(file))bukva = (char)fgetc(file);
			if (!feof(file))nbits[i] = (int)bukva - '0';
			else nbits[i] = 0;  // Дополняем нулями, если файл закончился
		}
		check_bits(nbits, size); // Исправляем одиночную ошибку перед удалением бит
		// Записываем только информационные биты (пропускаем степени двойки)
		for (int j = 0; j < size; j++) {
			// Пропускаем позиции контрольных битов: 0, 1, 3, 7, 15, 31, 63
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
	// Конвертируем исходный файл в текстовые биты
	if (transfer_to_bit(file_name, "binary.txt")) return 1;
	// Добавляем контрольные биты Хэмминга
	if (adding_cbits(blok))return 1;
	// Записываем результат в бинарный файл coding.txt
	file_write("coding.txt", "control.txt");
	remove("binary.txt");
	remove("control.txt");
	return 0;
}

// Процесс декодирования
int decoding(int blok) {
	//int blok = 0;
	int size = 0;
	if ((size = razmer(blok)) == 0) {
		printf("Неверный размер!\n");
		return 1;
	}
	// Конвертируем закодированный файл в текстовые биты
	if (transfer_to_bit(coding_file, "control.txt")) return 1;
	// Удаляем контрольные биты и исправляем ошибки
	if (delete_cbits(blok)) return 1;
	// Записываем декодированные данные в бинарный файл
	file_write("decoding.txt", "binary.txt");
	remove("binary.txt");
	remove("control.txt");
	return 0;
}

// перевод строки в число
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
	int arg1 = to_int(argv[1]);  // Размер блока для кодирования
	int arg2 = to_int(argv[2]);  // Размер блока для декодирования
	while (action!=3) {
		printf("Введите действие:\n1: Кодирование\n2: Декодирование\n3: Выход из программы\n");
		scanf("%d", &action);
		if (action == 1 || action == 2 || action == 3) {
			if (action == 1) {
				coding(arg1);
			}
			if (action == 2) {
				decoding(arg2);
			}
		}
		else printf("Такого действия нет\n");
	}
	return 0;
}
