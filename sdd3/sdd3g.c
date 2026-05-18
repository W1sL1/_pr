#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// ==========================================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ И НАСТРОЙКИ
// ==========================================
int currentFunc = 0;         // Индекс текущей функции (0-5)
const int totalFuncs = 6;    // Всего доступных функций

// Переменные для управления камерой (вращение и зум)
float rotX = 30.0f;
float rotY = -45.0f;
float zoom = -15.0f;

// Состояние мыши для вращения
int lastMouseX = 0;
int lastMouseY = 0;
int isLeftPressed = 0;

// Размеры сетки для построения графика
const float xMin = -10.0f, xMax = 10.0f;
const float yMin = -10.0f, yMax = 10.0f;
const float step = 0.3f; // Шаг сетки (чем меньше, тем детальнее график)

// ==========================================
// МАТЕМАТИЧЕСКИЕ ФУНКЦИИ (6 вариантов)
// ==========================================
float evaluateFunction(int index, float x, float y) {
    switch (index) {
        case 0:
            // 1. Стандартная функция из ТЗ
            return sinf(x) * cosf(y);
        
        case 1: {
            // 2. "Сомбреро" (Ripple)
            float r = sqrtf(x*x + y*y);
            if (r == 0.0f) return 2.0f; // Избегаем деления на ноль
            return 2.0f * sinf(r) / r;
        }
        case 2:
            // 3. Гиперболический параболоид (Седло)
            return (x*x - y*y) * 0.1f;
            
        case 3:
            // 4. Гауссиан (Холм)
            return 3.0f * expf(-0.1f * (x*x + y*y));
            
        case 4:
            // 5. "Ячеистая структура" (Egg Carton)
            return sinf(x) + cosf(y);
            
        case 5:
            // 6. Сложная интерференция волн
            return cosf(sqrtf(x*x + y*y)) + cosf(x) * sinf(y);
            
        default:
            return 0.0f;
    }
}

// ==========================================
// ИНТЕРФЕЙС И ВЫВОД ТЕКСТА
// ==========================================
void printMenu() {
    // Очистка консоли (работает на Windows и Linux)
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    printf("=== 3D Graph Visualizer ===\n");
    printf("Current function: ");
    switch(currentFunc) {
        case 0: printf("sin(x) * cos(y)\n"); break;
        case 1: printf("2 * sin(r) / r  [Sombrero]\n"); break;
        case 2: printf("0.1 * (x^2 - y^2)  [Saddle]\n"); break;
        case 3: printf("3 * exp(-0.1 * (x^2 + y^2))  [Gaussian Hill]\n"); break;
        case 4: printf("sin(x) + cos(y)  [Egg Carton]\n"); break;
        case 5: printf("cos(r) + sin(x)*cos(y)  [Wave Interference]\n"); break;
    }
    
    printf("\nControls:\n");
    printf("  Left mouse button + drag - rotate\n");
    printf("  Mouse wheel - zoom\n");
    printf("  F/f - enter new function (Switch)\n");
    printf("  ESC - exit\n");
    
    printf("\nAvailable functions:\n");
    printf("  1. sin(x)*cos(y)\n");
    printf("  2. sin(sqrt(x^2+y^2))/sqrt(x^2+y^2)\n");
    printf("  3. x^2 - y^2 (Hyperbolic Paraboloid)\n");
    printf("  4. exp(-x^2-y^2) (Gaussian)\n");
    printf("  5. sin(x) + cos(y)\n");
    printf("  6. cos(r) + cos(x)*sin(y)\n");
    printf("===========================\n");
}

// ==========================================
// ОТРИСОВКА ГРАФИКА И ОСЕЙ
// ==========================================
void drawAxes() {
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        // Ось X (Красная)
        glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(-11.0f, 0.0f, 0.0f); glVertex3f(11.0f, 0.0f, 0.0f);
        // Ось Y (Зеленая) - в OpenGL традиционно Z идет вверх, сделаем Y вертикальной для интуитивности математики
        glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(0.0f, -11.0f, 0.0f); glVertex3f(0.0f, 11.0f, 0.0f);
        // Ось Z (Синяя)
        glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(0.0f, 0.0f, -11.0f); glVertex3f(0.0f, 0.0f, 11.0f);
    glEnd();
}

void drawGraph() {
    // Будем рисовать сетку линиями для красивого 3D эффекта
    glLineWidth(1.0f);

    for (float x = xMin; x < xMax; x += step) {
        glBegin(GL_LINE_STRIP);
        for (float y = yMin; y <= yMax; y += step) {
            float z = evaluateFunction(currentFunc, x, y);
            
            // Динамический цвет в зависимости от высоты Z (от синего к красному)
            float colorFactor = (z + 2.0f) / 4.0f; 
            if (colorFactor < 0.0f) colorFactor = 0.0f;
            if (colorFactor > 1.0f) colorFactor = 1.0f;
            glColor3f(colorFactor, 0.5f, 1.0f - colorFactor);
            
            // В OpenGL: X=ширина, Y=высота(Z математическое), Z=глубина(Y математическое)
            glVertex3f(x, z, y);
        }
        glEnd();
    }

    // Отрисовка перпендикулярных линий сетки
    for (float y = yMin; y < yMax; y += step) {
        glBegin(GL_LINE_STRIP);
        for (float x = xMin; x <= xMax; x += step) {
            float z = evaluateFunction(currentFunc, x, y);
            float colorFactor = (z + 2.0f) / 4.0f;
            if (colorFactor < 0.0f) colorFactor = 0.0f;
            if (colorFactor > 1.0f) colorFactor = 1.0f;
            glColor3f(colorFactor, 0.5f, 1.0f - colorFactor);

            glVertex3f(x, z, y);
        }
        glEnd();
    }
}

// Основная функция рендеринга
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Позиционирование камеры
    glTranslatef(0.0f, 0.0f, zoom);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    drawAxes();
    drawGraph();

    glutSwapBuffers();
}

// ==========================================
// ОБРАБОТКА ВВОДНЫХ СОБЫТИЙ (Мышь и Клавиатура)
// ==========================================

// Нажатие кнопок мыши
void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isLeftPressed = 1;
            lastMouseX = x;
            lastMouseY = y;
        } else if (state == GLUT_UP) {
            isLeftPressed = 0;
        }
    }
}

// Движение мыши с зажатой кнопкой (Вращение)
void mouseMotion(int x, int y) {
    if (isLeftPressed) {
        rotY += (x - lastMouseX) * 0.5f;
        rotX += (y - lastMouseY) * 0.5f;
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay(); // Перерисовать окно
    }
}

// Скролл мыши (Зум)
void mouseWheel(int wheel, int direction, int x, int y) {
    if (direction > 0) {
        zoom += 1.0f; // Приближение
    } else {
        zoom -= 1.0f; // Отдаление
    }
    glutPostRedisplay();
}

// Нажатие клавиш
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // Клавиша ESC
            exit(0);
            break;
        case 'f':
        case 'F':
            // Переключение на следующий график
            currentFunc = (currentFunc + 1) % totalFuncs;
            printMenu();
            glutPostRedisplay();
            break;
    }
}

// ==========================================
// ИНИЦИАЛИЗАЦИЯ И НАСТРОЙКА ОКНА
// ==========================================
void initOpenGL() {
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f); // Темный задний фон
    glEnable(GL_DEPTH_TEST);                 // Включаем буфер глубины для корректного 3D отображения
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float ratio = (float)w / (float)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 0.1f, 100.0f); // Настройка перспективной проекции
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    // Инициализация GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3D Graph Visualizer");

    // Регистрация колбэков
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutMouseWheelFunc(mouseWheel); // Специфично для FreeGLUT (зум колесиком)

    // Настройка сцены и первый вывод меню
    initOpenGL();
    printMenu();

    // Основной цикл программы
    glutMainLoop();
    return 0;
}
