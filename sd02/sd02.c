#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
// gcc sd02.c -o sd02 -lm
// ./sd02

// --- Определение типов данных для токенов ---

typedef enum {
    TOKEN_NUMBER,
    TOKEN_VARIABLE,
    TOKEN_OPERATOR,
    TOKEN_FUNCTION,
    TOKEN_LPAREN,
    TOKEN_RPAREN
} TokenType;

typedef struct {
    TokenType type;
    double value;
    char str[32]; // Хранит имя переменной, функции или символ оператора
} Token;

// --- Структуры для линейных списков (Стек и Очередь) ---

typedef struct Node {
    Token data;
    struct Node* next;
} Node;

typedef struct {
    Node* top;
} Stack;

typedef struct {
    Node* front;
    Node* rear;
} Queue;

// --- Базовые операции со Стеком ---

void initStack(Stack* s) {
    s->top = NULL;
}

bool isEmptyStack(Stack* s) {
    return s->top == NULL;
}

void push(Stack* s, Token data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = s->top;
    s->top = newNode;
}

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

Token peekStack(Stack* s) {
    if (isEmptyStack(s)) {
        Token empty = {0};
        return empty;
    }
    return s->top->data;
}

// --- Базовые операции с Очередью ---

void initQueue(Queue* q) {
    q->front = q->rear = NULL;
}

bool isEmptyQueue(Queue* q) {
    return q->front == NULL;
}

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

int getPrecedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    if (op == '_') return 4; // Унарный минус
    if (op == '!') return 5; // Факториал
    return 0;
}

bool isRightAssociative(char op) {
    return op == '^' || op == '_';
}

bool isFunction(const char* name) {
    return strcmp(name, "sin") == 0 || strcmp(name, "cos") == 0 ||
            strcmp(name, "tg") == 0 || strcmp(name, "ctg") == 0 ||
            strcmp(name, "arcsin") == 0 || strcmp(name, "arccos") == 0 ||
            strcmp(name, "sqrt") == 0;
}

// --- Глобальные переменные для хранения пользовательских вводов ---

typedef struct {
    char name[32];
    double val;
} VariableDef;

VariableDef vars[100];
int var_count = 0;

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

    // Убираем символ новой строки
    expr[strcspn(expr, "\n")] = 0;

    Queue tokens;
    initQueue(&tokens);

    // 1. Токенизация (Лексический анализ)
    char* p = expr;
    bool expect_unary = true; // Флаг для определения унарного минуса

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (isdigit(*p) || *p == '.') {
            char* end;
            double val = strtod(p, &end);
            Token t = {TOKEN_NUMBER, val, ""};
            enqueue(&tokens, t);
            p = end;
            expect_unary = false;
        } else if (isalpha(*p)) {
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
                expect_unary = true;
            } else {
                t.type = TOKEN_VARIABLE;
                expect_unary = false;
            }
            enqueue(&tokens, t);
        } else if (*p == '(') {
            Token t = {TOKEN_LPAREN, 0, "("};
            enqueue(&tokens, t);
            expect_unary = true;
            p++;
        } else if (*p == ')') {
            Token t = {TOKEN_RPAREN, 0, ")"};
            enqueue(&tokens, t);
            expect_unary = false;
            p++;
        } else if (strchr("+-*/^!", *p)) {
            Token t = {TOKEN_OPERATOR, 0, ""};
            t.str[0] = *p;
            t.str[1] = '\0';

            if (*p == '-' && expect_unary) {
                t.str[0] = '_'; // Заменяем унарный минус специальным символом
            } else if (*p == '+' && expect_unary) {
                p++;
                continue; // Игнорируем унарный плюс
            }

            enqueue(&tokens, t);
            
            if (*p != '!') {
                expect_unary = true;
            } else {
                expect_unary = false; // Факториал - постфиксный, за ним не идет унарный оператор
            }
            p++;
        } else {
            p++; // Игнорируем неизвестные символы
        }
    }

    // 2. Сбор переменных и запрос значений у пользователя
    Node* curr = tokens.front;
    while (curr) {
        if (curr->data.type == TOKEN_VARIABLE) {
            bool found = false;
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

    // 3. Алгоритм сортировочной станции (Инфиксная -> Постфиксная)
    Queue output;
    initQueue(&output);
    Stack ops;
    initStack(&ops);

    while (!isEmptyQueue(&tokens)) {
        Token t = dequeue(&tokens);

        if (t.type == TOKEN_NUMBER || t.type == TOKEN_VARIABLE) {
            enqueue(&output, t);
        } else if (t.type == TOKEN_FUNCTION) {
            push(&ops, t);
        } else if (t.type == TOKEN_OPERATOR) {
            while (!isEmptyStack(&ops) && peekStack(&ops).type == TOKEN_OPERATOR) {
                Token top = peekStack(&ops);
                int precT = getPrecedence(t.str[0]);
                int precTop = getPrecedence(top.str[0]);

                if ((!isRightAssociative(t.str[0]) && precT <= precTop) ||
                    (isRightAssociative(t.str[0]) && precT < precTop)) {
                    enqueue(&output, pop(&ops));
                } else {
                    break;
                }
            }
            push(&ops, t);
        } else if (t.type == TOKEN_LPAREN) {
            push(&ops, t);
        } else if (t.type == TOKEN_RPAREN) {
            while (!isEmptyStack(&ops) && peekStack(&ops).type != TOKEN_LPAREN) {
                enqueue(&output, pop(&ops));
            }
            if (!isEmptyStack(&ops) && peekStack(&ops).type == TOKEN_LPAREN) {
                pop(&ops); // Выбрасываем '('
            }
            if (!isEmptyStack(&ops) && peekStack(&ops).type == TOKEN_FUNCTION) {
                enqueue(&output, pop(&ops));
            }
        }
    }

    while (!isEmptyStack(&ops)) {
        enqueue(&output, pop(&ops));
    }

    // 4. Вычисление постфиксного выражения
    Stack eval;
    initStack(&eval);

    while (!isEmptyQueue(&output)) {
        Token t = dequeue(&output);

        if (t.type == TOKEN_NUMBER) {
            push(&eval, t);
        } else if (t.type == TOKEN_VARIABLE) {
            Token numToken = {TOKEN_NUMBER, getVariableValue(t.str), ""};
            push(&eval, numToken);
        } else if (t.type == TOKEN_OPERATOR) {
            char op = t.str[0];
            if (op == '!' || op == '_') {
                double a = pop(&eval).value;
                double res = 0;
                if (op == '!') res = tgamma(a + 1); // Факториал через Гамма-функцию для вещественных чисел
                else if (op == '_') res = -a;       // Унарный минус
                Token resToken = {TOKEN_NUMBER, res, ""};
                push(&eval, resToken);
            } else {
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
        } else if (t.type == TOKEN_FUNCTION) {
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

    if (!isEmptyStack(&eval)) {
        printf("Результат: %f\n", pop(&eval).value);
    } else {
        printf("Ошибка: некорректное выражение.\n");
    }

    return 0;
}