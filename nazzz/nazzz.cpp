#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

// --- Constants ---
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PI = 3.1415926535f;

// --- Global Animation & Interaction Variables ---
float windmillAngle = 0.0f;
float spinSpeed = 2.0f;
float cloudOffset = 0.0f;

// Camera / Rotation Variables
float rotX = 0.0f;
float rotY = 0.0f;
float lastX = 0.0f;
float lastY = 0.0f;
bool isDragging = false;
float zoom = -90.0f;

// Day/Night Cycle Variables
float timeOfDay = 45.0f; // 0 to 360 degrees. 90=Sunset, 270=Sunrise
// Colors
float skyColor[3] = { 0.85f, 0.95f, 0.98f }; // Current Sky Color

// --- Helper Functions ---

void drawBox(float width, float height, float depth) {
    glPushMatrix();
    glScalef(width, height, depth);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawCylinder(float baseRadius, float topRadius, float height) {
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, baseRadius, topRadius, height, 20, 5);
    gluDeleteQuadric(quad);
}

// Linear Interpolation for colors
void mixColor(float* output, float* c1, float* c2, float t) {
    output[0] = c1[0] * (1.0f - t) + c2[0] * t;
    output[1] = c1[1] * (1.0f - t) + c2[1] * t;
    output[2] = c1[2] * (1.0f - t) + c2[2] * t;
}

// --- Scene Objects ---

void drawCelestialBodies() {
    glPushMatrix();
    // Rotate the whole celestial system based on timeOfDay
    glRotatef(timeOfDay, 0.0f, 0.0f, 1.0f);

    // SUN
    glPushMatrix();
    glTranslatef(0.0f, 60.0f, -40.0f); // Orbit radius 60
    // Sun Color Logic (Yellow at noon, Redder at horizon)
    if (timeOfDay > 60 && timeOfDay < 120) glColor3f(1.0f, 0.4f, 0.0f); // Sunset Orange
    else if (timeOfDay > 240 && timeOfDay < 300) glColor3f(1.0f, 0.5f, 0.2f); // Sunrise Orange
    else glColor3f(1.0f, 0.9f, 0.0f); // Noon Yellow

    glutSolidSphere(8.0f, 30, 30);
    glPopMatrix();

    // MOON (Opposite side)
    glPushMatrix();
    glTranslatef(0.0f, -60.0f, -40.0f);
    glColor3f(0.9f, 0.9f, 0.9f); // White/Grey Moon
    glutSolidSphere(6.0f, 20, 20);
    glPopMatrix();

    glPopMatrix();
}

void drawCloud(float x, float y, float z, float scale) {
    // Clouds darken at night
    float cloudIntensity = 1.0f;
    if (timeOfDay > 100 && timeOfDay < 260) cloudIntensity = 0.3f; // Darker at night

    glColor3f(0.88f * cloudIntensity, 0.95f * cloudIntensity, 1.0f * cloudIntensity);

    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    glutSolidSphere(3.0f, 10, 10);
    glTranslatef(3.5f, 0.0f, 0.0f);
    glutSolidSphere(2.5f, 10, 10);
    glTranslatef(-1.5f, 2.0f, 0.5f);
    glutSolidSphere(2.5f, 10, 10);

    glPopMatrix();
}

void drawMountains() {
    // Dim mountains at night
    float dim = 1.0f;
    if (timeOfDay > 100 && timeOfDay < 260) dim = 0.3f;

    glColor3f(0.13f * dim, 0.35f * dim, 0.05f * dim);

    glPushMatrix();
    glTranslatef(0.0f, -5.0f, -50.0f);

    // Main central peak
    glPushMatrix();
    glTranslatef(10.0f, 0.0f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(30.0f, 45.0f, 10, 10);
    glPopMatrix();

    // Right peak
    glPushMatrix();
    glTranslatef(45.0f, -5.0f, 5.0f);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(25.0f, 35.0f, 10, 10);
    glPopMatrix();

    // Left peak
    glPushMatrix();
    glTranslatef(-30.0f, -5.0f, 5.0f);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(28.0f, 40.0f, 10, 10);
    glPopMatrix();

    // Far Left filler
    glPushMatrix();
    glTranslatef(-60.0f, -8.0f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(20.0f, 30.0f, 10, 10);
    glPopMatrix();

    glPopMatrix();
}

void drawGroundAndWater() {
    float dim = 1.0f;
    if (timeOfDay > 100 && timeOfDay < 260) dim = 0.4f;

    // Sand (Beige)
    glColor3f(0.96f * dim, 0.91f * dim, 0.81f * dim);
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, -5.0f, 100.0f);
    glVertex3f(10.0f, -5.0f, 100.0f);
    glVertex3f(-20.0f, -5.0f, -100.0f);
    glVertex3f(-100.0f, -5.0f, -100.0f);
    glEnd();

    // Water (Blue)
    glColor3f(0.0f * dim, 0.47f * dim, 0.75f * dim);
    glBegin(GL_QUADS);
    glVertex3f(10.0f, -5.1f, 100.0f);
    glVertex3f(100.0f, -5.1f, 100.0f);
    glVertex3f(100.0f, -5.1f, -100.0f);
    glVertex3f(-20.0f, -5.1f, -100.0f);
    glEnd();
}

void drawWindmill(float x, float z, float scale, float rotationOffset) {
    float dim = 1.0f;
    if (timeOfDay > 100 && timeOfDay < 260) dim = 0.5f;

    glPushMatrix();
    glTranslatef(x, -5.0f, z);
    glScalef(scale, scale, scale);

    // Pole
    glColor3f(0.8f * dim, 0.8f * dim, 0.8f * dim);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    drawCylinder(0.6f, 0.4f, 18.0f);
    glPopMatrix();

    // Hub Group
    glPushMatrix();
    glTranslatef(0.0f, 18.0f, 0.5f);
    glRotatef(rotationOffset + windmillAngle, 0, 0, 1);

    // Center Hub
    glColor3f(0.3f * dim, 0.3f * dim, 0.3f * dim);
    glutSolidSphere(1.0f, 10, 10);

    // Blades
    glColor3f(0.7f * dim, 0.7f * dim, 0.7f * dim);
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glRotatef(i * 120, 0, 0, 1);
        glTranslatef(0.0f, 6.0f, 0.0f);
        drawBox(0.6f, 12.0f, 0.2f);
        glPopMatrix();
    }
    glPopMatrix();

    glPopMatrix();
}

void drawBlock(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawLetter(const std::vector<std::pair<int, int> >& blocks, float xOffset) {
    glPushMatrix();
    glTranslatef(xOffset, 0.0f, 0.0f);
    for (size_t i = 0; i < blocks.size(); ++i) {
        drawBlock((float)blocks[i].first, (float)blocks[i].second);
    }
    glPopMatrix();
}

void drawText3D() {
    // Text remains black or very dark grey
    glColor3f(0.05f, 0.05f, 0.05f);

    glPushMatrix();
    glTranslatef(10.0f, 5.0f, 10.0f);
    glScalef(2.5f, 2.5f, 2.5f);

    std::vector<std::pair<int, int> > I, L, O, C, S;

    for (int i = 0; i < 5; i++) I.push_back(std::make_pair(0, i));

    L.push_back(std::make_pair(0, 0)); L.push_back(std::make_pair(1, 0)); L.push_back(std::make_pair(2, 0));
    for (int i = 1; i < 5; i++) L.push_back(std::make_pair(0, i));

    O.push_back(std::make_pair(0, 0)); O.push_back(std::make_pair(1, 0)); O.push_back(std::make_pair(2, 0));
    O.push_back(std::make_pair(0, 4)); O.push_back(std::make_pair(1, 4)); O.push_back(std::make_pair(2, 4));
    for (int i = 1; i < 4; i++) { O.push_back(std::make_pair(0, i)); O.push_back(std::make_pair(2, i)); }

    C.push_back(std::make_pair(0, 0)); C.push_back(std::make_pair(1, 0)); C.push_back(std::make_pair(2, 0));
    C.push_back(std::make_pair(0, 4)); C.push_back(std::make_pair(1, 4)); C.push_back(std::make_pair(2, 4));
    for (int i = 1; i < 4; i++) C.push_back(std::make_pair(0, i));

    S.push_back(std::make_pair(0, 0)); S.push_back(std::make_pair(1, 0)); S.push_back(std::make_pair(2, 0));
    S.push_back(std::make_pair(2, 1)); S.push_back(std::make_pair(2, 2)); S.push_back(std::make_pair(1, 2));
    S.push_back(std::make_pair(0, 2)); S.push_back(std::make_pair(0, 3)); S.push_back(std::make_pair(0, 4));
    S.push_back(std::make_pair(1, 4)); S.push_back(std::make_pair(2, 4));

    float xPos = 0.0f;
    drawLetter(I, xPos); xPos += 2.0f;
    drawLetter(L, xPos); xPos += 4.0f;
    drawLetter(O, xPos); xPos += 4.0f;
    drawLetter(C, xPos); xPos += 4.0f;
    drawLetter(O, xPos); xPos += 4.0f;
    drawLetter(S, xPos);

    glPopMatrix();
}

// --- Interaction Functions ---

void updateEnvironmentColor() {
    float dayColor[] = { 0.85f, 0.95f, 0.98f };
    float sunsetColor[] = { 0.98f, 0.6f, 0.3f };
    float nightColor[] = { 0.05f, 0.05f, 0.15f };
    float sunriseColor[] = { 0.98f, 0.8f, 0.4f };

    // Calculate sky color based on timeOfDay angle (0-360)
    // 0 = Noon, 90 = Sunset, 180 = Midnight, 270 = Sunrise

    if (timeOfDay >= 0 && timeOfDay < 60) {
        // Pure Day
        std::copy(dayColor, dayColor + 3, skyColor);
    }
    else if (timeOfDay >= 60 && timeOfDay < 100) {
        // Day -> Sunset
        float t = (timeOfDay - 60) / 40.0f;
        mixColor(skyColor, dayColor, sunsetColor, t);
    }
    else if (timeOfDay >= 100 && timeOfDay < 130) {
        // Sunset -> Night
        float t = (timeOfDay - 100) / 30.0f;
        mixColor(skyColor, sunsetColor, nightColor, t);
    }
    else if (timeOfDay >= 130 && timeOfDay < 230) {
        // Pure Night
        std::copy(nightColor, nightColor + 3, skyColor);
    }
    else if (timeOfDay >= 230 && timeOfDay < 270) {
        // Night -> Sunrise
        float t = (timeOfDay - 230) / 40.0f;
        mixColor(skyColor, nightColor, sunriseColor, t);
    }
    else if (timeOfDay >= 270 && timeOfDay < 320) {
        // Sunrise -> Day
        float t = (timeOfDay - 270) / 50.0f;
        mixColor(skyColor, sunriseColor, dayColor, t);
    }
    else {
        // Pure Day
        std::copy(dayColor, dayColor + 3, skyColor);
    }

    glClearColor(skyColor[0], skyColor[1], skyColor[2], 1.0f);
}

void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = true;
            lastX = x;
            lastY = y;
        }
        else {
            isDragging = false;
        }
    }

    // SCROLL for Day/Night Cycle
    // Button 3 = Scroll Up (Forward in time)
    // Button 4 = Scroll Down (Backward in time)
    if (state == GLUT_UP) return; // Only trigger on press

    if (button == 3) {
        timeOfDay += 5.0f;
        if (timeOfDay >= 360.0f) timeOfDay -= 360.0f;
    }
    if (button == 4) {
        timeOfDay -= 5.0f;
        if (timeOfDay < 0.0f) timeOfDay += 360.0f;
    }

    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    if (isDragging) {
        float dx = x - lastX;
        float dy = y - lastY;

        rotY += dx * 0.5f;
        rotX += dy * 0.5f;

        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

// --- Display & Animation ---

void display() {
    // Update Sky Color
    updateEnvironmentColor();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f, -5.0f, zoom);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // Setup Lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // Light follows the sun logic (sort of)
    float lightIntensity = 1.0f;
    if (timeOfDay > 90 && timeOfDay < 270) lightIntensity = 0.2f; // Dim light at night

    GLfloat lightPos[] = { 0.0f, 50.0f, 0.0f, 0.0f }; // Overhead light basically
    GLfloat lightColor[] = { lightIntensity, lightIntensity, lightIntensity, 1.0f };

    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor);

    drawCelestialBodies(); // Draws Rotating Sun and Moon
    drawMountains();
    drawGroundAndWater();

    // Clouds
    float c1 = -40 + cloudOffset; if (c1 > 60) c1 -= 120;
    float c2 = 10 + cloudOffset;  if (c2 > 60) c2 -= 120;
    float c3 = -10 + cloudOffset; if (c3 > 60) c3 -= 120;

    drawCloud(c1, 35, -20, 1.2f);
    drawCloud(c2, 38, -25, 1.0f);
    drawCloud(c3, 32, -15, 1.5f);

    drawWindmill(-40, 10, 1.2f, 0);
    drawWindmill(-25, -10, 1.0f, 45);
    drawWindmill(-10, 20, 0.8f, 90);

    drawText3D();

    glutSwapBuffers();
}

void timer(int value) {
    windmillAngle -= spinSpeed;
    if (windmillAngle <= -360.0f) windmillAngle += 360.0f;

    cloudOffset += 0.02f;
    if (cloudOffset > 120.0f) cloudOffset = -120.0f;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float ratio = 1.0f * w / h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, ratio, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

// Keyboard controls for Speed (A/D) and Zoom (W/S)
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': case 'W': // Zoom In
        zoom += 2.0f;
        break;
    case 's': case 'S': // Zoom Out
        zoom -= 2.0f;
        break;
    case 'd': case 'D': // Increase Windmill Speed
        spinSpeed += 0.5f;
        break;
    case 'a': case 'A': // Decrease Windmill Speed
        spinSpeed -= 0.5f;
        break;
        // Keep Z/X as backups if desired, or remove them
    case 'z': case 'Z': zoom += 2.0f; break;
    case 'x': case 'X': zoom -= 2.0f; break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Ilocos 3D Day/Night Cycle");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}