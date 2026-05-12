// Лабораторная работа "14. Представление и обработка графов" . язык си . человек сам вводит входные данные . 
// Написать программу, на вход которой подается матрица смежности графа и номера двух узлов. Программа должна построить граф на основе представленной матрицы смежности и осуществить поиск согласно выданному варианту.
// Информация к вашему варианту задания:
// Необходимо осуществить поиск всех путей между двумя указанными узлами.
// Формат вывода: каждый путь пишется с новой строки и представляет собой список номеров узлов, разделенных запятыми. Если ни одного пути не существует вывести 0.
// Ввод:
// 0, 1, 1, 0
// 1, 0, 1, 1
// 1, 1, 0, 1
// 0, 1, 1, 0
// 1 4
// Вывод четко следующий:
// 1,2,4
// 1,3,4
// 1,2,3,4
// 1,3,2,4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXN 100
#define MAX_PATHS 100000

typedef struct {
    int nodes[MAXN];
    int len;
} Path;

int graph[MAXN][MAXN];
int n;

Path paths[MAX_PATHS];
int pathCount = 0;

int curPath[MAXN];
int visited[MAXN];

/* Парсинг всех целых чисел из строки (игнорирует запятые, пробелы и др. символы) */
int parseInts(const char *line, int out[], int maxOut) {
    int cnt = 0;
    const char *p = line;

    while (*p && cnt < maxOut) {
        while (*p && !isdigit((unsigned char)*p) && *p != '-') p++;
        if (!*p) break;

        char *endPtr;
        long val = strtol(p, &endPtr, 10);
        if (p == endPtr) break;

        out[cnt++] = (int)val;
        p = endPtr;
    }

    return cnt;
}

void savePath(int len) {
    if (pathCount >= MAX_PATHS) return;
    paths[pathCount].len = len;
    for (int i = 0; i < len; i++) {
        paths[pathCount].nodes[i] = curPath[i];
    }
    pathCount++;
}

void dfs(int u, int target, int depth) {
    visited[u] = 1;
    curPath[depth] = u;

    if (u == target) {
        savePath(depth + 1);
    } else {
        for (int v = 0; v < n; v++) {
            if (graph[u][v] && !visited[v]) {
                dfs(v, target, depth + 1);
            }
        }
    }

    visited[u] = 0;
}

int pathCompare(const void *a, const void *b) {
    const Path *p1 = (const Path *)a;
    const Path *p2 = (const Path *)b;

    /* Сначала по длине пути */
    if (p1->len != p2->len) return p1->len - p2->len;

    /* Затем лексикографически */
    int m = (p1->len < p2->len) ? p1->len : p2->len;
    for (int i = 0; i < m; i++) {
        if (p1->nodes[i] != p2->nodes[i]) return p1->nodes[i] - p2->nodes[i];
    }
    return 0;
}

int main(void) {
    char line[2048];
    int temp[MAXN];

    /* 1) Читаем первую строку матрицы, определяем n */
    if (!fgets(line, sizeof(line), stdin)) {
        printf("0\n");
        return 0;
    }

    n = parseInts(line, temp, MAXN);
    if (n <= 0 || n > MAXN) {
        printf("0\n");
        return 0;
    }

    for (int j = 0; j < n; j++) graph[0][j] = temp[j];

    /* 2) Читаем оставшиеся n-1 строк матрицы */
    for (int i = 1; i < n; i++) {
        if (!fgets(line, sizeof(line), stdin)) {
            printf("0\n");
            return 0;
        }

        int cnt = parseInts(line, temp, MAXN);
        if (cnt < n) {
            printf("0\n");
            return 0;
        }

        for (int j = 0; j < n; j++) {
            graph[i][j] = temp[j];
        }
    }

    /* 3) Читаем строку с двумя вершинами (1..n) */
    if (!fgets(line, sizeof(line), stdin)) {
        printf("0\n");
        return 0;
    }

    int uv[2];
    int got = parseInts(line, uv, 2);
    if (got < 2) {
        printf("0\n");
        return 0;
    }

    int start = uv[0] - 1;  /* перевод к 0-базовой индексации */
    int target = uv[1] - 1;

    if (start < 0 || start >= n || target < 0 || target >= n) {
        printf("0\n");
        return 0;
    }

    memset(visited, 0, sizeof(visited));
    dfs(start, target, 0);

    if (pathCount == 0) {
        printf("0\n");
        return 0;
    }

    qsort(paths, pathCount, sizeof(Path), pathCompare);

    for (int i = 0; i < pathCount; i++) {
        for (int j = 0; j < paths[i].len; j++) {
            printf("%d", paths[i].nodes[j] + 1); /* обратно к 1..n */
            if (j + 1 < paths[i].len) printf(",");
        }
        printf("\n");
    }

    return 0;
}






//Лабораторная работа "02. Модель процессора x86" . язык си . человек сам вводит входные данные .
//Пожелания: избегать ошибки Run - Time Check Failure #2 - Stack around the variable 'cmd' was corrupted.
//Реализовать структуру "процессор", которая содержит 32 - х битные регистры EAX, ECX, EDX, EIP,
//для реализации данных регистров(кроме EIP) использовать объединения,
//так что - бы возможно было обращаться к младшим 16 битам регистра(AX, CX, DX) 
//и к страшим и младшим 8 битами регистров AX, CX, DX(AH, AL, CH, CL, DH, DL).
//Рекомендуется использовать типы данных uint_32_t, uint_16_t и uint_8_t.
//Регистры EAX, ECX, EDX - целочисленные регистры общего назначения.
//Регистр EIP - счётчик инструкция, содержит номер следующей инструкции.
//Также необходимо реализовать набор арифметических и отладочных команд :
//1. mov - копирует значение.
//2. add - суммирует значения.
//3. sub - вычитает значения.
//4. shw - выводит в консоль значение регистра или константы в шестнадцатеричном виде.
//команды mov, add, sub работают со всеми регистрами кроме EIP и константами, но константы только во втором аргументе.
//Примеры использования команд с описанием :
//mov EAX, 0xf35d # копирование значения 0xf35d в регистре EAX(EAX = 0xf35d)
//mov CL, AH # копирование значения из регистра AH в регистре CL(CL = AH)
//add DX, 0x2 # суммирование значениия регистра DX и занчения 0x2 (DX = DX + 0x2)
//add EDX, DX # суммирование значениия регистра EDX и занчения DX(EDX = EDX + DX)
//sub CX, 0xff # вычитание значениия 0xff из регистра занчения CX(CX = CX - 0xff)
//sub EAX, ECX # вычитание значениия регистра ECX из регистра занчения EAX(EAX = EAX - ECX)
//shw EIP # выводит в консоль значение регистра EIP
//shw DX # выводит в консоль значение регистра DX
//Программа читает данные команды из файла commands.txt и выполняет на модели процессора,
//в стандартный поток вывода(stdout) выводит значения команды shw.
//Примеры ввода(текст в файле commands.txt) и вывода :
//Ввод:
//shw EIP
//mov EAX 0xffffffff
//shw EAX
//add AH 0x1
//shw EAX
//mov EDX 0x1234
//sub EDX EDX
//shw EDX
//mov DX EAX
//shw EDX
//shw EIP
//Вывод :
//0x1
//0xffffffff
//0xffff00ff
//0x0
//0xff
//0xb






#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
// Объединение для представления 32-битного регистра (например, EAX)
// Позволяет обращаться к частям регистра: 32 бита, 16 бит (AX) или 8 бит (AL/AH)
typedef union {
    uint32_t dword;   // Весь регистр (32 бита)
    struct {
        uint16_t lo;  // Младшие 16 бит
        uint16_t hi;  // Старшие 16 бит
    } w;
    struct {
        uint8_t lo;   // Младшие 8 бит (напр. AL)
        uint8_t hi;   // Следующие 8 бит (напр. AH)
        uint16_t hi16;
    } b;
} reg32_u;
// Структура состояния процессора
typedef struct {
    reg32_u eax;
    reg32_u ecx;
    reg32_u edx;
    uint32_t eip; // Указатель на текущую инструкцию
} cpu_t;
// Перечисление типов операндов
typedef enum {
    OP_NONE,
    OP_REG8,
    OP_REG16,
    OP_REG32,
    OP_IMM     // Константа (Immediate value)
} op_type_t;
// Структура для универсальной работы с операндом (регистр или число)
typedef struct {
    op_type_t type;
    uint8_t* r8;    // Указатель на 8-битный сегмент
    uint16_t* r16;  // Указатель на 16-битный сегмент
    uint32_t* r32;  // Указатель на 32-битный сегмент
    uint32_t imm;   // Значение, если это константа
} operand_t;
// Чтение значения из операнда в зависимости от его типа
static uint32_t read_operand(const operand_t* op) {
    switch (op->type) {
    case OP_REG8:  return (uint32_t)(*op->r8);
    case OP_REG16: return (uint32_t)(*op->r16);
    case OP_REG32: return *op->r32;
    case OP_IMM:   return op->imm;
    default:       return 0;
    }
}
// Запись значения в операнд (только если это регистр)
static void write_operand(operand_t* op, uint32_t value) {
    switch (op->type) {
    case OP_REG8:  *op->r8 = (uint8_t)value;  break;
    case OP_REG16: *op->r16 = (uint16_t)value; break;
    case OP_REG32: *op->r32 = value;           break;
    default: break; // В константу писать нельзя
    }
}
// Функция распознавания имени регистра из строки
static int parse_register(const char* name, cpu_t* cpu, operand_t* op, int allow_eip) {
    char buf[8];
    size_t len = strlen(name);
    if (len == 0 || len >= sizeof(buf)) return -1;
    // Приведение к верхнему регистру для сравнения
    for (size_t i = 0; i <= len; ++i)
        buf[i] = (char)toupper((unsigned char)name[i]);
    op->type = OP_NONE;
    // Логика маппинга строк на конкретные адреса в структуре cpu_t
    if (strcmp(buf, "EAX") == 0) { op->type = OP_REG32; op->r32 = &cpu->eax.dword; }
    else if (strcmp(buf, "ECX") == 0) { op->type = OP_REG32; op->r32 = &cpu->ecx.dword; }
    else if (strcmp(buf, "EDX") == 0) { op->type = OP_REG32; op->r32 = &cpu->edx.dword; }
    else if (strcmp(buf, "EIP") == 0) {
        if (!allow_eip) return -1;
        op->type = OP_REG32; op->r32 = &cpu->eip;
    }
    else if (strcmp(buf, "AX") == 0) { op->type = OP_REG16; op->r16 = &cpu->eax.w.lo; }
    else if (strcmp(buf, "CX") == 0) { op->type = OP_REG16; op->r16 = &cpu->ecx.w.lo; }
    else if (strcmp(buf, "DX") == 0) { op->type = OP_REG16; op->r16 = &cpu->edx.w.lo; }
    else if (strcmp(buf, "AL") == 0) { op->type = OP_REG8;  op->r8 = &cpu->eax.b.lo; }
    else if (strcmp(buf, "AH") == 0) { op->type = OP_REG8;  op->r8 = &cpu->eax.b.hi; }
    else if (strcmp(buf, "CL") == 0) { op->type = OP_REG8;  op->r8 = &cpu->ecx.b.lo; }
    else if (strcmp(buf, "CH") == 0) { op->type = OP_REG8;  op->r8 = &cpu->ecx.b.hi; }
    else if (strcmp(buf, "DL") == 0) { op->type = OP_REG8;  op->r8 = &cpu->edx.b.lo; }
    else if (strcmp(buf, "DH") == 0) { op->type = OP_REG8;  op->r8 = &cpu->edx.b.hi; }
    else return -1;
    return 0;
}
// Преобразование строки в число (поддерживает 0x для hex)
static int parse_imm(const char* s, uint32_t* out) {
    char* endptr;
    unsigned long val = strtoul(s, &endptr, 0);
    if (s == endptr) return -1;
    *out = (uint32_t)val;
    return 0;
}
int main(void) {
    cpu_t cpu; FILE* f; char line[128];
    memset(&cpu, 0, sizeof(cpu)); // Обнуляем регистры
    f = fopen("commands.txt", "r");
    if (!f) {fprintf(stderr, "Cannot open commands.txt\n"); return 1;}
    // Построчное чтение файла с командами
    while (fgets(line, sizeof(line), f)) {
        char *p = line, *op_str, *arg1_str, *arg2_str;
        operand_t dst, src; uint32_t val;
        line[strcspn(line, "\r\n")] = '\0'; // Удаление символа переноса строки
        while (isspace((unsigned char)*p)) ++p; // Пропуск пробелов в начале
        if (*p == '\0' || *p == '#') continue;  // Пропуск пустых строк и комментариев
        cpu.eip++; // Имитация продвижения по коду
        // Разбивка строки на токены: команда и два аргумента
        op_str = strtok(p, " \t");
        if (!op_str) continue;
        arg1_str = strtok(NULL, " \t,"); arg2_str = strtok(NULL, " \t,");
        // Игнорирование комментариев в конце строки
        if (arg1_str && arg1_str[0] == '#') arg1_str = NULL;
        if (arg2_str && arg2_str[0] == '#') arg2_str = NULL;
        for (char* t = op_str; *t; ++t) *t = (char)tolower((unsigned char)*t);
        // Обработка команды SHW (вывод значения)
        if (strcmp(op_str, "shw") == 0) {
            if (!arg1_str) continue;
            if (parse_register(arg1_str, &cpu, &dst, 1) == 0) val = read_operand(&dst);
            else if (parse_imm(arg1_str, &val) != 0) continue;
            printf("0x%x\n", val);
        }
        // Обработка арифметики и пересылки (MOV, ADD, SUB)
        else if (strcmp(op_str, "mov") == 0 || strcmp(op_str, "add") == 0 || strcmp(op_str, "sub") == 0) {
            int is_add = (strcmp(op_str, "add") == 0);
            int is_sub = (strcmp(op_str, "sub") == 0);
            if (!arg1_str || !arg2_str) continue;
            // Первый аргумент (куда пишем) обязан быть регистром
            if (parse_register(arg1_str, &cpu, &dst, 0) != 0) continue;
            // Второй аргумент может быть либо регистром, либо числом
            if (parse_register(arg2_str, &cpu, &src, 0) != 0) {
                if (parse_imm(arg2_str, &val) != 0) continue;
                src.type = OP_IMM;
                src.imm = val;
            }
            if (!is_add && !is_sub) { // MOV
                write_operand(&dst, read_operand(&src));
            } else { // ADD / SUB
                uint32_t a = read_operand(&dst);
                uint32_t b = read_operand(&src);
                write_operand(&dst, is_add ? (a + b) : (a - b));
            }
        }
    }
    fclose(f); return 0;
}







//Тема "4. Рекурсия". Лабораторная работа "4.1. Расстановка шахматных фигур" . язык си . человек сам вводит входные данные .
//Пожелания: исправить 4.1.c(5) : fatal error C1083 : 
//Cannot open include file : 'stdbool.h' : No such file or directory.
//Дана квадратная шахматная доска размером N x N.На доске уже размещено K фигур.Фигуры размещены так, 
//что находятся не под боем друг друга.
//Необходимо расставить на доске еще L фигур так, 
//чтобы никакая из фигур на доске не находилась под боем любой другой фигуры.Необходимо найти все возможные решения.
//Входные данные : файл input.txt.На первой строке файла записаны три числа : N L K(через пробел).
//Далее следует K строк, содержащих числа x и y(через пробел) - координаты уже стоящей на доске фигуры.
//Координаты отсчитываются от 0 до N - 1. 1 <= N <= 24.
//Выходные данные : файл output.txt.На каждое найденное решение необходимо записать в файл одну строку.
//Строка состоит из пар(x, y) - координаты фигур на доске.В решение следует вывести координаты всех фигур, 
//находящихся на доске.Каждую фигуру необходимо записать в виде пары координат, разделенных запятой и обрамленных скобками.
//Координаты отсчитываются от 0 до N - 1. Порядок, в котором фигуры перечислены в решении, не имеет значения.
//Порядок перечисления решений не имеет значения.Выводимые решения не должны содержать повторы, 
//т.е.каждое найденное решение необходимо вывести только один раз.
//Если не было найдено ни одного решения, в файл необходимо записать no solutions.
//Ваша фигура - магараджа.Фигура объединяет в себе возможности ферзя и коня, 
//т.е.под боем оказываются все вертикали, горизонтали, диагонали и клетки(+-1, +-2), (+-2, +-1) от фигуры.
//Пример 1 :
//input.txt :
//1 1 0
//output.txt :
//(0, 0)
//Пример 2 :
//input.txt :
//2 1 0
//output.txt :
//(0, 0)
//(1, 0)
//(0, 1)
//(1, 1)
//Пример 3 :
//input.txt :
//3 1 0
//output.txt :
//(0, 0)
//(1, 0)
//(2, 0)
//(0, 1)
//(0, 2)
//(1, 1)
//(1, 2)
//(2, 1)
//(2, 2)
//Пример 4:
//input.txt :
//4 2 0
//output.txt :
//(0, 0) (1, 3)
//(0, 0) (2, 3)
//(0, 0) (3, 1)
//(0, 0) (3, 2)
//(0, 3) (1, 0)
//(1, 0) (2, 3)
//(1, 0) (3, 3)
//(0, 3) (2, 0)
//(1, 3) (2, 0)
//(2, 0) (3, 3)
//(0, 1) (3, 0)
//(0, 2) (3, 0)
//(1, 3) (3, 0)
//(2, 3) (3, 0)
//(0, 1) (3, 2)
//(0, 1) (3, 3)
//(0, 2) (3, 3)
//(0, 2) (3, 1)
//(0, 3) (3, 1)
//(0, 3) (3, 2)
//Пример 5:
//input.txt :
//5 3 1
//1 0
//output.txt :
//(0, 3) (1, 0) (3, 4) (4, 1)






#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_N 24
typedef int my_bool; //error 
#define true 1
#define false 0
typedef struct {int x;int y;} Point;
//size  +fig  fig alredy
int N; int L; int K;       
//orig fig            //mask of boat: true - busy cell, false - free cell
Point initial[MAX_N]; my_bool board[MAX_N][MAX_N];
//(all fig: orig + new)   
Point solution[MAX_N];   int solution_count = 0;  FILE* output_file;
//knight=horse 
const int knight_dx[] = { -2, -1, 1, 2, 2, 1, -1, -2 };
const int knight_dy[] = { 1, 2, 2, 1, -1, -2, -2, -1 };
//queen=ferz 
const int queen_dx[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
const int queen_dy[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
//chek point on boat 
my_bool is_valid(int x, int y) {
    return x >= 0 && x < N && y >= 0 && y < N;
}
//from (x, y)
void attack_from(int x, int y, my_bool temp_board[MAX_N][MAX_N], my_bool add) {
    //knight attack
    for (int i = 0; i < 8; i++) {
        int nx = x + knight_dx[i]; int ny = y + knight_dy[i];
        if (is_valid(nx, ny)) temp_board[nx][ny] = temp_board[nx][ny] || add;
    }
    //queen attack
    for (int dir = 0; dir < 8; dir++) {
        for (int step = 1; step < N; step++) {
            int nx = x + queen_dx[dir] * step; int ny = y + queen_dy[dir] * step;
            if (!is_valid(nx, ny)) break;
            temp_board[nx][ny] = temp_board[nx][ny] || add;
        }
    }
    //this cell is busy too 
    temp_board[x][y] = temp_board[x][y] || add;
}
void init_board() {
    //clean boat
    memset(board, 0, sizeof(board));
    //add orig fig
    for (int i = 0; i < K; i++) {
        int x = initial[i].x; int y = initial[i].y;
        if (board[x][y]) {printf("Initial pieces attack each other\n");exit(1);}
        //add fig and attack
        attack_from(x, y, board, true);
    }
}
//check
my_bool can_place(int x, int y) {
    return !board[x][y]; 
}
//recurs
void place_pieces(int placed, int start_x, int start_y) {
    if (placed == L) {
        for (int i = 0; i < K + L; i++) {
            fprintf(output_file, "(%d,%d)", solution[i].x, solution[i].y);
            if (i < K + L - 1) fprintf(output_file, " ");
        }
        fprintf(output_file, "\n"); solution_count++;
        return;
    }
    //begin (start_x, start_y), enum all cells
    for (int x = start_x; x < N; x++) {
        int y_start = (x == start_x) ? start_y : 0;
        for (int y = y_start; y < N; y++) {
            //check
            if (can_place(x, y)) {
                //temp copy of boat 
                my_bool temp_board[MAX_N][MAX_N];
                memcpy(temp_board, board, sizeof(board));
                //plus new fig 
                attack_from(x, y, board, true);
                //save fig in solution 
                solution[K + placed].x = x;
                solution[K + placed].y = y;
                // recurse add next figs 
                if (y + 1 < N) place_pieces(placed + 1, x, y + 1);
                else place_pieces(placed + 1, x + 1, 0);
                //hill boat (restore)
                memcpy(board, temp_board, sizeof(board));
            }
        }
    }
}
int main() {
    FILE* input_file = fopen("input.txt", "r");
    if (!input_file) {printf("Cannot open input.txt\n");return 1;}
    fscanf(input_file, "%d %d %d", &N, &L, &K);
    if (N < 1 || N > MAX_N || K < 0 || K > N * N || L < 0 || L > N * N - K) {
        printf("Invalid input parameters\n");
        fclose(input_file); return 1;
    }
    //read orig fig
    for (int i = 0; i < K; i++) {
        fscanf(input_file, "%d %d", &initial[i].x, &initial[i].y);
        if (initial[i].x < 0 || initial[i].x >= N || initial[i].y < 0 || initial[i].y >= N) {
            printf("Invalid piece coordinates\n");
            fclose(input_file);
            return 1;
        }
        solution[i] = initial[i];
    }
    fclose(input_file);
    init_board();
    output_file = fopen("output.txt", "w");
    if (!output_file) {printf("Cannot open output.txt\n");return 1;}
    //start recurse search 
    if (L == 0) {
        //if not necessary add figs, print start config 
        if (K > 0) {
            for (int i = 0; i < K; i++) {
                fprintf(output_file, "(%d,%d)", solution[i].x, solution[i].y);
                if (i < K - 1) {
                    fprintf(output_file, " ");
                }
            }
            fprintf(output_file, "\n");
            solution_count++;
        }
        else {
            //nothing on board 
        }
    }
    else place_pieces(0, 0, 0);
    if (solution_count == 0) fprintf(output_file, "no_solutions\n");
    fclose(output_file);
    return 0;
}






// Лабораторная работа "07. Кузнечик" . язык си . человек сам вводит входные данные . 
// Кузнечик стоит в клетке 1 и хочет попасть в клетку n.
// За один прыжок он может перейти только в одну из трех следующих клеток:
// - i + 1
// - i + 2
// - i + 3
// Для каждой клетки задана стоимость приземления, а также некоторые клетки запрещены для посещения (их стоимость равна -1).
// Необходимо:
// 1. Найти минимальную суммарную стоимость пути из клетки 1 в клетку n.
// 2. Найти количество различных путей с минимальной стоимостью.
// Если добраться до клетки n нельзя, то вывести -1.
// Формат входных данных
// В первой строке записано одно целое число n - количество клеток.
// Во второй строке записано n целых чисел:
// a1 a2 ... an, - стоимости посещений клеток.
// Если добраться до клетки n невозможно, вывести:
// -1
// Иначе вывести:
// 1. В первой строке минимальную стоимость.
// 2. Во второй строке количество путей с минимальной стоимостью.
// Примеры: 
// Ввод1	
// 5
// 0 1 4 5 2	
// Вывод1
// 3
// 1
// Ввод2	
// 6
// 0 2 -1 -1 -1 1	
// Вывод2
// -1
// Ввод3	
// 5
// 0 1 1 1 1	
// Вывод3
// 2
// 3

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(void) {
    int n;
    if (scanf("%d", &n) != 1 || n <= 0) {
        return 0;
    }

    long long *a = (long long *)malloc((n + 1) * sizeof(long long));
    long long *minCost = (long long *)malloc((n + 1) * sizeof(long long));
    long long *ways = (long long *)malloc((n + 1) * sizeof(long long));

    if (!a || !minCost || !ways) {
        free(a);
        free(minCost);
        free(ways);
        return 0;
    }

    for (int i = 1; i <= n; i++) {
        scanf("%lld", &a[i]);
    }

    const long long INF = LLONG_MAX / 4;

    for (int i = 1; i <= n; i++) {
        minCost[i] = INF;
        ways[i] = 0;
    }

    // Старт: уже в клетке 1
    minCost[1] = 0;
    ways[1] = 1;

    for (int i = 2; i <= n; i++) {
        if (a[i] == -1) {
            continue; // запрещенная клетка
        }

        for (int d = 1; d <= 3; d++) {
            int p = i - d;
            if (p < 1) continue;
            if (minCost[p] == INF) continue; // до p нельзя добраться

            long long candidate = minCost[p] + a[i];

            if (candidate < minCost[i]) {
                minCost[i] = candidate;
                ways[i] = ways[p];
            } else if (candidate == minCost[i]) {
                ways[i] += ways[p];
            }
        }
    }

    if (minCost[n] == INF) {
        printf("-1\n");
    } else {
        printf("%lld\n", minCost[n]);
        printf("%lld\n", ways[n]);
    }

    free(a);
    free(minCost);
    free(ways);
    return 0;
}






// Лабораторная работа "13. Анализ деревьев" . язык си . человек сам вводит входные данные . 
// Написать программу, на вход которой подается представление бинарного дерева в виде списка значений узлов в порядке обхода в ширину, начиная с корня. На основе списка значений узлов программа должна построить бинарное дерево и выполнить ряд операций в соответствии с выданным вариантом.
// Применение рекурсии в данной работе является обязательным условием!
// Рассмотрим совершенное бинарное дерево №1:
//     __1__
//    /     \
//   2       3
//  / \     / \
// 4   5   6   7
// Представление бинарного дерева №1 в виде списка значений узлов в порядке обхода в ширину, начиная с корня следующее:
// [1, 2, 3, 4, 5, 6, 7]
// Рассмотрим бинарное дерево №2, полученное путем удаления поддерева [2, 4, 5] из дерева №1:
// 1__
//    \
//     3
//    / \
//   6   7
// Для осуществления обхода в ширину необходимо перечислить значения узлов на каждом уровне слева на право включая отсутствующие узлы. Для обозначения отсутствия узла используется значение NULL. Пример бинарного дерева №2 с метками NULL следующий:
//         _______1__
//        /          \
//    __NULL_         3
//   /       \       / \
// NULL      NULL   6   7
// Представление бинарного дерева №2 в виде списка значений узлов в порядке обхода в ширину, начиная с корня следующее:
// [1, NULL, 3, NULL, NULL, 6, 7]
// Рассмотрим бинарное дерево №3, полученное путем удаления узла 7 из дерева №2:
// 1__
//    \
//     3
//    /
//   6
// Последние отсутствующие листья (находящиеся справа) на последнем уровне не отмечаются значением NULL, таким образом пример бинарного дерева №3 с метками NULL будет следующий:
//         _______1__
//        /          \
//    __NULL_         3
//   /       \       /
// NULL      NULL   6
// Представление бинарного дерева №3 в виде списка значений узлов в порядке обхода в ширину, начиная с корня следующее:
// [1, NULL, 3, NULL, NULL, 6]
// Информация к вашему варианту задания:
// Помимо представления бинарного дерева на вход программе подается число N, которое используется во время анализа дерева. Программа должна вывести следующую информацию о построенном бинарном дереве:
// - высота дерева;
// - количество узлов;
// - количество листьев;
// - постфиксный (post-order) обход дерева;
// - количество поддеревьев, сумма элементов которых меньше поданного на вход числа N;
// - поддеревья, сумма элементов которых меньше указанного числа N.
// Пример входных данных:
// [4, 2, 10, 1, 3, 8, 12, 0, NULL, NULL, NULL, 7, 9, 11, 14]
// N 10
// Пример выходных данных:
// Tree height: 3
// Tree size: 12
// Leaf count: 6
// Post-order traversal: [0, 1, 3, 2, 7, 9, 8, 11, 14, 12, 10, 4]
// The subtrees: 6
// [0]
// [1, 0]
// [2, 1, 3, 0]
// [3]
// [7]
// [9]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 1000
#define INF -1234567 // Метка для NULL

typedef struct Node {
    int data;
    struct Node *left;
    struct Node *right;
} Node;

// --- Базовые функции дерева ---

Node* createNode(int value) {
    if (value == INF) return NULL;
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = value;
    newNode->left = newNode->right = NULL;
    return newNode;
}

// Построение дерева из массива (BFS-структура)
Node* buildTree(int arr[], int n, int i) {
    if (i >= n || arr[i] == INF) return NULL;
    Node* root = createNode(arr[i]);
    root->left = buildTree(arr, n, 2 * i + 1);
    root->right = buildTree(arr, n, 2 * i + 2);
    return root;
}

int getHeight(Node* root) {
    if (!root) return -1;
    int l = getHeight(root->left);
    int r = getHeight(root->right);
    return (l > r ? l : r) + 1;
}

int getSize(Node* root) {
    if (!root) return 0;
    return 1 + getSize(root->left) + getSize(root->right);
}

int countLeaves(Node* root) {
    if (!root) return 0;
    if (!root->left && !root->right) return 1;
    return countLeaves(root->left) + countLeaves(root->right);
}

void postOrder(Node* root, int* first) {
    if (!root) return;
    postOrder(root->left, first);
    postOrder(root->right, first);
    if (!(*first)) printf(", ");
    printf("%d", root->data);
    *first = 0;
}

int getSum(Node* root) {
    if (!root) return 0;
    return root->data + getSum(root->left) + getSum(root->right);
}

// --- Вывод поддерева в формате BFS списка ---

void printSubtreeBFS(Node* root) {
    if (!root) return;
    Node* queue[MAX_NODES];
    int values[MAX_NODES];
    int is_null[MAX_NODES];
    int head = 0, tail = 0;
    
    queue[tail++] = root;
    int count = 0;
    int last_valid = 0;

    while (head < tail && count < MAX_NODES) {
        Node* curr = queue[head++];
        if (curr) {
            values[count] = curr->data;
            is_null[count] = 0;
            last_valid = count;
            queue[tail++] = curr->left;
            queue[tail++] = curr->right;
        } else {
            is_null[count] = 1;
        }
        count++;
    }

    printf("[");
    for (int i = 0; i <= last_valid; i++) {
        if (is_null[i]) printf("NULL");
        else printf("%d", values[i]);
        if (i < last_valid) printf(", ");
    }
    printf("]\n");
}

// Поиск и анализ поддеревьев (In-order traversal для порядка как в примере)
void findSubtrees(Node* root, int N, int* count, int mode) {
    if (!root) return;

    // Сначала идем вглубь влево
    findSubtrees(root->left, N, count, mode);

    // Проверяем текущий узел (корень поддерева)
    if (getSum(root) < N) {
        if (mode == 0) (*count)++;
        else printSubtreeBFS(root);
    }

    // Затем идем вправо
    findSubtrees(root->right, N, count, mode);
}

// --- Парсинг ввода ---

int main() {
    char buf[1000];
    int arr[MAX_NODES];
    int n = 0, N_val;

    // Считывание массива
    if (fgets(buf, sizeof(buf), stdin)) {
        char* token = strtok(buf, " [],\n");
        while (token) {
            if (strcmp(token, "NULL") == 0) arr[n++] = INF;
            else arr[n++] = atoi(token);
            token = strtok(NULL, " [],\n");
        }
    }

    // Считывание N (игнорируя букву N если она есть)
    char n_pref[2];
    if (scanf("%s %d", n_pref, &N_val) != 2) {
        // Если ввели просто число
        N_val = atoi(n_pref);
    }

    Node* root = buildTree(arr, n, 0);

    // Вывод согласно формату
    printf("Tree height: %d\n", getHeight(root));
    printf("Tree size: %d\n", getSize(root));
    printf("Leaf count: %d\n", countLeaves(root));
    
    printf("Post-order traversal: [");
    int first = 1;
    postOrder(root, &first);
    printf("]\n");

    int subtreeCount = 0;
    findSubtrees(root, N_val, &subtreeCount, 0); // Первый проход: считаем
    printf("The subtrees: %d\n", subtreeCount);
    findSubtrees(root, N_val, NULL, 1);          // Второй проход: печатаем

    return 0;
}






// Написать программу, на вход которой подается представление бинарного дерева в виде списка значений узлов в порядке обхода в ширину, начиная с корня. На основе списка значений узлов программа должна построить бинарное дерево и вывести его в консоль.
// Данная работа является творческой, вы можете предложить свой вариант визуализации, схожий с представленным в примерах.
// Пример ввода №1:
// [1, 2, 3, 4, 5, 6, 7]
// Пример вывода №1:
//     __1__
//    /     \
//   2       3
//  / \     / \
// 4   5   6   7
// Пример ввода №2:
// [1, NULL, 3, NULL, NULL, 6, 7]
// Пример вывода №2:
// 1__
//    \
//     3
//    / \
//   6   7

// Лабораторная работа "Визуализация деревьев" . язык си . человек сам вводит входные данные .
// Написать программу, на вход которой подается представление бинарного дерева в виде списка значений узлов в порядке обхода в ширину, начиная с корня. На основе списка значений узлов программа должна построить бинарное дерево и вывести его в консоль.
// Данная работа является творческой, вы можете предложить свой вариант визуализации, схожий с представленным в примерах.
// Пример ввода №1:
// [1, 2, 3, 4, 5, 6, 7]
// Пример вывода №1:
//     __1__
//    /     \
//   2       3
//  / \     / \
// 4   5   6   7
// Пример ввода №2:
// [1, NULL, 3, NULL, NULL, 6, 7]
// Пример вывода №2:
// 1__
//    \
//     3
//    / \
//   6   7
// должны поддерживаться большие массивы и большие числа, представление при этом компактное. 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* Для старых MSVC */
#ifdef _MSC_VER
#if _MSC_VER < 1900
#define snprintf _snprintf
#endif
#endif

#define INITIAL_BUF 4096
#define MAX_CANVAS_WIDTH 180  /* ограничение ширины для компактного вывода */

typedef struct Node {
    long long val;
    struct Node *left;
    struct Node *right;
} Node;

/* ---------- Утилиты ---------- */

static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    {
        char *e = s + strlen(s) - 1;
        while (e > s && isspace((unsigned char)*e)) e--;
        e[1] = '\0';
    }
    return s;
}

static int is_null_token(const char *s) {
    return strcmp(s, "NULL") == 0 || strcmp(s, "null") == 0;
}

static Node *new_node(long long v) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) {
        fprintf(stderr, "Ошибка: не хватает памяти.\n");
        exit(1);
    }
    n->val = v;
    n->left = n->right = NULL;
    return n;
}

static void free_tree(Node *root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

static int tree_height(Node *root) {
    if (!root) return 0;
    {
        int hl = tree_height(root->left);
        int hr = tree_height(root->right);
        return (hl > hr ? hl : hr) + 1;
    }
}

/* Совместимый парсинг long long для старого MSVC */
static int parse_ll(const char *s, long long *out) {
    char *endp = NULL;

#if defined(_MSC_VER)
    __int64 v = _strtoi64(s, &endp, 10);
#else
    long long v = strtoll(s, &endp, 10);
#endif

    if (s == endp) return 0; /* не было цифр */

    while (*endp && isspace((unsigned char)*endp)) endp++;
    if (*endp != '\0') return 0; /* лишние символы */

    *out = (long long)v;
    return 1;
}

/* ---------- Динамические массивы парсинга ---------- */

typedef struct {
    long long *values;
    unsigned char *is_null;
    size_t size;
    size_t cap;
} Parsed;

static void parsed_init(Parsed *p) {
    p->values = NULL;
    p->is_null = NULL;
    p->size = 0;
    p->cap = 0;
}

static void parsed_push(Parsed *p, long long v, unsigned char nul) {
    if (p->size == p->cap) {
        size_t ncap = (p->cap == 0) ? 16 : p->cap * 2;
        long long *nv = (long long *)realloc(p->values, ncap * sizeof(long long));
        unsigned char *nn = (unsigned char *)realloc(p->is_null, ncap * sizeof(unsigned char));
        if (!nv || !nn) {
            free(nv);
            free(nn);
            fprintf(stderr, "Ошибка памяти.\n");
            exit(1);
        }
        p->values = nv;
        p->is_null = nn;
        p->cap = ncap;
    }
    p->values[p->size] = v;
    p->is_null[p->size] = nul;
    p->size++;
}

static void parsed_free(Parsed *p) {
    free(p->values);
    free(p->is_null);
}

/* ---------- Рисование на полотне ---------- */

static void put_str(char **canvas, int rows, int cols, int r, int c, const char *s) {
    int len;
    int start;
    int i;

    if (r < 0 || r >= rows) return;
    len = (int)strlen(s);

    start = c - len / 2;
    if (start < 0) start = 0;
    if (start + len > cols) start = cols - len;
    if (start < 0) return;

    for (i = 0; i < len; i++) {
        int cc = start + i;
        if (cc >= 0 && cc < cols) canvas[r][cc] = s[i];
    }
}

static void draw_tree(Node *node, char **canvas, int rows, int cols, int r, int left, int right, int min_gap) {
    int mid, next_row, span, half;
    char buf[64];

    if (!node || r >= rows || left > right) return;

    mid = (left + right) / 2;
    snprintf(buf, sizeof(buf), "%lld", node->val);
    put_str(canvas, rows, cols, r, mid, buf);

    next_row = r + 2;
    if (next_row >= rows) return;

    span = right - left + 1;
    half = span / 2;
    if (half < min_gap) half = min_gap;

    if (node->left) {
        int lmid = mid - half / 2;
        int bcol, x;

        if (lmid < left) lmid = left;

        /* Слеш прямо над левым ребенком */
        bcol = lmid;

        /* Горизонтальная линия '_' от ребенка к родителю */
        for (x = lmid + 1; x < mid; x++) {
            if (r >= 0 && r < rows && x >= 0 && x < cols && canvas[r][x] == ' ')
                canvas[r][x] = '_';
        }

        if (r + 1 < rows && bcol >= 0 && bcol < cols) canvas[r + 1][bcol] = '/';
        draw_tree(node->left, canvas, rows, cols, next_row, left, mid - 1, min_gap);
    }

    if (node->right) {
        int rmid = mid + half / 2;
        int bcol, x;

        if (rmid > right) rmid = right;

        /* Бэкслеш прямо над правым ребенком */
        bcol = rmid;

        /* Горизонтальная линия '_' от родителя к ребенку */
        for (x = mid + 1; x < rmid; x++) {
            if (r >= 0 && r < rows && x >= 0 && x < cols && canvas[r][x] == ' ')
                canvas[r][x] = '_';
        }

        if (r + 1 < rows && bcol >= 0 && bcol < cols) canvas[r + 1][bcol] = '\\';
        draw_tree(node->right, canvas, rows, cols, next_row, mid + 1, right, min_gap);
    }
}

static void print_canvas(char **canvas, int rows, int cols) {
    int r;
    for (r = 0; r < rows; r++) {
        int end = cols - 1;
        while (end >= 0 && canvas[r][end] == ' ') end--;
        if (end < 0) {
            printf("\n");
        } else {
            canvas[r][end + 1] = '\0';
            printf("%s\n", canvas[r]);
        }
    }
}

/* ---------- main ---------- */

int main(void) {
    char *input = (char *)malloc(INITIAL_BUF);
    char *s;
    size_t len;
    Parsed p;
    char *token;
    Node **nodes;
    Node *root;
    int h;
    int rows;
    int cols;
    char **canvas;
    int r;

    if (!input) {
        fprintf(stderr, "Ошибка памяти.\n");
        return 1;
    }

    printf("Введите список в формате [1, 2, 3, NULL, 5]:\n");
    if (!fgets(input, INITIAL_BUF, stdin)) {
        fprintf(stderr, "Ошибка чтения.\n");
        free(input);
        return 1;
    }

    s = trim(input);
    len = strlen(s);

    if (len < 2 || s[0] != '[' || s[len - 1] != ']') {
        fprintf(stderr, "Неверный формат. Ожидается [ ... ]\n");
        free(input);
        return 1;
    }

    s[len - 1] = '\0'; /* remove ']' */
    s++;               /* skip '[' */

    parsed_init(&p);

    token = strtok(s, ",");
    while (token) {
        char *t = trim(token);

        if (*t == '\0' || is_null_token(t)) {
            parsed_push(&p, 0, 1);
        } else {
            long long v;
            if (!parse_ll(t, &v)) {
                fprintf(stderr, "Некорректное число: %s\n", t);
                parsed_free(&p);
                free(input);
                return 1;
            }
            parsed_push(&p, v, 0);
        }

        token = strtok(NULL, ",");
    }

    if (p.size == 0 || p.is_null[0]) {
        printf("Пустое дерево.\n");
        parsed_free(&p);
        free(input);
        return 0;
    }

    nodes = (Node **)calloc(p.size, sizeof(Node *));
    if (!nodes) {
        fprintf(stderr, "Ошибка памяти.\n");
        parsed_free(&p);
        free(input);
        return 1;
    }

    {
        size_t i;
        for (i = 0; i < p.size; i++) {
            if (!p.is_null[i]) nodes[i] = new_node(p.values[i]);
        }

        for (i = 0; i < p.size; i++) {
            if (!nodes[i]) continue;
            {
                size_t li = 2 * i + 1;
                size_t ri = 2 * i + 2;
                if (li < p.size) nodes[i]->left = nodes[li];
                if (ri < p.size) nodes[i]->right = nodes[ri];
            }
        }
    }

    root = nodes[0];
    h = tree_height(root);

    rows = h * 2 - 1;
    if (rows < 1) rows = 1;

    /* Базовая ширина + ограничение для компактности */
    cols = 1 << (h + 1);
    if (cols < 40) cols = 40;
    if (cols > MAX_CANVAS_WIDTH) cols = MAX_CANVAS_WIDTH;

    canvas = (char **)malloc((size_t)rows * sizeof(char *));
    if (!canvas) {
        fprintf(stderr, "Ошибка памяти.\n");
        free_tree(root);
        free(nodes);
        parsed_free(&p);
        free(input);
        return 1;
    }

    for (r = 0; r < rows; r++) {
        canvas[r] = (char *)malloc((size_t)cols + 1);
        if (!canvas[r]) {
            int k;
            fprintf(stderr, "Ошибка памяти.\n");
            for (k = 0; k < r; k++) free(canvas[k]);
            free(canvas);
            free_tree(root);
            free(nodes);
            parsed_free(&p);
            free(input);
            return 1;
        }
        memset(canvas[r], ' ', (size_t)cols);
        canvas[r][cols] = '\0';
    }

    /* min_gap=3 -> чуть "воздушнее", подчеркивания выглядят лучше */
    draw_tree(root, canvas, rows, cols, 0, 0, cols - 1, 3);
    print_canvas(canvas, rows, cols);

    for (r = 0; r < rows; r++) free(canvas[r]);
    free(canvas);
    free_tree(root);
    free(nodes);
    parsed_free(&p);
    free(input);

    return 0;
}
