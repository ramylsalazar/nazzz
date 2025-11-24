#include <GL/glew.h>
#include <GL/glut.h>
#include <vector>
#include <iostream>

using namespace std;


GLuint vboPos;
GLuint vboColor;
int vertexCount = 0;

void setupVBO() {
    vector<float> positions;
    vector<float> colors;

    float radius = 0.7f;
    float circleCoords[] = {
        1.0f,      0.0f,      // Index 0
        0.9239f,   0.3827f,   // Index 1
        0.7071f,   0.7071f,   // Index 2
        0.3827f,   0.9239f,   // Index 3
        0.0f,      1.0f,      // Index 4
       -0.3827f,   0.9239f,   // Index 5
       -0.7071f,   0.7071f,   // Index 6
       -0.9239f,   0.3827f,   // Index 7
       -1.0f,      0.0f,      // Index 8
       -0.9239f,  -0.3827f,   // Index 9
       -0.7071f,  -0.7071f,   // Index 10
       -0.3827f,  -0.9239f,   // Index 11
        0.0f,     -1.0f,      // Index 12
        0.3827f,  -0.9239f,   // Index 13
        0.7071f,  -0.7071f,   // Index 14
        0.9239f,  -0.3827f,   // Index 15
        1.0f,      0.0f       // Index 16 
    };

    
    float bladeColors[8][3] = {
        {0.0f, 1.0f, 0.0f},   // Green
        {0.6f, 1.0f, 0.0f},   // Lime
        {1.0f, 1.0f, 0.0f},   // Yellow
        {1.0f, 0.6f, 0.0f},   // Orange
        {0.8f, 0.0f, 0.6f},   // Magenta
        {0.4f, 0.0f, 0.8f},   // Purple
        {0.0f, 0.3f, 1.0f},   // Dark Blue
        {0.0f, 0.6f, 1.0f}    // Light Blue
    };


    positions.push_back(0.0f); positions.push_back(0.0f);
    colors.push_back(0.0f); colors.push_back(0.0f); colors.push_back(0.0f);

   
    positions.push_back(radius * circleCoords[0]);     // x
    positions.push_back(radius * circleCoords[1]);     // y
    colors.push_back(0.0f); colors.push_back(0.0f); colors.push_back(0.0f);

   
   
    int currIdx = 2; 

    for (int i = 0; i < 8; i++) {
        //Blade Vertex 
        float bx = circleCoords[currIdx];
        float by = circleCoords[currIdx + 1];
        positions.push_back(radius * bx);
        positions.push_back(radius * by);

        // Assign Blade Color
        colors.push_back(bladeColors[i][0]);
        colors.push_back(bladeColors[i][1]);
        colors.push_back(bladeColors[i][2]);

        currIdx += 2; // Move to next coordinate pair

        //Gap Vertex
        float gx = circleCoords[currIdx];
        float gy = circleCoords[currIdx + 1];
        positions.push_back(radius * gx);
        positions.push_back(radius * gy);

        //Assign Gap Color (Black)
        colors.push_back(0.0f);
        colors.push_back(0.0f);
        colors.push_back(0.0f);

        currIdx += 2; // Move to next coordinate pair
    }

    vertexCount = positions.size() / 2;

    // Generate and Bind Position Buffer
    glGenBuffers(1, &vboPos);
    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), &positions[0], GL_STATIC_DRAW);

    // Generate and Bind Color Buffer
    glGenBuffers(1, &vboColor);
    glBindBuffer(GL_ARRAY_BUFFER, vboColor);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), &colors[0], GL_STATIC_DRAW);
}

void display() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    glShadeModel(GL_FLAT);

    // Enable Arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Bind Position VBO
    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    // Bind Color VBO
    glBindBuffer(GL_ARRAY_BUFFER, vboColor);
    glColorPointer(3, GL_FLOAT, 0, 0);

    // Draw
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

    // Cleanup
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Ramyl_Salazar_Lab7");

    // Initialize GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    setupVBO();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}