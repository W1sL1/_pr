#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<time.h>
#include<string.h>
// traverse - обход по всем элементам

typedef struct sosiska {
	int year;
	int mon;
	int day;
	int hour;
	int min;
	
	char day_of_week[100];
	char description[2000];
	char place[1000];
	int importance;

	struct Node* pLeft;
	struct Node* pRight;
} Node;

Node* Date_Root = NULL;  // корень дерева сортировки по дате
Node* Imp_Root = NULL;  // корень дерева сортировки по важности
Node* Place_Root = NULL;  // корень дерева сортировки по месту

Node* createNewNode(Node item) {
	Node* p = (Node*)malloc(sizeof(Node));
	p->year = item.year;
	p->mon = item.mon;
	p->day = item.day;
	p->hour = item.hour;
	p->min = item.min;
	strcpy(p->day_of_week, item.day_of_week);
	strcpy(p->description, item.description);
	strcpy(p->place, item.place);
	p->importance = item.importance;

	p->pLeft = NULL;
	p->pRight = NULL;
	return p; 
}

void add_to_Imp_tree(Node item) {
	if (Imp_Root == NULL) {
		Imp_Root = createNewNode(item);
	}
	else {
		Node* p = Imp_Root;
		while (1) {
			if (p->importance >= item.importance) {
				if (p->pLeft == NULL) {
					p->pLeft = createNewNode(item);
					break;
				}
				p = p->pLeft;
			}
			else {
				if (p->pRight == NULL) {
					p->pRight = createNewNode(item);
					break;
				}
				p = p->pRight;
			}
		}
	}
}

void add_to_Date_tree(Node item) {
	if (Date_Root == NULL) {
		Date_Root = createNewNode(item);
	}
	else {
		Node* p = Date_Root;
		long long event_time_item = ((item.year - 1970) * 525600) + item.mon * 43800 + item.day * 1400 + item.hour * 60 + item.min - 27180; // в конце поправочка на високосный год и всяеую фигню
		while (1) {
			long long event_time_p = ((p->year - 1970) * 525600) + p->mon * 43800 + p->day * 1400 + p->hour * 60 + p->min - 27180; // в конце поправочка для корректности
			if (event_time_p >= event_time_item) {
				if (p->pLeft == NULL) {
					p->pLeft = createNewNode(item);
					break;
				}
				p = p->pLeft;
			}
			else {
				if (p->pRight == NULL) {
					p->pRight = createNewNode(item);
					break;
				}
				p = p->pRight;
			}
		}
	}
}

void add_to_Place_tree(Node item) {
	if (Place_Root == NULL) {
		Place_Root = createNewNode(item);
	}
	else {
		Node* p = Place_Root;
		while (1) {
			if (strcmp(p->place, item.place) >= 0) {  // значение первой строки больше чем значение второй
				if (p->pLeft == NULL) {
					p->pLeft = createNewNode(item);
					break;
				}
				p = p->pLeft;
			}
			else {
				if (p->pRight == NULL) {
					p->pRight = createNewNode(item);
					break;
				}
				p = p->pRight;
			}
		}
	}
}

void Traverse(Node *p) {
	if (p == NULL) return;
	Traverse(p->pLeft);

	printf("%d.%d.%d %s", p->day, p->mon, p->year, p->day_of_week); // в day_of_week включен символ \n
	printf("%d:", p->hour);
	if (p->min < 10) printf("0"); // чтобы 13:01 не выглядело как 13:1
	printf("%d\n", p->min);

	printf("Description: ");
	fputs(p->description, stdout);
	printf("Place: ");
	fputs(p->place, stdout);

	printf("Importance: %d\n", p->importance);
	printf("\n");

	Traverse(p->pRight);
}

int if_data_correct(Node item) {
	int date_correct = 0;

	time_t current_time = time(NULL) / 60;

	long long event_time;
	//считаем введенное время в тиках
	event_time = ((item.year - 1970) * 525600 + item.mon * 43800 + item.day * 1400 + item.hour * 60 + item.min) - 27180; // в конце поправочка
	if (event_time > current_time) { 
		if ((item.year < 3000)) {
			if ((item.mon <= 12)) {
				if ((item.hour <= 24)) { // проверка на корректность отдельно снизу - с учетом месяца
					if ((item.mon == 2) && (item.day <= 29)) {
						if (item.year % 4 == 0) {
							if (item.year % 100 == 0) {
								if (item.year % 400 == 0) {
									if (item.day <= 29) date_correct = 1;  // год делится на 400
								}
							}
							else {
								if (item.day <= 29) date_correct = 1; // год делится на 4 и не делится на 100
							}
						}
						else {
							if (item.day <= 28) date_correct = 1;  // невисокосный год
						}
					}
					else if (item.mon % 2 == 1) {
						if (item.day <= 31) date_correct = 1; // нечетный месяц
					}
					else {
						if (item.day <= 30) date_correct = 1; // четный месяц
					}
				}
			}
		}
	}
	if (date_correct == 0) {
		printf("Date incorrect, try again. (Year must be less than 3000)\n");
		return 0;
	}
	if ((item.importance > 10) || (item.importance < 0)) {
		//printf("%d", item.importance);
		printf("Incorrect importance. Input value from 0 to 10. Try again.\n");
		return 0;
	}
	return 1;
}

int isLeapYear(int year) {
	if (year % 400 == 0) {
		return 1;
	}
	if (year % 100 == 0) {
		return 0;
	}
	if (year % 4 == 0) {
		return 1;
	}
	return 0;
}

void getDayOfWeek(int year, int month, int day, char* arr) {
	int monthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (isLeapYear(year)) {
		monthDays[1] = 29;
	}

	// Calculate the total number of days from the base date (e.g., 01/01/0000)
	long long totalDays = day;
	for (int i = 0; i < month - 1; i++) {
		totalDays += monthDays[i];
	}
	for (int i = 0; i < year; i++) {
		totalDays += (isLeapYear(i) ? 366 : 365);
	}

	// Day of the week calculation
	int dayOfWeek = totalDays % 7;
	char dayNames[10][50] = {"Friday\n", "Saturday\n", "Sunday\n", "Monday\n", "Tuesday\n", "Wednesday\n", "Thursday\n"};

	if (dayOfWeek == 0) strcpy(arr, dayNames[0]);
	else if (dayOfWeek == 1) strcpy(arr, dayNames[1]);
	else if (dayOfWeek == 2) strcpy(arr, dayNames[2]);
	else if (dayOfWeek == 3) strcpy(arr, dayNames[3]);
	else if (dayOfWeek == 4) strcpy(arr, dayNames[4]);
	else if (dayOfWeek == 5) strcpy(arr, dayNames[5]);
	else if (dayOfWeek == 6) strcpy(arr, dayNames[6]);
}

Node ask_for_new_object() {
	Node curr_item;

	do {
		printf("input year of event: ");
		scanf("%d", &(curr_item.year));
		printf("input mounth of event: ");
		scanf("%d", &(curr_item.mon));
		printf("input day of event: ");
		scanf("%d", &(curr_item.day));
		printf("input hour of event: ");
		scanf("%d", &(curr_item.hour));
		printf("input minute of event: ");
		scanf("%d", &(curr_item.min));

		char aaaa[213];  // хз почему но не хочет работать gets с первого включения
		fgets(aaaa, sizeof(aaaa), stdin); // понял)) - он забирает какой-то символ \n
		getDayOfWeek(curr_item.year, curr_item.mon, curr_item.day, curr_item.day_of_week);

		printf("input description of event: ");
		fgets(curr_item.description, sizeof(curr_item.description), stdin);
		//gets(curr_item.description);
		printf("input place of event: ");
		fgets(curr_item.place, sizeof(curr_item.place), stdin);
		//gets(curr_item.place);
		printf("input importance of event: ");
		scanf("%d", &(curr_item.importance));

		if (if_data_correct(curr_item) == 1) break;
	} while (1);

	return curr_item;
}

Node take_item_from_file(FILE* file) {
	Node item;
	fscanf(file, "%d\n", &(item.year));
	fscanf(file, "%d\n", &(item.mon));
	fscanf(file, "%d\n", &(item.day));
	fscanf(file, "%d\n", &(item.hour));
	fscanf(file, "%d\n", &(item.min));
	fgets(item.day_of_week, 100, file);
	fgets(item.description, 2000, file);
	fgets(item.place, 1000, file);
	fscanf(file, "%d\n", &(item.importance));

	return item;
}

void save_tree(FILE* file, Node* p) {
	if (p == NULL) return;
	save_tree(file, p->pLeft);
	fprintf(file, "%d\n", p->year);
	fprintf(file, "%d\n", p->mon);
	fprintf(file, "%d\n", p->day);
	fprintf(file, "%d\n", p->hour);
	fprintf(file, "%d\n", p->min);

	fputs(p->day_of_week, file);
	fputs(p->description, file);
	fputs(p->place, file);
	fprintf(file, "%d\n", p->importance);
	save_tree(file, p->pRight);
	
}
/*
Node* search_by_time(Node item, Node* p, Node** parent) {
	long long event_time_item = 60 * ((item.year - 1970) * 525600 + item.mon * 43800 + item.day * 1400 + item.hour * 60 + item.min) - 1630800; // в конце поправочка
	while (1) {
		long long event_time_p = 60 * ((p->year - 1970) * 525600 + p->mon * 43800 + p->day * 1400 + p->hour * 60 + p->min) - 1630800; // в конце поправочка для корректности
		if (event_time_p >= event_time_item) {
			if (event_time_item == event_time_p) {
				return p;
				break;
			}
			parent = &p;
			p = p->pLeft;
		}
		else {
			if (event_time_item == event_time_p) {
				return p;
				break;
			}
			parent = &p;
			p = p->pRight;
		}
	}
	printf("Date incorrect\n");
	return NULL;
}
void delete_event(Node itemToDelete) {
	// сначала удалим из дерева Imp_Root
	Node* parent = NULL;
	Node* p = search_by_time(itemToDelete, Imp_Root, &parent);
	if (p == NULL) return;
	
	// если потомков нет, то просто удаляем элемент
	//if (((*p)->pLeft == NULL) && ((*p)->pRight == NULL)) (*p) = NULL;

	else {
		// элемент, который поместим на место удаляемого
		Node* replaced_item = p;
		if (replaced_item->pLeft == NULL) replaced_item = p->pRight;

		// если есть два потомка - ищем максимальный элемент в левом поддереве (и потом ставим его на место удаляемого элемента
		else {
			replaced_item = replaced_item->pLeft;

			// если потомков нет, то элемент слева от p - максимальный в левом поддереве (единственный)
			if ((replaced_item->pLeft == NULL) && (replaced_item->pRight == NULL));
			else{
				// двигаемся вправо до упора по левому поддереву (и берем с конца максимальный элемент)
				while ((replaced_item)->pRight != NULL) {
					replaced_item = ((replaced_item)->pRight);
				}
			}
		}
		//нашли максимальный элемент, теперь копируем его значения в текущий элемент, а сам элемент потом удаляем
		(p)->year = (replaced_item)->year;
		(p)->mon = (replaced_item)->mon;
		(p)->day = (replaced_item)->day;
		(p)->hour = (replaced_item)->hour;
		(p)->min = (replaced_item)->min;
		strcpy((p)->day_of_week, (replaced_item)->day_of_week);
		strcpy((p)->description, (replaced_item)->description);
		strcpy((p)->place, (replaced_item)->place);
		(p)->importance = (replaced_item)->importance;

		replaced_item = NULL;
	}
}
*/

Node* search_by_time(Node item, Node* p, int* left_or_right) {
	Node* parent = NULL;
	long long event_time_item = 60 * ((item.year - 1970) * 525600 + item.mon * 43800 + item.day * 1400 + item.hour * 60 + item.min) - 1630800; // в конце поправочка
	while (1) {
		long long event_time_p = 60 * ((p->year - 1970) * 525600 + p->mon * 43800 + p->day * 1400 + p->hour * 60 + p->min) - 1630800; // в конце поправочка для корректности
		if (item.importance <= p->importance) {
			if (event_time_item == event_time_p) {
				return parent;
				break;
			}
			*left_or_right = 0;
			parent = p;
			p = p->pLeft;
		}
		else {
			if (event_time_item == event_time_p) {
				return parent;
				break;
			}
			*left_or_right = 1;
			parent = p;
			p = p->pRight;
		}
	}
	printf("Date incorrect\n");
	return NULL;
}

void copy_item(Node* destination, Node* sourse) { // копирует p2 в p1
	(destination)->year = (sourse)->year;
	(destination)->mon = (sourse)->mon;
	(destination)->day = (sourse)->day;
	(destination)->hour = (sourse)->hour;
	(destination)->min = (sourse)->min;
	strcpy((destination)->day_of_week, (sourse)->day_of_week);
	strcpy((destination)->description, (sourse)->description);
	strcpy((destination)->place, (sourse)->place);
	(destination)->importance = (sourse)->importance;
}

void delete_event(Node itemToDelete) {
	// сначала удалим из дерева Imp_Root
	int left_or_right = 0; // 0 - если удаляем левого ребенка, 1 - если правого
	Node* parent = search_by_time(itemToDelete, Imp_Root, &left_or_right);
	
	Node* p;
	// если не потомок (удаляем вершину)
	if (parent == NULL) {
		parent = Imp_Root;
		if (parent->pLeft == NULL && parent->pRight == NULL) {
			Imp_Root = NULL;
			return;
		}
		else if (parent->pLeft != NULL) {
			p = parent->pLeft;
			if (p->pLeft == NULL && p->pRight == NULL) {
				copy_item(parent, p);
				parent->pLeft = NULL;
				return;
			}
		}
		else {
			Imp_Root = Imp_Root->pRight;
			return;
		}
	}

	// если потомок слева
	else if (left_or_right == 0) {
		p = parent->pLeft;
		if ((p->pLeft == NULL) && (p->pRight == NULL)) {
			parent->pLeft = NULL;
			return;
		}
	}

	//если потомок справа
	else {
		p = parent->pRight;
		if ((p->pLeft == NULL) && (p->pRight == NULL)) {
			parent->pRight = NULL;
			return;
		}
	}

	// элемент, который поместим на место удаляемого
	Node* replaced_item = p;
	Node* parent_of_rp = parent;
	if (replaced_item->pLeft == NULL) {
		parent_of_rp = replaced_item;
		replaced_item = p->pRight;

		//нашли максимальный элемент, теперь копируем его значения в текущий элемент, а сам элемент потом удаляем
		copy_item(p, replaced_item);

		parent_of_rp->pRight = NULL;
	}

	// если есть два потомка - ищем максимальный элемент в левом поддереве (и потом ставим его на место удаляемого элемента
	else {
		parent_of_rp = replaced_item;
		replaced_item = replaced_item->pLeft;

		// если потомков нет, то элемент слева от p - максимальный в левом поддереве (единственный)
		if ((replaced_item->pLeft == NULL) && (replaced_item->pRight == NULL)) {
			//нашли максимальный элемент, теперь копируем его значения в текущий элемент, а сам элемент потом удаляем
			copy_item(p, replaced_item);

			parent_of_rp->pLeft = NULL;
		}
		else {
			// двигаемся вправо до упора по левому поддереву (и берем с конца максимальный элемент)
			while ((replaced_item)->pRight != NULL) {
				parent_of_rp = replaced_item;
				replaced_item = ((replaced_item)->pRight);
			}
			//нашли максимальный элемент, теперь копируем его значения в текущий элемент, а сам элемент потом удаляем
			copy_item(p, replaced_item);

			parent_of_rp->pRight = NULL;
		}
	}
}

int isFileEmpty(FILE* file) {
	// сохраняем текущую позицию
	int currentPosition = ftell(file);
	// находим конец файла
	fseek(file, 0, SEEK_END);
	// берем позицию конца файла
	int endPosition = ftell(file);
	// восстанавливаем изначальную позицию
	fseek(file, currentPosition, SEEK_SET);
	// проверяем файл на пустоту
	return (endPosition == 0);
}

void download_file(FILE* file) {
	if (isFileEmpty(file)) return; // проверка файла на пустоту
	// загрузка информации из файла
	while (!feof(file)) {
		Node curr_item = take_item_from_file(file);
		add_to_Imp_tree(curr_item);
		add_to_Date_tree(curr_item);
		add_to_Place_tree(curr_item);
	}
}

int main() {

	FILE* file;
	file = fopen("knizhka.txt", "a+");
	download_file(file);
	fclose(file);

	char input[100];
	// 0 - сохранить и выйти
	// 1 - добавить элемент в дерево
	// 2 - показать список дел, отсортированный по дате
	// 3 - показать список дел, отсортированный по важности
	// 4 - показать список дел, отсортированный по месту 
	// 5 - удалить элемент
	while (1) {
		printf("choose action:\n0 - save and exit\n1 - add new event\n2 - show list, sorted by date\n3 - show list, sorted by importance\n");
		printf("4 - show list, sorted by place\n5 - delete event\n");
		scanf("%s", &input);
		if (input[0] == '0' && input[1] == '\0') {
			// очищаем содержимое файла
			FILE* file1 = fopen("knizhka.txt", "w");
			save_tree(file1, Imp_Root);
			fclose(file1);
			break;
		}
		else if (input[0] == '1' && input[1] == '\0') {
			Node curr_item = ask_for_new_object();
			add_to_Imp_tree(curr_item);
			puts("\nItem added to list\n");
			
			// очищаем содержимое файла
			FILE* file1 = fopen("knizhka.txt", "w");
			save_tree(file1, Imp_Root);
			fclose(file1);
			// очищаем деревья
			Date_Root = NULL;  // корень дерева сортировки по дате
			Imp_Root = NULL;  // корень дерева сортировки по важности
			Place_Root = NULL;  // корень дерева сортировки по месту
			// загружаем обновленный файл
			FILE* file2 = fopen("knizhka.txt", "r");
			download_file(file2);
			fclose(file2);
		}
		else if (input[0] == '2' && input[1] == '\0') {
			printf("\nSorted by date list of events:\n");
			Traverse(Date_Root);
		}
		else if (input[0] == '3' && input[1] == '\0') {
			printf("\nSorted by importance list of events:\n");
			Traverse(Imp_Root);
		}
		else if (input[0] == '4' && input[1] == '\0') {
			printf("\nSorted by place list of events:\n");
			Traverse(Place_Root);
		}
		else if (input[0] == '5' && input[1] == '\0') {
			Node itemToDelete;
			do {
				printf("\nInput date and time of event, you want to delete (format: hour minute day mounth year): ");
				scanf("%d %d %d %d %d", &(itemToDelete.hour), &(itemToDelete.min), &(itemToDelete.day), &(itemToDelete.mon), &(itemToDelete.year));
				printf("Input importance of event: ");
				scanf("%d", &(itemToDelete.importance));
			} while (if_data_correct(itemToDelete) == 0);

			delete_event(itemToDelete);

			// очищаем содержимое файла
			FILE* file1 = fopen("knizhka.txt", "w");
			save_tree(file1, Imp_Root);
			fclose(file1);
			// очищаем деревья
			Date_Root = NULL;  // корень дерева сортировки по дате
			Imp_Root = NULL;  // корень дерева сортировки по важности
			Place_Root = NULL;  // корень дерева сортировки по месту
			// загружаем обновленный файл
			FILE* file2 = fopen("knizhka.txt", "a+");
			download_file(file2);
			fclose(file2);
		}
		else {
			printf("\nIncorrect value. Try again :)\n");
		}
	}
	

	

	//Traverse(Imp_Root);
	//Node curr_item = ask_for_new_object();

	//printf("%d\n%s", if_data_correct(curr_item), curr_item.day_of_week);




}
/*
Данные в файле хранятся в виде (9 строк на одну запись):
2024 - год
12 - месяц
11 - день
1 - час
50 - минута
Saturday - день недели
sosiski - описание
dacha - место
10 - важность
*/
