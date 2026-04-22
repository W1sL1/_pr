#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 4096
#define MAX_NODES 1024

typedef struct Node {
    int val;
    struct Node *left;
    struct Node *right;
} Node;

/* ---------- Утилиты ---------- */

static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;

    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return s;
}

static int is_null_token(const char *s) {
    return strcmp(s, "NULL") == 0 || strcmp(s, "null") == 0;
}

static Node *new_node(int v) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) {
        fprintf(stderr, "Ошибка: не хватает памяти.\n");
        exit(1);
    }
    n->val = v;
    n->left = n->right = NULL;
    return n;
}

static int tree_height(Node *root) {
    if (!root) return 0;
    int hl = tree_height(root->left);
    int hr = tree_height(root->right);
    return (hl > hr ? hl : hr) + 1;
}

static void free_tree(Node *root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

/* ---------- Рисование в "полотно" ---------- */

static void put_str(char **canvas, int rows, int cols, int r, int c, const char *s) {
    if (r < 0 || r >= rows) return;
    int len = (int)strlen(s);
    int start = c - len / 2;
    for (int i = 0; i < len; i++) {
        int cc = start + i;
        if (cc >= 0 && cc < cols) canvas[r][cc] = s[i];
    }
}

static void draw_tree(Node *node, char **canvas, int rows, int cols, int r, int left, int right) {
    if (!node || left > right || r >= rows) return;

    int mid = (left + right) / 2;

    char buf[32];
    snprintf(buf, sizeof(buf), "%d", node->val);
    put_str(canvas, rows, cols, r, mid, buf);

    int next_row = r + 2;
    if (next_row >= rows) return;

    if (node->left) {
        int lmid = (left + mid - 1) / 2;
        if (r + 1 < rows) {
            int slash_col = (mid + lmid) / 2;
            if (slash_col >= 0 && slash_col < cols) canvas[r + 1][slash_col] = '/';
        }
        draw_tree(node->left, canvas, rows, cols, next_row, left, mid - 1);
    }

    if (node->right) {
        int rmid = (mid + 1 + right) / 2;
        if (r + 1 < rows) {
            int slash_col = (mid + rmid) / 2;
            if (slash_col >= 0 && slash_col < cols) canvas[r + 1][slash_col] = '\\';
        }
        draw_tree(node->right, canvas, rows, cols, next_row, mid + 1, right);
    }
}

static void print_canvas(char **canvas, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
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

/* ---------- Парсинг и построение ---------- */

int main(void) {
    char input[MAX_INPUT];

    printf("Введите список в формате [1, 2, 3, NULL, 5]:\n");
    if (!fgets(input, sizeof(input), stdin)) {
        fprintf(stderr, "Ошибка чтения ввода.\n");
        return 1;
    }

    char *s = trim(input);
    size_t len = strlen(s);
    if (len < 2 || s[0] != '[' || s[len - 1] != ']') {
        fprintf(stderr, "Неверный формат. Ожидается: [ ... ]\n");
        return 1;
    }

    s[len - 1] = '\0';    // убираем ']'
    s++;                  // пропускаем '['

    int values[MAX_NODES];
    int is_null[MAX_NODES];
    int n = 0;

    char *token = strtok(s, ",");
    while (token && n < MAX_NODES) {
        char *t = trim(token);
        if (*t == '\0' || is_null_token(t)) {
            is_null[n] = 1;
            values[n] = 0;
        } else {
            is_null[n] = 0;
            values[n] = atoi(t);
        }
        n++;
        token = strtok(NULL, ",");
    }

    if (n == 0 || is_null[0]) {
        printf("Пустое дерево.\n");
        return 0;
    }

    Node *nodes[MAX_NODES] = {0};

    for (int i = 0; i < n; i++) {
        if (!is_null[i]) nodes[i] = new_node(values[i]);
    }

    for (int i = 0; i < n; i++) {
        if (!nodes[i]) continue;
        int li = 2 * i + 1;
        int ri = 2 * i + 2;
        if (li < n) nodes[i]->left = nodes[li];
        if (ri < n) nodes[i]->right = nodes[ri];
    }

    Node *root = nodes[0];
    int h = tree_height(root);

    int rows = h * 2 - 1;
    int cols = (1 << (h + 2));  // запас по ширине
    if (cols < 32) cols = 32;

    char **canvas = (char **)malloc(rows * sizeof(char *));
    if (!canvas) {
        fprintf(stderr, "Ошибка памяти.\n");
        free_tree(root);
        return 1;
    }

    for (int r = 0; r < rows; r++) {
        canvas[r] = (char *)malloc(cols + 1);
        if (!canvas[r]) {
            fprintf(stderr, "Ошибка памяти.\n");
            for (int k = 0; k < r; k++) free(canvas[k]);
            free(canvas);
            free_tree(root);
            return 1;
        }
        memset(canvas[r], ' ', cols);
        canvas[r][cols] = '\0';
    }

    draw_tree(root, canvas, rows, cols, 0, 0, cols - 1);
    print_canvas(canvas, rows, cols);

    for (int r = 0; r < rows; r++) free(canvas[r]);
    free(canvas);
    free_tree(root);
    return 0;
}