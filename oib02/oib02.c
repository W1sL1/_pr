#define _CRT_SECURE_NO_WARNINGS
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>

#define MAX_TEXT 20000
#define MAX_WORDS 2000
#define MAX_WORD_LEN 64
#define MAX_HISTORY 100


static const wchar_t RU_UP[33] = {
        L'А', L'Б', L'В', L'Г', L'Д', L'Е', L'Ё', L'Ж', L'З', L'И', L'Й',
        L'К', L'Л', L'М', L'Н', L'О', L'П', L'Р', L'С', L'Т', L'У', L'Ф',
        L'Х', L'Ц', L'Ч', L'Ш', L'Щ', L'Ъ', L'Ы', L'Ь', L'Э', L'Ю', L'Я'
};

static wchar_t RU_FREQ_ORDER[] = L"ОЕАИНТСРВЛКМДПУЯЫЬГЗБЧЙХЖШЮЦЩЭФЪЁ";

int ru_index(wchar_t ch) {
    int i;
    ch = towupper(ch);
    for (i = 0; i < 33; i++) {
        if (RU_UP[i] == ch) return i;
    }
    return -1;
}

int is_ru(wchar_t ch) {
    return ru_index(ch) >= 0;
}

// ---------------- Хранение текста ----------------

wchar_t text[MAX_TEXT + 1];
int text_len = 0;

// ---------------- Слова ----------------

typedef struct {
    int start; // индекс в text
    int len;   // длина
} Word;

Word words[MAX_WORDS];
int word_count = 0;

void extract_words() {
    int i = 0;
    word_count = 0;
    while (i < text_len && word_count < MAX_WORDS) {
        while (i < text_len && !is_ru(text[i])) i++;
        if (i >= text_len) break;
        int start = i;
        while (i < text_len && is_ru(text[i])) i++;
        int len = i - start;
        if (len > 0 && len <= MAX_WORD_LEN) {
            words[word_count].start = start;
            words[word_count].len = len;
            word_count++;
        }
    }
}

// ---------------- Замены и история ----------------

wchar_t mapping_cur[33];                // текущее соответствие: крипто -> открытая, 0 если нет
wchar_t history[MAX_HISTORY][33];       // история состояний
int history_pos = 0;                    // номер текущего
int history_size = 1;                   // сколько реально сохранено

void mapping_init() {
    int i, j;
    for (i = 0; i < 33; i++) mapping_cur[i] = 0;
    for (i = 0; i < MAX_HISTORY; i++) {
        for (j = 0; j < 33; j++) history[i][j] = 0;
    }
    for (i = 0; i < 33; i++) history[0][i] = mapping_cur[i];
    history_pos = 0;
    history_size = 1;
}

void history_save() {
    int i;
    if (history_pos + 1 >= MAX_HISTORY) return;
    history_pos++;
    history_size = history_pos + 1;
    for (i = 0; i < 33; i++) history[history_pos][i] = mapping_cur[i];
}

void history_undo() {
    int i;
    if (history_pos == 0) {
        wprintf(L"Нет более ранних состояний.\n");
        return;
    }
    history_pos--;
    for (i = 0; i < 33; i++) mapping_cur[i] = history[history_pos][i];
    wprintf(L"Откат выполнен.\n");
}

void set_mapping(wchar_t cipher, wchar_t plain) {
    int ci = ru_index(cipher);
    int i;
    if (ci < 0) {
        wprintf(L"Неверные буквы.\n");
        return;
    }
    for (i = 0; i < 33; i++) {
        if (mapping_cur[i] == towupper(plain)) mapping_cur[i] = 0;
    }
    mapping_cur[ci] = towupper(plain);
    history_save();
}

void unset_mapping(wchar_t cipher) {
    int ci = ru_index(cipher);
    if (ci < 0) {
        wprintf(L"Неверная буква.\n");
        return;
    }
    mapping_cur[ci] = 0;
    history_save();
}

wchar_t decrypt_ch(wchar_t ch) {
    int idx = ru_index(ch);
    if (idx < 0) return ch;
    if (mapping_cur[idx] == 0) return ch;
    if (iswlower(ch)) return towlower(mapping_cur[idx]);
    return mapping_cur[idx];
}

// ---------------- Частоты ----------------

void cmd_freq() {
    long cnt[33];
    long total = 0;
    int i;
    for (i = 0; i < 33; i++) cnt[i] = 0;
    for (i = 0; i < text_len; i++) {
        int idx = ru_index(text[i]);
        if (idx >= 0) {
            cnt[idx]++;
            total++;
        }
    }
    wprintf(L"Всего русских букв: %ld\n", total);
    for (i = 0; i < 33; i++) {
        wprintf(L"%lc : %ld\n", RU_UP[i], cnt[i]);
    }
}

void cmd_suggest() {
    long cnt[33];
    int order[33];
    int i, j;
    for (i = 0; i < 33; i++) {
        cnt[i] = 0;
        order[i] = i;
    }
    for (i = 0; i < text_len; i++) {
        int idx = ru_index(text[i]);
        if (idx >= 0) cnt[idx]++;
    }
    // очень простой пузырьковый сорт по убыванию
    for (i = 0; i < 33; i++) {
        for (j = i + 1; j < 33; j++) {
            if (cnt[order[j]] > cnt[order[i]]) {
                int tmp = order[i];
                order[i] = order[j];
                order[j] = tmp;
            }
        }
    }
    wprintf(L"Предполагаемые замены (крипто -> частотный русский):\n");
    for (i = 0; i < 33; i++) {
        wprintf(L"%lc -> %lc   (%ld)\n", RU_UP[order[i]], RU_FREQ_ORDER[i], cnt[order[i]]);
    }
}

// ---------------- Печать ----------------

void cmd_show() {
    int i;
    wprintf(L"--- Криптограмма ---\n");
    for (i = 0; i < text_len; i++) putwchar(text[i]);
    wprintf(L"\n--- Текущая расшифровка ---\n");
    for (i = 0; i < text_len; i++) putwchar(decrypt_ch(text[i]));
    wprintf(L"\n");
}

// ---------------- Слова по длине ----------------

void cmd_wordslen() {
    int max_len = 0;
    int i;
    if (word_count == 0) {
        wprintf(L"Слова не найдены.\n");
        return;
    }
    for (i = 0; i < word_count; i++) {
        if (words[i].len > max_len) max_len = words[i].len;
    }
    for (int L = 1; L <= max_len; L++) {
        int any = 0;
        for (i = 0; i < word_count; i++) if (words[i].len == L) any = 1;
        if (!any) continue;
        wprintf(L"Длина %d:\n", L);
        for (i = 0; i < word_count; i++) {
            if (words[i].len != L) continue;
            int k;
            for (k = 0; k < words[i].len; k++) putwchar(text[words[i].start + k]);
            wprintf(L"\n");
        }
    }
}

// ---------------- Слова по числу неизвестных букв ----------------

int word_unknown(int idx) {
    int i, u = 0;
    for (i = 0; i < words[idx].len; i++) {
        wchar_t ch = text[words[idx].start + i];
        int ri = ru_index(ch);
        if (ri >= 0 && mapping_cur[ri] == 0) u++;
    }
    return u;
}

void cmd_wordsunk() {
    int max_u = 0;
    int i;
    if (word_count == 0) {
        wprintf(L"Слова не найдены.\n");
        return;
    }
    for (i = 0; i < word_count; i++) {
        int u = word_unknown(i);
        if (u > max_u) max_u = u;
    }
    for (int U = 0; U <= max_u; U++) {
        int any = 0;
        for (i = 0; i < word_count; i++) if (word_unknown(i) == U) any = 1;
        if (!any) continue;
        wprintf(L"Неизвестных %d:\n", U);
        for (i = 0; i < word_count; i++) {
            if (word_unknown(i) != U) continue;
            int k;
            for (k = 0; k < words[i].len; k++) putwchar(text[words[i].start + k]);
            wprintf(L"\n");
        }
    }
}

// ---------------- Печать замены ----------------

void cmd_mapping() {
    int i;
    wprintf(L"Текущие замены:\n");
    for (i = 0; i < 33; i++) {
        if (mapping_cur[i]) {
            wprintf(L"%lc -> %lc\n", RU_UP[i], mapping_cur[i]);
        }
    }
}

void cmd_menu() {
    wprintf(L"Команды:\n");
    wprintf(L"freq        - частотный анализ\n");
    wprintf(L"suggest     - подсказка по частотам\n");
    wprintf(L"show        - показать текст и расшифровку\n");
    wprintf(L"map X Y     - задать X->Y\n");
    wprintf(L"unmap X     - удалить замену для X\n");
    wprintf(L"undo        - откатить последнюю замену\n");
    wprintf(L"mapping     - показать таблицу замен\n");
    wprintf(L"wordslen    - слова по длине\n");
    wprintf(L"wordsunk    - слова по числу неизвестных букв\n");
    wprintf(L"quit        - выход\n");
}

// ---------------- main ----------------

int main() {
    wchar_t cmd[64];
    wchar_t a, b;
    int i;

    setlocale(LC_ALL, "");

    // Читаем исходный текст из файла testText.txt в текущей папке.
    FILE *f = _wfopen(L"testText.txt", L"r, ccs=UTF-8");
    if (!f) {
        wprintf(L"Не удалось открыть файл testText.txt\n");
        return 1;
    }

    text_len = 0;
    while (text_len < MAX_TEXT - 1) {
        wint_t c = fgetwc(f);
        if (c == WEOF) break;
        text[text_len++] = (wchar_t)c;
    }
    fclose(f);

    text[text_len] = 0;

    extract_words();
    mapping_init();

    wprintf(L"Символов: %d, слов: %d\n", text_len, word_count);
    cmd_menu();

    for (;;) {
        wprintf(L"\n> ");
        if (wscanf(L"%63ls", cmd) != 1) break;

        if (wcscmp(cmd, L"freq") == 0) {
            cmd_freq();
        } else if (wcscmp(cmd, L"suggest") == 0) {
            cmd_suggest();
        } else if (wcscmp(cmd, L"show") == 0) {
            cmd_show();
        } else if (wcscmp(cmd, L"map") == 0) {
            if (wscanf(L" %lc %lc", &a, &b) != 2) {
                wprintf(L"Нужно: map А Б\n");
            } else {
                set_mapping(a, b);
            }
        } else if (wcscmp(cmd, L"unmap") == 0) {
            if (wscanf(L" %lc", &a) != 1) {
                wprintf(L"Нужно: unmap А\n");
            } else {
                unset_mapping(a);
            }
        } else if (wcscmp(cmd, L"undo") == 0) {
            history_undo();
        } else if (wcscmp(cmd, L"mapping") == 0) {
            cmd_mapping();
        } else if (wcscmp(cmd, L"wordslen") == 0) {
            cmd_wordslen();
        } else if (wcscmp(cmd, L"wordsunk") == 0) {
            cmd_wordsunk();
        } else if (wcscmp(cmd, L"quit") == 0) {
            break;
        } else {
            wprintf(L"Неизвестная команда.\n");
        }
    }

    return 0;
}
