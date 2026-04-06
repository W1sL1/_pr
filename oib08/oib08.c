#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#define ENCODE 1
#define DECODE 2
#define EXIT 3


void encoder() {
    int cnt = 0, bit = 0, bytetext = 0, bmpbyte = 0, i = 0, flag = 0;
    FILE* text = fopen("C:\\_pr\\oib08\\inp_txt.txt", "rb"), 
        * bmp1 = fopen("C:\\_pr\\oib08\\inp_pic.bmp", "rb"),
        * bmp2 = fopen("C:\\_pr\\oib08\\out_pic.bmp", "wb");
    char importantb = 0;          // для заголовка BMP
    unsigned char inform = 0, textmask = 255, imgmask = 255, other = 0, txtsymb = 0, imgsymb = 0, imgbyte = 0;
    char* textorig = NULL; 

    if ((text == NULL) || (bmp1 == NULL) || (bmp2 == NULL)) {
        printf("Ошибка открытия файла.\n");
        return;
    }

    // Определяем размер BMP-файла
    fseek(bmp1, 0, SEEK_END); 
    bmpbyte = ftell(bmp1); 
    fseek(bmp1, 0, SEEK_SET);

    // Определяем размер текстового сообщения
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
    
    // Проверка, поместится ли сообщение в изображение (54 байта заголовка не трогаем)
    if ((bytetext*8/bit) > (bmpbyte - 54)) {
        printf("Невозможно полностью поместить текст в графический файл.\nМаксимальная его вместимость при заданной упаковке %d бит. Текст занимает %d бит.\n", (bmpbyte - 54)*bit, bytetext*8);
        return;
    }

    // Копируем заголовок BMP (первые 54 байта) без изменений
    while (i < 54) {
        fread(&importantb, 1, 1, bmp1);
        fwrite(&importantb, 1, 1, bmp2);
        i++;
    }

    // Записываем длину сообщения: сначала 255, пока не остаток (схема с флагом)
    flag = bytetext / 255;
    while (flag != 0) {
        inform = 255;
        fseek(bmp1, 1, SEEK_CUR);   // пропускаем один байт в исходном изображении
        fwrite(&inform, 1, 1, bmp2);
        flag--;
    }
    flag = bytetext % 255;
    inform = (unsigned char)flag;
    fseek(bmp1, 1, SEEK_CUR);
    fwrite(&inform, 1, 1, bmp2);
    fseek(bmp1, 1, SEEK_CUR);
    fwrite(&bit, 1, 1, bmp2);       // сохраняем степень упаковки
    
    // Маски для выделения битов
    textmask = textmask << (8 - bit);   // маска для старших битов символа
    imgmask = imgmask << bit;           // маска для обнуления младших битов пикселя
    i = 1;
    for (cnt = 0; cnt < bytetext; cnt++) {
        fread(&txtsymb, 1, 1, text);
        // Упаковываем один байт текста в несколько байтов изображения (по `bit` бит)
        while (bit * i <= 8) {
            imgsymb = txtsymb & textmask;      // берем старшие `bit` бит
            imgsymb = imgsymb >> (8 - bit);    // сдвигаем вправо

            fread(&imgbyte, 1, 1, bmp1);
            imgbyte = imgbyte & imgmask;       // обнуляем младшие `bit` бит

            imgbyte = imgbyte | imgsymb;       // вставляем биты сообщения
            fwrite(&imgbyte, 1, 1, bmp2);

            txtsymb = txtsymb << bit;          // сдвигаем, чтобы взять следующие биты
            i++;
        }
        i = 1;
    }

    // Оставшиеся байты изображения копируем без изменений
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
    // Файлы: выходной текст и закодированное BMP
    FILE* text = fopen("C:\\_pr\\oib08\\out_txt.txt", "wb"), 
        * bmp2 = fopen("C:\\_pr\\oib08\\out_pic.bmp", "rb");
    
    unsigned char imgbyte = 0, info = 0, symbol = 0, imgmask = 255;
    int letterscnt = 1, bit = 0, n = 1, sum = 0;

    if ((text == NULL) || (bmp2 == NULL)) {
            printf("Ошибка открытия файла.\n");
            return;
    }

    // Пропускаем заголовок BMP (54 байта)
    fseek(bmp2, 54, SEEK_SET);
    
    // Считываем длину сообщения: читаем байты, пока не встретим не 255
    do{
        fread(&info, 1, 1, bmp2);
        sum = sum + (int)info;
    } while (info == 255);
    
    // Следующий байт — степень упаковки
    fread(&info, 1, 1, bmp2);
    bit = (int)info;
    
    // Маска для выделения младших `bit` бит из байта изображения
    imgmask = imgmask >> (8 - bit);
    
    // Восстанавливаем исходные байты сообщения
    while (letterscnt <= sum) {
        symbol = 0;
        while (bit * n <= 8) {
            fread(&imgbyte, 1, 1, bmp2);
            imgbyte = imgbyte & imgmask;   // выделяем полезные биты
            symbol = symbol << bit;        // освобождаем место для новых бит
            symbol = symbol | imgbyte;     // добавляем их
            n++;
        }
        fwrite(&symbol, 1, 1, text);       // записываем восстановленный байт
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