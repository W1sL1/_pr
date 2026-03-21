убрать статик и тд








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
   fclose(f);
   return 0;
}
