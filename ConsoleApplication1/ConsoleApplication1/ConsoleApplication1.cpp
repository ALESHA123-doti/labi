// ConsoleApplication1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <windows.h>
#include "include/glut.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glut.h>
#include <iostream>
#include <cmath>
#include "stb_image.h"

using namespace std;

// Параметры отображения
int displayMode = 0;
float dropRadius = 2.0f;
float deformationFactor = 1.0f;

// Параметры перемещения
float posX = 0.0f, posY = 0.0f, posZ = 0.0f;

// Параметры освещения
float lightPosX = 5.0f, lightPosY = 5.0f, lightPosZ = 5.0f;

// Текстура
GLuint textureID;

// Функция для вычисления координаты Z капли
float calculateZ(float B, float L) {
    float B_rad = B * 3.14159265f / 180.0f;
    float L_rad = L * 3.14159265f / 180.0f;

    float z = dropRadius * sin(B_rad);

    // Деформация капли (по условию задачи)
    if (B > 0) {
        z += dropRadius * pow((B / 90.0f), 4) * deformationFactor;
    }

    return z;
}

// Функция для вычисления нормали
void calculateNormal(float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float& nx, float& ny, float& nz) {
    float ux = x2 - x1, uy = y2 - y1, uz = z2 - z1;
    float vx = x3 - x1, vy = y3 - y1, vz = z3 - z1;

    nx = uy * vz - uz * vy;
    ny = uz * vx - ux * vz;
    nz = ux * vy - uy * vx;

    // Нормализация
    float len = sqrt(nx * nx + ny * ny + nz * nz);
    if (len > 0) {
        nx /= len;
        ny /= len;
        nz /= len;
    }
}

// Рисование каркаса капли
void drawWireframe(float step) {
    glColor3f(1.0f, 0.0f, 0.0f);

    // Рисуем меридианы
    for (float L = 0; L <= 360; L += step) {
        glBegin(GL_LINE_STRIP);
        for (float B = -90; B <= 90; B += step / 2) {
            float B_rad = B * 3.14159265f / 180.0f;
            float L_rad = L * 3.14159265f / 180.0f;

            float x = dropRadius * cos(B_rad) * cos(L_rad) + posX;
            float y = dropRadius * cos(B_rad) * sin(L_rad) + posY;
            float z = calculateZ(B, L) + posZ;

            glVertex3f(x, y, z);
        }
        glEnd();
    }

    // Рисуем параллели
    for (float B = -90; B <= 90; B += step) {
        glBegin(GL_LINE_STRIP);
        for (float L = 0; L <= 360; L += step / 2) {
            float B_rad = B * 3.14159265f / 180.0f;
            float L_rad = L * 3.14159265f / 180.0f;

            float x = dropRadius * cos(B_rad) * cos(L_rad) + posX;
            float y = dropRadius * cos(B_rad) * sin(L_rad) + posY;
            float z = calculateZ(B, L) + posZ;

            glVertex3f(x, y, z);
        }
        glEnd();
    }
}

// Рисование капли как многогранника
void drawSolidDrop(float step, bool withTexture = false) {
    float nx, ny, nz;

    for (float B = -90; B < 90; B += step) {
        for (float L = 0; L <= 360; L += step) {
            // Координаты вершин текущего квадрата
            float B1 = B, L1 = L;
            float B2 = B + step, L2 = L;
            float B3 = B + step, L3 = L + step;
            float B4 = B, L4 = L + step;

            // Вычисляем координаты
            float x1 = dropRadius * cos(B1 * 3.14159265f / 180) * cos(L1 * 3.14159265f / 180) + posX;
            float y1 = dropRadius * cos(B1 * 3.14159265f / 180) * sin(L1 * 3.14159265f / 180) + posY;
            float z1 = calculateZ(B1, L1) + posZ;

            float x2 = dropRadius * cos(B2 * 3.14159265f / 180) * cos(L2 * 3.14159265f / 180) + posX;
            float y2 = dropRadius * cos(B2 * 3.14159265f / 180) * sin(L2 * 3.14159265f / 180) + posY;
            float z2 = calculateZ(B2, L2) + posZ;

            float x3 = dropRadius * cos(B3 * 3.14159265f / 180) * cos(L3 * 3.14159265f / 180) + posX;
            float y3 = dropRadius * cos(B3 * 3.14159265f / 180) * sin(L3 * 3.14159265f / 180) + posY;
            float z3 = calculateZ(B3, L3) + posZ;

            float x4 = dropRadius * cos(B4 * 3.14159265f / 180) * cos(L4 * 3.14159265f / 180) + posX;
            float y4 = dropRadius * cos(B4 * 3.14159265f / 180) * sin(L4 * 3.14159265f / 180) + posY;
            float z4 = calculateZ(B4, L4) + posZ;

            // Вычисляем нормаль
            calculateNormal(x1, y1, z1, x2, y2, z2, x3, y3, z3, nx, ny, nz);

            // Рисуем полигон
            if (withTexture) {
                glBegin(GL_QUADS);
                glNormal3f(nx, ny, nz);
                glTexCoord2f(L / 360.0f, (B + 90) / 180.0f); glVertex3f(x1, y1, z1);
                glTexCoord2f(L / 360.0f, (B + step + 90) / 180.0f); glVertex3f(x2, y2, z2);
                glTexCoord2f((L + step) / 360.0f, (B + step + 90) / 180.0f); glVertex3f(x3, y3, z3);
                glTexCoord2f((L + step) / 360.0f, (B + 90) / 180.0f); glVertex3f(x4, y4, z4);
                glEnd();
            }
            else {
                glBegin(GL_QUADS);
                glNormal3f(nx, ny, nz);
                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
                glVertex3f(x3, y3, z3);
                glVertex3f(x4, y4, z4);
                glEnd();
            }
        }
    }
}

// Функция отображения
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // Настройка освещения
    GLfloat light_position[] = { lightPosX, lightPosY, lightPosZ, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    GLfloat material_diffuse[] = { 0.8f, 0.8f, 1.0f, 1.0f };
    GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat material_shininess[] = { 50.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    switch (displayMode) {
    case 0: // Каркасное изображение
        glDisable(GL_LIGHTING);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, 1, 0.1, 100);
        gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
        drawWireframe(15);
        break;

    case 1: // Многогранник с удалением невидимых линий (аксонометрическая проекция)
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-5, 5, -5, 5, -10, 10);
        gluLookAt(3, 3, 3, 0, 0, 0, 0, 1, 0);
        drawSolidDrop(10);
        break;

    case 2: // Многогранник с удалением невидимых линий (перспективная проекция)
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, 1, 0.1, 100);
        gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
        drawSolidDrop(10);
        break;

    case 3: // Закрашенный многогранник с освещением
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, 1, 0.1, 100);
        gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
        drawSolidDrop(10);
        break;

    case 4: // Текстурированный многогранник
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, 1, 0.1, 100);
        gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
        drawSolidDrop(10, true);
        glDisable(GL_TEXTURE_2D);
        break;
    }

    glutSwapBuffers();
}

// Функция загрузки текстуры
GLuint loadTexture(const char* filename) {
    GLuint texture;
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);

    if (!data) {
        cerr << "Failed to load texture: " << filename << endl;
        return 0;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return texture;
}

// Обработка клавиатуры
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '0': displayMode = 0; break;
    case '1': displayMode = 1; break;
    case '2': displayMode = 2; break;
    case '3': displayMode = 3; break;
    case '4': displayMode = 4; break;

        // Управление положением капли
    case 'w': posZ += 0.1f; break;
    case 's': posZ -= 0.1f; break;
    case 'a': posX -= 0.1f; break;
    case 'd': posX += 0.1f; break;
    case 'q': posY += 0.1f; break;
    case 'e': posY -= 0.1f; break;

        // Управление освещением
    case 'i': lightPosY += 0.5f; break;
    case 'k': lightPosY -= 0.5f; break;
    case 'j': lightPosX -= 0.5f; break;
    case 'l': lightPosX += 0.5f; break;
    case 'u': lightPosZ += 0.5f; break;
    case 'o': lightPosZ -= 0.5f; break;

        // Управление деформацией
    case '+': deformationFactor += 0.1f; break;
    case '-': deformationFactor -= 0.1f; break;

    case 27: exit(0); break; // ESC - выход
    }

    glutPostRedisplay();
}

// Инициализация OpenGL
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    // Загрузка текстуры
    textureID = loadTexture("texture.bmp"); // Замените на путь к вашей текстуре
    if (!textureID) {
        cerr << "Failed to load texture. Using default color." << endl;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Water Drop");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
