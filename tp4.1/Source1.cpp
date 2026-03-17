//#define _CRT_SECURE_NO_WARNINGS
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdbool.h>
//
//#define MAX_N 24
//
//// Структура для хранения координат фигуры
//typedef struct {
//    int x;
//    int y;
//} Point;
//
//// Глобальные переменные
//int N;                      // Размер доски
//int L;                      // Сколько фигур нужно добавить
//int K;                      // Сколько фигур уже стоит
//Point initial[MAX_N];       // Изначальные фигуры
//bool board[MAX_N][MAX_N];   // Маска доски: true - клетка занята или под боем, false - свободна
//Point solution[MAX_N];      // Текущее решение (все фигуры: начальные + новые)
//int solution_count = 0;     // Счетчик найденных решений
//FILE* output_file;          // Выходной файл
//
//// Направления для коня (8 возможных ходов)
//const int knight_dx[] = { -2, -1, 1, 2, 2, 1, -1, -2 };
//const int knight_dy[] = { 1, 2, 2, 1, -1, -2, -2, -1 };
//
//// Направления для ферзя (8 направлений)
//const int queen_dx[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
//const int queen_dy[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
//
//// Функция проверки, находится ли точка в пределах доски
//bool is_valid(int x, int y) {
//    return x >= 0 && x < N && y >= 0 && y < N;
//}
//
//// Функция атаки магараджей из точки (x, y)
//void attack_from(int x, int y, bool temp_board[MAX_N][MAX_N], bool add) {
//    // Атака как конь
//    for (int i = 0; i < 8; i++) {
//        int nx = x + knight_dx[i];
//        int ny = y + knight_dy[i];
//        if (is_valid(nx, ny)) {
//            temp_board[nx][ny] = temp_board[nx][ny] || add;
//        }
//    }
//
//    // Атака как ферзь (по всем направлениям)
//    for (int dir = 0; dir < 8; dir++) {
//        for (int step = 1; step < N; step++) {
//            int nx = x + queen_dx[dir] * step;
//            int ny = y + queen_dy[dir] * step;
//            if (!is_valid(nx, ny)) break;
//            temp_board[nx][ny] = temp_board[nx][ny] || add;
//        }
//    }
//
//    // Сама клетка тоже считается занятой/под боем
//    temp_board[x][y] = temp_board[x][y] || add;
//}
//
//// Функция для инициализации доски с учетом начальных фигур
//void init_board() {
//    // Очищаем доску
//    memset(board, 0, sizeof(board));
//
//    // Добавляем все начальные фигуры
//    for (int i = 0; i < K; i++) {
//        int x = initial[i].x;
//        int y = initial[i].y;
//
//        // Проверяем, что начальные фигуры не бьют друг друга
//        // (по условию задачи они уже размещены корректно, но проверим на всякий случай)
//        if (board[x][y]) {
//            printf("Error: Initial pieces attack each other!\n");
//            exit(1);
//        }
//
//        // Добавляем фигуру и её атаку
//        attack_from(x, y, board, true);
//    }
//}
//
//// Проверка, можно ли поставить фигуру в клетку (x, y)
//bool can_place(int x, int y) {
//    return !board[x][y]; // Клетка должна быть свободна и не под боем
//}
//
//// Основная рекурсивная функция для размещения фигур
//void place_pieces(int placed, int start_x, int start_y) {
//    // Если разместили все L фигур
//    if (placed == L) {
//        // Выводим решение
//        for (int i = 0; i < K + L; i++) {
//            fprintf(output_file, "(%d,%d)", solution[i].x, solution[i].y);
//            if (i < K + L - 1) {
//                fprintf(output_file, " ");
//            }
//        }
//        fprintf(output_file, "\n");
//        solution_count++;
//        return;
//    }
//
//    // Перебираем все клетки для размещения следующей фигуры
//    // Начинаем с (start_x, start_y) для избежания повторений
//    for (int x = start_x; x < N; x++) {
//        int y_start = (x == start_x) ? start_y : 0;
//        for (int y = y_start; y < N; y++) {
//            // Проверяем, можно ли поставить фигуру
//            if (can_place(x, y)) {
//                // Создаем временную копию доски
//                bool temp_board[MAX_N][MAX_N];
//                memcpy(temp_board, board, sizeof(board));
//
//                // Добавляем новую фигуру
//                attack_from(x, y, board, true);
//
//                // Сохраняем фигуру в решение
//                solution[K + placed].x = x;
//                solution[K + placed].y = y;
//
//                // Рекурсивно размещаем следующие фигуры
//                // Следующая фигура может быть на той же строке, но с большей колонкой
//                if (y + 1 < N) {
//                    place_pieces(placed + 1, x, y + 1);
//                }
//                else {
//                    place_pieces(placed + 1, x + 1, 0);
//                }
//
//                // Восстанавливаем доску
//                memcpy(board, temp_board, sizeof(board));
//            }
//        }
//    }
//}
//
//int main() {
//    // Открываем входной файл
//    FILE* input_file = fopen("input.txt", "r");
//    if (!input_file) {
//        printf("Error: Cannot open input.txt\n");
//        return 1;
//    }
//
//    // Читаем N, L, K
//    fscanf(input_file, "%d %d %d", &N, &L, &K);
//
//    // Проверяем ограничения
//    if (N < 1 || N > MAX_N || K < 0 || K > N * N || L < 0 || L > N * N - K) {
//        printf("Error: Invalid input parameters\n");
//        fclose(input_file);
//        return 1;
//    }
//
//    // Читаем начальные фигуры
//    for (int i = 0; i < K; i++) {
//        fscanf(input_file, "%d %d", &initial[i].x, &initial[i].y);
//        if (initial[i].x < 0 || initial[i].x >= N || initial[i].y < 0 || initial[i].y >= N) {
//            printf("Error: Invalid piece coordinates\n");
//            fclose(input_file);
//            return 1;
//        }
//        solution[i] = initial[i];
//    }
//
//    fclose(input_file);
//
//    // Инициализируем доску
//    init_board();
//
//    // Открываем выходной файл
//    output_file = fopen("output.txt", "w");
//    if (!output_file) {
//        printf("Error: Cannot open output.txt\n");
//        return 1;
//    }
//
//    // Запускаем рекурсивный поиск
//    if (L == 0) {
//        // Если не нужно добавлять фигуры, просто выводим начальную конфигурацию
//        if (K > 0) {
//            for (int i = 0; i < K; i++) {
//                fprintf(output_file, "(%d,%d)", solution[i].x, solution[i].y);
//                if (i < K - 1) {
//                    fprintf(output_file, " ");
//                }
//            }
//            fprintf(output_file, "\n");
//            solution_count++;
//        }
//        else {
//            // Если и K=0, и L=0 - это странно, но по условию...
//            // Можно ничего не выводить или вывести пустую строку?
//            // По логике, пустая доска - это решение? В условии не сказано.
//            // Оставим без вывода, т.к. "no solutions" выводится только если нет решений,
//            // а пустая доска формально удовлетворяет условиям.
//        }
//    }
//    else {
//        place_pieces(0, 0, 0);
//    }
//
//    // Если решений не найдено
//    if (solution_count == 0) {
//        fprintf(output_file, "no solutions\n");
//    }
//
//    fclose(output_file);
//
//    return 0;
//}