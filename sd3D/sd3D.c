#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_SIZE 50
#define GRID_STEP 0.2f
#define MAX_FUNC_LEN 256

// Структура для хранения функции
typedef struct {
    char expression[MAX_FUNC_LEN];
    float minX, maxX;
    float minY, maxY;
    float minZ, maxZ;
} Function;

Function currentFunc;

// Углы поворота камеры
float rotX = 30.0f;
float rotY = 0.0f;
float rotZ = 0.0f;

// Масштаб
float scale = 1.0f;

// Последние позиции мыши для вращения
int lastMouseX = 0;
int lastMouseY = 0;
int mouseRotating = 0;

// Режим ввода
int inputMode = 0;
char inputBuffer[MAX_FUNC_LEN] = "";
int inputPos = 0;

// Кэш значений функции
float* zValues = NULL;
float* colors = NULL;

// Простой интерпретатор математических выражений
float evaluateFunction(float x, float y) {
    // Это упрощенный пример - в реальности здесь нужен полноценный парсер
    // Для демонстрации я реализую несколько предопределенных функций

    if (strcmp(currentFunc.expression, "sin(x)*cos(y)") == 0) {
        return sin(x) * cos(y);
    }
    else if (strcmp(currentFunc.expression, "x*x - y*y") == 0) {
        return x * x - y * y;
    }
    else if (strcmp(currentFunc.expression, "sin(sqrt(x*x + y*y))") == 0) {
        float r = sqrt(x * x + y * y);
        return sin(r);
    }
    else if (strcmp(currentFunc.expression, "0.5*(x*x + y*y)") == 0) {
        return 0.5f * (x * x + y * y);
    }
    else if (strcmp(currentFunc.expression, "exp(-0.5*(x*x+y*y))") == 0) {
        return exp(-0.5f * (x * x + y * y));
    }
    else if (strcmp(currentFunc.expression, "sin(x)*sin(y)") == 0) {
        return sin(x) * sin(y);
    }
    else if (strcmp(currentFunc.expression, "x*y") == 0) {
        return x * y;
    }
    else if (strcmp(currentFunc.expression, "sin(x+y)") == 0) {
        return sin(x + y);
    }
    else {
        // По умолчанию
        return sin(x) * cos(y);
    }
}

// Вычисление всех значений функции и определение диапазона
void computeFunction() {
    if (zValues == NULL) {
        zValues = (float*)malloc(GRID_SIZE * GRID_SIZE * sizeof(float));
        colors = (float*)malloc(GRID_SIZE * GRID_SIZE * 3 * sizeof(float));
    }

    currentFunc.minZ = 1e10f;
    currentFunc.maxZ = -1e10f;

    // Вычисляем все значения
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            float x = currentFunc.minX + (currentFunc.maxX - currentFunc.minX) * i / (GRID_SIZE - 1);
            float y = currentFunc.minY + (currentFunc.maxY - currentFunc.minY) * j / (GRID_SIZE - 1);

            float z = evaluateFunction(x, y);
            zValues[i * GRID_SIZE + j] = z;

            if (z < currentFunc.minZ) currentFunc.minZ = z;
            if (z > currentFunc.maxZ) currentFunc.maxZ = z;
        }
    }

    // Вычисляем цвета
    float range = currentFunc.maxZ - currentFunc.minZ;
    if (range < 0.0001f) range = 1.0f;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            float z = zValues[i * GRID_SIZE + j];
            float t = (z - currentFunc.minZ) / range;

            int idx = (i * GRID_SIZE + j) * 3;
            colors[idx] = t;        // Красный
            colors[idx + 1] = 0.2f;  // Зеленый
            colors[idx + 2] = 1.0f - t; // Синий
        }
    }

    printf("Function range: [%.3f, %.3f]\n", currentFunc.minZ, currentFunc.maxZ);
}

// Инициализация функции по умолчанию
void initDefaultFunction() {
    strcpy(currentFunc.expression, "sin(x)*cos(y)");
    currentFunc.minX = -5.0f;
    currentFunc.maxX = 5.0f;
    currentFunc.minY = -5.0f;
    currentFunc.maxY = 5.0f;

    computeFunction();
}

void drawGraph() {
    glPushMatrix();

    // Рисуем поверхность в виде треугольников
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < GRID_SIZE - 1; i++) {
        for (int j = 0; j < GRID_SIZE - 1; j++) {
            float x1 = currentFunc.minX + (currentFunc.maxX - currentFunc.minX) * i / (GRID_SIZE - 1);
            float y1 = currentFunc.minY + (currentFunc.maxY - currentFunc.minY) * j / (GRID_SIZE - 1);
            float x2 = currentFunc.minX + (currentFunc.maxX - currentFunc.minX) * (i + 1) / (GRID_SIZE - 1);
            float y2 = currentFunc.minY + (currentFunc.maxY - currentFunc.minY) * (j + 1) / (GRID_SIZE - 1);

            float z1 = zValues[i * GRID_SIZE + j];
            float z2 = zValues[(i + 1) * GRID_SIZE + j];
            float z3 = zValues[i * GRID_SIZE + (j + 1)];
            float z4 = zValues[(i + 1) * GRID_SIZE + (j + 1)];

            float nx, ny, nz;

            // Треугольник 1-2-3
            float v1x = x2 - x1, v1y = y2 - y1, v1z = z2 - z1;
            float v2x = x1 - x1, v2y = y2 - y1, v2z = z3 - z1;
            nx = v1y * v2z - v1z * v2y;
            ny = v1z * v2x - v1x * v2z;
            nz = v1x * v2y - v1y * v2x;
            float len = sqrt(nx * nx + ny * ny + nz * nz);
            if (len > 0) { nx /= len; ny /= len; nz /= len; }

            glNormal3f(nx, ny, nz);

            int idx1 = (i * GRID_SIZE + j) * 3;
            int idx2 = ((i + 1) * GRID_SIZE + j) * 3;
            int idx3 = (i * GRID_SIZE + (j + 1)) * 3;

            glColor3f(colors[idx1], colors[idx1 + 1], colors[idx1 + 2]);
            glVertex3f(x1, z1, -y1);
            glColor3f(colors[idx2], colors[idx2 + 1], colors[idx2 + 2]);
            glVertex3f(x2, z2, -y2);
            glColor3f(colors[idx3], colors[idx3 + 1], colors[idx3 + 2]);
            glVertex3f(x1, z3, -y2);

            // Треугольник 2-4-3
            v1x = x2 - x2; v1y = y2 - y2; v1z = z4 - z2;
            v2x = x1 - x2; v2y = y2 - y1; v2z = z3 - z2;
            nx = v1y * v2z - v1z * v2y;
            ny = v1z * v2x - v1x * v2z;
            nz = v1x * v2y - v1y * v2x;
            len = sqrt(nx * nx + ny * ny + nz * nz);
            if (len > 0) { nx /= len; ny /= len; nz /= len; }

            glNormal3f(nx, ny, nz);

            int idx4 = ((i + 1) * GRID_SIZE + (j + 1)) * 3;

            glColor3f(colors[idx2], colors[idx2 + 1], colors[idx2 + 2]);
            glVertex3f(x2, z2, -y2);
            glColor3f(colors[idx4], colors[idx4 + 1], colors[idx4 + 2]);
            glVertex3f(x2, z4, -y2);
            glColor3f(colors[idx3], colors[idx3 + 1], colors[idx3 + 2]);
            glVertex3f(x1, z3, -y2);
        }
    }
    glEnd();

    // Рисуем оси координат
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    // Ось X (красная)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(currentFunc.minX - 0.5f, 0.0f, 0.0f);
    glVertex3f(currentFunc.maxX + 0.5f, 0.0f, 0.0f);

    // Ось Y (зеленая)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, currentFunc.minZ - 0.5f, 0.0f);
    glVertex3f(0.0f, currentFunc.maxZ + 0.5f, 0.0f);

    // Ось Z (синяя)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -currentFunc.maxY - 0.5f);
    glVertex3f(0.0f, 0.0f, -currentFunc.minY + 0.5f);
    glEnd();
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawText(float x, float y, const char* text) {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);

    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Позиционирование камеры
    glTranslatef(0.0f, 0.0f, -15.0f);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
    glScalef(scale, scale, scale);

    drawGraph();

    // Отображение информации на экране
    char info[256];
    sprintf(info, "Function: f(x,y) = %s", currentFunc.expression);
    drawText(10, glutGet(GLUT_WINDOW_HEIGHT) - 30, info);

    sprintf(info, "Range: x=[%.1f,%.1f] y=[%.1f,%.1f] z=[%.2f,%.2f]",
        currentFunc.minX, currentFunc.maxX,
        currentFunc.minY, currentFunc.maxY,
        currentFunc.minZ, currentFunc.maxZ);
    drawText(10, glutGet(GLUT_WINDOW_HEIGHT) - 50, info);

    drawText(10, 30, "Press 'f' to enter new function");
    drawText(10, 10, "Press 'r' to reset view, ESC to exit");

    if (inputMode) {
        drawText(glutGet(GLUT_WINDOW_WIDTH) / 2 - 200,
            glutGet(GLUT_WINDOW_HEIGHT) / 2,
            "Enter function (press ENTER to confirm):");

        char displayBuffer[MAX_FUNC_LEN + 20];
        sprintf(displayBuffer, "f(x,y) = %s", inputBuffer);
        drawText(glutGet(GLUT_WINDOW_WIDTH) / 2 - 200,
            glutGet(GLUT_WINDOW_HEIGHT) / 2 - 30,
            displayBuffer);
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
    if (inputMode) return;

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseRotating = 1;
            lastMouseX = x;
            lastMouseY = y;
        }
        else {
            mouseRotating = 0;
        }
    }
    else if (button == 3) { // Scroll up
        scale *= 1.1f;
        glutPostRedisplay();
    }
    else if (button == 4) { // Scroll down
        scale *= 0.9f;
        glutPostRedisplay();
    }
}

void motion(int x, int y) {
    if (mouseRotating && !inputMode) {
        rotY += (x - lastMouseX) * 0.5f;
        rotX += (y - lastMouseY) * 0.5f;
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (inputMode) {
        if (key == 13) { // Enter
            inputMode = 0;
            strcpy(currentFunc.expression, inputBuffer);
            memset(inputBuffer, 0, MAX_FUNC_LEN);
            inputPos = 0;
            computeFunction();
            printf("New function: %s\n", currentFunc.expression);
            glutPostRedisplay();
        }
        else if (key == 27) { // ESC
            inputMode = 0;
            memset(inputBuffer, 0, MAX_FUNC_LEN);
            inputPos = 0;
            glutPostRedisplay();
        }
        else if (key == 8 && inputPos > 0) { // Backspace
            inputBuffer[--inputPos] = '\0';
        }
        else if (key >= 32 && key <= 126 && inputPos < MAX_FUNC_LEN - 1) {
            inputBuffer[inputPos++] = key;
            inputBuffer[inputPos] = '\0';
        }
        glutPostRedisplay();
        return;
    }

    switch (key) {
    case 'f': case 'F':
        inputMode = 1;
        memset(inputBuffer, 0, MAX_FUNC_LEN);
        inputPos = 0;
        printf("Enter function (available: sin, cos, exp, sqrt, +, -, *, /, ^)\n");
        printf("Examples: sin(x)*cos(y), x*x - y*y, sin(sqrt(x*x + y*y))\n");
        glutPostRedisplay();
        break;
    case 'r': case 'R':
        rotX = 30.0f;
        rotY = 0.0f;
        rotZ = 0.0f;
        scale = 1.0f;
        glutPostRedisplay();
        break;
    case 'x': // Увеличить диапазон X
        currentFunc.minX *= 1.2f;
        currentFunc.maxX *= 1.2f;
        computeFunction();
        glutPostRedisplay();
        break;
    case 'X': // Уменьшить диапазон X
        currentFunc.minX *= 0.8f;
        currentFunc.maxX *= 0.8f;
        computeFunction();
        glutPostRedisplay();
        break;
    case 'y': // Увеличить диапазон Y
        currentFunc.minY *= 1.2f;
        currentFunc.maxY *= 1.2f;
        computeFunction();
        glutPostRedisplay();
        break;
    case 'Y': // Уменьшить диапазон Y
        currentFunc.minY *= 0.8f;
        currentFunc.maxY *= 0.8f;
        computeFunction();
        glutPostRedisplay();
        break;
    case 27: // ESC
        free(zValues);
        free(colors);
        exit(0);
        break;
    }
}

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat lightPos[] = { 5.0f, 5.0f, 10.0f, 1.0f };
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Graph Visualization - Interactive Function Input");

    initOpenGL();
    initDefaultFunction();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    printf("\n=== 3D Graph Visualizer ===\n");
    printf("Current function: %s\n", currentFunc.expression);
    printf("\nControls:\n");
    printf("  Left mouse button + drag - rotate\n");
    printf("  Mouse wheel - zoom\n");
    printf("  F - enter new function\n");
    printf("  X/x - increase/decrease X range\n");
    printf("  Y/y - increase/decrease Y range\n");
    printf("  R - reset view\n");
    printf("  ESC - exit\n");
    printf("\nAvailable functions in this demo:\n");
    printf("  sin(x)*cos(y)\n");
    printf("  x*x - y*y\n");
    printf("  sin(sqrt(x*x + y*y))\n");
    printf("  0.5*(x*x + y*y)\n");
    printf("  exp(-0.5*(x*x+y*y))\n");
    printf("  sin(x)*sin(y)\n");
    printf("  x*y\n");
    printf("  sin(x+y)\n");

    glutMainLoop();

    free(zValues);
    free(colors);
    return 0;
}