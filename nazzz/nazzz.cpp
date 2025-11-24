#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

// --- Constants ---
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PI = 3.1415926535f;

// --- Global Animation & Interaction Variables ---
float windmillAngle = 0.0f;
float spinSpeed = 2.0f;
float cloudOffset = 0.0f;
float wavePhase = 0.0f; // Animation variable for waves

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
    // Moved sun further back (z = -200) so it sets BEHIND mountains
    // Increased orbit radius to 90 to be visible over mountains
    glTranslatef(0.0f, 90.0f, -200.0f);
    // Sun Color Logic (Yellow at noon, Redder at horizon)
    if (timeOfDay > 60 && timeOfDay < 120) glColor3f(1.0f, 0.4f, 0.0f); // Sunset Orange
    else if (timeOfDay > 240 && timeOfDay < 300) glColor3f(1.0f, 0.5f, 0.2f); // Sunrise Orange
    else glColor3f(1.0f, 0.9f, 0.0f); // Noon Yellow

    glutSolidSphere(12.0f, 30, 30); // Slightly larger sun
    glPopMatrix();

    // MOON (Opposite side)
    glPushMatrix();
    glTranslatef(0.0f, -90.0f, -200.0f); // Match Sun's depth
    glColor3f(0.9f, 0.9f, 0.9f); // White/Grey Moon
    glutSolidSphere(8.0f, 20, 20);
    glPopMatrix();

    glPopMatrix();
}

void drawCloud(float x, float y, float z, float scale) {
    // Dynamic Cloud Colors based on Cycle
    float cloudColor[3];
    float dayCloud[] = { 0.95f, 0.95f, 1.0f };      // White
    float sunsetCloud[] = { 1.0f, 0.7f, 0.6f };     // Pinkish Orange
    float nightCloud[] = { 0.2f, 0.2f, 0.25f };     // Dark Grey
    float sunriseCloud[] = { 1.0f, 0.8f, 0.6f };    // Yellowish

    if (timeOfDay >= 0 && timeOfDay < 60) std::copy(dayCloud, dayCloud + 3, cloudColor);
    else if (timeOfDay >= 60 && timeOfDay < 100) { // Sunset transition
        float t = (timeOfDay - 60) / 40.0f;
        mixColor(cloudColor, dayCloud, sunsetCloud, t);
    }
    else if (timeOfDay >= 100 && timeOfDay < 130) { // To Night
        float t = (timeOfDay - 100) / 30.0f;
        mixColor(cloudColor, sunsetCloud, nightCloud, t);
    }
    else if (timeOfDay >= 130 && timeOfDay < 230) std::copy(nightCloud, nightCloud + 3, cloudColor);
    else if (timeOfDay >= 230 && timeOfDay < 270) { // To Sunrise
        float t = (timeOfDay - 230) / 40.0f;
        mixColor(cloudColor, nightCloud, sunriseCloud, t);
    }
    else if (timeOfDay >= 270 && timeOfDay < 320) { // To Day
        float t = (timeOfDay - 270) / 50.0f;
        mixColor(cloudColor, sunriseCloud, dayCloud, t);
    }
    else std::copy(dayCloud, dayCloud + 3, cloudColor);

    glColor3fv(cloudColor);

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

void drawTree(float x, float z) {
    float dim = 1.0f;
    if (timeOfDay > 100 && timeOfDay < 260) dim = 0.4f;

    glPushMatrix();
    glTranslatef(x, -5.0f, z);
    // Make trees smaller by scaling down
    glScalef(0.6f, 0.6f, 0.6f);

    // Trunk
    glColor3f(0.4f * dim, 0.3f * dim, 0.1f * dim); // Brown
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    drawCylinder(1.0f, 1.0f, 5.0f);
    glPopMatrix();

    // Foliage
    glColor3f(0.05f * dim, 0.4f * dim, 0.05f * dim); // Dark Green
    glPushMatrix();
    glTranslatef(0.0f, 4.0f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(4.0f, 10.0f, 10, 10);
    glPopMatrix();

    glPopMatrix();
}

void drawVegetation() {
    // Add MORE smaller Trees scattered around on the forest floor
    // Original trees
    drawTree(-70.0f, -40.0f);
    drawTree(-50.0f, -80.0f);
    drawTree(-80.0f, -120.0f);
    drawTree(60.0f, -60.0f);
    drawTree(80.0f, -100.0f);
    drawTree(40.0f, -130.0f);
    drawTree(20.0f, -110.0f);
    drawTree(-20.0f, -140.0f);

    // New trees
    drawTree(-30.0f, -20.0f);
    drawTree(-10.0f, -45.0f);
    drawTree(10.0f, -30.0f);
    drawTree(30.0f, -50.0f);
    drawTree(50.0f, -25.0f);
    drawTree(-60.0f, -60.0f);
    drawTree(-40.0f, -100.0f);
    drawTree(0.0f, -80.0f);
    drawTree(70.0f, -80.0f);
    drawTree(-90.0f, -30.0f);
    drawTree(90.0f, -40.0f);
    drawTree(-15.0f, -70.0f);
    drawTree(15.0f, -90.0f);
}

void drawMountains() {
    // Dim mountains at night
    float dim = 1.0f;
    if (timeOfDay > 100 && timeOfDay < 260) dim = 0.3f;

    glColor3f(0.13f * dim, 0.35f * dim, 0.05f * dim);

    glPushMatrix();
    // Move mountains further back to make room for forest
    glTranslatef(0.0f, -5.0f, -150.0f);

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

    // 1. FOREST (Green part) - At the back, before mountains
    glColor3f(0.1f * dim, 0.45f * dim, 0.1f * dim); // Forest Green
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, -5.0f, -20.0f);   // Meets Sand
    glVertex3f(100.0f, -5.0f, -20.0f);
    glVertex3f(100.0f, -5.0f, -150.0f);   // Meets Mountains
    glVertex3f(-100.0f, -5.0f, -150.0f);
    glEnd();

    // 2. SAND (Golden part) - In the middle
    glColor3f(0.85f * dim, 0.75f * dim, 0.55f * dim); // Golden Sand
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, -5.0f, 30.0f);    // Meets Water
    glVertex3f(100.0f, -5.0f, 30.0f);
    glVertex3f(100.0f, -5.0f, -20.0f);    // Meets Forest
    glVertex3f(-100.0f, -5.0f, -20.0f);
    glEnd();

    // 3. WATER (Blue with WAVES) - At the front
    // We render this as strips to animate vertices for the wave effect
    glColor3f(0.0f * dim, 0.47f * dim, 0.75f * dim);

    float waterLevel = -5.5f;
    // Loop through Z axis (from close to camera, to near sand)
    // Adjusted start Z to 30.0f to meet the new sand edge
    for (float z = 30.0f; z < 150.0f; z += 5.0f) {
        glBegin(GL_QUAD_STRIP);
        // Loop through X axis
        for (float x = -100.0f; x <= 100.0f; x += 10.0f) {
            // Calculate Wave Height using Sine function dependent on Position + Time
            float y1 = waterLevel + sin(x * 0.05f + z * 0.05f + wavePhase) * 0.8f;
            float y2 = waterLevel + sin(x * 0.05f + (z + 5.0f) * 0.05f + wavePhase) * 0.8f;

            glVertex3f(x, y1, z);
            glVertex3f(x, y2, z + 5.0f);
        }
        glEnd();
    }
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
    drawVegetation();      // Draws new grass

    // Clouds - ADDED MORE CLOUDS
    float c1 = -40 + cloudOffset; if (c1 > 60) c1 -= 120;
    float c2 = 10 + cloudOffset;  if (c2 > 60) c2 -= 120;
    float c3 = -10 + cloudOffset; if (c3 > 60) c3 -= 120;
    float c4 = -65 + cloudOffset; if (c4 > 60) c4 -= 120;
    float c5 = 50 + cloudOffset;  if (c5 > 60) c5 -= 120;
    float c6 = -25 + cloudOffset; if (c6 > 60) c6 -= 120;

    drawCloud(c1, 35, -20, 1.2f);
    drawCloud(c2, 38, -25, 1.0f);
    drawCloud(c3, 32, -15, 1.5f);
    drawCloud(c4, 36, -30, 1.3f);
    drawCloud(c5, 40, -10, 0.9f); // New
    drawCloud(c6, 30, -5, 1.1f);  // New

    // Windmills (Original + New Ones)
    // Left Cluster
    drawWindmill(-55, -10, 0.9f, 20); // New Far Left
    drawWindmill(-40, 0, 1.2f, 0);    // Original
    drawWindmill(-25, -15, 1.0f, 45); // Original
    drawWindmill(-10, 5, 0.8f, 90);   // Original

    // Middle/Back Cluster
    drawWindmill(-30, 10, 0.7f, 130); // New Back Middle
    drawWindmill(-15, -20, 1.1f, 60); // New Front Middle

    drawText3D();

    glutSwapBuffers();
}

void timer(int value) {
    windmillAngle -= spinSpeed;
    if (windmillAngle <= -360.0f) windmillAngle += 360.0f;

    cloudOffset += 0.02f;
    if (cloudOffset > 120.0f) cloudOffset = -120.0f;

    wavePhase += 0.1f; // Updates wave position constantly

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

    // --- PRINT INSTRUCTIONS ---
    std::cout << "========================================" << std::endl;
    std::cout << "   Ilocos 3D Scene - Controls Guide     " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << " [W] / [S]        : Zoom In / Out" << std::endl;
    std::cout << " [A] / [D]        : Decrease / Increase Windmill Speed" << std::endl;
    std::cout << " Mouse Left Drag  : Rotate Scene (360 degrees)" << std::endl;
    std::cout << " Mouse Scroll     : Change Time of Day (Sunrise/Sunset/Night)" << std::endl;
    std::cout << "========================================" << std::endl;

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