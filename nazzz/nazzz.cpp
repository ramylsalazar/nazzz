#include <GL/glut.h>
#include <cmath>

// --- Constants ---
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 450;
const float PI = 3.1415926535f;

// --- Global Animation Variables ---
float windmillAngle = 0.0f;
float spinSpeed = 2.0f;
float cloudOffset = 0.0f;
float wavePhase = 0.0f;
float textOffsetX = 0.0f;

// --- Colors ---
void setColorSky() { glColor3f(0.94f, 0.99f, 1.0f); }
void setColorSun() { glColor3f(1.0f, 0.65f, 0.1f); }
void setColorCloud() { glColor3f(0.85f, 0.96f, 0.98f); }
void setColorMountain() { glColor3f(0.18f, 0.42f, 0.12f); }
void setColorWater() { glColor3f(0.0f, 0.55f, 0.85f); }
void setColorSand() { glColor3f(0.98f, 0.92f, 0.84f); }
void setColorPole() { glColor3f(0.85f, 0.85f, 0.85f); }
void setColorBlade() { glColor3f(0.6f, 0.6f, 0.6f); }
void setColorHub() { glColor3f(0.3f, 0.3f, 0.3f); }
void setColorText() { glColor3f(0.0f, 0.0f, 0.0f); }

// --- Helper Shapes ---

void drawOctagon(float x, float y, float r) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < 8; i++) {
        float theta = 2.0f * PI * float(i) / 8.0f;
        float dx = r * cosf(theta);
        float dy = r * sinf(theta);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void drawRect(float x, float y, float w, float h) {
    glRectf(x, y, x + w, y + h);
}

// --- Scene Objects ---

void drawSun() {
    setColorSun();
    drawOctagon(780, 440, 90);
}

void drawCloudCluster(float x, float y, float scale) {
    setColorCloud();
    float currentX = x + cloudOffset;
    while (currentX > 950) {
        currentX -= 1100;
    }
    drawOctagon(currentX, y, 35 * scale);
    drawOctagon(currentX + 45 * scale, y + 5 * scale, 40 * scale);
    drawOctagon(currentX - 40 * scale, y + 10 * scale, 35 * scale);
}

void drawAllClouds() {
    drawCloudCluster(80, 380, 1.0);
    drawCloudCluster(280, 400, 1.1);
    drawCloudCluster(500, 390, 1.0);
    drawCloudCluster(720, 380, 1.1);
}

void drawMountains() {
    setColorMountain();
    glBegin(GL_POLYGON);
    glVertex2f(0, 0);
    glVertex2f(0, 100);
    glVertex2f(150, 220);
    glVertex2f(220, 180);
    glVertex2f(400, 320);
    glVertex2f(550, 180);
    glVertex2f(700, 280);
    glVertex2f(780, 200);
    glVertex2f(800, 220);
    glVertex2f(800, 0);
    glEnd();
}

void drawSandAndWater() {
    setColorSand();
    glBegin(GL_POLYGON);
    glVertex2f(0, 0);
    glVertex2f(800, 0);
    glVertex2f(800, 60);
    glVertex2f(0, 60);
    glEnd();

    float waveShift = 10.0f * sinf(wavePhase);
    setColorWater();
    glBegin(GL_POLYGON);
    glVertex2f(520 + waveShift, 0);
    glVertex2f(550 + waveShift, 25);
    glVertex2f(620 + waveShift, 25);
    glVertex2f(650 + waveShift, 50);
    glVertex2f(800, 50);
    glVertex2f(800, 0);
    glEnd();
}

void drawWindmill(float x, float y, float scale, float rotationOffset) {
    setColorPole();
    drawRect(x - 4 * scale, y, 8 * scale, 160 * scale);

    float hubX = x;
    float hubY = y + 160 * scale;

    glPushMatrix();
    glTranslatef(hubX, hubY, 0);
    glRotatef(rotationOffset + windmillAngle, 0, 0, 1);

    setColorBlade();
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glRotatef(i * 120, 0, 0, 1);
        drawRect(-2 * scale, 0, 4 * scale, 95 * scale);
        glPopMatrix();
    }
    glPopMatrix();

    setColorHub();
    drawOctagon(hubX, hubY, 8 * scale);
}

void drawText() {
    // Apply the scroll offset using Matrix Translation
    glPushMatrix();
    glTranslatef(textOffsetX, 0.0f, 0.0f);

    setColorText();
    float yBase = 160;
    float height = 90;
    float thick = 25;

    // I
    drawRect(395, yBase, thick, height);

    // L
    float lx = 440;
    drawRect(lx, yBase, thick, height);
    drawRect(lx, yBase, 55, thick);

    // O
    float ox = 510;
    drawRect(ox, yBase, thick, height);
    drawRect(ox + 55 - thick, yBase, thick, height);
    drawRect(ox, yBase, 55, thick);
    drawRect(ox, yBase + height - thick, 55, thick);

    // C
    float cx = 580;
    drawRect(cx, yBase, thick, height);
    drawRect(cx, yBase, 55, thick);
    drawRect(cx, yBase + height - thick, 55, thick);

    // O
    float o2x = 650;
    drawRect(o2x, yBase, thick, height);
    drawRect(o2x + 55 - thick, yBase, thick, height);
    drawRect(o2x, yBase, 55, thick);
    drawRect(o2x, yBase + height - thick, 55, thick);

    // S
    float sx = 720;
    drawRect(sx, yBase + height - thick, 55, thick);
    drawRect(sx, yBase + height / 2, thick, height / 2);
    drawRect(sx, yBase + height / 2 - thick / 2, 55, thick);
    drawRect(sx + 55 - thick, yBase, thick, height / 2);
    drawRect(sx, yBase, 55, thick);

    glPopMatrix();
}

// --- Display & Animation Logic ---

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawSun();
    drawAllClouds();
    drawMountains();
    drawSandAndWater();

    drawWindmill(80, 50, 1.0, 25);
    drawWindmill(200, 65, 0.9, 60);
    drawWindmill(330, 50, 1.05, 15);

    drawText();

    glutSwapBuffers();
}

void timer(int value) {
    windmillAngle -= spinSpeed;
    if (windmillAngle <= -360.0f) windmillAngle += 360.0f;
    else if (windmillAngle >= 360.0f) windmillAngle -= 360.0f;

    cloudOffset += 0.5f;
    wavePhase += 0.05f;
    if (wavePhase > 2 * PI) wavePhase -= 2 * PI;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// --- Standard Mouse Function for Compatibility ---
// In many GLUT versions, Scroll Up is Button 3, Scroll Down is Button 4
void mouse(int button, int state, int x, int y) {
    if (state == GLUT_UP) return; // Only act when button/wheel is pressed/rolled

    // Button 3 (Wheel Up) or Button 4 (Wheel Down)
    if (button == 3) {
        textOffsetX += 20.0f; // Move Right
    }
    else if (button == 4) {
        textOffsetX -= 20.0f; // Move Left
    }
}

// --- Special Keys (Arrow Keys) Backup ---
void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_RIGHT:
        textOffsetX += 20.0f;
        break;
    case GLUT_KEY_LEFT:
        textOffsetX -= 20.0f;
        break;
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'd': case 'D': spinSpeed += 0.5f; break;
    case 'a': case 'A': spinSpeed -= 0.5f; break;
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Ilocos - Scroll or Arrows to Move Text");

    glClearColor(0.94f, 0.99f, 1.0f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys); // Register Arrow Keys
    glutMouseFunc(mouse);         // Register Mouse (Buttons 3/4 for Scroll)
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}