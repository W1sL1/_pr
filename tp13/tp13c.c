#include <stdio.h> // printf, fgets
#include <stdlib.h> // malloc
#include <string.h> // strtok, strcmp
#define MAX_NODES 1000 // макс. узлов в очереди BFS и в массиве ввода
#define INF -1234567 // Метка для NULL

typedef struct Node {
    int data; // значение в узле
    struct Node *left; // левый потомок
    struct Node *right; // правый потомок
} Node;

// --- Базовые функции дерева ---
Node* createNode(int value) { // узел из числа; INF → NULL
    if (value == INF) return NULL; // пропускаем «дыру» в массиве
    Node* newNode = (Node*)malloc(sizeof(Node)); // выделение под узел
    newNode->data = value; // записываем значение
    newNode->left = newNode->right = NULL; // листья пока нет
    return newNode; // готовый узел
}

// Построение дерева из массива (BFS-структура)
Node* buildTree(int arr[], int n, int i) { // рекурсия по индексу i в куче
    if (i >= n || arr[i] == INF) return NULL; // вне массива или NULL
    Node* root = createNode(arr[i]); // корень поддерева
    root->left = buildTree(arr, n, 2 * i + 1); // левый: индекс 2*i+1
    root->right = buildTree(arr, n, 2 * i + 2); // правый: индекс 2*i+2
    return root; // собранное поддерево
}

int getHeight(Node* root) { // высота: пустое дерево → -1
    if (!root) return -1; // нет узла — «ниже листа»
    int l = getHeight(root->left); // высота левого
    int r = getHeight(root->right); // высота правого
    return (l > r ? l : r) + 1; // max + ребро к корню
}

int getSize(Node* root) { // число узлов в дереве
    if (!root) return 0; return 1 + getSize(root->left) + getSize(root->right);} // 1 + лево + право

int countLeaves(Node* root) { // количество листьев
    if (!root) return 0; // пусто — листов нет
    if (!root->left && !root->right) return 1; // оба сына NULL — лист
    return countLeaves(root->left) + countLeaves(root->right); // сумма в поддеревьях
}

void postOrder(Node* root, int* first) { // обход: лево, право, корень; first — запятая
    if (!root) return; // база рекурсии
    postOrder(root->left, first); // левое поддерево
    postOrder(root->right, first); // правое поддерево
    if (!(*first)) printf(", "); // разделитель после первого числа
    printf("%d", root->data); // печать значения узла
    *first = 0; // следующие элементы с запятой
}

int getSum(Node* root) { // сумма data по всем узлам
    if (!root) return 0; return root->data + getSum(root->left) + getSum(root->right);} // корень + дети

// --- Вывод поддерева в формате BFS списка ---
void printSubtreeBFS(Node* root) { // [v0, v1, NULL, ...] обход в ширину
    if (!root) return; // нечего печатать
    Node* queue[MAX_NODES]; int values[MAX_NODES]; // очередь указателей и значения по слоям
    int is_null[MAX_NODES]; int head = 0, tail = 0; // метки NULL и границы очереди
    
    queue[tail++] = root; int count = 0; int last_valid = 0; // старт BFS с корня

    while (head < tail && count < MAX_NODES) { // пока очередь не пуста
        Node* curr = queue[head++]; // извлекаем фронт
        if (curr) {
            values[count] = curr->data; // сохраняем значение
            is_null[count] = 0; // узел реальный
            last_valid = count; // обрезаем хвост по последнему значению
            queue[tail++] = curr->left; // в очередь левый (может NULL)
            queue[tail++] = curr->right; // в очередь правый
        } else is_null[count] = 1; // плейсхолдер NULL в массиве
        count++; // следующая позиция BFS
    }

    printf("["); // начало списка
    for (int i = 0; i <= last_valid; i++) { // только до last_valid
        if (is_null[i]) printf("NULL"); // пустая ячейка
        else printf("%d", values[i]); // число узла
        if (i < last_valid) printf(", "); // запятая между элементами
    }
    printf("]\n"); // конец строки
}

// Поиск и анализ поддеревьев (In-order traversal для порядка как в примере)
void findSubtrees(Node* root, int N, int* count, int mode) { // mode 0: счёт, 1: печать BFS
    if (!root) return; // пустое поддерево

    // Сначала идем вглубь влево
    findSubtrees(root->left, N, count, mode);

    // Проверяем текущий узел (корень поддерева)
    if (getSum(root) < N) { // сумма узлов поддерева меньше порога
        if (mode == 0) (*count)++; // первый проход — только счётчик
        else printSubtreeBFS(root); // второй проход — печать массива
    }

    // Затем идем вправо
    findSubtrees(root->right, N, count, mode);
}

// --- Парсинг ввода ---
int main() { // чтение дерева, статистика, поддеревья с суммой < N
    char buf[1000]; int arr[MAX_NODES]; int n = 0, N_val; // буфер строки, куча, размер, порог N

    // Считывание массива
    if (fgets(buf, sizeof(buf), stdin)) { // первая строка: [a, b, NULL, ...]
        char* token = strtok(buf, " [],\n"); // первый токен
        while (token) { // пока есть токены
            if (strcmp(token, "NULL") == 0) arr[n++] = INF; // NULL → метка INF
            else arr[n++] = atoi(token); // иначе целое значение
            token = strtok(NULL, " [],\n"); // следующий токен
        }
    }

    // Считывание N (игнорируя букву N если она есть)
    char n_pref[2]; // буфер под "N" или число
    if (scanf("%s %d", n_pref, &N_val) != 2) N_val = atoi(n_pref); // формат "N 42" или просто число
    Node* root = buildTree(arr, n, 0); // дерево из массива с индекса 0

    // Вывод согласно формату
    printf("Tree height: %d\n", getHeight(root)); // высота
    printf("Tree size: %d\n", getSize(root)); // число узлов
    printf("Leaf count: %d\n", countLeaves(root)); // листья
    printf("Post-order traversal: ["); // заголовок post-order
    int first = 1; // флаг «первый элемент без запятой»
    postOrder(root, &first); // печать обхода
    printf("]\n"); // закрывающая скобка
    int subtreeCount = 0; // счётчик подходящих поддеревьев
    findSubtrees(root, N_val, &subtreeCount, 0); // Первый проход: считаем
    printf("The subtrees: %d\n", subtreeCount); // итог счёта
    findSubtrees(root, N_val, NULL, 1);          // Второй проход: печатаем
    return 0; // успешное завершение
}
