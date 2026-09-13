#include <stdio.h>    // printf, fprintf, fgets, stderr
#include <stdlib.h>   // malloc, realloc, calloc, free, exit
#include <string.h>   // strlen, strcmp, strtok, memset
#include <ctype.h>    // isspace
#include <errno.h>    // errno (подключён; в коде почти не используется)

// Пример входа (level-order / BFS-массив дерева):
// [19, 18, 32, 15, NULL, 25, 53, 13, 17, NULL, NULL, 23, 27, 35, 60, 3, 14, 16, NULL, NULL, NULL, NULL, NULL, 20, 24, 26, 31, 34, 51, 56, 61, 1, 8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 21, NULL, NULL, NULL, NULL, 30, NULL, 33, NULL, 48, 52, 55, 58, NULL, 62]

// Для старых MSVC 
#ifdef _MSC_VER              // компилятор Microsoft Visual C++
#if _MSC_VER < 1900          // VS до 2015: нет нормального snprintf
#define snprintf _snprintf   // подмена имени функции макросом
#endif
#endif

#define INITIAL_BUF 4096      // размер буфера под строку ввода
#define MAX_CANVAS_WIDTH 180  // ограничение ширины для компактного вывода 

// узел бинарного дерева; struct Node * — указатель на такой же тип (рекурсия)
typedef struct Node {
    long long val;         // значение в узле
    struct Node *left;     // левый ребёнок
    struct Node *right;    // правый ребёнок
} Node;  // typedef даёт короткое имя Node

// Утилиты всякие 

// убрать пробелы слева и справа; меняет строку на месте
static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;  // сдвиг указателя вправо по пробелам
    if (*s == '\0') return s;                // пустая строка после trim
    {
        char *e = s + strlen(s) - 1;  // указатель на последний символ
        while (e > s && isspace((unsigned char)*e)) e--;  // идём влево по хвостовым пробелам
        e[1] = '\0';  // обрезаем строку: ставим конец после последнего непробела
    }
    return s;  // указатель на начало «очищенной» строки
}

// токен означает отсутствие узла?
static int is_null_token(const char *s) {
    return strcmp(s, "NULL") == 0 || strcmp(s, "null") == 0;  // || — логическое ИЛИ
}

// создать узел в куче; exit(1) при нехватке памяти
static Node *new_node(long long v) {
    Node *n = (Node *)malloc(sizeof(Node));  // sizeof(Node) — байты под один узел
    if (!n) {
        fprintf(stderr, "Not enough memory.\n");
        exit(1);  // аварийный выход из программы
    }
    n->val = v; n->left = n->right = NULL; return n;  // цепочка присваиваний: оба ребёнка = NULL
}

// рекурсивное освобождение дерева (постпорядок: дети, потом корень)
static void free_tree(Node *root) {
    if (!root) return;  // база рекурсии
    free_tree(root->left); free_tree(root->right); free(root);
}

// высота дерева: 0 для пустого, иначе 1 + max(высот детей)
static int tree_height(Node *root) {
    if (!root) return 0;
    {
        int hl = tree_height(root->left);
        int hr = tree_height(root->right);
        return (hl > hr ? hl : hr) + 1;  // ?: — выбрать большее
    }
}

// Совместимый парсинг long long для старого MSVC 
static int parse_ll(const char *s, long long *out) {
    char *endp = NULL;  // куда парсер положит конец разобранного участка

#if defined(_MSC_VER)
    __int64 v = _strtoi64(s, &endp, 10);  // MSVC: 64-бит из строки, base 10
#else
    long long v = strtoll(s, &endp, 10);  // POSIX/стандарт: то же самое
#endif

    if (s == endp) return 0; // не было цифр 

    while (*endp && isspace((unsigned char)*endp)) endp++;  // хвостовые пробелы ок
    if (*endp != '\0') return 0; // лишние символы 

    *out = (long long)v;  // записать результат через указатель
    return 1;             // успех
}

// Динамические массивы парсинга 

// разобранный level-order список: значения + флаги NULL
typedef struct {
    long long *values;       // числа (для NULL можно 0)
    unsigned char *is_null;  // 1 = NULL-токен, 0 = число
    size_t size;             // сколько элементов занято
    size_t cap;              // выделенная ёмкость
} Parsed;

static void parsed_init(Parsed *p) {
    p->values = NULL;
    p->is_null = NULL;
    p->size = 0;
    p->cap = 0;
}

// добавить элемент; при заполнении — удвоить ёмкость (realloc)
static void parsed_push(Parsed *p, long long v, unsigned char nul) {
    if (p->size == p->cap) {
        size_t ncap = (p->cap == 0) ? 16 : p->cap * 2;  // старт 16, потом ×2
        long long *nv = (long long *)realloc(p->values, ncap * sizeof(long long));
        unsigned char *nn = (unsigned char *)realloc(p->is_null, ncap * sizeof(unsigned char));
        if (!nv || !nn) {
            free(nv); free(nn); fprintf(stderr, "Storage err.\n");
            exit(1);
        }
        p->values = nv;
        p->is_null = nn;
        p->cap = ncap;
    }
    p->values[p->size] = v;
    p->is_null[p->size] = nul;
    p->size++;  // следующий свободный индекс
}

static void parsed_free(Parsed *p) {
    free(p->values); free(p->is_null);  // free(NULL) безопасен
}

// Рисование на полотне 

// вписать строку s в canvas[r], центрируя вокруг колонки c
static void put_str(char **canvas, int rows, int cols, int r, int c, const char *s) {
    int len; int start; int i;

    if (r < 0 || r >= rows) return;  // строка вне полотна
    len = (int)strlen(s);

    start = c - len / 2;             // сдвиг влево на половину длины → центр
    if (start < 0) start = 0;        // не выходим за левый край
    if (start + len > cols) start = cols - len;  // и за правый
    if (start < 0) return;           // число длиннее всей ширины

    for (i = 0; i < len; i++) {
        int cc = start + i;
        if (cc >= 0 && cc < cols) canvas[r][cc] = s[i];  // canvas[r] — указатель на строку r
    }
}

// рекурсивно нарисовать поддерево в полосе [left, right] на строке r
// min_gap — минимальный «размах» ветвей
static void draw_tree(Node *node, char **canvas, int rows, int cols, int r, int left, int right, int min_gap) {
    int mid, next_row, span, half;
    char buf[64];  // буфер под текстовое представление числа

    if (!node || r >= rows || left > right) return;  // нечего / некуда рисовать

    mid = (left + right) / 2;                        // центр текущей полосы
    snprintf(buf, sizeof(buf), "%lld", node->val);  // число → строка (безопасно по размеру)
    put_str(canvas, rows, cols, r, mid, buf);

    next_row = r + 2;  // дети на 2 строки ниже (между ними слой со / \)
    if (next_row >= rows) return;

    span = right - left + 1;  // ширина полосы
    half = span / 2;
    if (half < min_gap) half = min_gap;  // не сжимать ветви слишком сильно

    if (node->left) {
        int lmid = mid - half / 2;  // центр полосы левого ребёнка
        int bcol, x;

        if (lmid < left) lmid = left;

        // Слеш прямо над левым ребенком 
        bcol = lmid;

        // Горизонтальная линия '_' от ребенка к родителю 
        for (x = lmid + 1; x < mid; x++) {
            if (r >= 0 && r < rows && x >= 0 && x < cols && canvas[r][x] == ' ')
                canvas[r][x] = '_';
        }

        if (r + 1 < rows && bcol >= 0 && bcol < cols) canvas[r + 1][bcol] = '/';
        // рекурсия: левая половина [left, mid-1]
        draw_tree(node->left, canvas, rows, cols, next_row, left, mid - 1, min_gap);
    }

    if (node->right) {
        int rmid = mid + half / 2;  // центр полосы правого ребёнка
        int bcol, x;

        if (rmid > right) rmid = right;

        // Бэкслеш прямо над правым ребенком 
        bcol = rmid;

        // Горизонтальная линия '_' от родителя к ребенку 
        for (x = mid + 1; x < rmid; x++) {
            if (r >= 0 && r < rows && x >= 0 && x < cols && canvas[r][x] == ' ')
                canvas[r][x] = '_';
        }

        // '\\' в исходнике — один символ \ (экранирование в C-строке/символе)
        if (r + 1 < rows && bcol >= 0 && bcol < cols) canvas[r + 1][bcol] = '\\';
        draw_tree(node->right, canvas, rows, cols, next_row, mid + 1, right, min_gap);
    }
}

// печать полотна; хвостовые пробелы каждой строки обрезаются
static void print_canvas(char **canvas, int rows, int cols) {
    int r;
    for (r = 0; r < rows; r++) {
        int end = cols - 1;
        while (end >= 0 && canvas[r][end] == ' ') end--;  // найти последний непробел
        if (end < 0) {
            printf("\n");  // полностью пустая строка
        } else {
            canvas[r][end + 1] = '\0';  // временно обрезать для printf
            printf("%s\n", canvas[r]);
        }
    }
}

// Мэйник 

int main(void) {
    char *input = (char *)malloc(INITIAL_BUF);  // буфер ввода
    char *s; size_t len; Parsed p;              // рабочая строка, длина, разобранный список
    char *token; Node **nodes; Node *root;      // токен, массив узлов, корень
    int h; int rows; int cols; char **canvas; int r;  // высота, размеры полотна, индекс строки

    if (!input) {
        fprintf(stderr, "Storage err.\n"); return 1;
    }

    printf("Enter like [1, 2, 3, NULL, 5]:\n");

    // fgets читает строку (с \n); NULL = EOF/ошибка
    if (!fgets(input, INITIAL_BUF, stdin)) {
        fprintf(stderr, "Read err.\n"); free(input); return 1;
    }

    s = trim(input); len = strlen(s);

    // ожидаем вид [ ... ]
    if (len < 2 || s[0] != '[' || s[len - 1] != ']') {
        fprintf(stderr, "Format err.\n"); free(input); return 1;
    }

    s[len - 1] = '\0'; // remove ']' 
    s++;               // skip '['  — теперь s указывает внутрь скобок

    parsed_init(&p);

    token = strtok(s, ",");  // первый токен; strtok портит строку (ставит '\0')
    while (token) {
        char *t = trim(token);

        if (*t == '\0' || is_null_token(t)) {
            parsed_push(&p, 0, 1);  // NULL-узел: значение 0, флаг is_null=1
        } else {
            long long v;
            if (!parse_ll(t, &v)) {
                fprintf(stderr, "NOcorrect num: %s\n", t);
                parsed_free(&p); free(input); return 1;
            }
            parsed_push(&p, v, 0);  // обычное число, is_null=0
        }

        token = strtok(NULL, ",");  // NULL = продолжить разбор той же строки
    }

    // пустой список или корень NULL → пустое дерево
    if (p.size == 0 || p.is_null[0]) {
        printf("Hollow tree.\n");
        parsed_free(&p); free(input); return 0;
    }

    // calloc — выделить и обнулить (все указатели = NULL)
    nodes = (Node **)calloc(p.size, sizeof(Node *));
    if (!nodes) {
        fprintf(stderr, "Storage err.\n");
        parsed_free(&p); free(input); return 1;
    }

    {
        size_t i;
        // создать узлы только для ненулевых токенов
        for (i = 0; i < p.size; i++) {
            if (!p.is_null[i]) nodes[i] = new_node(p.values[i]);
        }

        // связать детей по формуле кучи: left=2i+1, right=2i+2
        for (i = 0; i < p.size; i++) {
            if (!nodes[i]) continue;  // NULL-токен — узла нет
            {
                size_t li = 2 * i + 1;
                size_t ri = 2 * i + 2;
                if (li < p.size) nodes[i]->left = nodes[li];   // может быть NULL
                if (ri < p.size) nodes[i]->right = nodes[ri];
            }
        }
    }

    root = nodes[0];
    h = tree_height(root);

    rows = h * 2 - 1;  // строки узлов + строки связей между ними
    if (rows < 1) rows = 1;

    // Базовая ширина + ограничение для компактности 
    // 1 << (h+1) — степень двойки: 2^(h+1)
    cols = 1 << (h + 1);
    if (cols < 40) cols = 40;
    if (cols > MAX_CANVAS_WIDTH) cols = MAX_CANVAS_WIDTH;

    // массив указателей на строки полотна
    canvas = (char **)malloc((size_t)rows * sizeof(char *));
    if (!canvas) {
        fprintf(stderr, "Storage err.\n");
        free_tree(root); free(nodes); parsed_free(&p);
        free(input); return 1;
    }

    for (r = 0; r < rows; r++) {
        canvas[r] = (char *)malloc((size_t)cols + 1);  // +1 под '\0'
        if (!canvas[r]) {
            int k;
            fprintf(stderr, "Storage err.\n");
            for (k = 0; k < r; k++) free(canvas[k]);  // откат уже выделенных строк
            free(canvas); free_tree(root); free(nodes);
            parsed_free(&p); free(input); return 1;
        }
        memset(canvas[r], ' ', (size_t)cols);  // заполнить пробелами
        canvas[r][cols] = '\0';                // завершить C-строку
    }

    // min_gap=3 -> чуть "воздушнее", подчеркивания выглядят лучше 
    draw_tree(root, canvas, rows, cols, 0, 0, cols - 1, 3);
    print_canvas(canvas, rows, cols);
    for (r = 0; r < rows; r++) free(canvas[r]);  // сначала строки
    free(canvas); free_tree(root); free(nodes);   // потом массив указателей и дерево
    parsed_free(&p); free(input); return 0;
}
