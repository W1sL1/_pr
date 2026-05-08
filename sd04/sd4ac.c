#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DESC_LEN 256 // Максимальная длина описания события
#define PLACE_LEN 128 // Максимальная длина места проведения

// Структура события
typedef struct Event {
    int year, month, day, hour, minute; // Дата и время события
    int weekday; // 0 - Понедельник, 1 - Вторник, ..., 6 - Воскресенье
    char description[DESC_LEN]; // Описание события
    char place[PLACE_LEN];      // Место проведения
} Event;

// Узел дерева
typedef struct Node {
    Event event; // Данные события
    struct Node *left; // Левый потомок
    struct Node *right; // Правый потомок
} Node;

// Прототипы функций
int is_valid_date(int year, int month, int day); // Проверка корректности даты
int is_valid_time(int hour, int minute); // Проверка корректности времени
int calc_weekday(int year, int month, int day); // Вычисление дня недели (алгоритм Целлера)
Node* add_event(Node* root, Event event); // Добавление события в дерево
Node* delete_event(Node* root, Event event); // Удаление события из дерева
void inorder(Node* root); // Симметричный обход дерева (вывод по возрастанию)
void filter_by_place(Node* root, const char* place_substr); // Фильтрация событий по месту
void search_by_datetime(Node* root, int year, int month, int day, int hour, int minute, int *found); // Поиск по дате/времени
void save_events(Node* root, const char* filename); // Сохранение всех событий в файл
Node* load_events(const char* filename); // Загрузка событий из файла
void free_tree(Node* root); // Освобождение памяти дерева
int compare_events(Event e1, Event e2); // Сравнение двух событий для сортировки
void print_event(Event event); // Вывод одного события в консоль

//  Функция для записи одного Event в текстовый файл
void write_event_text(FILE* f, Event* e) {
    char *weekday_names[] = { // Массив названий дней недели для вывода
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
    char buf[DESC_LEN + PLACE_LEN + 128]; // Буфер для строки файла
    char weekday_str[16]; // Буфер для названия дня недели из файла

    if (!fgets(buf, sizeof(buf), f)) return 0; // Чтение строки, если конец файла - выход

    // Удаляем перевод строки
    buf[strcspn(buf, "\n")] = 0;

    // Формат: "%d-%d-%d %d:%d (%[^)]) | Place: %[^|]| %[^\n]"
    int parsed = sscanf(buf, "%d-%d-%d %d:%d (%15[^)]) | Place: %127[^|]| %255[^\n]",
        &e->year, &e->month, &e->day, &e->hour, &e->minute,
        weekday_str, e->place, e->description);

    if (parsed < 8) { // Если не все 8 полей считаны
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
    for (int i = 0; i < 7; ++i) { // Поиск соответствия строки дню недели
        if (strcmp(weekday_str, weekday_names[i]) == 0) {
            found_weekday = i;
            break;
        }
    }
    if (found_weekday == -1) { // Если день недели не распознан
        // Если расчет не удался - пересчитаем днем недели
        e->weekday = calc_weekday(e->year, e->month, e->day);
    } else {
        e->weekday = found_weekday; // Присваиваем индекс дня недели
    }
    return 1;
}

int main() {
    Node* root = NULL; // Корень дерева (изначально пуст)
    int choice; // Выбор пункта меню
    while (1) { // Бесконечный цикл меню
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
        if (scanf("%d", &choice) != 1) { // Проверка успешности ввода числа
            // Защита ввода: очищаем ввод
            while (getchar() != '\n');
            printf("Invalid input! Please try again.\n");
            continue;
        }
        if (choice == 0) { // Выход из программы
            break;
        }
        if (choice == 1) { // Добавить событие
            Event event; // Временная переменная для нового события
            printf("Year: "); scanf("%d", &event.year);
            printf("Month: "); scanf("%d", &event.month);
            printf("Day: "); scanf("%d", &event.day);

            if (!is_valid_date(event.year, event.month, event.day)) { // Проверка даты
                printf("Invalid date!\n");
                continue;
            }

            printf("Hour: "); scanf("%d", &event.hour);
            printf("Minute: "); scanf("%d", &event.minute);

            if (!is_valid_time(event.hour, event.minute)) { // Проверка времени
                printf("Invalid time!\n");
                continue;
            }

            getchar(); // Считываем перевод строки
            printf("Description (up to %d chars): ", DESC_LEN-1);
            fgets(event.description, DESC_LEN, stdin); // Ввод описания
            event.description[strcspn(event.description, "\n")] = 0; // Удаляем перевод строки

            printf("Place (up to %d chars): ", PLACE_LEN-1);
            fgets(event.place, PLACE_LEN, stdin); // Ввод места
            event.place[strcspn(event.place, "\n")] = 0;
            event.weekday = calc_weekday(event.year, event.month, event.day); // Вычисляем день недели
            root = add_event(root, event); // Добавляем событие в дерево
            printf("Event added!\n");

        } else if (choice == 2) { // Удалить событие
            Event event; // Временная переменная для удаляемого события
            printf("Enter date and time of event to delete:\n");
            printf("Year: "); scanf("%d", &event.year);
            printf("Month: "); scanf("%d", &event.month);
            printf("Day: "); scanf("%d", &event.day);
            printf("Hour: "); scanf("%d", &event.hour);
            printf("Minute: "); scanf("%d", &event.minute);
            event.weekday = calc_weekday(event.year, event.month, event.day); // День недели

            getchar(); // Считываем перевод строки
            printf("Description (up to %d chars): ", DESC_LEN-1);
            fgets(event.description, DESC_LEN, stdin);
            event.description[strcspn(event.description, "\n")] = 0;

            printf("Place (up to %d chars): ", PLACE_LEN-1);
            fgets(event.place, PLACE_LEN, stdin);
            event.place[strcspn(event.place, "\n")] = 0;
            root = delete_event(root, event); // Удаляем событие из дерева
            printf("Event deleted (if it was found).\n");
            
        } else if (choice == 3) {
            inorder(root); // Выводим все события
        } else if (choice == 4) {
            char place[PLACE_LEN]; // Строка для поиска по месту
            getchar(); // Считываем перевод строки
            printf("Enter place (or substring): ");
            fgets(place, PLACE_LEN, stdin);
            place[strcspn(place, "\n")] = 0;
            filter_by_place(root, place); // Фильтрация и вывод
        } else if (choice == 5) {
            int y, m, d, h, min, found = 0; // Параметры поиска и флаг нахождения
            struct timespec begin, end; // Для замера времени поиска
            double elapsed; // Прошедшее время
            printf("Year: "); scanf("%d", &y);
            printf("Month: "); scanf("%d", &m);
            printf("Day: "); scanf("%d", &d);
            printf("Hour: "); scanf("%d", &h);
            printf("Minute: "); scanf("%d", &min);

            // Используем clock_gettime для точного измерения
            if (clock_gettime(CLOCK_MONOTONIC, &begin) != 0) {
                perror("clock_gettime");
            }

            search_by_datetime(root, y, m, d, h, min, &found); // Поиск события

            if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
                perror("clock_gettime");
            }

            elapsed = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1000000000.0; // Расчет прошедшего времени
            printf("Search time: %.9f seconds\n", elapsed);
            if (!found) printf("Event not found.\n");
        } else if (choice == 6) {
            char fname[128]; // Имя файла для сохранения
            getchar(); // Считываем перевод строки
            printf("Enter file name: ");
            fgets(fname, 128, stdin);
            fname[strcspn(fname, "\n")] = 0;
            save_events(root, fname); // Сохраняем в файл
        } else if (choice == 7) {
            char fname[128]; // Имя файла для загрузки
            getchar(); // Считываем перевод строки
            printf("Enter file name: ");
            fgets(fname, 128, stdin);
            fname[strcspn(fname, "\n")] = 0;
            free_tree(root); // Очищаем текущее дерево
            root = load_events(fname); // Загружаем из файла
        } else {
            printf("Invalid choice, please try again.\n");
        }
    }
    free_tree(root); // Освобождаем всю память перед выходом
    printf("Goodbye!\n");
    return 0;
}

// ---- Реализация функций ----

// Проверка корректности даты
int is_valid_date(int year, int month, int day) {
    if (year < 1900 || year > 2100) return 0; // Проверка года
    if (month < 1 || month > 12) return 0; // Проверка месяца
    int days_in_month[] = { 31,28,31,30,31,30,31,31,30,31,30,31 }; // Дни в месяцах
    // Проверка на високосный год
    if ( (year%4==0&&year%100!=0)||year%400==0 ) days_in_month[1]=29; // Февраль в високосном году
    if (day<1 || day>days_in_month[month-1]) return 0; // Проверка дня
    return 1;
}

// Проверка корректности времени
int is_valid_time(int hour, int minute) {
    return (hour >= 0 && hour < 24 && minute >= 0 && minute < 60); // Часы 0-23, минуты 0-59
}

// Алгоритм Целлера для вычисления дня недели: 0-Понедельник, 1-Вторник, ..., 6-Воскресенье
int calc_weekday(int year, int month, int day) {
    // Целлер возвращает: 0=Суббота, 1=Воскресенье, ..., 6=Пятница
    // Нужно получить: 0=Понедельник, ..., 6=Воскресенье
    if (month < 3) { month += 12; year--; } // Для формулы Целлера январь и февраль считаются 13 и 14 месяцем предыдущего года
    int K = year % 100; // Последние две цифры года
    int J = year / 100; // Первые две цифры года (век)
    int h = (day + 13*(month+1)/5 + K + K/4 + J/4 + 5*J) % 7; // Формула Целлера
    // h: 0=Суббота, 1=Воскресенье, ..., 6=Пятница
    // map: Пн=0,Вт=1,Ср=2,Чт=3,Пт=4,Сб=5,Вс=6
    int orig_weekday = h; // 0=Сб, ..., 6=Пт
    int convert[] = {5, 6, 0, 1, 2, 3, 4}; // Целлер->наш индекс (маппинг)
    return convert[orig_weekday]; // Возвращает день недели в нашем формате
}

// Сравнение двух событий для сортировки по дате, времени, описанию и месту
int compare_events(Event e1, Event e2) {
    if (e1.year != e2.year) return e1.year - e2.year; // Сравнение годов
    if (e1.month != e2.month) return e1.month - e2.month; // Сравнение месяцев
    if (e1.day != e2.day) return e1.day - e2.day; // Сравнение дней
    if (e1.hour != e2.hour) return e1.hour - e2.hour; // Сравнение часов
    if (e1.minute != e2.minute) return e1.minute - e2.minute; // Сравнение минут
    int cmpd = strcmp(e1.description, e2.description); // Сравнение описаний
    if (cmpd != 0) return cmpd;
    return strcmp(e1.place, e2.place); // Сравнение мест
}

// Добавить событие в бинарное дерево поиска
Node* add_event(Node* root, Event event) {
    if (root == NULL) { // Если дошли до пустого места
        Node* n = (Node*)malloc(sizeof(Node)); // Выделяем память под узел
        n->event = event; // Копируем событие
        n->left = n->right = NULL; // Левый и правый потомки пусты
        return n; // Возвращаем новый узел
    }
    if (compare_events(event, root->event) < 0) { // Если новое событие "меньше" текущего
        root->left = add_event(root->left, event); // Рекурсивно вставляем в левое поддерево
    } else {
        root->right = add_event(root->right, event); // Иначе вставляем в правое поддерево
    }
    return root; // Возвращаем (возможно, измененный) корень
}

// Найти узел с минимальным значением (самый левый в поддереве)
Node* min_value_node(Node* node) {
    Node* current = node;
    while (current && current->left != NULL) // Идем в самый левый узел
        current = current->left;
    return current; // Возвращаем узел с минимальным значением
}

// Удалить событие из дерева поиска
Node* delete_event(Node* root, Event event) {
    if (!root) return root; // Если дерево пустое
    int cmp = compare_events(event, root->event); // Сравниваем удаляемое событие с текущим узлом
    if (cmp < 0) { // Если удаляемое меньше
        root->left = delete_event(root->left, event); // Ищем в левом поддереве
    } else if (cmp > 0) { // Если удаляемое больше
        root->right = delete_event(root->right, event); // Ищем в правом поддереве
    } else { // Нашли узел для удаления
        if (!root->left) { // Если нет левого потомка
            Node* temp = root->right; // Сохраняем правое поддерево
            free(root); // Освобождаем память
            return temp; // Возвращаем правое поддерево как нового потомка
        } else if (!root->right) { // Если нет правого потомка
            Node* temp = root->left; // Сохраняем левое поддерево
            free(root); // Освобождаем память
            return temp; // Возвращаем левое поддерево
        }   // Случай с двумя потомками
        Node* temp = min_value_node(root->right); // Находим минимальный узел в правом поддереве
        root->event = temp->event; // Копируем данные из найденного узла в текущий
        root->right = delete_event(root->right, temp->event); // Удаляем минимальный узел в правом поддереве
    }
    return root; // Возвращаем измененный корень
}

// Вывод информации о событии
void print_event(Event event) {
    char *weekday_names[] = { // Массив названий дней недели
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
    if (!root) return; // Если узел пуст - выход
    inorder(root->left); // Обход левого поддерева
    print_event(root->event); // Вывод текущего события
    inorder(root->right); // Обход правого поддерева
}

// Фильтрация по подстроке места события
void filter_by_place(Node* root, const char* place_substr) {
    if (!root) return; // Если узел пуст
    filter_by_place(root->left, place_substr); // Проверяем левое поддерево
    if (strstr(root->event.place, place_substr)) // Если место содержит подстроку
        print_event(root->event); // Выводим событие
    filter_by_place(root->right, place_substr); // Проверяем правое поддерево
}

void search_by_datetime(Node* root, int year, int month, int day, int hour, int minute, int *found) {
    if (!root) return; // Если узел пуст
    search_by_datetime(root->left, year, month, day, hour, minute, found); // Поиск в левом поддереве
    if (root->event.year == year && root->event.month == month && root->event.day == day && // Проверка всех полей даты/времени
        root->event.hour == hour && root->event.minute == minute)
    {
        print_event(root->event); // Выводим найденное событие
        *found = 1; // Устанавливаем флаг, что событие найдено
    }
    search_by_datetime(root->right, year, month, day, hour, minute, found); // Поиск в правом поддереве
}

// Рекурсивная запись событий в текстовый файл (формат консоли)
void write_node_text(FILE* f, Node* node) {
    if (!node) return; // Если узел пуст
    write_node_text(f, node->left); // Запись левого поддерева
    write_event_text(f, &node->event); // Запись текущего события
    write_node_text(f, node->right); // Запись правого поддерева
}

// Сохранение событий дерева в текстовый файл (человекочитаемо, формат консоли)
void save_events(Node* root, const char* filename) {
    FILE* f = fopen(filename, "w"); // Открываем файл для записи
    if (!f) { 
        printf("Error opening file for writing\n"); 
        return; 
    }
    write_node_text(f, root); // Рекурсивно записываем все события
    fclose(f); // Закрываем файл
    printf("Events successfully saved.\n");
}

// Загрузка событий из текстового файла и их добавление в дерево (формат консоли)
Node* load_events(const char* filename) {
    FILE* f = fopen(filename, "r"); // Открываем файл для чтения
    if (!f) { 
        printf("Error opening file for reading\n"); 
        return NULL; 
    }
    Node* root = NULL; // Изначально пустое дерево
    Event e;
    while (read_event_text(f, &e)) { // Пока успешно читаем события
        root = add_event(root, e); // Добавляем в дерево
    }
    fclose(f); // Закрываем файл
    printf("Events successfully loaded.\n");
    return root; // Возвращаем корень построенного дерева
}

// Освобождение выделенной памяти под дерево
void free_tree(Node* root) {
    if (!root) return; // Если узел пуст
    free_tree(root->left); // Освобождаем левое поддерево
    free_tree(root->right); // Освобождаем правое поддерево
    free(root); // Освобождаем текущий узел
}