#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Константа Пи для математических расчетов
#define M_PI 3.14159265358979323846

// --- Глобальные переменные для управления камерой ---
float rotationX = 60.0f;   // Начальный угол поворота по X
float rotationY = 0.0f;    // Начальный угол поворота по Y
float zoom = -15.0f;       // Дистанция камеры (зум)
int lastMouseX, lastMouseY; // Последние координаты мыши
int isDragging = 0;        // Флаг удержания кнопки мыши

// --- Управление функциями ---
int currentFunctionIndex = 0; // Индекс текущей функции
const int totalFunctions = 6;  // Всего функций в программе

// --- Прототипы функций ---
void printMenu(void);
float evaluateFunction(int index, float x, float y);
const char* getFunctionName(int index);

// --- БЛОК МАТЕМАТИЧЕСКИХ ФУНКЦИЙ ---

// Вычисление значения Z на основе выбранной функции
float evaluateFunction(int index, float x, float y) {
    switch (index) {
        case 0: 
            return sinf(x) * cosf(y);
        case 1: { // Sombrero (Мексиканская шляпа)
            float r = sqrtf(x * x + y * y);
            return (r == 0.0f) ? 1.0f : sinf(r) / r;
        }
        case 2: // Hyperbolic Paraboloid (Седло)
            return (x * x - y * y) * 0.1f;
        case 3: // Paraboloid (Параболоид)
            return (x * x + y * y) * 0.05f;
        case 4: // Egg-carton (Волны / Ячеистая поверхность)
            return (cosf(x) + cosf(y)) * 0.5f;
        case 5: // Gaussian Hill (Гауссиана)
            return expf(-(x * x + y * y) * 0.1f) * 2.0f;
        default: 
            return 0.0f;
    }
}

// Получение строкового имени функции для вывода в консоль
const char* getFunctionName(int index) {
    switch (index) {
        case 0: return "sin(x) * cos(y)";
        case 1: return "sin(sqrt(x^2 + y^2)) / sqrt(x^2 + y^2) [Sombrero]";
        case 2: return "(x^2 - y^2) * 0.1 [Saddle]";
        case 3: return "(x^2 + y^2) * 0.05 [Paraboloid]";
        case 4: return "(cos(x) + cos(y)) * 0.5 [Ripples]";
        case 5: return "2 * exp(-0.1 * (x^2 + y^2)) [Gaussian]";
        default: return "Unknown";
    }
}

// Вывод интерфейса и управления в консоль Windows
void printMenu(void) {
    system("cls"); // Очистка консоли (работает в Windows)
    printf("=== 3D Graph Visualizer ===\n");
    printf("Current function: %s\n", getFunctionName(currentFunctionIndex));
    printf("Controls:\n");
    printf("  Left mouse button + drag - rotate\n");
    printf("  Mouse wheel - zoom\n");
    printf("  F/f - enter new function\n");
    printf("  ESC - exit\n");
    printf("Available functions:\n");
    for (int i = 0; i < totalFunctions; i++) {
        printf("  %d. %s\n", i + 1, getFunctionName(i));
    }
    printf("===========================\n");
}

// --- БЛОК ОТРИСОВКИ (OPENGL) ---

// Отрисовка координатных осей (X - Красная, Y - Зеленая, Z - Синяя)
void drawAxes(void) {
    glBegin(GL_LINES);
    // Ось X
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-7.0f, 0.0f, 0.0f); glVertex3f(7.0f, 0.0f, 0.0f);
    // Ось Y
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -7.0f, 0.0f); glVertex3f(0.0f, 7.0f, 0.0f);
    // Ось Z
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -7.0f); glVertex3f(0.0f, 0.0f, 7.0f);
    glEnd();
}

// Отрисовка 3D графика функции в виде сетки (Wireframe)
void drawGraph(void) {
    float step = 0.2f;      // Шаг сетки (чем меньше, тем детализированнее)
    float range = 6.0f;     // Диапазон отрисовки от -range до range

    // Переключаем режим отображения на линии (сетка), чтобы график был прозрачным и четким
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glBegin(GL_QUADS);
    for (float x = -range; x < range; x += step) {
        for (float y = -range; y < range; y += step) {
            
            // Вычисляем координаты Z для 4 точек полигона
            float z1 = evaluateFunction(currentFunctionIndex, x, y);
            float z2 = evaluateFunction(currentFunctionIndex, x + step, y);
            float z3 = evaluateFunction(currentFunctionIndex, x + step, y + step);
            float z4 = evaluateFunction(currentFunctionIndex, x, y + step);

            // Динамический цвет зависит от высоты Z (создает красивый градиент)
            glColor3f((z1 + 1.0f) * 0.5f, 0.4f, 1.0f - (z1 + 1.0f) * 0.5f);
            glVertex3f(x, y, z1);

            glColor3f((z2 + 1.0f) * 0.5f, 0.4f, 1.0f - (z2 + 1.0f) * 0.5f);
            glVertex3f(x + step, y, z2);

            glColor3f((z3 + 1.0f) * 0.5f, 0.4f, 1.0f - (z3 + 1.0f) * 0.5f);
            glVertex3f(x + step, y + step, z3);

            glColor3f((z4 + 1.0f) * 0.5f, 0.4f, 1.0f - (z4 + 1.0f) * 0.5f);
            glVertex3f(x, y + step, z4);
        }
    }
    glEnd();
    
    // Возвращаем режим заполнения полигонов по умолчанию
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Главный callback отрисовки GLUT
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Позиционирование камеры
    glTranslatef(0.0f, 0.0f, zoom);
    glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 0.0f, 1.0f); // Вращение вокруг вертикальной оси Z

    drawAxes();
    drawGraph();

    glutSwapBuffers();
}

// Настройка проекции при изменении размеров окна
void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// --- БЛОК ОБРАБОТКИ ВВОДА (ИНТЕРАКТИВ) ---

// Обработка стандартных клавиш клавиатуры
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // Клавиша ESC
            exit(0);
            break;
        case 'f':
        case 'F': // Переключение функции вперед
            currentFunctionIndex = (currentFunctionIndex + 1) % totalFunctions;
            printMenu();
            glutPostRedisplay(); // Перерисовать окно
            break;
    }
}

// Обработка кликов мыши
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = 1;
            lastMouseX = x;
            lastMouseY = y;
        } else if (state == GLUT_UP) {
            isDragging = 0;
        }
    }
}

// Обработка движения мыши с зажатой кнопкой (вращение)
void motion(int x, int y) {
    if (isDragging) {
        rotationY += (x - lastMouseX) * 0.5f;
        rotationX += (y - lastMouseY) * 0.5f;
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

// Обработка колесика мыши (масштабирование) через FreeGLUT расширение
void mouseWheel(int wheel, int direction, int x, int y) {
    if (direction > 0) {
        zoom += 1.0f; // Приближение
    } else {
        zoom -= 1.0f; // Отдаление
    }
    glutPostRedisplay();
}

// --- ИНИЦИАЛИЗАЦИЯ И МЕЙН ---

void initOpenGL(void) {
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f); // Темный задний фон
    glEnable(GL_DEPTH_TEST);                 // Включение буфера глубины
}

int main(int argc, char** argv) {
    // Инициализация GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3D Graph Visualizer");

    // Регистрация коллбэков
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    
    // Специфичный для FreeGLUT (nupengl) коллбэк колесика мыши
    glutMouseWheelFunc(mouseWheel);

    initOpenGL();
    printMenu();

    // Запуск главного цикла
    glutMainLoop();
    return 0;
}
