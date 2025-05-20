#include <GL/glut.h>
#include <cmath>

// Rotation and color state
float angle = 0.0f;
float red = 0.0f;
bool makeGreen = false;

// Timer update function
void update(int value) {
    angle += 0.5f; // Rotate slowly
    if (!makeGreen && red < 1.0f)
        red += 0.01f; // Increase red gradually

    glutPostRedisplay();              // Redraw
    glutTimerFunc(16, update, 0);     // Call update every 16ms (~60 FPS)
}

// Render function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Rotate the triangle
    glTranslatef(0.0f, 0.0f, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    // Set color
    if (makeGreen)
        glColor3f(0.0f, 1.0f, 0.0f);  // Green
    else
        glColor3f(red, 0.0f, 0.0f);   // Animated red

    // Draw triangle
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glEnd();

    glFlush();
}

// Keyboard press handler
void keyboard(unsigned char key, int x, int y) {
    if (key == 's' || key == 'S') {
        makeGreen = true;
        glutPostRedisplay();
    }
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0); // Background black
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);         // Set 2D projection
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Rotating Triangle - Shuvo");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);     // Start timer
    glutMainLoop();
    return 0;
}
