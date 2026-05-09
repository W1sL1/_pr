#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifndef MY_PI
#define MY_PI 3.14159265358979323846
#endif

#ifndef MY_PI_05
#define MY_PI_05 1.57079632679489661923
#endif

#define MAX_VARIABLES 100
#define MAX_TOKEN_LENGTH 100

typedef struct StringNode {
    char data[MAX_TOKEN_LENGTH];
    struct StringNode* next;
} StringNode;

typedef struct {
    StringNode* head;
    StringNode* tail;
} Queue;

typedef struct {
    StringNode* top;
} StringStack;

typedef struct {
    char name[MAX_TOKEN_LENGTH];
    double value;
} Variable;

typedef struct NumberNode {
    double value;
    struct NumberNode* next;
} NumberNode;

typedef struct {
    NumberNode* top;
} NumberStack;

Variable variables[MAX_VARIABLES];
int variableCount = 0;

void initNumberStack(NumberStack* stack) {
    stack->top = NULL;
}

void pushNumber(NumberStack* stack, double value) {
    NumberNode* newNode = (NumberNode*)malloc(sizeof(NumberNode));
    newNode->value = value;
    newNode->next = stack->top;
    stack->top = newNode;
}

double popNumber(NumberStack* stack) {
    if (stack->top == NULL) return 0;
    NumberNode* temp = stack->top;
    double value = temp->value;
    stack->top = stack->top->next;
    free(temp);
    return value;
}

int isNumberStackEmpty(NumberStack* stack) {
    return stack->top == NULL;
}

void initStringStack(StringStack* stack) {
    stack->top = NULL;
}

void pushString(StringStack* stack, const char* value) {
    StringNode* newNode = (StringNode*)malloc(sizeof(StringNode));
    strcpy(newNode->data, value);
    newNode->next = stack->top;
    stack->top = newNode;
}

char* popString(StringStack* stack) {
    if (stack->top == NULL) return NULL;
    StringNode* temp = stack->top;
    char* value = (char*)malloc(MAX_TOKEN_LENGTH * sizeof(char));
    strcpy(value, temp->data);
    stack->top = stack->top->next;
    free(temp);
    return value;
}

char* peekString(StringStack* stack) {
    if (stack->top == NULL) return NULL;
    return stack->top->data;
}

int isStringStackEmpty(StringStack* stack) {
    return stack->top == NULL;
}

void initQueue(Queue* queue) {
    queue->head = queue->tail = NULL;
}

void enqueue(Queue* queue, const char* value) {
    StringNode* newNode = (StringNode*)malloc(sizeof(StringNode));
    strcpy(newNode->data, value);
    newNode->next = NULL;

    if (queue->tail == NULL) {
        queue->head = queue->tail = newNode;
    }
    else {
        queue->tail->next = newNode;
        queue->tail = newNode;
    }
}

char* dequeue(Queue* queue) {
    if (queue->head == NULL) return NULL;
    StringNode* temp = queue->head;
    char* value = (char*)malloc(MAX_TOKEN_LENGTH * sizeof(char));
    strcpy(value, temp->data);
    queue->head = queue->head->next;
    if (queue->head == NULL) queue->tail = NULL;
    free(temp);
    return value;
}

int isQueueEmpty(Queue* queue) {
    return queue->head == NULL;
}

void freeQueue(Queue* queue) {
    while (!isQueueEmpty(queue)) {
        char* val = dequeue(queue);
        free(val);
    }
}

int getOperatorPrecedence(const char* operator) {
    if (strcmp(operator, "+") == 0 || strcmp(operator, "-") == 0) return 1;
    if (strcmp(operator, "*") == 0 || strcmp(operator, "/") == 0) return 2;
    if (strcmp(operator, "^") == 0) return 3;
    if (strcmp(operator, "!") == 0) return 4;
    return 0;
}

int isMathFunction(const char* token) {
    return (strcmp(token, "sin") == 0 || strcmp(token, "cos") == 0 ||
        strcmp(token, "tg") == 0 || strcmp(token, "ctg") == 0 ||
        strcmp(token, "arcsin") == 0 || strcmp(token, "arccos") == 0 ||
        strcmp(token, "arctg") == 0 || strcmp(token, "arcctg") == 0 ||
        strcmp(token, "sqrt") == 0);
}

int isOperator(const char* token) {
    return (strcmp(token, "+") == 0 || strcmp(token, "-") == 0 ||
        strcmp(token, "*") == 0 || strcmp(token, "/") == 0 ||
        strcmp(token, "^") == 0 || strcmp(token, "!") == 0);
}

int isNumber(const char* token) {
    char* endptr;
    strtod(token, &endptr);
    return *endptr == '\0';
}

int isVariable(const char* token) {
    if (isdigit(token[0]) || isMathFunction(token)) return 0;
    for (int i = 0; token[i]; i++) {
        if (!isalpha(token[i])) return 0;
    }
    return 1;
}

void clearVariables(void) {
    variableCount = 0;
}

Queue* convertInfixToPostfix(const char* expression) {
    StringStack operatorStack;
    Queue* outputQueue = (Queue*)malloc(sizeof(Queue));
    initStringStack(&operatorStack);
    initQueue(outputQueue);

    char token[MAX_TOKEN_LENGTH];
    int position = 0;
    int length = strlen(expression);

    while (position < length) {
        if (isspace(expression[position])) {
            position++;
            continue;
        }

        if (isdigit(expression[position]) || isalpha(expression[position])) {
            int tokenIndex = 0;
            while (position < length && (isalnum(expression[position]) || expression[position] == '.')) {
                token[tokenIndex++] = expression[position++];
            }
            token[tokenIndex] = '\0';

            if (isMathFunction(token)) {
                pushString(&operatorStack, token);
            }
            else {
                enqueue(outputQueue, token);
            }
            continue;
        }

        if (strchr("+-*/^!", expression[position])) {
            token[0] = expression[position];
            token[1] = '\0';

            if (token[0] == '-' && (position == 0 || expression[position - 1] == '(')) {
                pushString(&operatorStack, "u-");
            }
            else {
                while (!isStringStackEmpty(&operatorStack) && isOperator(peekString(&operatorStack))) {
                    if ((getOperatorPrecedence(peekString(&operatorStack)) > getOperatorPrecedence(token)) ||
                        (getOperatorPrecedence(peekString(&operatorStack)) == getOperatorPrecedence(token) && strcmp(token, "^") != 0)) {
                        char* op = popString(&operatorStack);
                        enqueue(outputQueue, op);
                        free(op);
                    }
                    else {
                        break;
                    }
                }
                pushString(&operatorStack, token);
            }
            position++;
            continue;
        }

        if (expression[position] == '(') {
            pushString(&operatorStack, "(");
            position++;
            continue;
        }

        if (expression[position] == ')') {
            while (!isStringStackEmpty(&operatorStack) && strcmp(peekString(&operatorStack), "(") != 0) {
                char* op = popString(&operatorStack);
                enqueue(outputQueue, op);
                free(op);
            }
            if (!isStringStackEmpty(&operatorStack)) popString(&operatorStack);

            if (!isStringStackEmpty(&operatorStack) && isMathFunction(peekString(&operatorStack))) {
                char* func = popString(&operatorStack);
                enqueue(outputQueue, func);
                free(func);
            }
            position++;
            continue;
        }

        position++;
    }

    while (!isStringStackEmpty(&operatorStack)) {
        char* op = popString(&operatorStack);
        enqueue(outputQueue, op);
        free(op);
    }

    return outputQueue;
}

double getVariableValue(const char* name) {
    for (int i = 0; i < variableCount; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return variables[i].value;
        }
    }
    return 0.0;
}

double evaluatePostfixExpression(Queue* postfixQueue) {
    NumberStack valueStack;
    initNumberStack(&valueStack);
    Queue tempQueue;
    initQueue(&tempQueue);

    StringNode* current = postfixQueue->head;
    while (current != NULL) {
        enqueue(&tempQueue, current->data);
        current = current->next;
    }

    while (!isQueueEmpty(&tempQueue)) {
        char* token = dequeue(&tempQueue);

        if (isNumber(token)) {
            pushNumber(&valueStack, atof(token));
        }
        else if (isVariable(token)) {
            double value = getVariableValue(token);
            pushNumber(&valueStack, value);
        }
        else if (strcmp(token, "u-") == 0) {
            double operand = popNumber(&valueStack);
            pushNumber(&valueStack, -operand);
        }
        else if (isOperator(token)) {
            if (strcmp(token, "!") == 0) {
                double operand = popNumber(&valueStack);
                double result = tgamma(operand + 1);
                pushNumber(&valueStack, result);
            }
            else {
                double rightOperand = popNumber(&valueStack);
                double leftOperand = popNumber(&valueStack);
                double result = 0;

                if (strcmp(token, "+") == 0) result = leftOperand + rightOperand;
                else if (strcmp(token, "-") == 0) result = leftOperand - rightOperand;
                else if (strcmp(token, "*") == 0) result = leftOperand * rightOperand;
                else if (strcmp(token, "/") == 0) {
                    if (rightOperand != 0) result = leftOperand / rightOperand;
                    else {
                        printf("Error: Division by zero!\n");
                        result = 0;
                    }
                }
                else if (strcmp(token, "^") == 0) result = pow(leftOperand, rightOperand);

                pushNumber(&valueStack, result);
            }
        }
        else if (isMathFunction(token)) {
            double argument = popNumber(&valueStack);
            double result = 0;

            if (strcmp(token, "sin") == 0) result = sin(argument);
            else if (strcmp(token, "cos") == 0) result = cos(argument);
            else if (strcmp(token, "tg") == 0) result = tan(argument);
            else if (strcmp(token, "ctg") == 0) {
                if (tan(argument) != 0) result = 1.0 / tan(argument);
                else {
                    printf("Error: Cotangent is undefined when tan(x)=0\n");
                    result = 0;
                }
            }
            else if (strcmp(token, "arcsin") == 0) {
                if (argument >= -1 && argument <= 1) result = asin(argument);
                else {
                    printf("Error: Arcsin is defined only for |x| <= 1\n");
                    result = 0;
                }
            }
            else if (strcmp(token, "arccos") == 0) {
                if (argument >= -1 && argument <= 1) result = acos(argument);
                else {
                    printf("Error: Arccos is defined only for |x| <= 1\n");
                    result = 0;
                }
            }
            else if (strcmp(token, "arctg") == 0) result = atan(argument);
            else if (strcmp(token, "arcctg") == 0) result = MY_PI_05 - atan(argument);
            else if (strcmp(token, "sqrt") == 0) {
                if (argument >= 0) result = sqrt(argument);
                else {
                    printf("Error: Square root of negative number\n");
                    result = 0;
                }
            }

            pushNumber(&valueStack, result);
        }

        free(token);
    }

    double finalResult = popNumber(&valueStack);
    freeQueue(&tempQueue);

    return finalResult;
}

void findAllVariables(Queue* postfixQueue, char* foundVariables[], int* foundCount) {
    StringNode* current = postfixQueue->head;
    *foundCount = 0;

    while (current != NULL) {
        if (isVariable(current->data) && !isMathFunction(current->data)) {
            int alreadyFound = 0;
            for (int i = 0; i < *foundCount; i++) {
                if (strcmp(foundVariables[i], current->data) == 0) {
                    alreadyFound = 1;
                    break;
                }
            }
            if (!alreadyFound) {
                foundVariables[*foundCount] = (char*)malloc(MAX_TOKEN_LENGTH * sizeof(char));
                strcpy(foundVariables[*foundCount], current->data);
                (*foundCount)++;
            }
        }
        current = current->next;
    }
}

void inputVariableValues(char* variableNames[], int variableCountToInput) {
    for (int i = 0; i < variableCountToInput; i++) {
        int existingIndex = -1;
        for (int j = 0; j < variableCount; j++) {
            if (strcmp(variables[j].name, variableNames[i]) == 0) {
                existingIndex = j;
                break;
            }
        }

        printf("Enter value for variable %s: ", variableNames[i]);
        double value;
        scanf("%lf", &value);
        while (getchar() != '\n');

        if (existingIndex >= 0) {
            variables[existingIndex].value = value;
        }
        else {
            strcpy(variables[variableCount].name, variableNames[i]);
            variables[variableCount].value = value;
            variableCount++;
        }
    }
}

int main() {
    char inputExpression[1000];

    printf("Supported operations: +, -, *, /, ^, !, sqrt\n");
    printf("Supported functions: sin, cos, tg, ctg, arcsin, arccos, arctg, arcctg\n");
    printf("Variables: any alphabetic names (a, b, x, y, etc.)\n");

    while (1) {
        printf("\n> ");
        if (fgets(inputExpression, sizeof(inputExpression), stdin) == NULL) {
            break;
        }
        inputExpression[strcspn(inputExpression, "\n")] = '\0';

        if (strcmp(inputExpression, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }

        if (strlen(inputExpression) == 0) {
            continue;
        }

        clearVariables();

        Queue* postfixQueue = convertInfixToPostfix(inputExpression);

        if (postfixQueue == NULL) {
            printf("Error parsing expression!\n");
            continue;
        }

        char* foundVariables[MAX_VARIABLES];
        int foundVariableCount = 0;
        findAllVariables(postfixQueue, foundVariables, &foundVariableCount);

        if (foundVariableCount > 0) {
            printf("\nVariables detected: ");
            for (int i = 0; i < foundVariableCount; i++) {
                printf("%s ", foundVariables[i]);
            }
            printf("\n");
            inputVariableValues(foundVariables, foundVariableCount);
            for (int i = 0; i < foundVariableCount; i++) {
                free(foundVariables[i]);
            }
        }

        double result = evaluatePostfixExpression(postfixQueue);
        printf("Result: %.10lf\n", result);

        freeQueue(postfixQueue);
        free(postfixQueue);
    }

    return 0;
}