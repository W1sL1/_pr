//Найти n - ную подходящую дробь по значению цепной дроби.
//В стандартный поток ввода(stdin) через пробел подаются заранее не известное количество 
//элементов цепной дроби и n(ввод оканчивается EOF).
//Данные элементы необходимо сохранить в динамический массив.
//Далее на основе этих значений вычислить n - ную подходящую дробь.
//n - ной подходящей дробью для цепной дроби[a0; a1, a2, a3, …, an], 
//является значение некоторого рационального числа pn / qn
//Эйлер вывел рекуррентные формулы для вычисления числителей и знаменателей подходящих дробей :
//p - 1 = 1,
//p0 = a0,
//pn = an * pn - 1 + pn - 2
//q - 1 = 0,
//q0 = 1,
//qn = an * qn - 1 + qn - 2.
//вход: элементы цепной дроби и n.
//пример : 5 3 7 15 1 292 1 1
//выход : числитель и знаменатель n - ной подходящей дробь.
//пример : 104348 33215






#include <stdio.h>
#include <stdlib.h>

int main() {
   int* a = NULL;
   int count = 0;
   int num;

   // Читаем все числа
   while (scanf("%d", &num) == 1) {
       a = (int*)realloc(a, (count + 1) * sizeof(int));
       if (a == NULL) {
           return 1;
       }
       a[count++] = num;
   }

   if (count < 2) {
       free(a);
       return 1;
   }

   // Последнее число - это n
   int n = a[count - 1];
   count--; // Теперь count - количество элементов цепной дроби

   if (n >= count) {
       free(a);
       return 1;
   }

   long long p_prev2 = 1, p_prev1 = a[0];
   long long q_prev2 = 0, q_prev1 = 1;
   long long p_curr = p_prev1, q_curr = q_prev1;

   for (int i = 1; i <= n; i++) {
       p_curr = a[i] * p_prev1 + p_prev2;
       q_curr = a[i] * q_prev1 + q_prev2;

       p_prev2 = p_prev1;
       q_prev2 = q_prev1;
       p_prev1 = p_curr;
       q_prev1 = q_curr;
   }

   printf("%lld %lld\n", p_curr, q_curr);

   free(a);
   return 0;
}






//Язык "Си"
//Лабораторная работа "02. Модель процессора x86"
//Пожелания: избегать ошибки Run - Time Check Failure #2 - Stack around the variable 'cmd' was corrupted.
//Реализовать структуру "процессор", которая содержит 32 - х битные регистры EAX, ECX, EDX, EIP,
//для реализации данных регистров(кроме EIP) использовать объединения,
//так что - бы возможно было обращаться к младшим 16 битам регистра(AX, CX, DX) и к страшим и младшим 8 битами регистров AX, CX, DX(AH, AL, CH, CL, DH, DL).
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

typedef union {
   uint32_t dword;   // EAX/ECX/EDX целиком (32 бита)
   struct {
       uint16_t lo;  // младшие 16 бит (AX/CX/DX)
       uint16_t hi;  // старшие 16 бит
   } w;
   struct {
       uint8_t lo;   // младшие 8 бит (AL/CL/DL)
       uint8_t hi;   // старшие 8 бит (AH/CH/DH)
       uint16_t hi16;
   } b;
} reg32_u;

typedef struct {
   reg32_u eax;
   reg32_u ecx;
   reg32_u edx;
   uint32_t eip;
} cpu_t;

typedef enum {
   OP_NONE,
   OP_REG8,
   OP_REG16,
   OP_REG32,
   OP_IMM
} op_type_t;

typedef struct {
   op_type_t type;
   uint8_t* r8;
   uint16_t* r16;
   uint32_t* r32;
   uint32_t imm;
} operand_t;

static uint32_t read_operand(const operand_t* op) {
   switch (op->type) {
   case OP_REG8:  return (uint32_t)(*op->r8);
   case OP_REG16: return (uint32_t)(*op->r16);
   case OP_REG32: return *op->r32;
   case OP_IMM:   return op->imm;
   default:       return 0;
   }
}

static void write_operand(operand_t* op, uint32_t value) {
   switch (op->type) {
   case OP_REG8:  *op->r8 = (uint8_t)value;  break;
   case OP_REG16: *op->r16 = (uint16_t)value; break;
   case OP_REG32: *op->r32 = value;           break;
   default: break;
   }
}

static int parse_register(const char* name, cpu_t* cpu, operand_t* op, int allow_eip) {
   char buf[8];
   size_t len = strlen(name);
   size_t i;

   if (len == 0 || len >= sizeof(buf))
       return -1;

   for (i = 0; i <= len; ++i)
       buf[i] = (char)toupper((unsigned char)name[i]);

   op->r8 = NULL;
   op->r16 = NULL;
   op->r32 = NULL;
   op->type = OP_NONE;

   if (strcmp(buf, "EAX") == 0) {
       op->type = OP_REG32;
       op->r32 = &cpu->eax.dword;
   }
   else if (strcmp(buf, "ECX") == 0) {
       op->type = OP_REG32;
       op->r32 = &cpu->ecx.dword;
   }
   else if (strcmp(buf, "EDX") == 0) {
       op->type = OP_REG32;
       op->r32 = &cpu->edx.dword;
   }
   else if (strcmp(buf, "EIP") == 0) {
       if (!allow_eip)
           return -1;
       op->type = OP_REG32;
       op->r32 = &cpu->eip;
   }
   else if (strcmp(buf, "AX") == 0) {
       op->type = OP_REG16;
       op->r16 = &cpu->eax.w.lo;
   }
   else if (strcmp(buf, "CX") == 0) {
       op->type = OP_REG16;
       op->r16 = &cpu->ecx.w.lo;
   }
   else if (strcmp(buf, "DX") == 0) {
       op->type = OP_REG16;
       op->r16 = &cpu->edx.w.lo;
   }
   else if (strcmp(buf, "AL") == 0) {
       op->type = OP_REG8;
       op->r8 = &cpu->eax.b.lo;
   }
   else if (strcmp(buf, "AH") == 0) {
       op->type = OP_REG8;
       op->r8 = &cpu->eax.b.hi;
   }
   else if (strcmp(buf, "CL") == 0) {
       op->type = OP_REG8;
       op->r8 = &cpu->ecx.b.lo;
   }
   else if (strcmp(buf, "CH") == 0) {
       op->type = OP_REG8;
       op->r8 = &cpu->ecx.b.hi;
   }
   else if (strcmp(buf, "DL") == 0) {
       op->type = OP_REG8;
       op->r8 = &cpu->edx.b.lo;
   }
   else if (strcmp(buf, "DH") == 0) {
       op->type = OP_REG8;
       op->r8 = &cpu->edx.b.hi;
   }
   else {
       return -1;
   }

   return 0;
}

static int parse_imm(const char* s, uint32_t* out) {
   char* endptr;
   unsigned long val = strtoul(s, &endptr, 0);
   if (s == endptr)
       return -1;
   *out = (uint32_t)val;
   return 0;
}

int main(void) {
   cpu_t cpu;
   FILE* f;
   char line[128];

   memset(&cpu, 0, sizeof(cpu));

   f = fopen("commands.txt", "r");
   if (!f) {
       fprintf(stderr, "Cannot open commands.txt\n");
       return 1;
   }

   while (fgets(line, sizeof(line), f)) {
       char* p = line;
       char* tok;
       char* op_str;
       char* arg1_str;
       char* arg2_str;
       operand_t dst, src;
       uint32_t val;

       /* убираем перевод строки */
       line[strcspn(line, "\r\n")] = '\0';

       /* пропуск начальных пробелов */
       while (isspace((unsigned char)*p))
           ++p;

       if (*p == '\0' || *p == '#')
           continue;

       /* увеличиваем EIP перед выполнением каждой инструкции */
       cpu.eip++;

       /* разбор: op arg1 arg2 (запятая отделяется как разделитель) */
       op_str = strtok(p, " \t");
       if (!op_str)
           continue;

       arg1_str = strtok(NULL, " \t,");
       arg2_str = strtok(NULL, " \t,");

       /* если пошёл комментарий после аргументов, игнорируем его */
       if (arg1_str && arg1_str[0] == '#')
           arg1_str = NULL;
       if (arg2_str && arg2_str[0] == '#')
           arg2_str = NULL;

       /* приводим команду к нижнему регистру */
       for (tok = op_str; *tok; ++tok)
           *tok = (char)tolower((unsigned char)*tok);

       if (strcmp(op_str, "shw") == 0) {
           if (!arg1_str)
               continue;

           if (parse_register(arg1_str, &cpu, &dst, 1) == 0) {
               val = read_operand(&dst);
           }
           else {
               if (parse_imm(arg1_str, &val) != 0)
                   continue;
           }
           printf("0x%x\n", val);
       }
       else if (strcmp(op_str, "mov") == 0 ||
           strcmp(op_str, "add") == 0 ||
           strcmp(op_str, "sub") == 0) {

           int is_add = (strcmp(op_str, "add") == 0);
           int is_sub = (strcmp(op_str, "sub") == 0);

           if (!arg1_str || !arg2_str)
               continue;

           /* EIP недопустим в mov/add/sub */
           if (parse_register(arg1_str, &cpu, &dst, 0) != 0)
               continue;

           if (parse_register(arg2_str, &cpu, &src, 0) != 0) {
               /* второй аргумент может быть константой */
               if (parse_imm(arg2_str, &val) != 0)
                   continue;
               src.type = OP_IMM;
               src.imm = val;
           }

           if (!is_add && !is_sub) { /* mov */
               val = read_operand(&src);
               write_operand(&dst, val);
           }
           else { /* add/sub */
               uint32_t a = read_operand(&dst);
               uint32_t b = read_operand(&src);
               uint32_t res = is_add ? (a + b) : (a - b);
               write_operand(&dst, res);
           }
       }
       /* неизвестные команды просто игнорируем */
   }

   fclose(f);
   return 0;
}








//Язык "Си"
//Тема "4. Рекурсия"
//Лабораторная работа "4.1. Расстановка шахматных фигур"
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
