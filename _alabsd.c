// Условия лабораторной работы №2 Калькулятор на языке си, табуляция 4 пробела.
// Необходимо написать программу:
// - входные данные: выражение в инфиксной нотации  
// - выходные данные: значение введенного выражения. 
// Возможности: 
// - поддержка арифметических операций (+, -, *, /, ^, !, корень числа (sqrt),  
// - поддержка скобок (в том числе вложенных, без ограничения на максимальную вложенность)
// - поддержка тригонометрических операций (sin, cos,tg, ctg, arcsin, … ),  
// - введение пользовательских переменных
// При вводе выражения, содержащего переменные, программа должна спросить у пользователя значения этих переменных и с этими значениями вычислять результат выражения.
// Программа должна быть реализована на основе алгоритма подсчета значения в постфиксной нотации и алгоритма сортировочной станции с использованием структур данных стек и очередь.
// Стек и очередь должны быть реализованы на основе линейных списков.
// Примеры входных выражений, которые программа должна поддерживать:
// 2+2
// a * b
// sin(a)^2 + cos(b)^2
// sqrt(2^((a + b) ^ (c + d * sin(cos(x))))! - 17)















#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <locale.h>
#define MAX_SIZE 100

typedef struct {
    float data[MAX_SIZE];
    int top;
} Stack;

void initialize(Stack* stack) {
    stack->top = -1;
}

int isEmpty(Stack* stack) {
    return (stack->top == -1);
}

void push(Stack* stack, float ch) {
    if (stack->top == MAX_SIZE - 1) {
        printf("Стек переполнен\n");
        return;
    }
    stack->data[++(stack->top)] = ch;
}

float pop(Stack* stack) {
    if (isEmpty(stack)) {
        printf("Стек пуст\n");
        return '\0';
    }
    return stack->data[(stack->top)--];
}

int isOperator(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^' || ch == '!' || ch == 'q' || ch == 's' || ch == 'c');
}

int getPriority(char ch) {
    if (ch == '+' || ch == '-')
        return 1;
    else if (ch == '*' || ch == '/')
        return 2;
    else if (ch == '^' || ch == '!' || ch == 'q' || ch == 's' || ch == 'c')
        return 3;
    else
        return 0;
}

void infixToPostfix(char* infix, char* postfix) {
    Stack stack;
    initialize(&stack);
    int i, j;
    int k = 0;
    char ch;
    char tmp[10];
    char xs[123] = { 0 };
    char xs1[123][10];

    for (i = 0, j = 0; infix[i] != '\0'; i++) {
        ch = infix[i];

        if (ch >= 97 && ch <= 122 && ch != 'q' && ch != 's' && ch != 'c') {
            if (xs[ch] > 0) {
                while (isdigit(xs1[ch][k])) {
                    postfix[j++] = xs1[ch][k++];
                }
                k = 0;
                postfix[j++] = ' ';
            }
            else {
                printf("Введите значение переменной %c: ", ch);
                gets_s(tmp, 10);
                while (isdigit(tmp[k])) {
                    postfix[j++] = tmp[k++];
                }
                k = 0;
                postfix[j++] = ' ';
                xs[ch]++;
                while (isdigit(tmp[k])) {
                    xs1[ch][k++] = tmp[k++];
                }
                k = 0;
            }
        }


        if (ch == ' ' || ch == '\t')
            continue;

        if (isdigit(ch)) {
            while (isdigit(infix[i])) {
                postfix[j++] = infix[i++];
            }
            postfix[j++] = ' ';
            i--;
        }
        else if (isOperator(ch)) {
            while (!isEmpty(&stack) && getPriority(stack.data[stack.top]) >= getPriority(ch)) {
                postfix[j++] = pop(&stack);
                postfix[j++] = ' ';
            }
            push(&stack, ch);
        }
        else if (ch == '(') {
            push(&stack, ch);
        }
        else if (ch == ')') {
            while (!isEmpty(&stack) && stack.data[stack.top] != '(') {
                postfix[j++] = pop(&stack);
                postfix[j++] = ' ';
            }
            if (!isEmpty(&stack) && stack.data[stack.top] == '(') {
                pop(&stack);
            }
            else {
                printf("Ошибка: неправильное выражение\n");
                return;
            }
        }
    }

    while (!isEmpty(&stack)) {
        if (stack.data[stack.top] == '(') {
            printf("Ошибка: неправильное выражение\n");
            return;
        }
        postfix[j++] = pop(&stack);
        postfix[j++] = ' ';
    }

    postfix[j] = '\0';
}

float evaluatePostfix(char* postfix) {
    Stack stack;
    initialize(&stack);
    int i;
    float result, operand1, operand2;
    char num[10];
    int numIndex = 0;
    char ch;

    for (i = 0; postfix[i] != '\0'; i++) {
        ch = postfix[i];

        if (isdigit(ch)) {
            while (isdigit(postfix[i])) {
                num[numIndex++] = postfix[i++];
            }
            num[numIndex] = '\0';
            numIndex = 0;
            push(&stack, atoi(num));
        }
        else if (ch == ' ') {
            continue;
        }
        else if (isOperator(ch)) {
            if (ch == '!' || ch == 'q' || ch == 's' || ch == 'c') {
                operand1 = pop(&stack);
                switch (ch) {
                case '!':
                    result = 1;
                    for (int j = 1; j <= operand1; j++) {
                        result *= j;
                    }
                    break;
                case 'q':
                    result = sqrt(operand1);
                    break;
                case 's':
                    result = sin(operand1);
                    break;
                case 'c':
                    result = cos(operand1);
                    break;
                default:
                    printf("Ошибка: неподдерживаемый оператор\n");
                    return 0;
                }
            }
            else {
                operand2 = pop(&stack);
                operand1 = pop(&stack);
                switch (ch) {
                case '+':
                    result = operand1 + operand2;
                    break;
                case '-':
                    result = operand1 - operand2;
                    break;
                case '*':
                    result = operand1 * operand2;
                    break;
                case '/':
                    result = operand1 / operand2;
                    break;
                case '^':
                    result = pow(operand1, operand2);
                    break;
                default:
                    printf("Ошибка: неподдерживаемый оператор\n");
                    return 0;
                }
            }
            push(&stack, result);
        }
        else {
            printf("Ошибка: неправильный символ в постфиксной записи\n");
            return 0;
        }
    }

    result = pop(&stack);
    if (!isEmpty(&stack)) {
        printf("Ошибка: неправильное выражение\n");
        return 0;
    }

    return result;
}


int main() {
    setlocale(LC_ALL, "rus");
    char infix[MAX_SIZE];
    char postfix[MAX_SIZE];
    printf("q - квадратный корень, s - синус, c - косинус, на вход принимает целые числа\n");

    printf("Введите выражение: ");
    fgets(infix, sizeof(infix), stdin);
    infix[strlen(infix) - 1] = '\0';

    infixToPostfix(infix, postfix);
    //printf("Постфиксная запись: %s\n", postfix);

    float result = evaluatePostfix(postfix);
    printf("Результат: %.2f\n", result);

    return 0;
}


















// Условия лабораторной работы №1 по Структурам данных. 
// Тема: "Работа с файлами". Язык: Си.
// Необходимо реализовать программу, осуществляющую замену слов в тексте и вывод полученного текста в выходной файл.
//  Входные данные:
// - путь к файлу1 с текстом (имя file1.txt)
// - путь к файлу2 со словами, которые требуется заменить (имя file2.txt)
// - путь к файлу3 со словами, на которые требуется заменить (имя file3.txt)
// - путь к выходному файлу4, в который будет записан результирующий текст (имя file4.txt).
// Слова в файлах 2 и 3 разделяются переносом строки. Слово на первой строке файла 2 должно заменяться на слово первой строки файла 3, слово на второй строке - на соответствующее слово второй строки и т.д. 
// Требования:
// - текст может включать английские и русские буквы, различные знаки препинания и специальные символы (в т.ч. пробелы, табуляции, и т.д.)
// - результирующий текст должен повторять все знаки препинания и специальные символы оригинального текста.
// - программа должна эффективно управлять памятью       (применяем динамическую память, используем каждый байт оптимально).
// - входные данные в программу должны передаваться с использованием аргументов командной строки и входных параметров main (argv, argc). 
// - в программе необходимо найти применение и грамотно заиспользовать структуры (или объединения или ENUM'ы).
//
// - замена целых слов 
// - учитывать регистр 
//
// Пример входных данных
// file1.txt
// Hello, world! Привет мир.
// WORLD world World. 
// file2.txt
// world
// мир
// file3.txt
// WORLD2
// МИР2
// file4.txt
// (файл-приемник, изначально можно пустой)
// Ожидаемый выход (file4.txt)
// Hello, WORLD2! Привет МИР2.
// WORLD WORLD2 World.




