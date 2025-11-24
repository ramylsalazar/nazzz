#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>

using namespace std;

float faceX = 0.0f;
float faceY = 0.0f;
float moveSpeed = 0.05f;

int leftEyeColorState = 0;
int rightEyeColorState = 0;

vector<float> faceVertices;
vector<float> leftEyeVertices;
vector<float> rightEyeVertices;
vector<float> mouthVertices;

void initVertexArrays() {
    float r = 0.4f;
    float step = 0.01f;
    faceVertices.push_back(0.0f);
    faceVertices.push_back(0.0f);

    for (float x = r; x >= -r; x -= step) {
        faceVertices.push_back(x);
        faceVertices.push_back(sqrt(r * r - x * x));
    }
    for (float x = -r; x <= r; x += step) {
        faceVertices.push_back(x);
        faceVertices.push_back(-sqrt(r * r - x * x));
    }

    float es = 0.08f;
    float ex = -0.15f;
    float ey = 0.1f;
    leftEyeVertices = { ex - es, ey - es, ex + es, ey - es, ex + es, ey + es, ex - es, ey + es };

    ex = 0.15f;
    rightEyeVertices = { ex - es, ey - es, ex + es, ey - es, ex + es, ey + es, ex - es, ey + es };

    float mr = 0.25f;
    for (float x = -0.15f; x <= 0.15f; x += 0.01f) {
        mouthVertices.push_back(x);
        mouthVertices.push_back(sqrt(mr * mr - x * x) - 0.35f);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(faceX, faceY, 0.0f);
    glEnableClientState(GL_VERTEX_ARRAY);

    glColor3f(1.0f, 0.9f, 0.5f);
    glVertexPointer(2, GL_FLOAT, 0, &faceVertices[0]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, faceVertices.size() / 2);

    if (leftEyeColorState == 0) glColor3f(0.3f, 0.0f, 0.3f);
    else if (leftEyeColorState == 1) glColor3f(1.0f, 0.0f, 0.0f);
    else glColor3f(0.0f, 0.0f, 0.0f);
    glVertexPointer(2, GL_FLOAT, 0, &leftEyeVertices[0]);
    glDrawArrays(GL_QUADS, 0, 4);

    if (rightEyeColorState == 0) glColor3f(0.0f, 0.6f, 0.0f);
    else if (rightEyeColorState == 1) glColor3f(0.0f, 0.0f, 1.0f);
    else glColor3f(1.0f, 0.5f, 0.0f);
    glVertexPointer(2, GL_FLOAT, 0, &rightEyeVertices[0]);
    glDrawArrays(GL_QUADS, 0, 4);

    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(3.0f);
    glVertexPointer(2, GL_FLOAT, 0, &mouthVertices[0]);
    glDrawArrays(GL_LINE_STRIP, 0, mouthVertices.size() / 2);

    glDisableClientState(GL_VERTEX_ARRAY);
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27: exit(0); break;
    case 'w': case 'W': faceY += moveSpeed; break;
    case 's': case 'S': faceY -= moveSpeed; break;
    case 'a': case 'A': faceX -= moveSpeed; break;
    case 'd': case 'D': faceX += moveSpeed; break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            leftEyeColorState = (leftEyeColorState + 1) % 3;
            cout << "Left eye color changed." << endl;
        }
        else if (button == GLUT_RIGHT_BUTTON) {
            rightEyeColorState = (rightEyeColorState + 1) % 3;
            cout << "Right eye color changed." << endl;
        }
        glutPostRedisplay();
    }
}

void init() {
    glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
    initVertexArrays();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Face");

    cout << "OPENGL FACE CONTROLS" << endl;
    cout << "====================" << endl;
    cout << "[ESC] - Exit Program" << endl;
    cout << "[W] - Move Face Up" << endl;
    cout << "[S] - Move Face Down" << endl;
    cout << "[A] - Move Face Left" << endl;
    cout << "[D] - Move Face Right" << endl;
    cout << "(Left Click) - Change Left Eye Color" << endl;
    cout << "(Right Click) - Change Right Eye Color" << endl;

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}