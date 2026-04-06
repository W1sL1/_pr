#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#define ENCODE 1
#define DECODE 2
#define EXIT 3


void encoder() {
	int cnt = 0, bit = 0, bytetext = 0, bmpbyte = 0, i = 0, flag = 0;
	FILE* text = fopen("C:\\Users\\main\\Documents\\hiew32demo.exe", "rb"), 
		* bmp1 = fopen("C:\\Users\\main\\Documents\\pict.bmp", "rb"),
		* bmp2 = fopen("C:\\Users\\main\\Documents\\New.bmp", "wb");
	char importantb = 0;
	unsigned char inform = 0, textmask = 255, imgmask = 255, other = 0, txtsymb = 0, imgsymb = 0, imgbyte = 0;
	char* textorig = NULL;

	if ((text == NULL) || (bmp1 == NULL) || (bmp2 == NULL)) {
		printf("Ошибка открытия файла.\n");
		return;
	}

	fseek(bmp1, 0, SEEK_END); 
	bmpbyte = ftell(bmp1); 
	fseek(bmp1, 0, SEEK_SET);

	fseek(text, 0, SEEK_END);
	bytetext = ftell(text);
	fseek(text, 0, SEEK_SET);

	if (bytetext == 0) {
		printf("Сообщение пустое.");
		return;
	}

	printf("Выбирете степень упаковки сообщения(1/2/4/8)\n");
	scanf("%d", &bit);
	if (bit != 1 && bit != 2 && bit != 4 && bit != 8) {
		printf("Введено неверное значение\n");
		return;
	}
	
	if ((bytetext*8/bit) > (bmpbyte - 54)) {
		printf("Невозможно полностью поместить текст в графический файл.\nМаксимальная его вместимость при заданной упаковке %d бит. Текст занимает %d бит.\n", (bmpbyte - 54)*bit, bytetext*8);
		return;
	}

	while (i < 54) {
		fread(&importantb, 1, 1, bmp1);
		fwrite(&importantb, 1, 1, bmp2);
		i++;
	}

	flag = bytetext / 255;
	while (flag != 0) {
		inform = 255;
		fseek(bmp1, 1, SEEK_CUR);
		fwrite(&inform, 1, 1, bmp2);
		flag--;
	}
	flag = bytetext % 255;
	inform = (unsigned char)flag;
	fseek(bmp1, 1, SEEK_CUR);
	fwrite(&inform, 1, 1, bmp2);
	fseek(bmp1, 1, SEEK_CUR);
	fwrite(&bit, 1, 1, bmp2);
	
	textmask = textmask << (8 - bit);
	imgmask = imgmask << bit;
	i = 1;
	for (cnt = 0; cnt < bytetext; cnt++) {
		fread(&txtsymb, 1, 1, text);
		while (bit * i <= 8) {
			imgsymb = txtsymb & textmask;
			imgsymb = imgsymb >> (8 - bit);

			fread(&imgbyte, 1, 1, bmp1);
			imgbyte = imgbyte & imgmask;

			imgbyte = imgbyte | imgsymb;
			fwrite(&imgbyte, 1, 1, bmp2);

			txtsymb = txtsymb << bit;
			i++;
		}
		i = 1;
	}

	while (fread(&other, 1, 1, bmp1) == 1) {
		fwrite(&other, 1, 1, bmp2);
	}

	printf("Успешно закодировано.\n");
	fclose(text);
	fclose(bmp1);
	fclose(bmp2);
	return;
}

void decoder() {
	FILE* text = fopen("C:\\Users\\main\\Documents\\done.exe", "wb"), 
		* bmp2 = fopen("C:\\Users\\main\\Documents\\New.bmp", "rb");
	
	unsigned char imgbyte = 0, info = 0, symbol = 0, imgmask = 255;
	int letterscnt = 1, bit = 0, n = 1, sum = 0;

	if ((text == NULL) || (bmp2 == NULL)) {
			printf("Ошибка открытия файла.\n");
			return;
	}

	fseek(bmp2, 54, SEEK_SET);
	do{
		fread(&info, 1, 1, bmp2);
		sum = sum + (int)info;
	} while (info == 255);
	fread(&info, 1, 1, bmp2);
	bit = (int)info;
	
	imgmask = imgmask >> (8 - bit);
	while (letterscnt <= sum) {
		symbol = 0;
		while (bit * n <= 8) {
			fread(&imgbyte, 1, 1, bmp2);
			imgbyte = imgbyte & imgmask;
			symbol = symbol << bit;
			symbol = symbol | imgbyte;
			n++;
		}
		fwrite(&symbol, 1, 1, text);
		letterscnt++;
		n = 1;
	}
	printf("Успешно раскодировано.\n");
	fclose(text);
	fclose(bmp2);
	return;
}

int main() {
	int menu = 0;
	while (1) {
		printf("Введите номер выбранного действия:\n1 - запустить программу кодер\n2 - запустить декодер\n3 - выйти\n");
		scanf("%d", &menu);
		switch (menu)
		{
		case ENCODE:
			encoder();
			break;
		case DECODE:
			decoder();
			break;
		case EXIT:
			return 0;
		default:
			printf("Неверный ввод.\n");
		}
	}
	return 0;
}
