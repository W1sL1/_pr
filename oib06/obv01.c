#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VARS 1000
#define MAX_VAR_LEN 128
#define MAX_CODE_LINES 2000
#define MAX_LINE_LEN 512

char Variables[MAX_VARS][MAX_VAR_LEN]; // Массив с именами переменных исходного кода
int numVariables = 0;                  // Количество переменных
char NewNames[MAX_VARS][MAX_VAR_LEN];  // Массив с новыми (обфусцированными) именами

// Безопасное копирование строки с ограничением по длине (bounds-checked)
// dst — куда, src — откуда, maxlen — максимальная длина (включая '\0')
static void safe_strcpy(char *dst, const char *src, size_t maxlen) {
    size_t i = 0;
    if (maxlen == 0) return;
    while (src[i] && i < maxlen - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = 0;
}

// Загрузка новых имен из файла Variables.txt
void LoadNewNames() {
    FILE *f;
    int idx = 0;
    char buf[MAX_VAR_LEN];
    f = fopen("Variables.txt", "r");
    if (f == NULL) {
        printf("Cannot open Variables.txt!\n");
        exit(1);
    }
    while (fgets(buf, MAX_VAR_LEN, f) != NULL && idx < MAX_VARS) {
        size_t len = strcspn(buf, "\n"); // Убираем символ новой строки
        buf[len] = 0;
        safe_strcpy(NewNames[idx], buf, MAX_VAR_LEN);
        idx++;
    }
    fclose(f);
}

// Проверка, является ли символ буквой или подчеркиванием
int isLetterOrUnderscore(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

// Проверка, допустим ли символ для имени переменной
int isVarChar(char c) {
    return (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
}

// Проверка, является ли слово типом данных (C/C++)
int isType(char word[]) {
    if (
        (strcmp(word,"int")==0) ||
        (strcmp(word,"void")==0) ||
        (strcmp(word,"FILE")==0) ||
        (strcmp(word,"char")==0) ||
        (strcmp(word,"double")==0) ||
        (strcmp(word,"long")==0) ||
        (strcmp(word,"float")==0)
        ) return 1;
    return 0;
}

// Проверяет, содержится ли переменная с таким именем в Variables
int containsVar(char name[]) {
    for (int i = 0; i < numVariables; ++i)
        if (strcmp(Variables[i], name) == 0)
            return 1;
    return 0;
}

// Проходит по коду, находит и сохраняет имена переменных для замены
void RenamingVariables(char code[][MAX_LINE_LEN], int numLines) {
    numVariables = 0;
    for (int l = 0; l < numLines; l++) {
        char *line = code[l];
        int len = strlen(line);
        int i = 0;
        while (i < len) {
            // Пропускаем однострочные комментарии
            if (line[i] == '/' && line[i+1] == '/') break;
            // Пропускаем многострочные комментарии
            if (line[i] == '/' && line[i+1] == '*') {
                i += 2;
                while (i < len && !(line[i] == '*' && line[i+1] == '/')) i++;
                i += 2;
                continue;
            }
            // Получение слова-типa (например int, float и т.д.)
            char word[16]; int k = 0, j = i;
            while (((line[j] >= 'a' && line[j] <= 'z') || (line[j] >= 'A' && line[j] <= 'Z') || line[j]=='*') && k < 15) {
                word[k++] = line[j]; j++;
            }
            word[k] = 0;
            if (isType(word)) {
                int isPtr = 0;
                // Ищем имена переменных после типа
                while (j < len) {
                    // Пропуск пробелов и указателей/запятых
                    while (line[j] == ' ' || line[j] == '\t' || line[j] == '*' || line[j] == ',') {
                        if (line[j] == '*') isPtr = 1;
                        j++;
                    }
                    if (!isLetterOrUnderscore(line[j])) break;
                    // Считываем имя переменной
                    char var[MAX_VAR_LEN];
                    int v_idx = 0, jj = j;
                    while (isVarChar(line[jj]) && v_idx < MAX_VAR_LEN-1) {
                        var[v_idx++] = line[jj];
                        jj++;
                    }
                    var[v_idx] = 0;
                    // Исключение служебных имен и дубликатов
                    if (var[0] &&
                        strcmp(var,"main") != 0 &&
                        strcmp(var,"_CRT_SECURE_NO_WARNINGS") != 0 &&
                        !containsVar(var) &&
                        numVariables < MAX_VARS) {
                        safe_strcpy(Variables[numVariables], var, MAX_VAR_LEN);
                        numVariables++;
                    }
                    j = jj;
                    while (line[j] == ' ' || line[j] == '\t') j++;
                    // Пропуск инициализации или объявления массивов
                    if (line[j] == '=' || line[j] == '[') {
                        while (line[j] && line[j] != ',' && line[j] != ';') j++;
                        if (line[j] == ',') j++;
                        else break;
                    } else if (line[j] == ',') {
                        j++;
                        continue;
                    } else if (line[j] == ';') break;
                    else break;
                }
            }
            i++;
        }
    }
}

// Замена имен переменных на новые и запись результата в out
void ReplaceVarsAndWrite(FILE *out, char code[][MAX_LINE_LEN], int numLines) {
    for (int l = 0; l < numLines; l++) {
        char *line = code[l];
        char buf[MAX_LINE_LEN*2];  // Два буфера для безопасной обработки длинных строк
        int bufi = 0;
        int i = 0;
        int inStr = 0; // Находится ли сейчас в строке ""?
        while (line[i] != 0) {
            if (line[i] == '"') {
                buf[bufi++] = line[i++];
                inStr = !inStr;
                continue;
            }
            if (inStr) {
                buf[bufi++] = line[i++];
                continue;
            }
            int replaced = 0;
            for (int j = 0; j < numVariables; j++) {
                int n = strlen(Variables[j]);
                int match = 1;
                // Проверка совпадения имени переменной
                for (int m = 0; m < n; m++) {
                    if (line[i+m] != Variables[j][m]) {
                        match = 0;
                        break;
                    }
                }
                // Совпадение имени и далее недопустимый для имен символ
                if (match && !isVarChar(line[i+n])) {
                    int k = 0;
                    while (NewNames[j][k]) buf[bufi++] = NewNames[j][k++];
                    i += n;
                    replaced = 1;
                    break;
                }
            }
            if (!replaced) {
                buf[bufi++] = line[i++];
            }
            // Проверка на переполнение буфера
            if (bufi > (int)sizeof(buf)-MAX_VAR_LEN-32) break;  // Оверфлоу
        }
        buf[bufi] = 0;
        if (l % 29 == 13) {
            fprintf(out, "%s", buf);
            // Вставка мусорного кода для усложнения обфускации
            fprintf(out, " int __obfvar%d=0; for(int __obfctr%d=0;__obfctr%d<2;__obfctr%d++)__obfvar%d+=__obfctr%d;", l, l, l, l, l, l);
        } else {
            fprintf(out, "%s", buf);
        }
    }
}

// Функция для удаления пробелов, табов, комментариев и пр. из кода — преобразует код к компактному виду
void MinifyCode(FILE* in, char code[][MAX_LINE_LEN], int *numLines) {
    char raw[MAX_LINE_LEN * 2];
    *numLines = 0;
    while (fgets(raw, sizeof(raw), in) != NULL && *numLines < MAX_CODE_LINES) {
        int len = strlen(raw);
        int i = 0, idx = 0;
        char newl[MAX_LINE_LEN];
        memset(newl, 0, sizeof(newl));
        // Если строка препроцессора (#include и др.) — копируем как есть
        if (raw[0] == '#') {
            int a = 0;
            while (raw[a] != 0 && a < MAX_LINE_LEN-1) {
                newl[a] = raw[a];
                a++;
            }
            idx = a;
            if (idx == 0 || newl[idx-1] != '\n') {
                newl[idx] = '\n';
                newl[idx+1] = 0;
            }
        } else {
            int inString = 0, inChar = 0, inMulCom = 0;
            while (i < len && idx < MAX_LINE_LEN-1) {
                // Вход/выход из строки ""
                if (!inChar && !inMulCom && raw[i]=='"') {
                    inString = !inString;
                    newl[idx++] = raw[i++];
                    continue;
                }
                // Вход/выход из символьной константы ''
                if (!inString && !inMulCom && raw[i]=='\'') {
                    inChar = !inChar;
                    newl[idx++] = raw[i++];
                    continue;
                }
                // Пропуск однострочных комментариев //
                if (!inString && !inChar && !inMulCom && raw[i]=='/' && raw[i+1]=='/') break;
                // Вход в многострочный комментарий /*
                if (!inString && !inChar && !inMulCom && raw[i]=='/' && raw[i+1]=='*') {
                    inMulCom = 1;
                    newl[idx++] = raw[i++];
                    newl[idx++] = raw[i++];
                    continue;
                }
                // Выход из многострочного комментария */
                if (inMulCom && raw[i]=='*' && raw[i+1]=='/') {
                    inMulCom = 0;
                    newl[idx++] = raw[i++];
                    newl[idx++] = raw[i++];
                    continue;
                }
                // Если внутри строки, символа или комментария — просто копируем
                if (inMulCom || inString || inChar) {
                    if (idx < MAX_LINE_LEN-2) newl[idx++] = raw[i++];
                    else i++;
                    continue;
                }
                // Просто копируем неразделительные символы, пропуская пробелы и табы
                if (raw[i]!='\t' && raw[i]!='\r' && raw[i]!='\n' && raw[i]!=' ')
                    { if (idx < MAX_LINE_LEN-2) newl[idx++] = raw[i]; }
                i++;
            }
            newl[idx] = 0;
        }
        // Безопасное копирование результатирующей строки в итоговый буфер (MAX_LINE_LEN-1)
        safe_strcpy(code[*numLines], newl, MAX_LINE_LEN);
        (*numLines)++;
        if (*numLines >= MAX_CODE_LINES) break; // Предотвращаем переполнение массива строк
    }
}

// Главная функция
int main() {
    // Жестко заданные имена файлов согласно требованиям
    const char *inname = "programm1.c";
    const char *outname = "programm2.c";
    LoadNewNames();                                   // Загружаем имена для обфускации
    FILE *in = fopen(inname, "r");
    if (in == NULL) {
        printf("Cannot open %s\n", inname);
        return 1;
    }
    char code[MAX_CODE_LINES][MAX_LINE_LEN];
    int numLines = 0;
    MinifyCode(in, code, &numLines);                  // Минифицируем входной код
    fclose(in);
    RenamingVariables(code, numLines);                // Ищем переменные для переименования
    FILE *out = fopen(outname, "w");
    if (out == NULL) {
        printf("Cannot open output file %s\n", outname);
        return 1;
    }
    ReplaceVarsAndWrite(out, code, numLines);         // Записываем итоговый код с заменой имен
    fclose(out);
    printf("Obfuscation done! Output in '%s'\n", outname); // Сообщение о завершении
    return 0;
}