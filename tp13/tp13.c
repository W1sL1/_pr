#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 1000
#define INF -1234567 // Метка для NULL

typedef struct Node {
    int data;
    struct Node *left;
    struct Node *right;
} Node;

// --- Базовые функции дерева ---

Node* createNode(int value) {
    if (value == INF) return NULL;
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = value;
    newNode->left = newNode->right = NULL;
    return newNode;
}

// Построение дерева из массива (BFS-структура)
Node* buildTree(int arr[], int n, int i) {
    if (i >= n || arr[i] == INF) return NULL;
    Node* root = createNode(arr[i]);
    root->left = buildTree(arr, n, 2 * i + 1);
    root->right = buildTree(arr, n, 2 * i + 2);
    return root;
}

int getHeight(Node* root) {
    if (!root) return -1;
    int l = getHeight(root->left);
    int r = getHeight(root->right);
    return (l > r ? l : r) + 1;
}

int getSize(Node* root) {
    if (!root) return 0;
    return 1 + getSize(root->left) + getSize(root->right);
}

int countLeaves(Node* root) {
    if (!root) return 0;
    if (!root->left && !root->right) return 1;
    return countLeaves(root->left) + countLeaves(root->right);
}

void postOrder(Node* root, int* first) {
    if (!root) return;
    postOrder(root->left, first);
    postOrder(root->right, first);
    if (!(*first)) printf(", ");
    printf("%d", root->data);
    *first = 0;
}

int getSum(Node* root) {
    if (!root) return 0;
    return root->data + getSum(root->left) + getSum(root->right);
}

// --- Вывод поддерева в формате BFS списка ---

void printSubtreeBFS(Node* root) {
    if (!root) return;
    Node* queue[MAX_NODES];
    int values[MAX_NODES];
    int is_null[MAX_NODES];
    int head = 0, tail = 0;
    
    queue[tail++] = root;
    int count = 0;
    int last_valid = 0;

    while (head < tail && count < MAX_NODES) {
        Node* curr = queue[head++];
        if (curr) {
            values[count] = curr->data;
            is_null[count] = 0;
            last_valid = count;
            queue[tail++] = curr->left;
            queue[tail++] = curr->right;
        } else {
            is_null[count] = 1;
        }
        count++;
    }

    printf("[");
    for (int i = 0; i <= last_valid; i++) {
        if (is_null[i]) printf("NULL");
        else printf("%d", values[i]);
        if (i < last_valid) printf(", ");
    }
    printf("]\n");
}

// Поиск и анализ поддеревьев (In-order traversal для порядка как в примере)
void findSubtrees(Node* root, int N, int* count, int mode) {
    if (!root) return;

    // Сначала идем вглубь влево
    findSubtrees(root->left, N, count, mode);

    // Проверяем текущий узел (корень поддерева)
    if (getSum(root) < N) {
        if (mode == 0) (*count)++;
        else printSubtreeBFS(root);
    }

    // Затем идем вправо
    findSubtrees(root->right, N, count, mode);
}

// --- Парсинг ввода ---

int main() {
    char buf[1000];
    int arr[MAX_NODES];
    int n = 0, N_val;

    // Считывание массива
    if (fgets(buf, sizeof(buf), stdin)) {
        char* token = strtok(buf, " [],\n");
        while (token) {
            if (strcmp(token, "NULL") == 0) arr[n++] = INF;
            else arr[n++] = atoi(token);
            token = strtok(NULL, " [],\n");
        }
    }

    // Считывание N (игнорируя букву N если она есть)
    char n_pref[2];
    if (scanf("%s %d", n_pref, &N_val) != 2) {
        // Если ввели просто число
        N_val = atoi(n_pref);
    }

    Node* root = buildTree(arr, n, 0);

    // Вывод согласно формату
    printf("Tree height: %d\n", getHeight(root));
    printf("Tree size: %d\n", getSize(root));
    printf("Leaf count: %d\n", countLeaves(root));
    
    printf("Post-order traversal: [");
    int first = 1;
    postOrder(root, &first);
    printf("]\n");

    int subtreeCount = 0;
    findSubtrees(root, N_val, &subtreeCount, 0); // Первый проход: считаем
    printf("The subtrees: %d\n", subtreeCount);
    findSubtrees(root, N_val, NULL, 1);          // Второй проход: печатаем

    return 0;
}
