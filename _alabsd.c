// Условия лабораторной работы №4 по Структурам Данных.
// Лабораторная работа "Записная книжка"
// Необходимо разработать программу, реализующую функционал записной книжки дел:
// 1) хранение объектов типа "событие":
//     - год, месяц, число, час, минута события
//     - день недели
//     - описание события (текст)
//     - место события (текст)
// 2) возможность добавления новых событий
// 3) возможность удаления событий
// 4) вывод событий на экран:
//     - возможность сортировки событий по дате и времени
//     - возможность фильтрации событий по месту
// 5) возможность поиска событий по дате и времени (с выводом длительности операции поиска)
// 5) возможность сохранения и загрузки событий из файла
// Требования: 
//     - должна быть реализована "защита от дурака", которая не позволит вводить некорректные данные в поля
//     - день недели должен высчитываться автоматически
//     - хранение событий должно быть реализовано на основе деревьев бинарного поиска (не сбалансированных)
//     - сортировки должны быть реализованы с помощью обхода деревьев
//     - фильтр должен работать в том числе и по подстрокам
//     - структура хранения событий должна быть оптимальна по логике и памяти.
// Тестирование:
// - необходимо подготовить входные данные (файл с событиями), в котором даты событий распределены равномерно
// - необходимо подготовить входные данные (файл с событиями), в котором даты событий идут по порядку
// - необходимо сравнить скорость работы поиска событий для первого и второго файла, объяснить результаты


































// Лабораторная работа на си, табуляция 4 пробела: Калькулятор. 
// Необходимо написать программу:
// - входные данные: выражение в инфиксной нотации  
// - выходные данные: значение введенного выражения. 
// Возможности: 
// - поддержка арифметических операций (+, -, *, /, ^, !, корень числа (sqrt)),  
// - поддержка скобок (в том числе вложенных, без ограничения на максимальную вложенность)
// - поддержка тригонометрических операций (sin, cos, tg, ctg, arcsin, arccos),  
// - введение пользовательских переменных
// При вводе выражения, содержащего переменные, программа должна спросить у пользователя значения этих переменных и с этими значениями вычислять результат выражения.
// Программа должна быть реализована на основе алгоритма подсчета значения в постфиксной нотации и алгоритма сортировочной станции с использованием структур данных стек и очередь.
// Стек и очередь должны быть реализованы на основе линейных списков.
// Примеры входных выражений, которые программа должна поддерживать:
// 2+2
// a * b
// sin(a)^2 + cos(b)^2
// sqrt(2^((a + b) ^ (c + d * sin(cos(x))))! - 17)

// Пример 1 из терминала: 
// Введите математическое выражение: (sqrt(a^2 + b^2) + sin(x)^2 + cos(x)^2 + tg(y)*ctg(y) + arcsin(sin(0.1)) + arccos(0)*2 + (z+1)! - 5^2^1) / 2
// Введите значение для переменной a: 3
// Введите значение для переменной b: 4
// Введите значение для переменной x: 0.5
// Введите значение для переменной y: 0.8
// Введите значение для переменной z: 0.1
// Результат: -6.855961 

// Пример 2 из терминала: 
// Введите математическое выражение: sqrt(2^((a + b) ^ (c + d * sin(cos(x))))! - 17)
// Введите значение для переменной a: 1
// Введите значение для переменной b: 1
// Введите значение для переменной c: 1
// Введите значение для переменной d: 1
// Введите значение для переменной x: 1
// Результат: ???

// sqrt(2^((1 + 1) ^ (1 + 1 * sin(cos(1))))! - 17) = ???

// выведи txt со значениями аргументов и ответами (тесты си) в формате выражение - количество переменных - (их значение через пробел) - ответ
// мои операции { скобки, +, -, *, /, ^, !, sqrt, sin, cos, tg, ctg, arcsin, arccos }

// gcc sd02.c -o sd02 -lm
// ./sd01

// 2+2*2 - 0 - - 6.000000
// (2+2)*2 - 0 - - 8.000000
// a + b * c - 3 - 2.0 3.0 4.0 - 14.000000
// x^2 - 1 - 5.0 - 25.000000
// 5! - 0 - - 120.000000
// sqrt(a^2 + b^2) - 2 - 3.0 4.0 - 5.000000
// sin(x)^2 + cos(x)^2 - 1 - 0.785398 - 1.000000
// tg(a) * ctg(a) - 1 - 0.5 - 1.000000
// arcsin(sin(0.5)) - 0 - - 0.500000
// arccos(0) * 2 - 0 - - 3.141593
// 2^3^2 - 0 - - 512.000000
// -x + 10 - 1 - 5.0 - 5.000000
// (a + b)! / c - 3 - 2.0 1.0 3.0 - 2.000000
// sqrt(81) + sin(0) - 0 - - 9.000000
// (x + 1)^(y - 1) - 2 - 1.0 4.0 - 8.000000





#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

// --- ТИПЫ ДАННЫХ И ЛЕКСЕМ (ТОКЕНОВ) ---

typedef enum {
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_FUNCTION,
    TOKEN_VARIABLE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_NONE
} TokenType;

typedef struct {
    TokenType type;
    double value;       // Для чисел
    char str[32];       // Для функций, переменных и операторов (как строка для удобства)
    char op;            // Для операторов (+, -, *, /, ^, !, ~)
} Token;

// --- СТРУКТУРЫ ДАННЫХ: УЗЕЛ, СТЕК И ОЧЕРЕДЬ НА БАЗЕ СВЯЗНЫХ СПИСКОВ ---

typedef struct Node {
    Token data;
    struct Node* next;
} Node;

typedef struct {
    Node* top;
} Stack;

typedef struct {
    Node* head;
    Node* tail;
} Queue;

// Функции для работы со Стеком
void initStack(Stack* s) {
    s->top = NULL;
}

int isStackEmpty(Stack* s) {
    return s->top == NULL;
}

void push(Stack* s, Token t) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = t;
    newNode->next = s->top;
    s->top = newNode;
}

Token pop(Stack* s) {
    if (isStackEmpty(s)) {
        Token empty = {TOKEN_NONE, 0, "", 0};
        return empty;
    }
    Node* temp = s->top;
    Token popped = temp->data;
    s->top = temp->next;
    free(temp);
    return popped;
}

Token peek(Stack* s) {
    if (isStackEmpty(s)) {
        Token empty = {TOKEN_NONE, 0, "", 0};
        return empty;
    }
    return s->top->data;
}

// Функции для работы с Очередью
void initQueue(Queue* q) {
    q->head = NULL;
    q->tail = NULL;
}

int isQueueEmpty(Queue* q) {
    return q->head == NULL;
}

void enqueue(Queue* q, Token t) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = t;
    newNode->next = NULL;
    if (isQueueEmpty(q)) {
        q->head = newNode;
        q->tail = newNode;
    } else {
        q->tail->next = newNode;
        q->tail = newNode;
    }
}

Token dequeue(Queue* q) {
    if (isQueueEmpty(q)) {
        Token empty = {TOKEN_NONE, 0, "", 0};
        return empty;
    }
    Node* temp = q->head;
    Token dequeued = temp->data;
    q->head = temp->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    free(temp);
    return dequeued;
}

// --- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ АЛГОРИТМА ---

// Приоритет операторов
int getPrecedence(char op) {
    if (op == '!') return 5;
    if (op == '~') return 4; // Унарный минус
    if (op == '^') return 3;
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

// Проверка на правоассоциативность
int isRightAssociative(char op) {
    return (op == '^' || op == '~');
}

// Распознавание функций
int isFunction(const char* str) {
    return (strcmp(str, "sin") == 0 || strcmp(str, "cos") == 0 ||
            strcmp(str, "tg") == 0 || strcmp(str, "ctg") == 0 ||
            strcmp(str, "arcsin") == 0 || strcmp(str, "arccos") == 0 ||
            strcmp(str, "sqrt") == 0);
}

// --- ТОКЕНИЗАЦИЯ И АЛГОРИТМ СОРТИРОВОЧНОЙ СТАНЦИИ ---

void tokenizeAndShuntingYard(const char* expression, Queue* outputQueue) {
    Stack opStack;
    initStack(&opStack);
    TokenType prevType = TOKEN_NONE;

    int i = 0;
    while (expression[i] != '\0') {
        if (isspace(expression[i])) {
            i++;
            continue;
        }

        Token t;
        t.type = TOKEN_NONE;
        memset(t.str, 0, sizeof(t.str));

        // 1. Парсинг чисел
        if (isdigit(expression[i]) || expression[i] == '.') {
            char numBuf[64];
            int j = 0;
            while (isdigit(expression[i]) || expression[i] == '.') {
                numBuf[j++] = expression[i++];
            }
            numBuf[j] = '\0';
            t.type = TOKEN_NUMBER;
            t.value = atof(numBuf);
            enqueue(outputQueue, t);
            prevType = TOKEN_NUMBER;
            continue;
        }

        // 2. Парсинг слов (функции или переменные)
        if (isalpha(expression[i])) {
            int j = 0;
            while (isalpha(expression[i]) || isdigit(expression[i])) {
                t.str[j++] = expression[i++];
            }
            t.str[j] = '\0';

            if (isFunction(t.str)) {
                t.type = TOKEN_FUNCTION;
                push(&opStack, t);
            } else {
                t.type = TOKEN_VARIABLE;
                enqueue(outputQueue, t);
            }
            prevType = t.type;
            continue;
        }

        // 3. Скобки
        if (expression[i] == '(') {
            t.type = TOKEN_LPAREN;
            t.op = '(';
            push(&opStack, t);
            prevType = TOKEN_LPAREN;
            i++;
            continue;
        }

        if (expression[i] == ')') {
            while (!isStackEmpty(&opStack) && peek(&opStack).type != TOKEN_LPAREN) {
                enqueue(outputQueue, pop(&opStack));
            }
            if (!isStackEmpty(&opStack)) {
                pop(&opStack); // Удаляем '(' из стека
            }
            if (!isStackEmpty(&opStack) && peek(&opStack).type == TOKEN_FUNCTION) {
                enqueue(outputQueue, pop(&opStack)); // Перемещаем функцию в очередь
            }
            prevType = TOKEN_RPAREN;
            i++;
            continue;
        }

        // 4. Операторы
        if (strchr("+-*/^!", expression[i])) {
            t.type = TOKEN_OPERATOR;
            t.op = expression[i];

            // Определение унарного минуса
            if (t.op == '-' && (prevType == TOKEN_NONE || prevType == TOKEN_LPAREN || prevType == TOKEN_OPERATOR)) {
                t.op = '~';
            }

            // Факториал (постфиксный унарный оператор) идет сразу в очередь
            if (t.op == '!') {
                enqueue(outputQueue, t);
                prevType = TOKEN_OPERATOR;
                i++;
                continue;
            }

            while (!isStackEmpty(&opStack) && peek(&opStack).type == TOKEN_OPERATOR) {
                char topOp = peek(&opStack).op;
                if ((!isRightAssociative(t.op) && getPrecedence(t.op) <= getPrecedence(topOp)) ||
                    (isRightAssociative(t.op) && getPrecedence(t.op) < getPrecedence(topOp))) {
                    enqueue(outputQueue, pop(&opStack));
                } else {
                    break;
                }
            }
            push(&opStack, t);
            prevType = TOKEN_OPERATOR;
            i++;
            continue;
        }

        i++; // Пропуск неизвестных символов (если есть)
    }

    // Выталкиваем оставшиеся операторы
    while (!isStackEmpty(&opStack)) {
        enqueue(outputQueue, pop(&opStack));
    }
}

// --- УПРАВЛЕНИЕ ПЕРЕМЕННЫМИ ---

typedef struct {
    char name[32];
    double value;
} Variable;

void promptVariables(Queue* postfixQueue, Variable* vars, int* varCount) {
    Node* current = postfixQueue->head;
    while (current != NULL) {
        if (current->data.type == TOKEN_VARIABLE) {
            // Проверяем, запрашивали ли мы уже эту переменную
            int exists = 0;
            for (int i = 0; i < *varCount; i++) {
                if (strcmp(vars[i].name, current->data.str) == 0) {
                    exists = 1;
                    break;
                }
            }
            // Если переменной нет в списке, запрашиваем
            if (!exists) {
                strcpy(vars[*varCount].name, current->data.str);
                printf("Введите значение для переменной %s: ", current->data.str);
                scanf("%lf", &vars[*varCount].value);
                (*varCount)++;
            }
        }
        current = current->next;
    }
}

double getVariableValue(const char* name, Variable* vars, int varCount) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            return vars[i].value;
        }
    }
    return 0.0;
}

// --- ВЫЧИСЛЕНИЕ ПОСТФИКСНОГО ВЫРАЖЕНИЯ ---

double evaluatePostfix(Queue* postfixQueue, Variable* vars, int varCount) {
    Stack evalStack;
    initStack(&evalStack);

    while (!isQueueEmpty(postfixQueue)) {
        Token t = dequeue(postfixQueue);

        if (t.type == TOKEN_NUMBER) {
            push(&evalStack, t);
        } 
        else if (t.type == TOKEN_VARIABLE) {
            t.value = getVariableValue(t.str, vars, varCount);
            t.type = TOKEN_NUMBER;
            push(&evalStack, t);
        }
        else if (t.type == TOKEN_OPERATOR) {
            if (t.op == '~') { // Унарный минус
                Token a = pop(&evalStack);
                a.value = -a.value;
                push(&evalStack, a);
            } else if (t.op == '!') { // Факториал
                Token a = pop(&evalStack);
                a.value = tgamma(a.value + 1); // tgamma обобщает факториал
                push(&evalStack, a);
            } else {
                Token b = pop(&evalStack);
                Token a = pop(&evalStack);
                Token res = {TOKEN_NUMBER, 0.0, "", 0};

                switch (t.op) {
                    case '+': res.value = a.value + b.value; break;
                    case '-': res.value = a.value - b.value; break;
                    case '*': res.value = a.value * b.value; break;
                    case '/': res.value = a.value / b.value; break;
                    case '^': res.value = pow(a.value, b.value); break;
                }
                push(&evalStack, res);
            }
        }
        else if (t.type == TOKEN_FUNCTION) {
            Token a = pop(&evalStack);
            Token res = {TOKEN_NUMBER, 0.0, "", 0};

            if (strcmp(t.str, "sin") == 0) res.value = sin(a.value);
            else if (strcmp(t.str, "cos") == 0) res.value = cos(a.value);
            else if (strcmp(t.str, "tg") == 0) res.value = tan(a.value);
            else if (strcmp(t.str, "ctg") == 0) res.value = 1.0 / tan(a.value);
            else if (strcmp(t.str, "arcsin") == 0) res.value = asin(a.value);
            else if (strcmp(t.str, "arccos") == 0) res.value = acos(a.value);
            else if (strcmp(t.str, "sqrt") == 0) res.value = sqrt(a.value);
            
            push(&evalStack, res);
        }
    }

    return pop(&evalStack).value;
}

// --- ГЛАВНАЯ ФУНКЦИЯ ---

int main() {
    char expression[256];
    printf("Лабораторная работа №2: Калькулятор\n");
    printf("Введите математическое выражение: ");
    
    if (fgets(expression, sizeof(expression), stdin) == NULL) {
        return 1;
    }

    // Удаляем символ переноса строки
    expression[strcspn(expression, "\n")] = 0;

    Queue postfixQueue;
    initQueue(&postfixQueue);

    // 1. Преобразование в постфиксную нотацию
    tokenizeAndShuntingYard(expression, &postfixQueue);

    // 2. Обработка переменных (если они есть)
    Variable vars[50];
    int varCount = 0;
    promptVariables(&postfixQueue, vars, &varCount);

    // 3. Вычисление результата
    double result = evaluatePostfix(&postfixQueue, vars, varCount);
    
    printf("Результат: %lf\n", result);

    return 0;
}
