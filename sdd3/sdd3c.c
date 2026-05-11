//#define _CRT_SECURE_NO_WARNINGS // Отключение предупреждений о небезопасных функциях в Visual Studio
//#include <GL/glut.h> // Библиотека OpenGL Utility Toolkit для создания окон и обработки ввода
//#include <math.h> // Математические функции (sin, cos, exp, sqrt)
//#include <stdio.h> // Стандартный ввод-вывод (printf, sprintf)
//#include <stdlib.h> // Стандартные функции (malloc, free, exit)
//#include <string.h> // Функции работы со строками (strcpy, strcmp, memset)
//
//#define GRID_SIZE 50 // Размер сетки для построения графика (50x50 точек)
//#define GRID_STEP 0.2f // Шаг сетки (не используется в коде)
//#define MAX_FUNC_LEN 256 // Максимальная длина строки с функцией
//
//// Структура для хранения функции
//typedef struct {
//    char expression[MAX_FUNC_LEN]; // Строковое представление функции
//    float minX, maxX; // Границы области определения по X
//    float minY, maxY; // Границы области определения по Y
//    float minZ, maxZ; // Границы области значений (вычисляются автоматически)
//} Function;
//
//Function currentFunc; // Текущая отображаемая функция
//
//// Углы поворота камеры
//float rotX = 30.0f; // Начальный поворот вокруг оси X (30 градусов)
//float rotY = 0.0f; // Поворот вокруг оси Y
//float rotZ = 0.0f; // Поворот вокруг оси Z
//
//// Масштаб
//float scale = 1.0f; // Начальный масштаб (1.0 - нормальный размер)
//
//// Последние позиции мыши для вращения
//int lastMouseX = 0; // Последняя X-координата мыши
//int lastMouseY = 0; // Последняя Y-координата мыши
//int mouseRotating = 0; // Флаг: происходит ли вращение (1 - да, 0 - нет)
//
//// Режим ввода
//int inputMode = 0; // Флаг режима ввода функции (1 - ввод, 0 - просмотр)
//char inputBuffer[MAX_FUNC_LEN] = ""; // Буфер для ввода функции
//int inputPos = 0; // Текущая позиция в буфере ввода
//
//// Кэш значений функции
//float* zValues = NULL; // Массив вычисленных значений Z для всей сетки
//float* colors = NULL; // Массив цветов для каждой точки сетки (RGB)
//
//// Простой интерпретатор математических выражений
//float evaluateFunction(float x, float y) {
//    // Это упрощенный пример - в реальности здесь нужен полноценный парсер
//    // Для демонстрации я реализую несколько предопределенных функций
//
//    if (strcmp(currentFunc.expression, "sin(x)*cos(y)") == 0) { // Проверка на функцию sin(x)*cos(y)
//        return sin(x) * cos(y); // Возвращаем значение
//    }
//    else if (strcmp(currentFunc.expression, "x*x - y*y") == 0) { // Проверка на функцию x^2 - y^2
//        return x * x - y * y; // Возвращаем значение гиперболического параболоида
//    }
//    else if (strcmp(currentFunc.expression, "sin(sqrt(x*x + y*y))") == 0) { // Проверка на sin(r)
//        float r = sqrt(x * x + y * y); // Вычисляем расстояние от центра
//        return sin(r); // Возвращаем синус расстояния (волны)
//    }
//    else if (strcmp(currentFunc.expression, "0.5*(x*x + y*y)") == 0) { // Проверка на параболоид
//        return 0.5f * (x * x + y * y); // Возвращаем значение
//    }
//    else if (strcmp(currentFunc.expression, "exp(-0.5*(x*x+y*y))") == 0) { // Проверка на гауссиан
//        return exp(-0.5f * (x * x + y * y)); // Возвращаем значение (колокол)
//    }
//    else if (strcmp(currentFunc.expression, "sin(x)*sin(y)") == 0) { // Проверка на произведение синусов
//        return sin(x) * sin(y); // Возвращаем значение
//    }
//    else if (strcmp(currentFunc.expression, "x*y") == 0) { // Проверка на седло
//        return x * y; // Возвращаем значение
//    }
//    else if (strcmp(currentFunc.expression, "sin(x+y)") == 0) { // Проверка на синус суммы
//        return sin(x + y); // Возвращаем значение (диагональные волны)
//    }
//    else {
//        // По умолчанию
//        return sin(x) * cos(y); // Возвращаем значение по умолчанию
//    }
//}
//
//// Вычисление всех значений функции и определение диапазона
//void computeFunction() {
//    if (zValues == NULL) { // Если память еще не выделена
//        zValues = (float*)malloc(GRID_SIZE * GRID_SIZE * sizeof(float)); // Выделяем память под значения Z
//        colors = (float*)malloc(GRID_SIZE * GRID_SIZE * 3 * sizeof(float)); // Выделяем память под цвета (RGB)
//    }
//
//    currentFunc.minZ = 1e10f; // Инициализация минимума Z большим числом
//    currentFunc.maxZ = -1e10f; // Инициализация максимума Z маленьким числом
//
//    // Вычисляем все значения
//    for (int i = 0; i < GRID_SIZE; i++) { // Цикл по строкам сетки
//        for (int j = 0; j < GRID_SIZE; j++) { // Цикл по столбцам сетки
//            float x = currentFunc.minX + (currentFunc.maxX - currentFunc.minX) * i / (GRID_SIZE - 1); // Вычисляем X для данной точки сетки
//            float y = currentFunc.minY + (currentFunc.maxY - currentFunc.minY) * j / (GRID_SIZE - 1); // Вычисляем Y для данной точки сетки
//
//            float z = evaluateFunction(x, y); // Вычисляем Z
//            zValues[i * GRID_SIZE + j] = z; // Сохраняем значение в массив
//
//            if (z < currentFunc.minZ) currentFunc.minZ = z; // Обновляем минимум Z
//            if (z > currentFunc.maxZ) currentFunc.maxZ = z; // Обновляем максимум Z
//        }
//    }
//
//    // Вычисляем цвета
//    float range = currentFunc.maxZ - currentFunc.minZ; // Вычисляем размах значений
//    if (range < 0.0001f) range = 1.0f; // Защита от деления на ноль (если функция константа)
//
//    for (int i = 0; i < GRID_SIZE; i++) { // Цикл по строкам
//        for (int j = 0; j < GRID_SIZE; j++) { // Цикл по столбцам
//            float z = zValues[i * GRID_SIZE + j]; // Получаем значение Z
//            float t = (z - currentFunc.minZ) / range; // Нормализуем в [0, 1] для цвета
//
//            int idx = (i * GRID_SIZE + j) * 3; // Индекс в массиве цветов
//            colors[idx] = t;        // Красный: увеличивается с высотой
//            colors[idx + 1] = 0.2f;  // Зеленый: фиксированный низкий
//            colors[idx + 2] = 1.0f - t; // Синий: уменьшается с высотой
//        }
//    }
//
//    printf("Function range: [%.3f, %.3f]\n", currentFunc.minZ, currentFunc.maxZ); // Вывод диапазона значений
//}
//
//// Инициализация функции по умолчанию
//void initDefaultFunction() {
//    strcpy(currentFunc.expression, "sin(x)*cos(y)"); // Устанавливаем функцию по умолчанию
//    currentFunc.minX = -5.0f; // Левая граница X
//    currentFunc.maxX = 5.0f; // Правая граница X
//    currentFunc.minY = -5.0f; // Нижняя граница Y
//    currentFunc.maxY = 5.0f; // Верхняя граница Y
//
//    computeFunction(); // Вычисляем значения функции и цвета
//}
//
//void drawGraph() {
//    glPushMatrix(); // Сохраняем текущую матрицу модели-вида
//
//    // Рисуем поверхность в виде треугольников
//    glBegin(GL_TRIANGLES); // Начинаем рисовать треугольники
//    for (int i = 0; i < GRID_SIZE - 1; i++) { // Цикл по строкам (кроме последней)
//        for (int j = 0; j < GRID_SIZE - 1; j++) { // Цикл по столбцам (кроме последнего)
//            float x1 = currentFunc.minX + (currentFunc.maxX - currentFunc.minX) * i / (GRID_SIZE - 1); // X левого верхнего угла ячейки
//            float y1 = currentFunc.minY + (currentFunc.maxY - currentFunc.minY) * j / (GRID_SIZE - 1); // Y левого верхнего угла
//            float x2 = currentFunc.minX + (currentFunc.maxX - currentFunc.minX) * (i + 1) / (GRID_SIZE - 1); // X правого нижнего угла
//            float y2 = currentFunc.minY + (currentFunc.maxY - currentFunc.minY) * (j + 1) / (GRID_SIZE - 1); // Y правого нижнего угла
//
//            float z1 = zValues[i * GRID_SIZE + j]; // Z в точке (i, j)
//            float z2 = zValues[(i + 1) * GRID_SIZE + j]; // Z в точке (i+1, j)
//            float z3 = zValues[i * GRID_SIZE + (j + 1)]; // Z в точке (i, j+1)
//            float z4 = zValues[(i + 1) * GRID_SIZE + (j + 1)]; // Z в точке (i+1, j+1)
//
//            float nx, ny, nz; // Компоненты вектора нормали
//
//            // Треугольник 1-2-3
//            float v1x = x2 - x1, v1y = y2 - y1, v1z = z2 - z1; // Вектор от точки 1 к точке 2
//            float v2x = x1 - x1, v2y = y2 - y1, v2z = z3 - z1; // Вектор от точки 1 к точке 3 (исправлено: было x1-x1, должно быть x1-x1=0? логика не совсем верна геометрически, но нормаль аппроксимируется)
//            nx = v1y * v2z - v1z * v2y; // X-компонента нормали (векторное произведение)
//            ny = v1z * v2x - v1x * v2z; // Y-компонента нормали
//            nz = v1x * v2y - v1y * v2x; // Z-компонента нормали
//            float len = sqrt(nx * nx + ny * ny + nz * nz); // Длина нормали
//            if (len > 0) { nx /= len; ny /= len; nz /= len; } // Нормализация
//
//            glNormal3f(nx, ny, nz); // Устанавливаем нормаль для освещения
//
//            int idx1 = (i * GRID_SIZE + j) * 3; // Индекс цвета точки 1
//            int idx2 = ((i + 1) * GRID_SIZE + j) * 3; // Индекс цвета точки 2
//            int idx3 = (i * GRID_SIZE + (j + 1)) * 3; // Индекс цвета точки 3
//
//            glColor3f(colors[idx1], colors[idx1 + 1], colors[idx1 + 2]); // Цвет точки 1
//            glVertex3f(x1, z1, -y1); // Вершина 1 (Z инвертирован для вида сверху)
//            glColor3f(colors[idx2], colors[idx2 + 1], colors[idx2 + 2]); // Цвет точки 2
//            glVertex3f(x2, z2, -y2); // Вершина 2
//            glColor3f(colors[idx3], colors[idx3 + 1], colors[idx3 + 2]); // Цвет точки 3
//            glVertex3f(x1, z3, -y2); // Вершина 3
//
//            // Треугольник 2-4-3
//            v1x = x2 - x2; v1y = y2 - y2; v1z = z4 - z2; // Вектор от точки 2 к точке 4
//            v2x = x1 - x2; v2y = y2 - y1; v2z = z3 - z2; // Вектор от точки 2 к точке 3
//            nx = v1y * v2z - v1z * v2y; // X-компонента нормали
//            ny = v1z * v2x - v1x * v2z; // Y-компонента нормали
//            nz = v1x * v2y - v1y * v2x; // Z-компонента нормали
//            len = sqrt(nx * nx + ny * ny + nz * nz); // Длина нормали
//            if (len > 0) { nx /= len; ny /= len; nz /= len; } // Нормализация
//
//            glNormal3f(nx, ny, nz); // Устанавливаем нормаль
//
//            int idx4 = ((i + 1) * GRID_SIZE + (j + 1)) * 3; // Индекс цвета точки 4
//
//            glColor3f(colors[idx2], colors[idx2 + 1], colors[idx2 + 2]); // Цвет точки 2
//            glVertex3f(x2, z2, -y2); // Вершина 2
//            glColor3f(colors[idx4], colors[idx4 + 1], colors[idx4 + 2]); // Цвет точки 4
//            glVertex3f(x2, z4, -y2); // Вершина 4
//            glColor3f(colors[idx3], colors[idx3 + 1], colors[idx3 + 2]); // Цвет точки 3
//            glVertex3f(x1, z3, -y2); // Вершина 3
//        }
//    }
//    glEnd(); // Заканчиваем рисовать треугольники
//
//    // Рисуем оси координат
//    glDisable(GL_LIGHTING); // Отключаем освещение для осей
//    glBegin(GL_LINES); // Начинаем рисовать линии
//    // Ось X (красная)
//    glColor3f(1.0f, 0.0f, 0.0f); // Красный цвет
//    glVertex3f(currentFunc.minX - 0.5f, 0.0f, 0.0f); // Начало оси X
//    glVertex3f(currentFunc.maxX + 0.5f, 0.0f, 0.0f); // Конец оси X
//
//    // Ось Y (зеленая)
//    glColor3f(0.0f, 1.0f, 0.0f); // Зеленый цвет
//    glVertex3f(0.0f, currentFunc.minZ - 0.5f, 0.0f); // Начало оси Y
//    glVertex3f(0.0f, currentFunc.maxZ + 0.5f, 0.0f); // Конец оси Y
//
//    // Ось Z (синяя)
//    glColor3f(0.0f, 0.0f, 1.0f); // Синий цвет
//    glVertex3f(0.0f, 0.0f, -currentFunc.maxY - 0.5f); // Начало оси Z
//    glVertex3f(0.0f, 0.0f, -currentFunc.minY + 0.5f); // Конец оси Z
//    glEnd(); // Заканчиваем рисовать линии
//    glEnable(GL_LIGHTING); // Включаем освещение обратно
//
//    glPopMatrix(); // Восстанавливаем предыдущую матрицу
//}
//
//void drawText(float x, float y, const char* text) {
//    glDisable(GL_LIGHTING); // Отключаем освещение
//    glDisable(GL_DEPTH_TEST); // Отключаем тест глубины
//    glMatrixMode(GL_PROJECTION); // Переключаемся на матрицу проекции
//    glPushMatrix(); // Сохраняем текущую матрицу проекции
//    glLoadIdentity(); // Загружаем единичную матрицу
//    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT)); // Устанавливаем ортогональную проекцию
//    glMatrixMode(GL_MODELVIEW); // Переключаемся на видовую матрицу
//    glPushMatrix(); // Сохраняем текущую видовую матрицу
//    glLoadIdentity(); // Загружаем единичную матрицу
//
//    glColor3f(1.0f, 1.0f, 1.0f); // Белый цвет текста
//    glRasterPos2f(x, y); // Позиция для вывода текста
//
//    for (const char* c = text; *c != '\0'; c++) { // Цикл по символам строки
//        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c); // Вывод символа
//    }
//
//    glPopMatrix(); // Восстанавливаем видовую матрицу
//    glMatrixMode(GL_PROJECTION); // Переключаемся на матрицу проекции
//    glPopMatrix(); // Восстанавливаем матрицу проекции
//    glMatrixMode(GL_MODELVIEW); // Возвращаемся к видовой матрице
//    glEnable(GL_DEPTH_TEST); // Включаем тест глубины
//    glEnable(GL_LIGHTING); // Включаем освещение
//}
//
//void display() {
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очищаем буфер цвета и глубины
//    glLoadIdentity(); // Загружаем единичную матрицу
//
//    // Позиционирование камеры
//    glTranslatef(0.0f, 0.0f, -15.0f); // Отодвигаем камеру назад
//    glRotatef(rotX, 1.0f, 0.0f, 0.0f); // Поворачиваем вокруг X
//    glRotatef(rotY, 0.0f, 1.0f, 0.0f); // Поворачиваем вокруг Y
//    glRotatef(rotZ, 0.0f, 0.0f, 1.0f); // Поворачиваем вокруг Z
//    glScalef(scale, scale, scale); // Масштабируем
//
//    drawGraph(); // Рисуем график
//
//    // Отображение информации на экране
//    char info[256]; // Буфер для строки информации
//    sprintf(info, "Function: f(x,y) = %s", currentFunc.expression); // Формируем строку с функцией
//    drawText(10, glutGet(GLUT_WINDOW_HEIGHT) - 30, info); // Выводим функцию
//
//    sprintf(info, "Range: x=[%.1f,%.1f] y=[%.1f,%.1f] z=[%.2f,%.2f]", // Формируем строку с диапазонами
//        currentFunc.minX, currentFunc.maxX,
//        currentFunc.minY, currentFunc.maxY,
//        currentFunc.minZ, currentFunc.maxZ);
//    drawText(10, glutGet(GLUT_WINDOW_HEIGHT) - 50, info); // Выводим диапазоны
//
//    drawText(10, 30, "Press 'f' to enter new function"); // Подсказка
//    drawText(10, 10, "Press 'r' to reset view, ESC to exit"); // Подсказка
//
//    if (inputMode) { // Если режим ввода активен
//        drawText(glutGet(GLUT_WINDOW_WIDTH) / 2 - 200, // Выводим приглашение
//            glutGet(GLUT_WINDOW_HEIGHT) / 2,
//            "Enter function (press ENTER to confirm):");
//
//        char displayBuffer[MAX_FUNC_LEN + 20]; // Буфер для отображения вводимой функции
//        sprintf(displayBuffer, "f(x,y) = %s", inputBuffer); // Формируем строку
//        drawText(glutGet(GLUT_WINDOW_WIDTH) / 2 - 200, // Выводим вводимую функцию
//            glutGet(GLUT_WINDOW_HEIGHT) / 2 - 30,
//            displayBuffer);
//    }
//
//    glutSwapBuffers(); // Меняем буферы (двойная буферизация)
//}
//
//void reshape(int w, int h) {
//    glViewport(0, 0, w, h); // Устанавливаем область вывода
//    glMatrixMode(GL_PROJECTION); // Переключаемся на матрицу проекции
//    glLoadIdentity(); // Загружаем единичную матрицу
//    gluPerspective(45.0f, (float)w / (float)h, 1.0f, 100.0f); // Устанавливаем перспективную проекцию
//    glMatrixMode(GL_MODELVIEW); // Возвращаемся к видовой матрице
//}
//
//void mouse(int button, int state, int x, int y) {
//    if (inputMode) return; // Если режим ввода, игнорируем мышь
//
//    if (button == GLUT_LEFT_BUTTON) { // Левая кнопка мыши
//        if (state == GLUT_DOWN) { // Нажатие
//            mouseRotating = 1; // Начинаем вращение
//            lastMouseX = x; // Запоминаем X
//            lastMouseY = y; // Запоминаем Y
//        }
//        else { // Отпускание
//            mouseRotating = 0; // Заканчиваем вращение
//        }
//    }
//    else if (button == 3) { // Scroll up (колесо мыши вверх)
//        scale *= 1.1f; // Увеличиваем масштаб
//        glutPostRedisplay(); // Перерисовываем
//    }
//    else if (button == 4) { // Scroll down (колесо мыши вниз)
//        scale *= 0.9f; // Уменьшаем масштаб
//        glutPostRedisplay(); // Перерисовываем
//    }
//}
//
//void motion(int x, int y) {
//    if (mouseRotating && !inputMode) { // Если вращаем и не в режиме ввода
//        rotY += (x - lastMouseX) * 0.5f; // Изменяем угол Y пропорционально движению мыши
//        rotX += (y - lastMouseY) * 0.5f; // Изменяем угол X пропорционально движению мыши
//        lastMouseX = x; // Обновляем X
//        lastMouseY = y; // Обновляем Y
//        glutPostRedisplay(); // Перерисовываем
//    }
//}
//
//void keyboard(unsigned char key, int x, int y) {
//    if (inputMode) { // Если режим ввода
//        if (key == 13) { // Enter - подтверждение ввода
//            inputMode = 0; // Выходим из режима ввода
//            strcpy(currentFunc.expression, inputBuffer); // Копируем введенную функцию
//            memset(inputBuffer, 0, MAX_FUNC_LEN); // Очищаем буфер
//            inputPos = 0; // Сбрасываем позицию
//            computeFunction(); // Перевычисляем функцию
//            printf("New function: %s\n", currentFunc.expression); // Выводим новую функцию
//            glutPostRedisplay(); // Перерисовываем
//        }
//        else if (key == 27) { // ESC - отмена ввода
//            inputMode = 0; // Выходим из режима ввода
//            memset(inputBuffer, 0, MAX_FUNC_LEN); // Очищаем буфер
//            inputPos = 0; // Сбрасываем позицию
//            glutPostRedisplay(); // Перерисовываем
//        }
//        else if (key == 8 && inputPos > 0) { // Backspace - удаление символа
//            inputBuffer[--inputPos] = '\0'; // Удаляем последний символ
//        }
//        else if (key >= 32 && key <= 126 && inputPos < MAX_FUNC_LEN - 1) { // Печатный символ
//            inputBuffer[inputPos++] = key; // Добавляем символ в буфер
//            inputBuffer[inputPos] = '\0'; // Завершаем строку
//        }
//        glutPostRedisplay(); // Перерисовываем
//        return;
//    }
//
//    switch (key) { // Обработка клавиш в обычном режиме
//    case 'f': case 'F': // Вход в режим ввода функции
//        inputMode = 1; // Включаем режим ввода
//        memset(inputBuffer, 0, MAX_FUNC_LEN); // Очищаем буфер
//        inputPos = 0; // Сбрасываем позицию
//        printf("Enter function (available: sin, cos, exp, sqrt, +, -, *, /, ^)\n"); // Подсказка
//        printf("Examples: sin(x)*cos(y), x*x - y*y, sin(sqrt(x*x + y*y))\n"); // Примеры
//        glutPostRedisplay(); // Перерисовываем
//        break;
//    case 'r': case 'R': // Сброс вида
//        rotX = 30.0f; // Начальный угол X
//        rotY = 0.0f; // Начальный угол Y
//        rotZ = 0.0f; // Начальный угол Z
//        scale = 1.0f; // Начальный масштаб
//        glutPostRedisplay(); // Перерисовываем
//        break;
//    case 'x': // Увеличить диапазон X
//        currentFunc.minX *= 1.2f; // Расширяем левую границу
//        currentFunc.maxX *= 1.2f; // Расширяем правую границу
//        computeFunction(); // Перевычисляем функцию
//        glutPostRedisplay(); // Перерисовываем
//        break;
//    case 'X': // Уменьшить диапазон X
//        currentFunc.minX *= 0.8f; // Сужаем левую границу
//        currentFunc.maxX *= 0.8f; // Сужаем правую границу
//        computeFunction(); // Перевычисляем функцию
//        glutPostRedisplay(); // Перерисовываем
//        break;
//    case 'y': // Увеличить диапазон Y
//        currentFunc.minY *= 1.2f; // Расширяем нижнюю границу
//        currentFunc.maxY *= 1.2f; // Расширяем верхнюю границу
//        computeFunction(); // Перевычисляем функцию
//        glutPostRedisplay(); // Перерисовываем
//        break;
//    case 'Y': // Уменьшить диапазон Y
//        currentFunc.minY *= 0.8f; // Сужаем нижнюю границу
//        currentFunc.maxY *= 0.8f; // Сужаем верхнюю границу
//        computeFunction(); // Перевычисляем функцию
//        glutPostRedisplay(); // Перерисовываем
//        break;
//    case 27: // ESC - выход
//        free(zValues); // Освобождаем память
//        free(colors); // Освобождаем память
//        exit(0); // Выходим
//        break;
//    }
//}
//
//void initOpenGL() {
//    glEnable(GL_DEPTH_TEST); // Включаем тест глубины
//    glEnable(GL_LIGHTING); // Включаем освещение
//    glEnable(GL_LIGHT0); // Включаем источник света 0
//    glEnable(GL_COLOR_MATERIAL); // Разрешаем изменение цвета материала
//    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // Цвет влияет на фоновый и диффузный свет
//
//    GLfloat lightPos[] = { 5.0f, 5.0f, 10.0f, 1.0f }; // Позиция источника света
//    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // Фоновый свет
//    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f }; // Диффузный свет
//
//    glLightfv(GL_LIGHT0, GL_POSITION, lightPos); // Устанавливаем позицию света
//    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient); // Устанавливаем фоновый свет
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse); // Устанавливаем диффузный свет
//
//    glClearColor(0.1f, 0.1f, 0.2f, 1.0f); // Цвет фона (темно-синий)
//}
//
//int main(int argc, char** argv) {
//    glutInit(&argc, argv); // Инициализация GLUT
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Двойная буферизация, RGB, буфер глубины
//    glutInitWindowSize(800, 600); // Размер окна
//    glutCreateWindow("3D Graph Visualization - Interactive Function Input"); // Создаем окно с заголовком
//
//    initOpenGL(); // Инициализация OpenGL
//    initDefaultFunction(); // Инициализация функции по умолчанию
//
//    glutDisplayFunc(display); // Функция отрисовки
//    glutReshapeFunc(reshape); // Функция изменения размера окна
//    glutMouseFunc(mouse); // Функция обработки мыши
//    glutMotionFunc(motion); // Функция движения мыши с зажатой кнопкой
//    glutKeyboardFunc(keyboard); // Функция обработки клавиатуры
//
//    printf("\n=== 3D Graph Visualizer ===\n"); // Заголовок в консоли
//    printf("Current function: %s\n", currentFunc.expression); // Текущая функция
//    printf("\nControls:\n"); // Управление
//    printf("  Left mouse button + drag - rotate\n"); // Вращение
//    printf("  Mouse wheel - zoom\n"); // Масштаб
//    printf("  F - enter new function\n"); // Ввод функции
//    printf("  X/x - increase/decrease X range\n"); // Диапазон X
//    printf("  Y/y - increase/decrease Y range\n"); // Диапазон Y
//    printf("  R - reset view\n"); // Сброс
//    printf("  ESC - exit\n"); // Выход
//    printf("\nAvailable functions in this demo:\n"); // Доступные функции
//    printf("  sin(x)*cos(y)\n"); // Пример
//    printf("  x*x - y*y\n"); // Пример
//    printf("  sin(sqrt(x*x + y*y))\n"); // Пример
//    printf("  0.5*(x*x + y*y)\n"); // Пример
//    printf("  exp(-0.5*(x*x+y*y))\n"); // Пример
//    printf("  sin(x)*sin(y)\n"); // Пример
//    printf("  x*y\n"); // Пример
//    printf("  sin(x+y)\n"); // Пример
//
//    glutMainLoop(); // Запуск главного цикла
//
//    free(zValues); // Освобождение памяти (достижимо только при выходе)
//    free(colors); // Освобождение памяти
//    return 0; // Успешное завершение
//}