#include <stdio.h>  // printf, fgets, stderr
#include <stdlib.h>  // malloc, calloc, realloc, free, exit
#include <string.h>  // strlen, strtok, strcmp, memset
#include <ctype.h>  // isspace
#include <errno.h>  // errno (на будущее при расширении ввода)

// Для старых MSVC 
#ifdef _MSC_VER  // только под Windows MSVC
#if _MSC_VER < 1900  // VS до 2015: нет стандартного snprintf
#define snprintf _snprintf  // макроподмена имени функции
#endif
#endif

#define INITIAL_BUF 4096  // стартовый размер буфера одной строки stdin
#define MAX_CANVAS_WIDTH 180  // ограничение ширины для компактного вывода 

typedef struct Node {  // узел бинарного дерева поиска (здесь — обычное BT)
    long long val;  // ключ/значение в узле
    struct Node *left;  // левый потомок
    struct Node *right;  // правый потомок
} Node;

// ---------- Утилиты ---------- 

static char *trim(char *s) {  // срез пробелов слева и справа, in-place
    while (isspace((unsigned char)*s)) s++;  // сдвиг за ведущие пробелы
    if (*s == '\0') return s;  // строка из одних пробелов
    {
        char *e = s + strlen(s) - 1;  // последний символ строки
        while (e > s && isspace((unsigned char)*e)) e--;  // отступаем от хвостовых пробелов
        e[1] = '\0';  // обрезаем хвост
    }
    return s;  // указатель на начало содержимого
}

static int is_null_token(const char *s) {  // распознать NULL/null в BFS-массиве
    return strcmp(s, "NULL") == 0 || strcmp(s, "null") == 0;
}

static Node *new_node(long long v) {  // выделить лист/внутренний узел
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) {
        fprintf(stderr, "Ошибка: не хватает памяти.\n");
        exit(1);  // фатально: дальше работать нельзя
    }
    n->val = v;
    n->left = n->right = NULL;  // пока без детей
    return n;
}

static void free_tree(Node *root) {  // постпорядковое освобождение поддерева
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

static int tree_height(Node *root) {  // высота: 0 для пустого, иначе max(детей)+1
    if (!root) return 0;
    {
        int hl = tree_height(root->left);
        int hr = tree_height(root->right);
        return (hl > hr ? hl : hr) + 1;  // +1 за текущий уровень
    }
}

// Совместимый парсинг long long для старого MSVC 
static int parse_ll(const char *s, long long *out) {  // 1=успех, 0=ошибка формата
    char *endp = NULL;

#if defined(_MSC_VER)
    __int64 v = _strtoi64(s, &endp, 10);  // MSVC-вариант strtoll
#else
    long long v = strtoll(s, &endp, 10);  // POSIX/C99
#endif

    if (s == endp) return 0; // не было цифр 

    while (*endp && isspace((unsigned char)*endp)) endp++;  // хвостовые пробелы OK
    if (*endp != '\0') return 0; // лишние символы 

    *out = (long long)v;
    return 1;  // корректное целое
}

// ---------- Динамические массивы парсинга ---------- 

typedef struct {  // плоское представление ввода до сборки дерева
    long long *values;  // числа по порядку BFS
    unsigned char *is_null;  // флаг «пустая ячейка» (1 = NULL)
    size_t size;  // сколько токенов разобрано
    size_t cap;  // ёмкость массивов
} Parsed;

static void parsed_init(Parsed *p) {  // обнулить дескриптор
    p->values = NULL;
    p->is_null = NULL;
    p->size = 0;
    p->cap = 0;
}

static void parsed_push(Parsed *p, long long v, unsigned char nul) {  // append токена
    if (p->size == p->cap) {  // нужно расширение
        size_t ncap = (p->cap == 0) ? 16 : p->cap * 2;  // геометрия x2
        long long *nv = (long long *)realloc(p->values, ncap * sizeof(long long));
        unsigned char *nn = (unsigned char *)realloc(p->is_null, ncap * sizeof(unsigned char));
        if (!nv || !nn) {
            free(nv);
            free(nn);
            fprintf(stderr, "Ошибка памяти.\n");
            exit(1);
        }
        p->values = nv;
        p->is_null = nn;
        p->cap = ncap;
    }
    p->values[p->size] = v;
    p->is_null[p->size] = nul;
    p->size++;  // новый хвост
}

static void parsed_free(Parsed *p) {  // освободить буферы разбора
    free(p->values);
    free(p->is_null);
}

// ---------- Рисование на полотне ---------- 

static void put_str(char **canvas, int rows, int cols, int r, int c, const char *s) {  // текст по центру col c
    int len;
    int start;
    int i;

    if (r < 0 || r >= rows) return;  // строка вне полотна
    len = (int)strlen(s);

    start = c - len / 2;  // центрирование подписи узла
    if (start < 0) start = 0;
    if (start + len > cols) start = cols - len;  // не вылезать вправо
    if (start < 0) return;  // слишком длинная строка для cols

    for (i = 0; i < len; i++) {
        int cc = start + i;
        if (cc >= 0 && cc < cols) canvas[r][cc] = s[i];  // печать символа
    }
}

static void draw_tree(Node *node, char **canvas, int rows, int cols, int r, int left, int right, int min_gap) {  // рекурсивная отрисовка
    int mid, next_row, span, half;
    char buf[64];

    if (!node || r >= rows || left > right) return;  // база рекурсии

    mid = (left + right) / 2;  // колонка текущего узла в отрезке [left,right]
    snprintf(buf, sizeof(buf), "%lld", node->val);
    put_str(canvas, rows, cols, r, mid, buf);  // значение на строке r

    next_row = r + 2;  // дети на строке через одну (между — «/» и «\»)
    if (next_row >= rows) return;

    span = right - left + 1;  // ширина доступного интервала
    half = span / 2;
    if (half < min_gap) half = min_gap;  // минимальный разнос веток

    if (node->left) {
        int lmid = mid - half / 2;  // колонка левого ребёнка
        int bcol, x;

        if (lmid < left) lmid = left;  // не выходить за левую границу

        // Слеш прямо над левым ребенком 
        bcol = lmid;

        // Горизонтальная линия '_' от ребенка к родителю 
        for (x = lmid + 1; x < mid; x++) {
            if (r >= 0 && r < rows && x >= 0 && x < cols && canvas[r][x] == ' ')
                canvas[r][x] = '_';  // горизонталь к родителю
        }

        if (r + 1 < rows && bcol >= 0 && bcol < cols) canvas[r + 1][bcol] = '/';
        draw_tree(node->left, canvas, rows, cols, next_row, left, mid - 1, min_gap);  // левое поддерево
    }

    if (node->right) {
        int rmid = mid + half / 2;  // колонка правого ребёнка
        int bcol, x;

        if (rmid > right) rmid = right;

        // Бэкслеш прямо над правым ребенком 
        bcol = rmid;

        // Горизонтальная линия '_' от родителя к ребенку 
        for (x = mid + 1; x < rmid; x++) {
            if (r >= 0 && r < rows && x >= 0 && x < cols && canvas[r][x] == ' ')
                canvas[r][x] = '_';
        }

        if (r + 1 < rows && bcol >= 0 && bcol < cols) canvas[r + 1][bcol] = '\\';
        draw_tree(node->right, canvas, rows, cols, next_row, mid + 1, right, min_gap);
    }
}

static void print_canvas(char **canvas, int rows, int cols) {  // вывод с обрезкой хвостовых пробелов
    int r;
    for (r = 0; r < rows; r++) {
        int end = cols - 1;
        while (end >= 0 && canvas[r][end] == ' ') end--;  // последний непробельный
        if (end < 0) {
            printf("\n");  // пустая строка полотна
        } else {
            canvas[r][end + 1] = '\0';  // временный терминатор строки
            printf("%s\n", canvas[r]);
        }
    }
}

// ---------- main ---------- 

int main(void) {  // чтение BFS-массива, сборка дерева, печать ASCII
    char *input = (char *)malloc(INITIAL_BUF);
    char *s;
    size_t len;
    Parsed p;
    char *token;
    Node **nodes;
    Node *root;
    int h;
    int rows;
    int cols;
    char **canvas;
    int r;

    if (!input) {
        fprintf(stderr, "Ошибка памяти.\n");
        return 1;
    }

    printf("Введите список в формате [1, 2, 3, NULL, 5]:\n");
    if (!fgets(input, INITIAL_BUF, stdin)) {
        fprintf(stderr, "Ошибка чтения.\n");
        free(input);
        return 1;
    }

    s = trim(input);
    len = strlen(s);

    if (len < 2 || s[0] != '[' || s[len - 1] != ']') {  // обязательные скобки
        fprintf(stderr, "Неверный формат. Ожидается [ ... ]\n");
        free(input);
        return 1;
    }

    s[len - 1] = '\0'; // remove ']' 
    s++;               // skip '[' 

    parsed_init(&p);

    token = strtok(s, ",");  // разбиение по запятым
    while (token) {
        char *t = trim(token);

        if (*t == '\0' || is_null_token(t)) {
            parsed_push(&p, 0, 1);  // пустая ячейка уровня
        } else {
            long long v;
            if (!parse_ll(t, &v)) {
                fprintf(stderr, "Некорректное число: %s\n", t);
                parsed_free(&p);
                free(input);
                return 1;
            }
            parsed_push(&p, v, 0);  // обычный узел
        }

        token = strtok(NULL, ",");
    }

    if (p.size == 0 || p.is_null[0]) {  // нет корня
        printf("Пустое дерево.\n");
        parsed_free(&p);
        free(input);
        return 0;
    }

    nodes = (Node **)calloc(p.size, sizeof(Node *));  // массив указателей по индексу BFS
    if (!nodes) {
        fprintf(stderr, "Ошибка памяти.\n");
        parsed_free(&p);
        free(input);
        return 1;
    }

    {
        size_t i;
        for (i = 0; i < p.size; i++) {
            if (!p.is_null[i]) nodes[i] = new_node(p.values[i]);  // создать только не-NULL
        }

        for (i = 0; i < p.size; i++) {
            if (!nodes[i]) continue;  // пропуск NULL-ячеек
            {
                size_t li = 2 * i + 1;  // индекс левого ребёнка в массиве
                size_t ri = 2 * i + 2;  // индекс правого
                if (li < p.size) nodes[i]->left = nodes[li];
                if (ri < p.size) nodes[i]->right = nodes[ri];
            }
        }
    }

    root = nodes[0];  // корень всегда в [0]
    h = tree_height(root);

    rows = h * 2 - 1;  // строка узла + строка связей на уровень
    if (rows < 1) rows = 1;

    // Базовая ширина + ограничение для компактности 
    cols = 1 << (h + 1);  // экспоненциальная ширина от высоты
    if (cols < 40) cols = 40;
    if (cols > MAX_CANVAS_WIDTH) cols = MAX_CANVAS_WIDTH;

    canvas = (char **)malloc((size_t)rows * sizeof(char *));
    if (!canvas) {
        fprintf(stderr, "Ошибка памяти.\n");
        free_tree(root);
        free(nodes);
        parsed_free(&p);
        free(input);
        return 1;
    }

    for (r = 0; r < rows; r++) {
        canvas[r] = (char *)malloc((size_t)cols + 1);
        if (!canvas[r]) {
            int k;
            fprintf(stderr, "Ошибка памяти.\n");
            for (k = 0; k < r; k++) free(canvas[k]);  // откат частично выделенных строк
            free(canvas);
            free_tree(root);
            free(nodes);
            parsed_free(&p);
            free(input);
            return 1;
        }
        memset(canvas[r], ' ', (size_t)cols);  // заполнить пробелами
        canvas[r][cols] = '\0';
    }

    // min_gap=3 -> чуть "воздушнее", подчеркивания выглядят лучше 
    draw_tree(root, canvas, rows, cols, 0, 0, cols - 1, 3);
    print_canvas(canvas, rows, cols);
    for (r = 0; r < rows; r++) free(canvas[r]);
    free(canvas);
    free_tree(root);
    free(nodes);
    parsed_free(&p);
    free(input);
    return 0;
}
