#define _CRT_SECURE_NO_WARNINGS // Отключение предупреждений о небезопасных функциях в Visual Studio
#include <stdio.h> // Стандартный ввод-вывод
#include <stdlib.h> // Стандартная библиотека (exit и др.)
#include <string.h> // Работа со строками и памятью (memset, memcpy)
#define MAX_N 24 // Максимальный размер доски
typedef int my_bool; // Определение булевого типа через int //error 
#define true 1 // Определение истины
#define false 0 // Определение лжи
typedef struct {int x;int y;} Point; // Структура для хранения координат фигуры
//size +fig fig alredy // Комментарий о параметрах: размер, фигуры, уже есть
int N; int L; int K; // N - размер доски, L - сколько новых фигур добавить, K - сколько изначальных фигур
//orig fig //mask of boat: true - busy cell, false - free cell // Исходные фигуры и маска доски
Point initial[MAX_N]; my_bool board[MAX_N][MAX_N]; // Массив исходных фигур и доска (занято/свободно)
//(all fig: orig + new) // Все фигуры вместе
Point solution[MAX_N]; int solution_count = 0; FILE* output_file; // Массив решения, счётчик решений, файл вывода
//knight=horse // Движения коня
const int knight_dx[] = { -2, -1, 1, 2, 2, 1, -1, -2 }; // Смещения коня по X
const int knight_dy[] = { 1, 2, 2, 1, -1, -2, -2, -1 }; // Смещения коня по Y
//queen=ferz // Движения ферзя
const int queen_dx[] = { -1, -1, 0, 1, 1, 1, 0, -1 }; // Смещения ферзя по X (8 направлений)
const int queen_dy[] = { 0, 1, 1, 1, 0, -1, -1, -1 }; // Смещения ферзя по Y (8 направлений)
//chek point on boat // Проверка, что точка на доске
my_bool is_valid(int x, int y) { // Функция проверки валидности координат
    return x >= 0 && x < N && y >= 0 && y < N; // Возвращает true, если координаты внутри доски
}
//from (x, y) // Функция атаки от заданной клетки
void attack_from(int x, int y, my_bool temp_board[MAX_N][MAX_N], my_bool add) { // Помечает клетки под атакой или снимает атаку
    //knight attack // Атака коня
    for (int i = 0; i < 8; i++) { // Перебор всех 8 ходов коня
        int nx = x + knight_dx[i]; int ny = y + knight_dy[i]; // Вычисление новых координат
        if (is_valid(nx, ny)) temp_board[nx][ny] = temp_board[nx][ny] || add; // Если валидно, установка флага атаки
    }
    //queen attack // Атака ферзя
    for (int dir = 0; dir < 8; dir++) { // Перебор 8 направлений
        for (int step = 1; step < N; step++) { // Шаги в текущем направлении
            int nx = x + queen_dx[dir] * step; int ny = y + queen_dy[dir] * step; // Вычисление координат
            if (!is_valid(nx, ny)) break; // Если вышли за доску, прервать направление
            temp_board[nx][ny] = temp_board[nx][ny] || add; // Установка флага атаки
        }
    }
    //this cell is busy too // Сама клетка тоже занята
    temp_board[x][y] = temp_board[x][y] || add; // Пометка клетки с фигурой
}
void init_board() { // Инициализация доски
    //clean boat // Очистка доски
    memset(board, 0, sizeof(board)); // Заполнение доски нулями (false)
    //add orig fig // Добавление исходных фигур
    for (int i = 0; i < K; i++) { // Перебор всех исходных фигур
        int x = initial[i].x; int y = initial[i].y; // Координаты фигуры
        if (board[x][y]) {printf("Initial pieces attack each other\n");exit(1);} // Проверка, не бьют ли исходные фигуры друг друга
        //add fig and attack // Добавление фигуры и её атаки
        attack_from(x, y, board, true); // Пометка атак от этой фигуры
    }
}
//check // Проверка, можно ли поставить фигуру
my_bool can_place(int x, int y) { // Функция проверки возможности размещения
    return !board[x][y]; // Возвращает true, если клетка свободна
}
//recurs // Рекурсивная функция размещения фигур
void place_pieces(int placed, int start_x, int start_y) { // placed - сколько уже поставили, start_x, start_y - откуда перебирать
    if (placed == L) { // Если поставили все требуемые фигуры
        for (int i = 0; i < K + L; i++) { // Вывод всех фигур (исходных и новых)
            fprintf(output_file, "(%d,%d)", solution[i].x, solution[i].y); // Запись координат в файл
            if (i < K + L - 1) fprintf(output_file, " "); // Пробел между координатами
        }
        fprintf(output_file, "\n"); solution_count++; // Перевод строки и увеличение счётчика решений
        return; // Выход из рекурсии
    }
    //begin (start_x, start_y), enum all cells // Начиная с (start_x, start_y), перебор всех клеток
    for (int x = start_x; x < N; x++) { // Перебор строк
        int y_start = (x == start_x) ? start_y : 0; // С какого столбца начинать в текущей строке
        for (int y = y_start; y < N; y++) { // Перебор столбцов
            //check // Проверка
            if (can_place(x, y)) { // Если можно поставить фигуру
                //temp copy of boat // Временная копия доски
                my_bool temp_board[MAX_N][MAX_N]; // Создание копии
                memcpy(temp_board, board, sizeof(board)); // Сохранение состояния доски
                //plus new fig // Добавление новой фигуры
                attack_from(x, y, board, true); // Пометка атак от новой фигуры
                //save fig in solution // Сохранение фигуры в решение
                solution[K + placed].x = x; // Запись X координаты
                solution[K + placed].y = y; // Запись Y координаты
                // recurse add next figs // Рекурсивный вызов для следующей фигуры
                if (y + 1 < N) place_pieces(placed + 1, x, y + 1); // Если есть следующая клетка в строке
                else place_pieces(placed + 1, x + 1, 0); // Переход на следующую строку
                //hill boat (restore) // Восстановление доски (backtracking)
                memcpy(board, temp_board, sizeof(board)); // Возврат сохранённого состояния
            }
        }
    }
}
int main() { // Главная функция
    FILE* input_file = fopen("input.txt", "r"); // Открытие входного файла
    if (!input_file) {printf("Cannot open input.txt\n");return 1;} // Проверка успешности открытия
    fscanf(input_file, "%d %d %d", &N, &L, &K); // Чтение параметров: размер, новые фигуры, исходные фигуры
    if (N < 1 || N > MAX_N || K < 0 || K > N * N || L < 0 || L > N * N - K) { // Валидация параметров
        printf("Invalid input parameters\n"); // Сообщение об ошибке
        fclose(input_file); return 1; // Закрытие файла и выход
    }
    //read orig fig // Чтение исходных фигур
    for (int i = 0; i < K; i++) { // Перебор исходных фигур
        fscanf(input_file, "%d %d", &initial[i].x, &initial[i].y); // Чтение координат
        if (initial[i].x < 0 || initial[i].x >= N || initial[i].y < 0 || initial[i].y >= N) { // Проверка валидности
            printf("Invalid piece coordinates\n"); // Сообщение об ошибке
            fclose(input_file); // Закрытие файла
            return 1; // Выход
        }
        solution[i] = initial[i]; // Копирование в массив решения
    }
    fclose(input_file); // Закрытие входного файла
    init_board(); // Инициализация доски исходными фигурами
    output_file = fopen("output.txt", "w"); // Открытие выходного файла
    if (!output_file) {printf("Cannot open output.txt\n");return 1;} // Проверка успешности открытия
    //start recurse search // Запуск рекурсивного поиска
    if (L == 0) { // Если не нужно добавлять новые фигуры
        //if not necessary add figs, print start config // Вывод начальной конфигурации
        if (K > 0) { // Если есть исходные фигуры
            for (int i = 0; i < K; i++) { // Перебор
                fprintf(output_file, "(%d,%d)", solution[i].x, solution[i].y); // Вывод координат
                if (i < K - 1) { // Если не последняя фигура
                    fprintf(output_file, " "); // Пробел
                }
            }
            fprintf(output_file, "\n"); // Перевод строки
            solution_count++; // Увеличение счётчика
        }
        else {
            //nothing on board // На доске ничего нет
        }
    }
    else place_pieces(0, 0, 0); // Запуск рекурсии с начала доски
    if (solution_count == 0) fprintf(output_file, "no_solutions\n"); // Если решений нет, вывод сообщения
    fclose(output_file); // Закрытие выходного файла
    return 0; // Успешное завершение
}