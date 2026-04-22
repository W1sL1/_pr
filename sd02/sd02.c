#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
// gcc sd02.c -o sd02 -lm
// ./sd02

// --- Определение типов данных для токенов ---

// Перечисление типов лексем (токенов)
typedef enum {
    TOKEN_NUMBER,    // Число (например, 3.14)
    TOKEN_VARIABLE,  // Переменная (например, x, y)
    TOKEN_OPERATOR,  // Оператор (+, -, *, /, ^, !)
    TOKEN_FUNCTION,  // Функция (sin, cos, tg и т.д.)
    TOKEN_LPAREN,    // Левая круглая скобка '('
    TOKEN_RPAREN     // Правая круглая скобка ')'
} TokenType;

// Структура токена: хранит тип, числовое значение и строковое представление
typedef struct {
    TokenType type;
    double value;       // Для чисел
    char str[32];       // Для переменных, функций, операторов
} Token;

// --- Структуры для линейных списков (Стек и Очередь) ---

// Узел списка: хранит токен и указатель на следующий узел
typedef struct Node {
    Token data;
    struct Node* next;
} Node;

// Стек: указатель на вершину
typedef struct {
    Node* top;
} Stack;

// Очередь: указатели на начало и конец
typedef struct {
    Node* front;
    Node* rear;
} Queue;

// --- Базовые операции со Стеком ---

// Инициализация стека (вершина = NULL)
void initStack(Stack* s) {
    s->top = NULL;
}

// Проверка стека на пустоту
bool isEmptyStack(Stack* s) {
    return s->top == NULL;
}

// Добавление элемента на вершину стека
void push(Stack* s, Token data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = s->top;
    s->top = newNode;
}

// Удаление элемента с вершины стека и возврат его значения
Token pop(Stack* s) {
    if (isEmptyStack(s)) {
        Token empty = {0};
        return empty;
    }
    Node* temp = s->top;
    Token data = temp->data;
    s->top = s->top->next;
    free(temp);
    return data;
}

// Просмотр вершины стека без удаления
Token peekStack(Stack* s) {
    if (isEmptyStack(s)) {
        Token empty = {0};
        return empty;
    }
    return s->top->data;
}

// --- Базовые операции с Очередью ---

// Инициализация очереди
void initQueue(Queue* q) {
    q->front = q->rear = NULL;
}

// Проверка очереди на пустоту
bool isEmptyQueue(Queue* q) {
    return q->front == NULL;
}

// Добавление элемента в конец очереди
void enqueue(Queue* q, Token data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
        return;
    }
    q->rear->next = newNode;
    q->rear = newNode;
}

// Удаление элемента из начала очереди
Token dequeue(Queue* q) {
    if (isEmptyQueue(q)) {
        Token empty = {0};
        return empty;
    }
    Node* temp = q->front;
    Token data = temp->data;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    return data;
}

// --- Вспомогательные функции для парсинга и алгоритма ---

// Возвращает приоритет оператора (чем больше число, тем выше приоритет)
int getPrecedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    if (op == '_') return 4; // Унарный минус
    if (op == '!') return 5; // Факториал
    return 0;
}

// Проверяет, является ли оператор правоассоциативным
// (например, степень и унарный минус вычисляются справа налево)
bool isRightAssociative(char op) {
    return op == '^' || op == '_';
}

// Проверяет, является ли строка именем встроенной функции
bool isFunction(const char* name) {
    return strcmp(name, "sin") == 0 || strcmp(name, "cos") == 0 ||
            strcmp(name, "tg") == 0 || strcmp(name, "ctg") == 0 ||
            strcmp(name, "arcsin") == 0 || strcmp(name, "arccos") == 0 ||
            strcmp(name, "sqrt") == 0;
}

// --- Глобальные переменные для хранения пользовательских вводов ---

// Структура для хранения переменной: имя и значение
typedef struct {
    char name[32];
    double val;
} VariableDef;

// Массив для хранения определённых переменных (максимум 100)
VariableDef vars[100];
int var_count = 0; // Количество сохранённых переменных

// Возвращает значение переменной по её имени (если не найдено — 0)
double getVariableValue(const char* name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            return vars[i].val;
        }
    }
    return 0.0;
}

// --- Основная логика ---

int main() {
    char expr[1024];
    printf("Введите математическое выражение: ");
    if (!fgets(expr, sizeof(expr), stdin)) return 1;

    // Убираем символ новой строки, который остался после fgets
    expr[strcspn(expr, "\n")] = 0;

    Queue tokens;       // Очередь для хранения исходных токенов
    initQueue(&tokens);

    // 1. Токенизация (Лексический анализ)
    char* p = expr;
    bool expect_unary = true; // Флаг: ожидается ли унарный оператор (например, после '(' или в начале)

    while (*p) {
        // Пропускаем пробелы
        if (isspace(*p)) {
            p++;
            continue;
        }

        // Обработка чисел (целых и с плавающей точкой)
        if (isdigit(*p) || *p == '.') {
            char* end;
            double val = strtod(p, &end);
            Token t = {TOKEN_NUMBER, val, ""};
            enqueue(&tokens, t);
            p = end;
            expect_unary = false; // После числа унарный оператор не ожидается
        } 
        // Обработка буквенных имен (переменные или функции)
        else if (isalpha(*p)) {
            char name[32];
            int i = 0;
            while (isalpha(*p) || isdigit(*p)) {
                name[i++] = *p++;
            }
            name[i] = '\0';

            Token t;
            strcpy(t.str, name);
            if (isFunction(name)) {
                t.type = TOKEN_FUNCTION;
                expect_unary = true; // После функции может идти '('
            } else {
                t.type = TOKEN_VARIABLE;
                expect_unary = false;
            }
            enqueue(&tokens, t);
        } 
        // Обработка '('
        else if (*p == '(') {
            Token t = {TOKEN_LPAREN, 0, "("};
            enqueue(&tokens, t);
            expect_unary = true; // После '(' может идти унарный оператор
            p++;
        } 
        // Обработка ')'
        else if (*p == ')') {
            Token t = {TOKEN_RPAREN, 0, ")"};
            enqueue(&tokens, t);
            expect_unary = false;
            p++;
        } 
        // Обработка операторов (+, -, *, /, ^, !)
        else if (strchr("+-*/^!", *p)) {
            Token t = {TOKEN_OPERATOR, 0, ""};
            t.str[0] = *p;
            t.str[1] = '\0';

            // Если встретили '-' и ожидается унарный — заменяем на '_'
            if (*p == '-' && expect_unary) {
                t.str[0] = '_'; // Специальный символ для унарного минуса
            } 
            // Унарный плюс просто игнорируем
            else if (*p == '+' && expect_unary) {
                p++;
                continue;
            }

            enqueue(&tokens, t);
            
            // После факториала унарный не ожидается, после других — ожидается
            if (*p != '!') {
                expect_unary = true;
            } else {
                expect_unary = false;
            }
            p++;
        } 
        // Все остальные символы игнорируем
        else {
            p++;
        }
    }

    // 2. Сбор переменных и запрос значений у пользователя
    Node* curr = tokens.front;
    while (curr) {
        if (curr->data.type == TOKEN_VARIABLE) {
            bool found = false;
            // Проверяем, не спрашивали ли уже эту переменную
            for (int i = 0; i < var_count; i++) {
                if (strcmp(vars[i].name, curr->data.str) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                strcpy(vars[var_count].name, curr->data.str);
                printf("Введите значение для переменной %s: ", vars[var_count].name);
                scanf("%lf", &vars[var_count].val);
                var_count++;
            }
        }
        curr = curr->next;
    }

    // 3. Алгоритм сортировочной станции (инфиксная -> постфиксная)
    Queue output;       // Выходная очередь (постфиксная форма)
    initQueue(&output);
    Stack ops;          // Стек операторов
    initStack(&ops);

    while (!isEmptyQueue(&tokens)) {
        Token t = dequeue(&tokens);

        // Числа и переменные сразу идут в выходную очередь
        if (t.type == TOKEN_NUMBER || t.type == TOKEN_VARIABLE) {
            enqueue(&output, t);
        } 
        // Функции — в стек операторов
        else if (t.type == TOKEN_FUNCTION) {
            push(&ops, t);
        } 
        // Операторы — обрабатываем согласно приоритету и ассоциативности
        else if (t.type == TOKEN_OPERATOR) {
            while (!isEmptyStack(&ops) && peekStack(&ops).type == TOKEN_OPERATOR) {
                Token top = peekStack(&ops);
                int precT = getPrecedence(t.str[0]);
                int precTop = getPrecedence(top.str[0]);

                // Правило: выталкиваем, если текущий оператор имеет меньший или равный приоритет
                // (с учётом правоассоциативности)
                if ((!isRightAssociative(t.str[0]) && precT <= precTop) ||
                    (isRightAssociative(t.str[0]) && precT < precTop)) {
                    enqueue(&output, pop(&ops));
                } else {
                    break;
                }
            }
            push(&ops, t);
        } 
        // Левая скобка — просто в стек
        else if (t.type == TOKEN_LPAREN) {
            push(&ops, t);
        } 
        // Правая скобка — выталкиваем всё до '('
        else if (t.type == TOKEN_RPAREN) {
            while (!isEmptyStack(&ops) && peekStack(&ops).type != TOKEN_LPAREN) {
                enqueue(&output, pop(&ops));
            }
            // Убираем '(' из стека
            if (!isEmptyStack(&ops) && peekStack(&ops).type == TOKEN_LPAREN) {
                pop(&ops);
            }
            // Если на вершине оказалась функция — отправляем её в выходную очередь
            if (!isEmptyStack(&ops) && peekStack(&ops).type == TOKEN_FUNCTION) {
                enqueue(&output, pop(&ops));
            }
        }
    }

    // Выталкиваем оставшиеся операторы из стека в выходную очередь
    while (!isEmptyStack(&ops)) {
        enqueue(&output, pop(&ops));
    }

    // 4. Вычисление постфиксного выражения (используя стек eval)
    Stack eval;
    initStack(&eval);

    while (!isEmptyQueue(&output)) {
        Token t = dequeue(&output);

        // Число — кладём на стек eval
        if (t.type == TOKEN_NUMBER) {
            push(&eval, t);
        } 
        // Переменная — заменяем её числовым значением и кладём на стек
        else if (t.type == TOKEN_VARIABLE) {
            Token numToken = {TOKEN_NUMBER, getVariableValue(t.str), ""};
            push(&eval, numToken);
        } 
        // Оператор — извлекаем операнды и вычисляем
        else if (t.type == TOKEN_OPERATOR) {
            char op = t.str[0];
            // Унарные операторы (факториал '!', унарный минус '_')
            if (op == '!' || op == '_') {
                double a = pop(&eval).value;
                double res = 0;
                if (op == '!') 
                    res = tgamma(a + 1); // Гамма-функция для факториала вещественных чисел
                else if (op == '_') 
                    res = -a;            // Унарный минус
                Token resToken = {TOKEN_NUMBER, res, ""};
                push(&eval, resToken);
            } 
            // Бинарные операторы
            else {
                double b = pop(&eval).value;
                double a = pop(&eval).value;
                double res = 0;
                switch (op) {
                    case '+': res = a + b; break;
                    case '-': res = a - b; break;
                    case '*': res = a * b; break;
                    case '/': res = a / b; break;
                    case '^': res = pow(a, b); break;
                }
                Token resToken = {TOKEN_NUMBER, res, ""};
                push(&eval, resToken);
            }
        } 
        // Функция — извлекаем аргумент, вычисляем и кладём результат
        else if (t.type == TOKEN_FUNCTION) {
            double a = pop(&eval).value;
            double res = 0;
            if (strcmp(t.str, "sin") == 0) res = sin(a);
            else if (strcmp(t.str, "cos") == 0) res = cos(a);
            else if (strcmp(t.str, "tg") == 0) res = tan(a);
            else if (strcmp(t.str, "ctg") == 0) res = 1.0 / tan(a);
            else if (strcmp(t.str, "arcsin") == 0) res = asin(a);
            else if (strcmp(t.str, "arccos") == 0) res = acos(a);
            else if (strcmp(t.str, "sqrt") == 0) res = sqrt(a);
            
            Token resToken = {TOKEN_NUMBER, res, ""};
            push(&eval, resToken);
        }
    }

    // Вывод результата, если стек eval не пуст
    if (!isEmptyStack(&eval)) {
        printf("Результат: %f\n", pop(&eval).value);
    } else {
        printf("Ошибка: некорректное выражение.\n");
    }

    return 0;
}