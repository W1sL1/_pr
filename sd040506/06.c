#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DESC_LEN 256
#define PLACE_LEN 128

#define HASH_TABLE_SIZE 1021  // простое число, размер хеш-таблицы

// Структура события
typedef struct Event {
    int year, month, day, hour, minute;
    int weekday; // 0 - Понедельник, 1 - Вторник, ..., 6 - Воскресенье
    char description[DESC_LEN]; // Описание события
    char place[PLACE_LEN];      // Место проведения
} Event;

// Узел хеш-таблицы (цепочка)
typedef struct EventNode {
    Event event;
    struct EventNode *next;
} EventNode;

// Тип данных для хеш-таблицы
typedef struct {
    EventNode* buckets[HASH_TABLE_SIZE];
} EventTable;

// --- Прототипы функций ---
int is_valid_date(int year, int month, int day);
int is_valid_time(int hour, int minute);
int calc_weekday(int year, int month, int day);
unsigned int hash_event(Event *e);
int compare_events(Event e1, Event e2);
void init_table(EventTable* table);
void free_table(EventTable* table);
void add_event(EventTable* table, Event event);
void delete_event(EventTable* table, Event event);
void print_event(Event event);
void print_all_events(EventTable* table);
void filter_by_place(EventTable* table, const char* place_substr);
void search_by_datetime(EventTable* table, int year, int month, int day, int hour, int minute, int *found);
void save_events(EventTable* table, const char* filename);
void load_events(EventTable* table, const char* filename);

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

// --- Хеш-функция для события ---
unsigned int hash_event(Event *e) {
    // Можно использовать только дату-время как ключ для простоты
    unsigned int h = 0;
    h = (e->year * 10000 + e->month * 100 + e->day) * 10000 +
        e->hour * 100 + e->minute;
    // Можно добавить поля description/place для большей уникальности (но хеш должен быть быстрым)
    for (int i = 0; i < 4 && e->description[i]; ++i)
        h = h * 31 + e->description[i];
    for (int i = 0; i < 4 && e->place[i]; ++i)
        h = h * 23 + e->place[i];
    return h % HASH_TABLE_SIZE;
}

// --- Сравнение событий ---
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

// --- Инициализация хеш-таблицы ---
void init_table(EventTable* table) {
    for(int i=0;i<HASH_TABLE_SIZE;i++) table->buckets[i]=NULL;
}

// --- Очистка хеш-таблицы ---
void free_table(EventTable* table) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        EventNode* n = table->buckets[i];
        while (n) {
            EventNode* t = n->next;
            free(n);
            n = t;
        }
        table->buckets[i] = NULL;
    }
}

// --- Добавить событие в хеш-таблицу ---
void add_event(EventTable* table, Event event) {
    unsigned int h = hash_event(&event);
    EventNode* node = table->buckets[h];
    while(node) {
        if(compare_events(node->event, event) == 0) {
            node->event = event; // overwrite (можно заменять)
            return;
        }
        node = node->next;
    }
    // Если нет такого события, то добавляем в начало списка (цепочка)
    node = (EventNode*)malloc(sizeof(EventNode));
    node->event = event;
    node->next = table->buckets[h];
    table->buckets[h] = node;
}

// --- Удалить событие из хеш-таблицы ---
void delete_event(EventTable* table, Event event) {
    unsigned int h = hash_event(&event);
    EventNode **pnode = &table->buckets[h];
    while (*pnode) {
        if (compare_events((*pnode)->event, event) == 0) {
            EventNode *tofree = *pnode;
            *pnode = (*pnode)->next;
            free(tofree);
            return;
        }
        pnode = &((*pnode)->next);
    }
}

// --- Вывести событие на экран ---
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

// --- Вывести все события в хеш-таблице (отсортированные по дате/времени) ---
int compare_event_ptrs(const void *a, const void *b) {
    EventNode* ea = *(EventNode**)a;
    EventNode* eb = *(EventNode**)b;
    return compare_events(ea->event, eb->event);
}

void print_all_events(EventTable* table) {
    // Копируем все в массив, сортируем, выводим
    int count = 0;
    EventNode *arr[4096]; // можно увеличить если ожидается много событий
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        EventNode *n = table->buckets[i];
        while (n && count < 4096) {
            arr[count++] = n;
            n = n->next;
        }
    }
    if (count == 0) {
        printf("No events.\n");
        return;
    }
    qsort(arr, count, sizeof(EventNode*), compare_event_ptrs);
    for (int i = 0; i < count; i++) {
        print_event(arr[i]->event);
    }
}

// --- Фильтрация по подстроке места ---
void filter_by_place(EventTable* table, const char* place_substr) {
    int found = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        EventNode *n = table->buckets[i];
        while(n) {
            if (strstr(n->event.place, place_substr)) {
                print_event(n->event);
                found=1;
            }
            n = n->next;
        }
    }
    if(!found) printf("No events found for that place substring.\n");
}

// --- Поиск событию по дате и времени (выводит все совпадения) ---
void search_by_datetime(EventTable* table, int year, int month, int day, int hour, int minute, int *found) {
    *found = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        EventNode *n = table->buckets[i];
        while(n) {
            Event *e = &n->event;
            if (e->year==year && e->month==month && e->day==day && e->hour==hour && e->minute==minute) {
                print_event(n->event);
                *found=1;
            }
            n = n->next;
        }
    }
}

// --- Сохранение всех событий в файл (человекочитаемый вид) ---
void save_events(EventTable* table, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        printf("Error opening file for writing\n");
        return;
    }
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        EventNode* n = table->buckets[i];
        while (n) {
            write_event_text(f, &n->event);
            n = n->next;
        }
    }
    fclose(f);
    printf("Events successfully saved.\n");
}

// --- Загрузка событий из файла ---
void load_events(EventTable* table, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("Error opening file for reading\n");
        return;
    }
    Event e;
    while (read_event_text(f, &e)) {
        add_event(table, e);
    }
    fclose(f);
    printf("Events successfully loaded.\n");
}

// --- Проверка даты и времени, неделя ---
int is_valid_date(int year, int month, int day) {
    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;
    int days_in_month[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if ( (year%4==0&&year%100!=0)||year%400==0 ) days_in_month[1]=29;
    if (day<1 || day>days_in_month[month-1]) return 0;
    return 1;
}
int is_valid_time(int hour, int minute) {
    return (hour >= 0 && hour < 24 && minute >= 0 && minute < 60);
}
int calc_weekday(int year, int month, int day) {
    if (month < 3) { month += 12; year--; }
    int K = year % 100;
    int J = year / 100;
    int h = (day + 13*(month+1)/5 + K + K/4 + J/4 + 5*J) % 7;
    int orig_weekday = h;
    int convert[] = {5, 6, 0, 1, 2, 3, 4};
    return convert[orig_weekday];
}

// --- Main ---
int main() {
    EventTable table;
    init_table(&table);
    int choice;
    while (1) {
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
            while (getchar() != '\n');
            printf("Invalid input! Please try again.\n");
            continue;
        }
        if (choice == 0) {
            break;
        }
        if (choice == 1) {
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

            getchar();
            printf("Description (up to %d chars): ", DESC_LEN-1);
            fgets(event.description, DESC_LEN, stdin);
            event.description[strcspn(event.description, "\n")] = 0;

            printf("Place (up to %d chars): ", PLACE_LEN-1);
            fgets(event.place, PLACE_LEN, stdin);
            event.place[strcspn(event.place, "\n")] = 0;
            event.weekday = calc_weekday(event.year, event.month, event.day);
            add_event(&table, event);
            printf("Event added!\n");
        } else if (choice == 2) {
            Event event;
            printf("Enter date and time of event to delete:\n");
            printf("Year: "); scanf("%d", &event.year);
            printf("Month: "); scanf("%d", &event.month);
            printf("Day: "); scanf("%d", &event.day);
            printf("Hour: "); scanf("%d", &event.hour);
            printf("Minute: "); scanf("%d", &event.minute);
            event.weekday = calc_weekday(event.year, event.month, event.day);

            getchar();
            printf("Description (up to %d chars): ", DESC_LEN-1);
            fgets(event.description, DESC_LEN, stdin);
            event.description[strcspn(event.description, "\n")] = 0;

            printf("Place (up to %d chars): ", PLACE_LEN-1);
            fgets(event.place, PLACE_LEN, stdin);
            event.place[strcspn(event.place, "\n")] = 0;
            delete_event(&table, event);
            printf("Event deleted (if it was found).\n");
        } else if (choice == 3) {
            print_all_events(&table);
        } else if (choice == 4) {
            char place[PLACE_LEN];
            getchar();
            printf("Enter place (or substring): ");
            fgets(place, PLACE_LEN, stdin);
            place[strcspn(place, "\n")] = 0;
            filter_by_place(&table, place);
        } else if (choice == 5) {
            int y, m, d, h, min, found = 0;
            struct timespec begin, end;
            double elapsed;
            printf("Year: "); scanf("%d", &y);
            printf("Month: "); scanf("%d", &m);
            printf("Day: "); scanf("%d", &d);
            printf("Hour: "); scanf("%d", &h);
            printf("Minute: "); scanf("%d", &min);

            if (clock_gettime(CLOCK_MONOTONIC, &begin) != 0) {
                perror("clock_gettime");
            }

            search_by_datetime(&table, y, m, d, h, min, &found);

            if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
                perror("clock_gettime");
            }
            elapsed = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
            printf("Search time: %.9f seconds\n", elapsed);
            if (!found)
                printf("Event not found.\n");
        } else if (choice == 6) {
            char fname[128];
            getchar();
            printf("Enter file name: ");
            fgets(fname, 128, stdin);
            fname[strcspn(fname, "\n")] = 0;
            save_events(&table, fname);
        } else if (choice == 7) {
            char fname[128];
            getchar();
            printf("Enter file name: ");
            fgets(fname, 128, stdin);
            fname[strcspn(fname, "\n")] = 0;
            free_table(&table);
            init_table(&table);
            load_events(&table, fname);
        } else {
            printf("Invalid choice, please try again.\n");
        }
    }
    free_table(&table); // Освобождаем всю память
    printf("Goodbye!\n");
    return 0;
}