#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_NODES 100
#define EMPTY -11111 // Флаг для NULL значений

// Структура узла дерева
typedef struct Node {
    int data;
    struct Node *left, *right;
} Node;

// Создание нового узла
Node* createNode(int data) {
    if (data == EMPTY) return NULL;
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->left = newNode->right = NULL;
    return newNode;
}

// Функция для парсинга входной строки вида [1, 2, NULL, 3]
int parseInput(char* input, int* values) {
    int count = 0;
    char* token = strtok(input, " [],");
    while (token != NULL) {
        if (strcmp(token, "NULL") == 0 || strcmp(token, "null") == 0) {
            values[count++] = EMPTY;
        } else {
            values[count++] = atoi(token);
        }
        token = strtok(NULL, " [],");
    }
    return count;
}

// Построение дерева из массива (BFS order)
Node* buildTree(int* values, int n) {
    if (n == 0 || values[0] == EMPTY) return NULL;

    Node* root = createNode(values[0]);
    Node* queue[MAX_NODES];
    int head = 0, tail = 0;
    queue[tail++] = root;

    int i = 1;
    while (i < n) {
        Node* parent = queue[head++];

        // Левый ребенок
        if (i < n && values[i] != EMPTY) {
            parent->left = createNode(values[i]);
            queue[tail++] = parent->left;
        }
        i++;

        // Правый ребенок
        if (i < n && values[i] != EMPTY) {
            parent->right = createNode(values[i]);
            queue[tail++] = parent->right;
        }
        i++;
    }
    return root;
}

// Визуализация дерева (рекурсивный вывод "боком")
// Это наиболее надежный способ визуализации в консоли без сторонних библиотек
void printTree(Node* root, int space) {
    if (root == NULL) return;

    space += 8; // Расстояние между уровнями

    // Сначала печатаем правую сторону
    printTree(root->right, space);

    printf("\n");
    for (int i = 8; i < space; i++) printf(" ");
    
    // Вывод текущего узла
    if (root->data != EMPTY)
        printf("%d\n", root->data);

    // Затем печатаем левую сторону
    printTree(root->left, space);
}

// Красивый симметричный вывод (упрощенный вариант примера)
void visualPrint(Node* root, int level) {
    if (root == NULL) return;
    if (level == 0) printf("Визуализация структуры (повернуто на 90°):\n");
    printTree(root, 0);
}

int main() {
    char input[256];
    int values[MAX_NODES];

    printf("Введите элементы дерева в формате BFS (например, [1, 2, 3, NULL, 5]):\n");
    fgets(input, sizeof(input), stdin);

    int n = parseInput(input, values);
    Node* root = buildTree(values, n);

    printf("\n--- Результат ---\n");
    if (root == NULL) {
        printf("Дерево пустое.\n");
    } else {
        printTree(root, 0);
    }

    return 0;
}