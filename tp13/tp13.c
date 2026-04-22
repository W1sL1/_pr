#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Структура узла бинарного дерева
typedef struct Node {
    int val;
    int sum; // Сумма всех элементов в поддереве
    struct Node* left;
    struct Node* right;
} Node;

// Рекурсивное построение дерева из массива
Node* buildTree(int* arr, int* is_null, int index, int n) {
    if (index >= n || is_null[index]) {
        return NULL;
    }
    Node* root = (Node*)malloc(sizeof(Node));
    root->val = arr[index];
    root->sum = 0;
    root->left = buildTree(arr, is_null, 2 * index + 1, n);
    root->right = buildTree(arr, is_null, 2 * index + 2, n);
    return root;
}

// Рекурсивное вычисление высоты дерева
int getHeight(Node* root) {
    if (!root) return -1;
    int leftHeight = getHeight(root->left);
    int rightHeight = getHeight(root->right);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

// Рекурсивный подсчет количества узлов
int getSize(Node* root) {
    if (!root) return 0;
    return 1 + getSize(root->left) + getSize(root->right);
}

// Рекурсивный подсчет количества листьев
int getLeafCount(Node* root) {
    if (!root) return 0;
    if (!root->left && !root->right) return 1;
    return getLeafCount(root->left) + getLeafCount(root->right);
}

// Рекурсивный постфиксный обход (post-order)
void printPostOrder(Node* root, int* is_first) {
    if (!root) return;
    printPostOrder(root->left, is_first);
    printPostOrder(root->right, is_first);
    
    if (*is_first) {
        printf("%d", root->val);
        *is_first = 0;
    } else {
        printf(", %d", root->val);
    }
}

// Рекурсивный подсчет суммы всех поддеревьев
int computeSums(Node* root) {
    if (!root) return 0;
    root->sum = root->val + computeSums(root->left) + computeSums(root->right);
    return root->sum;
}

// Рекурсивный (In-order) подсчет подходящих поддеревьев 
void countSubtrees(Node* root, int N, int* count) {
    if (!root) return;
    countSubtrees(root->left, N, count);
    if (root->sum < N) (*count)++;
    countSubtrees(root->right, N, count);
}

// Рекурсивный сбор узлов на определенном уровне (для обхода в ширину)
void getNodesAtLevel(Node* root, int level, int* arr, int* size) {
    if (!root) return;
    if (level == 0) {
        arr[(*size)++] = root->val;
    } else {
        getNodesAtLevel(root->left, level - 1, arr, size);
        getNodesAtLevel(root->right, level - 1, arr, size);
    }
}

// Обход в ширину для печати отдельного поддерева
void printSubtreeLevelOrder(Node* root) {
    int h = getHeight(root);
    int arr[1000];
    int size = 0;
    
    for (int i = 0; i <= h; i++) {
        getNodesAtLevel(root, i, arr, &size);
    }
    
    printf("[");
    for(int i = 0; i < size; i++) {
        printf("%d%s", arr[i], (i == size - 1) ? "" : ", ");
    }
    printf("]\n");
}

// Рекурсивная (In-order) печать самих поддеревьев
void printTargetSubtrees(Node* root, int N) {
    if (!root) return;
    printTargetSubtrees(root->left, N);
    if (root->sum < N) {
        printSubtreeLevelOrder(root);
    }
    printTargetSubtrees(root->right, N);
}

// Очистка памяти
void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int main() {
    int arr[10000];
    int is_null[10000];
    int n = 0;
    int c;

    // Чтение массива узлов
    while ((c = getchar()) != EOF) {
        if (c == '[') break;
    }
    
    while ((c = getchar()) != EOF) {
        if (c == ' ' || c == ',') continue;
        if (c == ']') break;
        
        if (c == 'N') { // Обработка значения NULL
            getchar(); // U
            getchar(); // L
            getchar(); // L
            is_null[n] = 1;
            arr[n] = 0;
            n++;
        } else if (c == '-' || c == '+' || (c >= '0' && c <= '9')) {
            ungetc(c, stdin);
            int val;
            if (scanf("%d", &val) == 1) {
                is_null[n] = 0;
                arr[n] = val;
                n++;
            }
        }
    }

    // Чтение числа N
    while ((c = getchar()) != EOF) {
        if (c == '-' || c == '+' || (c >= '0' && c <= '9')) {
            ungetc(c, stdin);
            break;
        }
    }
    int N_val;
    if (scanf("%d", &N_val) != 1) return 1;

    // Построение дерева
    Node* root = buildTree(arr, is_null, 0, n);

    // 1. Вывод основных параметров дерева
    printf("Tree height: %d\n", getHeight(root));
    printf("Tree size: %d\n", getSize(root));
    printf("Leaf count: %d\n", getLeafCount(root));

    // 2. Post-order обход
    printf("Post-order traversal: [");
    int is_first = 1;
    printPostOrder(root, &is_first);
    printf("]\n");

    // 3. Вычисление сумм в поддеревьях и подсчет
    computeSums(root);
    int subtree_count = 0;
    countSubtrees(root, N_val, &subtree_count);
    printf("The subtrees: %d\n", subtree_count);

    // 4. Вывод самих поддеревьев, сумма которых меньше N
    printTargetSubtrees(root, N_val);

    // Очистка
    freeTree(root);
    return 0;
}