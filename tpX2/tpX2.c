#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* Для старых MSVC */
#ifdef _MSC_VER
#if _MSC_VER < 1900
#define snprintf _snprintf
#endif
#endif

#define INITIAL_BUF 4096
#define MAX_CANVAS_WIDTH 180  /* ограничение ширины для компактного вывода */

typedef struct Node {
    long long val;
    struct Node *left;
    struct Node *right;
} Node;

/* ---------- Утилиты ---------- */

static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    {
        char *e = s + strlen(s) - 1;
        while (e > s && isspace((unsigned char)*e)) e--;
        e[1] = '\0';
    }
    return s;
}

static int is_null_token(const char *s) {
    return strcmp(s, "NULL") == 0 || strcmp(s, "null") == 0;
}

static Node *new_node(long long v) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) {
        fprintf(stderr, "Ошибка: не хватает памяти.\n");
        exit(1);
    }
    n->val = v;
    n->left = n->right = NULL;
    return n;
}

static void free_tree(Node *root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

static int tree_height(Node *root) {
    if (!root) return 0;
    {
        int hl = tree_height(root->left);
        int hr = tree_height(root->right);
        return (hl > hr ? hl : hr) + 1;
    }
}

/* Совместимый парсинг long long для старого MSVC */
static int parse_ll(const char *s, long long *out) {
    char *endp = NULL;

#if defined(_MSC_VER)
    __int64 v = _strtoi64(s, &endp, 10);
#else
    long long v = strtoll(s, &endp, 10);
#endif

    if (s == endp) return 0; /* не было цифр */

    while (*endp && isspace((unsigned char)*endp)) endp++;
    if (*endp != '\0') return 0; /* лишние символы */

    *out = (long long)v;
    return 1;
}

/* ---------- Динамические массивы парсинга ---------- */

typedef struct {
    long long *values;
    unsigned char *is_null;
    size_t size;
    size_t cap;
} Parsed;

static void parsed_init(Parsed *p) {
    p->values = NULL;
    p->is_null = NULL;
    p->size = 0;
    p->cap = 0;
}

static void parsed_push(Parsed *p, long long v, unsigned char nul) {
    if (p->size == p->cap) {
        size_t ncap = (p->cap == 0) ? 16 : p->cap * 2;
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
    p->size++;
}

static void parsed_free(Parsed *p) {
    free(p->values);
    free(p->is_null);
}

/* ---------- Рисование на полотне ---------- */

static void put_str(char **canvas, int rows, int cols, int r, int c, const char *s) {
    int len;
    int start;
    int i;

    if (r < 0 || r >= rows) return;
    len = (int)strlen(s);

    start = c - len / 2;
    if (start < 0) start = 0;
    if (start + len > cols) start = cols - len;
    if (start < 0) return;

    for (i = 0; i < len; i++) {
        int cc = start + i;
        if (cc >= 0 && cc < cols) canvas[r][cc] = s[i];
    }
}

static void draw_tree(Node *node, char **canvas, int rows, int cols, int r, int left, int right, int min_gap) {
    int mid, next_row, span, half;
    char buf[64];

    if (!node || r >= rows || left > right) return;

    mid = (left + right) / 2;
    snprintf(buf, sizeof(buf), "%lld", node->val);
    put_str(canvas, rows, cols, r, mid, buf);

    next_row = r + 2;
    if (next_row >= rows) return;

    span = right - left + 1;
    half = span / 2;
    if (half < min_gap) half = min_gap;

    if (node->left) {
        int lmid = mid - half / 2;
        int bcol, x;

        if (lmid < left) lmid = left;

        /* Слеш прямо над левым ребенком */
        bcol = lmid;

        /* Горизонтальная линия '_' от ребенка к родителю */
        for (x = lmid + 1; x < mid; x++) {
            if (r >= 0 && r < rows && x >= 0 && x < cols && canvas[r][x] == ' ')
                canvas[r][x] = '_';
        }

        if (r + 1 < rows && bcol >= 0 && bcol < cols) canvas[r + 1][bcol] = '/';
        draw_tree(node->left, canvas, rows, cols, next_row, left, mid - 1, min_gap);
    }

    if (node->right) {
        int rmid = mid + half / 2;
        int bcol, x;

        if (rmid > right) rmid = right;

        /* Бэкслеш прямо над правым ребенком */
        bcol = rmid;

        /* Горизонтальная линия '_' от родителя к ребенку */
        for (x = mid + 1; x < rmid; x++) {
            if (r >= 0 && r < rows && x >= 0 && x < cols && canvas[r][x] == ' ')
                canvas[r][x] = '_';
        }

        if (r + 1 < rows && bcol >= 0 && bcol < cols) canvas[r + 1][bcol] = '\\';
        draw_tree(node->right, canvas, rows, cols, next_row, mid + 1, right, min_gap);
    }
}

static void print_canvas(char **canvas, int rows, int cols) {
    int r;
    for (r = 0; r < rows; r++) {
        int end = cols - 1;
        while (end >= 0 && canvas[r][end] == ' ') end--;
        if (end < 0) {
            printf("\n");
        } else {
            canvas[r][end + 1] = '\0';
            printf("%s\n", canvas[r]);
        }
    }
}

/* ---------- main ---------- */

int main(void) {
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

    if (len < 2 || s[0] != '[' || s[len - 1] != ']') {
        fprintf(stderr, "Неверный формат. Ожидается [ ... ]\n");
        free(input);
        return 1;
    }

    s[len - 1] = '\0'; /* remove ']' */
    s++;               /* skip '[' */

    parsed_init(&p);

    token = strtok(s, ",");
    while (token) {
        char *t = trim(token);

        if (*t == '\0' || is_null_token(t)) {
            parsed_push(&p, 0, 1);
        } else {
            long long v;
            if (!parse_ll(t, &v)) {
                fprintf(stderr, "Некорректное число: %s\n", t);
                parsed_free(&p);
                free(input);
                return 1;
            }
            parsed_push(&p, v, 0);
        }

        token = strtok(NULL, ",");
    }

    if (p.size == 0 || p.is_null[0]) {
        printf("Пустое дерево.\n");
        parsed_free(&p);
        free(input);
        return 0;
    }

    nodes = (Node **)calloc(p.size, sizeof(Node *));
    if (!nodes) {
        fprintf(stderr, "Ошибка памяти.\n");
        parsed_free(&p);
        free(input);
        return 1;
    }

    {
        size_t i;
        for (i = 0; i < p.size; i++) {
            if (!p.is_null[i]) nodes[i] = new_node(p.values[i]);
        }

        for (i = 0; i < p.size; i++) {
            if (!nodes[i]) continue;
            {
                size_t li = 2 * i + 1;
                size_t ri = 2 * i + 2;
                if (li < p.size) nodes[i]->left = nodes[li];
                if (ri < p.size) nodes[i]->right = nodes[ri];
            }
        }
    }

    root = nodes[0];
    h = tree_height(root);

    rows = h * 2 - 1;
    if (rows < 1) rows = 1;

    /* Базовая ширина + ограничение для компактности */
    cols = 1 << (h + 1);
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
            for (k = 0; k < r; k++) free(canvas[k]);
            free(canvas);
            free_tree(root);
            free(nodes);
            parsed_free(&p);
            free(input);
            return 1;
        }
        memset(canvas[r], ' ', (size_t)cols);
        canvas[r][cols] = '\0';
    }

    /* min_gap=3 -> чуть "воздушнее", подчеркивания выглядят лучше */
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
