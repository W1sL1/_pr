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