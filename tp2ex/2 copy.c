#define _CRT_SECURE_NO_WARNINGS  // отключает предупреждения MSVC про "небезопасные" scanf/fopen
#include <stdio.h>    // printf, fopen, fgets, fprintf
#include <stdint.h>   // uint8_t, uint16_t, uint32_t — фиксированные размеры
#include <string.h>   // strlen, strcmp, strtok, memset, strcspn
#include <stdlib.h>   // strtoul
#include <ctype.h>    // toupper, tolower, isspace

// Объединение для представления 32-битного регистра (например, EAX)
// Позволяет обращаться к частям регистра: 32 бита, 16 бит (AX) или 8 бит (AL/AH)
// union — все поля лежат в одной памяти (разные "взгляды" на те же биты)
typedef union {
    // Весь регистр (32 бита)
    uint32_t dword;   
    struct {
        // Младшие 16 бит   // Старшие 16 бит 
        uint16_t lo;        uint16_t hi;  
    } w;  // доступ как .w.lo / .w.hi
    struct {
        // Младшие 8 бит (напр. AL)  // Следующие 8 бит (напр. AH)
        uint8_t lo;   uint8_t hi;   uint16_t hi16;  // hi16 — старшие 16 бит (неиспользуемый "хвост")
    } b;  // доступ как .b.lo / .b.hi
} reg32_u;  // typedef ... reg32_u — имя типа вместо "union { ... }"

// Структура состояния процессора
// struct — набор полей рядом в памяти
typedef struct {
    reg32_u eax;    reg32_u ecx;  // регистры общего назначения
    reg32_u edx;    uint32_t eip; // Указатель на текущую инструкцию
} cpu_t;

// Перечисление типов операндов
// enum — именованные целые константы (OP_NONE=0, OP_REG8=1, ...)
typedef enum {
    OP_NONE, OP_REG8, OP_REG16,
    OP_REG32, OP_IMM // Константа (Immediate value)
} op_type_t;

// Структура для универсальной работы с операндом (регистр или число)
typedef struct {
    op_type_t type;     // какой вид операнда
    uint8_t* r8;    // Указатель на 8-битный сегмент
    uint16_t* r16;  // Указатель на 16-битный сегмент
    uint32_t* r32;  // Указатель на 32-битный сегмент
    uint32_t imm;   // Значение, если это константа
} operand_t;

// Чтение значения из операнда в зависимости от его типа
// static — видима только в этом .c файле; const — не меняем op
static uint32_t read_operand(const operand_t* op) {
    switch (op->type) {  // -> доступ к полю через указатель (эквивалент (*op).type)
    case OP_REG8:  return (uint32_t)(*op->r8);   // * — разыменование; (uint32_t) — расширение до 32 бит
    case OP_REG16: return (uint32_t)(*op->r16);
    case OP_REG32: return *op->r32;
    case OP_IMM:   return op->imm;
    default:       return 0;  // неизвестный тип
    }
}

// Запись значения в операнд (только если это регистр)
static void write_operand(operand_t* op, uint32_t value) {
    switch (op->type) {
    case OP_REG8:  *op->r8 = (uint8_t)value;  break;   // обрезка до 8 бит; break — выход из switch
    case OP_REG16: *op->r16 = (uint16_t)value; break;  // обрезка до 16 бит
    case OP_REG32: *op->r32 = value;           break;
    default: break; // В константу писать нельзя
    }
}

// Функция распознавания имени регистра из строки
// возвращает 0 при успехе, -1 при ошибке; allow_eip — разрешён ли EIP
static int parse_register(const char* name, cpu_t* cpu, operand_t* op, int allow_eip) {
    char buf[8];                    // буфер под имя в верхнем регистре
    size_t len = strlen(name);      // длина строки без '\0'
    if (len == 0 || len >= sizeof(buf)) return -1;  // пустое или слишком длинное имя
    // Приведение к верхнему регистру для сравнения
    // i <= len — копируем и завершающий '\0'
    for (size_t i = 0; i <= len; ++i)
        buf[i] = (char)toupper((unsigned char)name[i]);  // unsigned char — корректный вызов ctype
    op->type = OP_NONE;
    // Логика маппинга строк на конкретные адреса в структуре cpu_t
    // strcmp == 0 — строки равны; &cpu->eax.dword — адрес поля dword
    if (strcmp(buf, "EAX") == 0) { op->type = OP_REG32; op->r32 = &cpu->eax.dword; }
    else if (strcmp(buf, "ECX") == 0) { op->type = OP_REG32; op->r32 = &cpu->ecx.dword; }
    else if (strcmp(buf, "EDX") == 0) { op->type = OP_REG32; op->r32 = &cpu->edx.dword; }
    else if (strcmp(buf, "EIP") == 0) {
        if (!allow_eip) return -1;  // EIP запрещён в этой команде
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
    else return -1;  // неизвестное имя регистра
    return 0;        // успех
}

// Преобразование строки в число (поддерживает 0x для hex)
static int parse_imm(const char* s, uint32_t* out) {
    char* endptr;  // куда strtoul положит указатель на первый непрочитанный символ
    // base 0: авто — 0x... = hex, 0... = oct, иначе decimal
    unsigned long val = strtoul(s, &endptr, 0);
    if (s == endptr) return -1;  // ничего не разобрали — не число
    *out = (uint32_t)val;        // пишем результат по адресу out
    return 0;
}

int main(void) {
    cpu_t cpu; FILE* f; char line[128];  // состояние CPU, файл, буфер строки
    memset(&cpu, 0, sizeof(cpu)); // Обнуляем регистры
    f = fopen("commands.txt", "r");  // открыть файл на чтение
    if (!f) {fprintf(stderr, "Cannot open commands.txt\n"); return 1;}  // stderr — поток ошибок; код 1 = ошибка
    // Построчное чтение файла с командами
    while (fgets(line, sizeof(line), f)) {  // читает строку; NULL в конце файла → цикл стоп
        // p — курсор по строке; op_str/arg*_str — токены команды
        char *p = line, *op_str, *arg1_str, *arg2_str;
        operand_t dst, src; uint32_t val;  // приёмник, источник, временное число
        line[strcspn(line, "\r\n")] = '\0'; // Удаление символа переноса строки
        while (isspace((unsigned char)*p)) ++p; // Пропуск пробелов в начале
        if (*p == '\0' || *p == '#') continue;  // Пропуск пустых строк и комментариев
        cpu.eip++; // Имитация продвижения по коду
        // Разбивка строки на токены: команда и два аргумента
        op_str = strtok(p, " \t");  // первый токен (мнемоника); разделители — пробел/таб
        if (!op_str) continue;
        // NULL = продолжить разбор той же строки; ", " тоже разделители
        arg1_str = strtok(NULL, " \t,"); arg2_str = strtok(NULL, " \t,");
        // Игнорирование комментариев в конце строки
        if (arg1_str && arg1_str[0] == '#') arg1_str = NULL;  // && — короткое И (не лезем в NULL)
        if (arg2_str && arg2_str[0] == '#') arg2_str = NULL;
        // приводим мнемонику к нижнему регистру: *t — текущий символ; ++t — следующий
        for (char* t = op_str; *t; ++t) *t = (char)tolower((unsigned char)*t);
        // Обработка команды SHW (вывод значения)
        if (strcmp(op_str, "shw") == 0) {
            if (!arg1_str) continue;  // нужен хотя бы один аргумент
            // регистр → читаем; иначе пробуем число; иначе пропускаем строку
            if (parse_register(arg1_str, &cpu, &dst, 1) == 0) val = read_operand(&dst);
            else if (parse_imm(arg1_str, &val) != 0) continue;
            printf("0x%x\n", val);  // %x — hex без ведущих нулей
        }
        // Обработка арифметики и пересылки (MOV, ADD, SUB)
        else if (strcmp(op_str, "mov") == 0 || strcmp(op_str, "add") == 0 || strcmp(op_str, "sub") == 0) {
            int is_add = (strcmp(op_str, "add") == 0);  // 1 если ADD
            int is_sub = (strcmp(op_str, "sub") == 0);  // 1 если SUB
            if (!arg1_str || !arg2_str) continue;  // нужны оба операнда
            // Первый аргумент (куда пишем) обязан быть регистром
            if (parse_register(arg1_str, &cpu, &dst, 0) != 0) continue;  // 0 = EIP запрещён
            // Второй аргумент может быть либо регистром, либо числом
            if (parse_register(arg2_str, &cpu, &src, 0) != 0) {
                if (parse_imm(arg2_str, &val) != 0) continue;  // не регистр и не число
                src.type = OP_IMM;
                src.imm = val;
            }
            if (!is_add && !is_sub) { // MOV
                write_operand(&dst, read_operand(&src));  // dst := src
            } else { // ADD / SUB
                uint32_t a = read_operand(&dst);
                uint32_t b = read_operand(&src);
                // тернарный ?: — если is_add то a+b, иначе a-b
                write_operand(&dst, is_add ? (a + b) : (a - b));
            }
        }
    }
    fclose(f); return 0;  // закрыть файл; 0 = успешное завершение
}
