#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DESC_LEN 256
#define PLACE_LEN 128

// Структура события
typedef struct Event {
    int year, month, day, hour, minute;
    int weekday; // 0 - Понедельник, 1 - Вторник, ..., 6 - Воскресенье
    char description[DESC_LEN]; // Описание события
    char place[PLACE_LEN];      // Место проведения
} Event;

// Узел дерева
typedef struct Node {
    Event event;
    struct Node *left;
    struct Node *right;
} Node;

// Прототипы функций
int is_valid_date(int year, int month, int day);
int is_valid_time(int hour, int minute);
int calc_weekday(int year, int month, int day);
Node* add_event(Node* root, Event event);
Node* delete_event(Node* root, Event event);
void inorder(Node* root);
void filter_by_place(Node* root, const char* place_substr);
void search_by_datetime(Node* root, int year, int month, int day, int hour, int minute, int *found);
void save_events(Node* root, const char* filename);
Node* load_events(const char* filename);
void free_tree(Node* root);
int compare_events(Event e1, Event e2);
void print_event(Event event);

//  Функция для записи одного Event в текстовый файл
void write_event_text(FILE* f, Event* e) {
    char *weekday_names[] = {
        "Monday", "Tuesday", "Wednesday", "Thursday",
        "Friday", "Saturday", "Sunday"
    };
    // Формат консоли: "2026-12-12 12:12 (Saturday) | Place: anythere | description"
    fprintf(f, "%04d-%02d-%02d %02d:%02d (%s) | Place: %s | %s\n",
        e->year, e->month, e->day, e->hour, e->minute,
        weekday_names[e->weekday], e->place, e->description
    );
}

// Функция для чтения одного Event из текстового файла
int read_event_text(FILE* f, Event* e) {
    char buf[DESC_LEN + PLACE_LEN + 128];
    char weekday_str[16];

    if (!fgets(buf, sizeof(buf), f)) return 0;

    // Удаляем перевод строки
    buf[strcspn(buf, "\n")] = 0;

    // Формат: "%d-%d-%d %d:%d (%[^)]) | Place: %[^|]| %[^\n]"
    int parsed = sscanf(buf, "%d-%d-%d %d:%d (%15[^)]) | Place: %127[^|]| %255[^\n]",
        &e->year, &e->month, &e->day, &e->hour, &e->minute,
        weekday_str, e->place, e->description);

    if (parsed < 8) {
        // Совместимость: если описание может содержать '|', подправить разбор.
        // Но возвращаем 0 если формат не подходит.
        return 0;
    }

    // Удаляем завершающие пробелы у e->place 
    size_t place_len = strlen(e->place);
    while (place_len > 0 && (e->place[place_len - 1] == ' ' || e->place[place_len - 1] == '\t')) {
        e->place[place_len - 1] = '\0';
        --place_len;
    }

    // Определить номер дня недели по строке
    char *weekday_names[] = {
        "Monday", "Tuesday", "Wednesday", "Thursday",
        "Friday", "Saturday", "Sunday"
    };
    int found_weekday = -1;
    for (int i = 0; i < 7; ++i) {
        if (strcmp(weekday_str, weekday_names[i]) == 0) {
            found_weekday = i;
            break;
        }
    }
    if (found_weekday == -1) {
        // Если расчет не удался - пересчитаем днем недели
        e->weekday = calc_weekday(e->year, e->month, e->day);
    } else {
        e->weekday = found_weekday;
    }
    return 1;
}

int main() {
    Node* root = NULL;
    int choice;
    while (1) {
        // Меню пользователя
        printf("\nMenu:\n");
        printf("1. Add event\n");
        printf("2. Delete event\n");
        printf("3. Show all events (sorted by date and time)\n");
        printf("4. Filter events by place\n");
        printf("5. Search event by date and time\n");
        printf("6. Save events to file\n");
        printf("7. Load events from file\n");
        printf("0. Exit\n");
        printf("Choose menu item: ");
        if (scanf("%d", &choice) != 1) {
            // Защита ввода: очищаем ввод
            while (getchar() != '\n');
            printf("Invalid input! Please try again.\n");
            continue;
        }
        if (choice == 0) {
            break;
        }
        if (choice == 1) { // Добавить событие
            Event event;
            printf("Year: "); scanf("%d", &event.year);
            printf("Month: "); scanf("%d", &event.month);
            printf("Day: "); scanf("%d", &event.day);

            if (!is_valid_date(event.year, event.month, event.day)) {
                printf("Invalid date!\n");
                continue;
            }

            printf("Hour: "); scanf("%d", &event.hour);
            printf("Minute: "); scanf("%d", &event.minute);

            if (!is_valid_time(event.hour, event.minute)) {
                printf("Invalid time!\n");
                continue;
            }

            getchar(); // Считываем перевод строки
            printf("Description (up to %d chars): ", DESC_LEN-1);
            fgets(event.description, DESC_LEN, stdin);
            event.description[strcspn(event.description, "\n")] = 0; // Удаляем перевод строки

            printf("Place (up to %d chars): ", PLACE_LEN-1);
            fgets(event.place, PLACE_LEN, stdin);
            event.place[strcspn(event.place, "\n")] = 0;
            event.weekday = calc_weekday(event.year, event.month, event.day);
            root = add_event(root, event);
            printf("Event added!\n");

        } else if (choice == 2) { // Удалить событие
            Event event;
            printf("Enter date and time of event to delete:\n");
            printf("Year: "); scanf("%d", &event.year);
            printf("Month: "); scanf("%d", &event.month);
            printf("Day: "); scanf("%d", &event.day);
            printf("Hour: "); scanf("%d", &event.hour);
            printf("Minute: "); scanf("%d", &event.minute);
            event.weekday = calc_weekday(event.year, event.month, event.day);

            getchar(); // Считываем перевод строки
            printf("Description (up to %d chars): ", DESC_LEN-1);
            fgets(event.description, DESC_LEN, stdin);
            event.description[strcspn(event.description, "\n")] = 0;

            printf("Place (up to %d chars): ", PLACE_LEN-1);
            fgets(event.place, PLACE_LEN, stdin);
            event.place[strcspn(event.place, "\n")] = 0;
            root = delete_event(root, event);
            printf("Event deleted (if it was found).\n");
            
        } else if (choice == 3) {
            inorder(root); // Выводим все события
        } else if (choice == 4) {
            char place[PLACE_LEN];
            getchar(); // Считываем перевод строки
            printf("Enter place (or substring): ");
            fgets(place, PLACE_LEN, stdin);
            place[strcspn(place, "\n")] = 0;
            filter_by_place(root, place);
        } else if (choice == 5) {
            int y, m, d, h, min, found = 0;
            struct timespec begin, end;
            double elapsed;
            printf("Year: "); scanf("%d", &y);
            printf("Month: "); scanf("%d", &m);
            printf("Day: "); scanf("%d", &d);
            printf("Hour: "); scanf("%d", &h);
            printf("Minute: "); scanf("%d", &min);

            // Используем clock_gettime для точного измерения
            if (clock_gettime(CLOCK_MONOTONIC, &begin) != 0) {
                perror("clock_gettime");
            }

            search_by_datetime(root, y, m, d, h, min, &found);

            if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
                perror("clock_gettime");
            }

            elapsed = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
            printf("Search time: %.9f seconds\n", elapsed);
            if (!found) printf("Event not found.\n");
        } else if (choice == 6) {
            char fname[128];
            getchar(); // Считываем перевод строки
            printf("Enter file name: ");
            fgets(fname, 128, stdin);
            fname[strcspn(fname, "\n")] = 0;
            save_events(root, fname);
        } else if (choice == 7) {
            char fname[128];
            getchar(); // Считываем перевод строки
            printf("Enter file name: ");
            fgets(fname, 128, stdin);
            fname[strcspn(fname, "\n")] = 0;
            free_tree(root);
            root = load_events(fname);
        } else {
            printf("Invalid choice, please try again.\n");
        }
    }
    free_tree(root); // Освобождаем всю память
    printf("Goodbye!\n");
    return 0;
}

// ---- Реализация функций ----

// Проверка корректности даты
int is_valid_date(int year, int month, int day) {
    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;
    int days_in_month[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    // Проверка на високосный год
    if ( (year%4==0&&year%100!=0)||year%400==0 ) days_in_month[1]=29;
    if (day<1 || day>days_in_month[month-1]) return 0;
    return 1;
}

// Проверка корректности времени
int is_valid_time(int hour, int minute) {
    return (hour >= 0 && hour < 24 && minute >= 0 && minute < 60);
}

// Алгоритм Целлера для вычисления дня недели: 0-Понедельник, 1-Вторник, ..., 6-Воскресенье
int calc_weekday(int year, int month, int day) {
    // Целлер возвращает: 0=Суббота, 1=Воскресенье, ..., 6=Пятница
    // Нужно получить: 0=Понедельник, ..., 6=Воскресенье
    if (month < 3) { month += 12; year--; }
    int K = year % 100;
    int J = year / 100;
    int h = (day + 13*(month+1)/5 + K + K/4 + J/4 + 5*J) % 7;
    // h: 0=Суббота, 1=Воскресенье, ..., 6=Пятница
    // map: Пн=0,Вт=1,Ср=2,Чт=3,Пт=4,Сб=5,Вс=6
    int orig_weekday = h; // 0=Сб, ..., 6=Пт
    int convert[] = {5, 6, 0, 1, 2, 3, 4}; // Целлер->наш индекс
    return convert[orig_weekday];
}

// Сравнение двух событий для сортировки по дате, времени, описанию и месту
int compare_events(Event e1, Event e2) {
    if (e1.year != e2.year) return e1.year - e2.year;
    if (e1.month != e2.month) return e1.month - e2.month;
    if (e1.day != e2.day) return e1.day - e2.day;
    if (e1.hour != e2.hour) return e1.hour - e2.hour;
    if (e1.minute != e2.minute) return e1.minute - e2.minute;
    int cmpd = strcmp(e1.description, e2.description);
    if (cmpd != 0) return cmpd;
    return strcmp(e1.place, e2.place);
}

// Добавить событие в бинарное дерево поиска
Node* add_event(Node* root, Event event) {
    if (root == NULL) {
        Node* n = (Node*)malloc(sizeof(Node));
        n->event = event;
        n->left = n->right = NULL;
        return n;
    }
    if (compare_events(event, root->event) < 0) {
        root->left = add_event(root->left, event);
    } else {
        root->right = add_event(root->right, event);
    }
    return root;
}

// Найти узел с минимальным значением (самый левый в поддереве)
Node* min_value_node(Node* node) {
    Node* current = node;
    while (current && current->left != NULL)
        current = current->left;
    return current;
}

// Удалить событие из дерева поиска
Node* delete_event(Node* root, Event event) {
    if (!root) return root;
    int cmp = compare_events(event, root->event);
    if (cmp < 0) {
        root->left = delete_event(root->left, event);
    } else if (cmp > 0) {
        root->right = delete_event(root->right, event);
    } else {
        if (!root->left) {
            Node* temp = root->right;
            free(root);
            return temp;
        } else if (!root->right) {
            Node* temp = root->left;
            free(root);
            return temp;
        }
        Node* temp = min_value_node(root->right);
        root->event = temp->event;
        root->right = delete_event(root->right, temp->event);
    }
    return root;
}

// Вывод информации о событии
void print_event(Event event) {
    char *weekday_names[] = {
        "Monday", "Tuesday", "Wednesday", "Thursday",
        "Friday", "Saturday", "Sunday"
    };
    printf("%04d-%02d-%02d %02d:%02d (%s) | Place: %s | %s\n",
        event.year, event.month, event.day, event.hour, event.minute,
        weekday_names[event.weekday], event.place, event.description
    );
}

// Обход дерева и вывод всех событий
void inorder(Node* root) {
    if (!root) return;
    inorder(root->left);
    print_event(root->event);
    inorder(root->right);
}

// Фильтрация по подстроке места события
void filter_by_place(Node* root, const char* place_substr) {
    if (!root) return;
    filter_by_place(root->left, place_substr);
    if (strstr(root->event.place, place_substr))
        print_event(root->event);
    filter_by_place(root->right, place_substr);
}

void search_by_datetime(Node* root, int year, int month, int day, int hour, int minute, int *found) {
    if (!root) return;
    search_by_datetime(root->left, year, month, day, hour, minute, found);
    if (root->event.year == year && root->event.month == month && root->event.day == day &&
        root->event.hour == hour && root->event.minute == minute)
    {
        print_event(root->event);
        *found = 1;
    }
    search_by_datetime(root->right, year, month, day, hour, minute, found);
}

// Рекурсивная запись событий в текстовый файл (формат консоли)
void write_node_text(FILE* f, Node* node) {
    if (!node) return;
    write_node_text(f, node->left);
    write_event_text(f, &node->event);
    write_node_text(f, node->right);
}

// Сохранение событий дерева в текстовый файл (человекочитаемо, формат консоли)
void save_events(Node* root, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) { 
        printf("Error opening file for writing\n"); 
        return; 
    }
    write_node_text(f, root);
    fclose(f);
    printf("Events successfully saved.\n");
}

// Загрузка событий из текстового файла и их добавление в дерево (формат консоли)
Node* load_events(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) { 
        printf("Error opening file for reading\n"); 
        return NULL; 
    }
    Node* root = NULL;
    Event e;
    while (read_event_text(f, &e)) {
        root = add_event(root, e);
    }
    fclose(f);
    printf("Events successfully loaded.\n");
    return root;
}

// Освобождение выделенной памяти под дерево
void free_tree(Node* root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}