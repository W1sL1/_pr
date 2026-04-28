#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 1000 // Максимальное количество узлов
#define INF -1234567 // Метка для NULL (специальное значение)

typedef struct Node { // Структура узла дерева
    int data; // Данные узла
    struct Node *left; // Левый потомок
    struct Node *right; // Правый потомок
} Node;

// --- Базовые функции дерева ---

Node* createNode(int value) { // Создание нового узла
    if (value == INF) return NULL; // Если значение INF, узел не создается
    Node* newNode = (Node*)malloc(sizeof(Node)); // Выделение памяти
    newNode->data = value; // Запись данных
    newNode->left = newNode->right = NULL; // Инициализация потомков
    return newNode; // Возврат узла
}

// Построение дерева из массива (BFS-структура)
Node* buildTree(int arr[], int n, int i) { // Рекурсивное построение дерева
    if (i >= n || arr[i] == INF) return NULL; // Базовый случай: выход за границы или INF
    Node* root = createNode(arr[i]); // Создание корня поддерева
    root->left = buildTree(arr, n, 2 * i + 1); // Левый потомок (индекс 2*i+1)
    root->right = buildTree(arr, n, 2 * i + 2); // Правый потомок (индекс 2*i+2)
    return root; // Возврат построенного поддерева
}

int getHeight(Node* root) { // Вычисление высоты дерева
    if (!root) return -1; // Пустое дерево имеет высоту -1
    int l = getHeight(root->left); // Высота левого поддерева
    int r = getHeight(root->right); // Высота правого поддерева
    return (l > r ? l : r) + 1; // Максимальная высота + 1
}

int getSize(Node* root) { // Вычисление количества узлов
    if (!root) return 0; // Пустое дерево размера 0
    return 1 + getSize(root->left) + getSize(root->right); // 1 + размеры поддеревьев
}

int countLeaves(Node* root) { // Подсчет листьев
    if (!root) return 0; // Нет узла - нет листьев
    if (!root->left && !root->right) return 1; // Если нет потомков, это лист
    return countLeaves(root->left) + countLeaves(root->right); // Сумма листьев в поддеревьях
}

void postOrder(Node* root, int* first) { // Постфиксный обход (левый, правый, корень)
    if (!root) return; // Базовый случай
    postOrder(root->left, first); // Обход левого поддерева
    postOrder(root->right, first); // Обход правого поддерева
    if (!(*first)) printf(", "); // Управление запятыми в выводе
    printf("%d", root->data); // Вывод данных узла
    *first = 0; // Сброс флага первого элемента
}

int getSum(Node* root) { // Сумма всех значений в поддереве
    if (!root) return 0; // Базовый случай: нет узла - сумма 0
    return root->data + getSum(root->left) + getSum(root->right); // Данные + суммы поддеревьев
}

// --- Вывод поддерева в формате BFS списка ---

void printSubtreeBFS(Node* root) { // Печать поддерева в BFS-формате
    if (!root) return; // Нечего печатать для пустого дерева
    Node* queue[MAX_NODES]; // Очередь для BFS
    int values[MAX_NODES]; // Массив значений
    int is_null[MAX_NODES]; // Флаги NULL-узлов
    int head = 0, tail = 0; // Указатели головы и хвоста очереди
    
    queue[tail++] = root; // Помещаем корень в очередь
    int count = 0; // Счетчик обработанных элементов
    int last_valid = 0; // Индекс последнего не-NULL элемента

    while (head < tail && count < MAX_NODES) { // Пока очередь не пуста и не превышен лимит
        Node* curr = queue[head++]; // Извлечение из очереди
        if (curr) { // Если узел существует
            values[count] = curr->data; // Сохранение значения
            is_null[count] = 0; // Это не NULL
            last_valid = count; // Обновление последнего валидного индекса
            queue[tail++] = curr->left; // Добавление левого потомка в очередь
            queue[tail++] = curr->right; // Добавление правого потомка в очередь
        } else { // Если NULL
            is_null[count] = 1; // Пометка как NULL
        }
        count++; // Увеличение счетчика
    }

    printf("["); // Начало вывода массива
    for (int i = 0; i <= last_valid; i++) { // Цикл до последнего валидного элемента
        if (is_null[i]) printf("NULL"); // Вывод NULL
        else printf("%d", values[i]); // Вывод значения
        if (i < last_valid) printf(", "); // Запятая между элементами
    }
    printf("]\n"); // Конец вывода массива
}

// Поиск и анализ поддеревьев (In-order traversal для порядка как в примере)
void findSubtrees(Node* root, int N, int* count, int mode) { // Поиск поддеревьев с суммой < N
    if (!root) return; // Базовый случай

    // Сначала идем вглубь влево
    findSubtrees(root->left, N, count, mode); // Обход левого поддерева

    // Проверяем текущий узел (корень поддерева)
    if (getSum(root) < N) { // Если сумма поддерева меньше N
        if (mode == 0) (*count)++; // Режим подсчета: инкремент счетчика
        else printSubtreeBFS(root); // Режим печати: вывод поддерева
    }

    // Затем идем вправо
    findSubtrees(root->right, N, count, mode); // Обход правого поддерева
}

// --- Парсинг ввода ---

int main() {
    char buf[1000]; // Буфер для ввода строки
    int arr[MAX_NODES]; // Массив для хранения узлов
    int n = 0, N_val; // Счетчик элементов и значение N

    // Считывание массива
    if (fgets(buf, sizeof(buf), stdin)) { // Чтение строки с массивом
        char* token = strtok(buf, " [],\n"); // Разбиение строки на токены
        while (token) { // Пока есть токены
            if (strcmp(token, "NULL") == 0) arr[n++] = INF; // NULL -> INF
            else arr[n++] = atoi(token); // Число -> запись в массив
            token = strtok(NULL, " [],\n"); // Следующий токен
        }
    }

    // Считывание N (игнорируя букву N если она есть)
    char n_pref[2]; // Буфер для возможного префикса "N"
    if (scanf("%s %d", n_pref, &N_val) != 2) { // Пробуем считать "N значение"
        // Если ввели просто число
        N_val = atoi(n_pref); // Конвертация строки в число
    }

    Node* root = buildTree(arr, n, 0); // Построение дерева из массива

    // Вывод согласно формату
    printf("Tree height: %d\n", getHeight(root)); // Вывод высоты дерева
    printf("Tree size: %d\n", getSize(root)); // Вывод размера дерева
    printf("Leaf count: %d\n", countLeaves(root)); // Вывод количества листьев
    
    printf("Post-order traversal: ["); // Начало постфиксного обхода
    int first = 1; // Флаг первого элемента
    postOrder(root, &first); // Постфиксный обход с выводом
    printf("]\n"); // Конец обхода

    int subtreeCount = 0; // Счетчик подходящих поддеревьев
    findSubtrees(root, N_val, &subtreeCount, 0); // Первый проход: считаем количество
    printf("The subtrees: %d\n", subtreeCount); // Вывод количества поддеревьев
    findSubtrees(root, N_val, NULL, 1);          // Второй проход: печатаем поддеревья

    return 0;
}
