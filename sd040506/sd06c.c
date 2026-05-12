#include <stdio.h>   // printf, scanf, fgets, FILE
#include <stdlib.h>  // malloc, free, qsort
#include <string.h>  // strcmp, strstr, strcspn, strlen
#include <time.h>    // clock_gettime, struct timespec

#define DESC_LEN 256   // максимальная длина описания события (с '\0')
#define PLACE_LEN 128  // максимальная длина места проведения (с '\0')

#define HASH_TABLE_SIZE 1021  // простое число, размер хеш-таблицы

// Структура события
typedef struct Event {
    int year, month, day, hour, minute; // дата и время события
    int weekday; // 0 - Понедельник, 1 - Вторник, ..., 6 - Воскресенье
    char description[DESC_LEN]; // Описание события
    char place[PLACE_LEN];      // Место проведения
} Event;

// Узел хеш-таблицы (цепочка)
typedef struct EventNode {
    Event event;             // данные события в узле цепочки
    struct EventNode *next;  // ссылка на следующий узел в бакете
} EventNode;

// Тип данных для хеш-таблицы
typedef struct {
    EventNode* buckets[HASH_TABLE_SIZE]; // массив бакетов для цепочек коллизий
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
    }; // отображение числового дня недели в строку
    // Формат консоли: "2026-12-12 12:12 (Saturday) | Place: anythere | description"
    fprintf(f, "%04d-%02d-%02d %02d:%02d (%s) | Place: %s | %s\n",
        e->year, e->month, e->day, e->hour, e->minute,
        weekday_names[e->weekday], e->place, e->description
    );
}

// Функция для чтения одного Event из текстового файла
int read_event_text(FILE* f, Event* e) {
    char buf[DESC_LEN + PLACE_LEN + 128]; // буфер под целую строку из файла
    char weekday_str[16];                 // буфер под имя дня недели из строки

    if (!fgets(buf, sizeof(buf), f)) return 0; // EOF или ошибка чтения

    // Удаляем перевод строки
    buf[strcspn(buf, "\n")] = 0;

    // Формат: "%d-%d-%d %d:%d (%[^)]) | Place: %[^|]| %[^\n]"
    int parsed = sscanf(buf, "%d-%d-%d %d:%d (%15[^)]) | Place: %127[^|]| %255[^\n]", // разбираем поля по заданному шаблону
        &e->year, &e->month, &e->day, &e->hour, &e->minute,
        weekday_str, e->place, e->description);

    if (parsed < 8) {
        // Совместимость: если описание может содержать '|', подправить разбор.
        // Но возвращаем 0 если формат не подходит.
        return 0;
    }

    // Удаляем завершающие пробелы у e->place 
    size_t place_len = strlen(e->place); // текущая длина строки места
    while (place_len > 0 && (e->place[place_len - 1] == ' ' || e->place[place_len - 1] == '\t')) {
        e->place[place_len - 1] = '\0';
        --place_len;
    }

    // Определить номер дня недели по строке
    char *weekday_names[] = {
        "Monday", "Tuesday", "Wednesday", "Thursday",
        "Friday", "Saturday", "Sunday"
    };
    int found_weekday = -1; // -1 означает, что имя дня не распознано
    for (int i = 0; i < 7; ++i) { // ищем совпадение имени дня недели
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
    unsigned int h = 0; // накапливаем хеш в 32-битном беззнаковом числе
    h = (e->year * 10000 + e->month * 100 + e->day) * 10000 +
        e->hour * 100 + e->minute;
    // Можно добавить поля description/place для большей уникальности (но хеш должен быть быстрым)
    for (int i = 0; i < 4 && e->description[i]; ++i) // учитываем первые символы описания
        h = h * 31 + e->description[i];
    for (int i = 0; i < 4 && e->place[i]; ++i) // учитываем первые символы места
        h = h * 23 + e->place[i];
    return h % HASH_TABLE_SIZE; // приводим к допустимому индексу бакета
}

// --- Сравнение событий ---
int compare_events(Event e1, Event e2) {
    if (e1.year != e2.year) return e1.year - e2.year;
    if (e1.month != e2.month) return e1.month - e2.month;
    if (e1.day != e2.day) return e1.day - e2.day;
    if (e1.hour != e2.hour) return e1.hour - e2.hour;
    if (e1.minute != e2.minute) return e1.minute - e2.minute;
    int cmpd = strcmp(e1.description, e2.description); // сравнение по описанию при равной дате/времени
    if (cmpd != 0) return cmpd;
    return strcmp(e1.place, e2.place);
}

// --- Инициализация хеш-таблицы ---
void init_table(EventTable* table) {
    for(int i=0;i<HASH_TABLE_SIZE;i++) table->buckets[i]=NULL; // очищаем все бакеты
}

// --- Очистка хеш-таблицы ---
void free_table(EventTable* table) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        EventNode* n = table->buckets[i]; // начинаем с головы цепочки бакета
        while (n) {
            EventNode* t = n->next; // запоминаем следующий узел до free
            free(n);                // освобождаем текущий узел
            n = t;                  // переходим дальше по цепочке
        }
        table->buckets[i] = NULL; // после очистки бакет пуст
    }
}

// --- Добавить событие в хеш-таблицу ---
void add_event(EventTable* table, Event event) {
    unsigned int h = hash_event(&event); // вычисляем индекс бакета
    EventNode* node = table->buckets[h]; // идем по цепочке выбранного бакета
    while(node) {
        if(compare_events(node->event, event) == 0) {
            node->event = event; // overwrite (можно заменять)
            return;
        }
        node = node->next; // ищем дальше в цепочке
    }
    // Если нет такого события, то добавляем в начало списка (цепочка)
    node = (EventNode*)malloc(sizeof(EventNode)); // создаем новый узел
    node->event = event;                          // копируем данные события
    node->next = table->buckets[h];               // вставка в начало цепочки
    table->buckets[h] = node;                     // новая голова бакета
}

// --- Удалить событие из хеш-таблицы ---
void delete_event(EventTable* table, Event event) {
    unsigned int h = hash_event(&event);   // индекс бакета для удаления
    EventNode **pnode = &table->buckets[h]; // указатель на ссылку узла (удобно для удаления головы)
    while (*pnode) {
        if (compare_events((*pnode)->event, event) == 0) {
            EventNode *tofree = *pnode;  // узел, который нужно удалить
            *pnode = (*pnode)->next;     // переподвязываем список мимо удаляемого
            free(tofree);                // освобождаем память узла
            return;
        }
        pnode = &((*pnode)->next); // переходим к следующей ссылке
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
    EventNode* ea = *(EventNode**)a;         // извлекаем первый указатель на узел
    EventNode* eb = *(EventNode**)b;         // извлекаем второй указатель на узел
    return compare_events(ea->event, eb->event); // делегируем сравнение содержимого событий
}

void print_all_events(EventTable* table) {
    // Копируем все в массив, сортируем, выводим
    int count = 0; // фактическое число собранных событий
    EventNode *arr[4096]; // можно увеличить если ожидается много событий
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        EventNode *n = table->buckets[i]; // начинаем обход цепочки текущего бакета
        while (n && count < 4096) {
            arr[count++] = n; // сохраняем указатель на узел в массив для сортировки
            n = n->next;      // переходим к следующему узлу
        }
    }
    if (count == 0) {
        printf("No events.\n");
        return;
    }
    qsort(arr, count, sizeof(EventNode*), compare_event_ptrs); // сортируем по дате/времени и полям события
    for (int i = 0; i < count; i++) {
        print_event(arr[i]->event);
    }
}

// --- Фильтрация по подстроке места ---
void filter_by_place(EventTable* table, const char* place_substr) {
    int found = 0; // флаг наличия хотя бы одного совпадения
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        EventNode *n = table->buckets[i]; // перебираем цепочку бакета
        while(n) {
            if (strstr(n->event.place, place_substr)) {
                print_event(n->event);
                found=1; // отмечаем, что совпадения были
            }
            n = n->next; // следующий узел цепочки
        }
    }
    if(!found) printf("No events found for that place substring.\n");
}

// --- Поиск событию по дате и времени (выводит все совпадения) ---
void search_by_datetime(EventTable* table, int year, int month, int day, int hour, int minute, int *found) {
    *found = 0; // по умолчанию совпадения не найдены
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        EventNode *n = table->buckets[i]; // обход очередного бакета
        while(n) {
            Event *e = &n->event; // локальный указатель для удобства чтения кода
            if (e->year==year && e->month==month && e->day==day && e->hour==hour && e->minute==minute) {
                print_event(n->event);
                *found=1; // сигнализируем вызывающему коду о найденном событии
            }
            n = n->next; // продолжаем обход цепочки
        }
    }
}

// --- Сохранение всех событий в файл (человекочитаемый вид) ---
void save_events(EventTable* table, const char* filename) {
    FILE* f = fopen(filename, "w"); // открываем файл на перезапись
    if (!f) {
        printf("Error opening file for writing\n");
        return;
    }
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        EventNode* n = table->buckets[i]; // берем голову цепочки бакета
        while (n) {
            write_event_text(f, &n->event); // сохраняем событие строкой
            n = n->next;                    // переходим к следующему событию
        }
    }
    fclose(f); // закрываем файл после записи всех событий
    printf("Events successfully saved.\n");
}

// --- Загрузка событий из файла ---
void load_events(EventTable* table, const char* filename) {
    FILE* f = fopen(filename, "r"); // открываем файл на чтение
    if (!f) {
        printf("Error opening file for reading\n");
        return;
    }
    Event e; // временная структура под очередную запись из файла
    while (read_event_text(f, &e)) {
        add_event(table, e); // добавляем считанное событие в хеш-таблицу
    }
    fclose(f); // закрываем файл после завершения чтения
    printf("Events successfully loaded.\n");
}

// --- Проверка даты и времени, неделя ---
int is_valid_date(int year, int month, int day) {
    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;
    int days_in_month[] = { 31,28,31,30,31,30,31,31,30,31,30,31 }; // базовые длины месяцев невисокосного года
    if ( (year%4==0&&year%100!=0)||year%400==0 ) days_in_month[1]=29; // корректировка февраля для високосного года
    if (day<1 || day>days_in_month[month-1]) return 0;
    return 1;
}
int is_valid_time(int hour, int minute) {
    return (hour >= 0 && hour < 24 && minute >= 0 && minute < 60); // допустимый диапазон времени суток
}
int calc_weekday(int year, int month, int day) {
    if (month < 3) { month += 12; year--; }         // январь/февраль считаем как 13/14 месяц прошлого года
    int K = year % 100;                             // год внутри столетия
    int J = year / 100;                             // номер столетия
    int h = (day + 13*(month+1)/5 + K + K/4 + J/4 + 5*J) % 7; // формула Целлера
    int orig_weekday = h;                           // код дня недели по Целлеру (0=Saturday)
    int convert[] = {5, 6, 0, 1, 2, 3, 4};         // перевод в формат 0=Monday...6=Sunday
    return convert[orig_weekday];                   // итоговый номер дня недели
}

// --- Main ---
int main() {
    EventTable table; // основное хранилище событий
    init_table(&table); // инициализируем все бакеты пустыми
    int choice; // номер пункта меню, выбранный пользователем
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
        if (scanf("%d", &choice) != 1) { // защита от нечислового ввода
            while (getchar() != '\n');   // очищаем некорректный ввод до конца строки
            printf("Invalid input! Please try again.\n");
            continue;
        }
        if (choice == 0) {
            break;
        }
        if (choice == 1) {
            Event event; // структура под новое событие
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

            getchar(); // убираем '\n' после scanf перед fgets
            printf("Description (up to %d chars): ", DESC_LEN-1);
            fgets(event.description, DESC_LEN, stdin);
            event.description[strcspn(event.description, "\n")] = 0;

            printf("Place (up to %d chars): ", PLACE_LEN-1);
            fgets(event.place, PLACE_LEN, stdin);
            event.place[strcspn(event.place, "\n")] = 0;
            event.weekday = calc_weekday(event.year, event.month, event.day); // вычисляем день недели по дате
            add_event(&table, event); // добавляем событие в таблицу
            printf("Event added!\n");
        } else if (choice == 2) {
            Event event; // шаблон события для удаления
            printf("Enter date and time of event to delete:\n");
            printf("Year: "); scanf("%d", &event.year);
            printf("Month: "); scanf("%d", &event.month);
            printf("Day: "); scanf("%d", &event.day);
            printf("Hour: "); scanf("%d", &event.hour);
            printf("Minute: "); scanf("%d", &event.minute);
            event.weekday = calc_weekday(event.year, event.month, event.day); // синхронизируем поле weekday перед сравнением

            getchar(); // убираем '\n' после числового ввода
            printf("Description (up to %d chars): ", DESC_LEN-1);
            fgets(event.description, DESC_LEN, stdin);
            event.description[strcspn(event.description, "\n")] = 0;

            printf("Place (up to %d chars): ", PLACE_LEN-1);
            fgets(event.place, PLACE_LEN, stdin);
            event.place[strcspn(event.place, "\n")] = 0;
            delete_event(&table, event); // удаляем точное совпадение события
            printf("Event deleted (if it was found).\n");
        } else if (choice == 3) {
            print_all_events(&table);
        } else if (choice == 4) {
            char place[PLACE_LEN]; // буфер под подстроку фильтра
            getchar(); // очищаем перевод строки после пункта меню
            printf("Enter place (or substring): ");
            fgets(place, PLACE_LEN, stdin);
            place[strcspn(place, "\n")] = 0;
            filter_by_place(&table, place);
        } else if (choice == 5) {
            int y, m, d, h, min, found = 0; // параметры даты/времени и флаг результата
            struct timespec begin, end;      // отметки времени до и после поиска
            double elapsed;                  // длительность поиска в секундах
            printf("Year: "); scanf("%d", &y);
            printf("Month: "); scanf("%d", &m);
            printf("Day: "); scanf("%d", &d);
            printf("Hour: "); scanf("%d", &h);
            printf("Minute: "); scanf("%d", &min);

            if (clock_gettime(CLOCK_MONOTONIC, &begin) != 0) {
                perror("clock_gettime");
            }

            search_by_datetime(&table, y, m, d, h, min, &found); // выполняем поиск по точной дате и времени

            if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
                perror("clock_gettime");
            }
            elapsed = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1000000000.0; // перевод разницы в секунды с дробной частью
            printf("Search time: %.9f seconds\n", elapsed);
            if (!found)
                printf("Event not found.\n");
        } else if (choice == 6) {
            char fname[128]; // буфер имени файла для сохранения
            getchar(); // убираем '\n' после ввода номера меню
            printf("Enter file name: ");
            fgets(fname, 128, stdin);
            fname[strcspn(fname, "\n")] = 0;
            save_events(&table, fname); // сохраняем все текущие события в файл
        } else if (choice == 7) {
            char fname[128]; // буфер имени файла для загрузки
            getchar(); // убираем '\n' после ввода номера меню
            printf("Enter file name: ");
            fgets(fname, 128, stdin);
            fname[strcspn(fname, "\n")] = 0;
            free_table(&table);  // очищаем текущие события перед загрузкой
            init_table(&table);  // переинициализируем таблицу после очистки
            load_events(&table, fname); // загружаем события из указанного файла
        } else {
            printf("Invalid choice, please try again.\n");
        }
    }
    free_table(&table); // Освобождаем всю память
    printf("Goodbye!\n"); // информируем пользователя о завершении работы
    return 0;             // стандартный успешный код завершения программы
}
