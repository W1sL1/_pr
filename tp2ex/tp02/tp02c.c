#define _CRT_SECURE_NO_WARNINGS // MSVC: не ругаться на fopen/strtok
#include <stdio.h> // printf, fprintf, fopen, fgets
#include <stdint.h> // uint8_t, uint16_t, uint32_t
#include <string.h> // memset, strlen, strcmp, strtok, strcspn
#include <stdlib.h> // strtoul
#include <ctype.h> // toupper, tolower, isspace
// Объединение для представления 32-битного регистра (например, EAX)
// Позволяет обращаться к частям регистра: 32 бита, 16 бит (AX) или 8 бит (AL/AH)
typedef union { // reg32_u: все поля делят одни и те же 4 байта
    // Весь регистр (32 бита)
    uint32_t dword;   
    struct {
        // Младшие 16 бит   // Старшие 16 бит 
        uint16_t lo;        uint16_t hi;  
    } w;
    struct {
        // Младшие 8 бит (напр. AL)  // Следующие 8 бит (напр. AH)
        uint8_t lo;   uint8_t hi;   uint16_t hi16; // hi16 — старшие 16 бит (не трогаем при AL/AH)
    } b;
} reg32_u;
// Структура состояния процессора
typedef struct { // cpu_t: состояние «процессора» между командами
    reg32_u eax;    reg32_u ecx; // 32-битные регистры с вложенными AX/AL…
    reg32_u edx;    uint32_t eip; // Указатель на текущую инструкцию
} cpu_t;
// Перечисление типов операндов
typedef enum { // как интерпретировать operand_t
    OP_NONE, OP_REG8, OP_REG16,
    OP_REG32, OP_IMM // Константа (Immediate value)
} op_type_t;
// Структура для универсальной работы с операндом (регистр или число)
typedef struct { // operand_t: указатель на ячейку или imm
    op_type_t type; // что именно читать/писать
    uint8_t* r8;    // Указатель на 8-битный сегмент
    uint16_t* r16;  // Указатель на 16-битный сегмент
    uint32_t* r32;  // Указатель на 32-битный сегмент
    uint32_t imm;   // Значение, если это константа
} operand_t;
// Чтение значения из операнда в зависимости от его типа
static uint32_t read_operand(const operand_t* op) { // унифицированное чтение в 32 бита
    switch (op->type) {
    case OP_REG8:  return (uint32_t)(*op->r8); // расширяем без знака
    case OP_REG16: return (uint32_t)(*op->r16);
    case OP_REG32: return *op->r32;
    case OP_IMM:   return op->imm; // константа из поля imm
    default:       return 0; // неизвестный тип — 0
    }
}
// Запись значения в операнд (только если это регистр)
static void write_operand(operand_t* op, uint32_t value) { // усечение по ширине регистра
    switch (op->type) {
    case OP_REG8:  *op->r8 = (uint8_t)value;  break; // только младший байт
    case OP_REG16: *op->r16 = (uint16_t)value; break;
    case OP_REG32: *op->r32 = value;           break;
    default: break; // В константу писать нельзя
    }
}
// Функция распознавания имени регистра из строки
static int parse_register(const char* name, cpu_t* cpu, operand_t* op, int allow_eip) { // 0 ок, -1 ошибка
    char buf[8]; // буфер для имени в верхнем регистре
    size_t len = strlen(name);
    if (len == 0 || len >= sizeof(buf)) return -1; // пусто или слишком длинно
    // Приведение к верхнему регистру для сравнения
    for (size_t i = 0; i <= len; ++i) // i<=len: копируем и завершающий '\0'
        buf[i] = (char)toupper((unsigned char)name[i]);
    op->type = OP_NONE; // сброс перед разбором
    // Логика маппинга строк на конкретные адреса в структуре cpu_t
    if (strcmp(buf, "EAX") == 0) { op->type = OP_REG32; op->r32 = &cpu->eax.dword; }
    else if (strcmp(buf, "ECX") == 0) { op->type = OP_REG32; op->r32 = &cpu->ecx.dword; }
    else if (strcmp(buf, "EDX") == 0) { op->type = OP_REG32; op->r32 = &cpu->edx.dword; }
    else if (strcmp(buf, "EIP") == 0) {
        if (!allow_eip) return -1; // EIP нельзя как приёмник mov/add/sub
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
    else return -1; // неизвестное имя
    return 0;
}
// Преобразование строки в число (поддерживает 0x для hex)
static int parse_imm(const char* s, uint32_t* out) { // 0 ок, -1 не число
    char* endptr; // куда остановился strtoul
    unsigned long val = strtoul(s, &endptr, 0); // base 0: 0x… hex, иначе dec
    if (s == endptr) return -1; // ни одной цифры не разобрано
    *out = (uint32_t)val;
    return 0;
}
int main(void) { // читает commands.txt, выполняет shw/mov/add/sub
    cpu_t cpu; FILE* f; char line[128];
    memset(&cpu, 0, sizeof(cpu)); // Обнуляем регистры
    f = fopen("commands.txt", "r"); // входной скрипт команд
    if (!f) {fprintf(stderr, "Cannot open commands.txt\n"); return 1;}
    // Построчное чтение файла с командами
    while (fgets(line, sizeof(line), f)) { // одна строка — одна «инструкция»
        char *p = line, *op_str, *arg1_str, *arg2_str; // указатели на токены
        operand_t dst, src; uint32_t val; // приёмник, источник, временное значение
        line[strcspn(line, "\r\n")] = '\0'; // Удаление символа переноса строки
        while (isspace((unsigned char)*p)) ++p; // Пропуск пробелов в начале
        if (*p == '\0' || *p == '#') continue;  // Пропуск пустых строк и комментариев
        cpu.eip++; // Имитация продвижения по коду
        // Разбивка строки на токены: команда и два аргумента
        op_str = strtok(p, " \t"); // первое слово — имя команды
        if (!op_str) continue;
        arg1_str = strtok(NULL, " \t,"); arg2_str = strtok(NULL, " \t,"); // аргументы через пробел/запятую
        // Игнорирование комментариев в конце строки
        if (arg1_str && arg1_str[0] == '#') arg1_str = NULL;
        if (arg2_str && arg2_str[0] == '#') arg2_str = NULL;
        for (char* t = op_str; *t; ++t) *t = (char)tolower((unsigned char)*t); // команда в нижнем регистре
        // Обработка команды SHW (вывод значения)
        if (strcmp(op_str, "shw") == 0) {
            if (!arg1_str) continue; // shw без аргумента — пропуск
            if (parse_register(arg1_str, &cpu, &dst, 1) == 0) val = read_operand(&dst); // регистр, EIP разрешён
            else if (parse_imm(arg1_str, &val) != 0) continue; // иначе попробовать число
            printf("0x%x\n", val); // вывод в hex
        }
        // Обработка арифметики и пересылки (MOV, ADD, SUB)
        else if (strcmp(op_str, "mov") == 0 || strcmp(op_str, "add") == 0 || strcmp(op_str, "sub") == 0) {
            int is_add = (strcmp(op_str, "add") == 0); // флаги вида операции
            int is_sub = (strcmp(op_str, "sub") == 0);
            if (!arg1_str || !arg2_str) continue; // нужны оба операнда
            // Первый аргумент (куда пишем) обязан быть регистром
            if (parse_register(arg1_str, &cpu, &dst, 0) != 0) continue;
            // Второй аргумент может быть либо регистром, либо числом
            if (parse_register(arg2_str, &cpu, &src, 0) != 0) {
                if (parse_imm(arg2_str, &val) != 0) continue; // не регистр и не число
                src.type = OP_IMM; // оформить как константу
                src.imm = val;
            }
            if (!is_add && !is_sub) { // MOV
                write_operand(&dst, read_operand(&src)); // dst := src
            } else { // ADD / SUB
                uint32_t a = read_operand(&dst); // старое значение приёмника
                uint32_t b = read_operand(&src);
                write_operand(&dst, is_add ? (a + b) : (a - b)); // записать результат
            }
        }
    }
    fclose(f); return 0;
}
