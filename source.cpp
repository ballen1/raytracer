#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define WINDOW_STARTX 100
#define WINDOW_STARTY 100

#define EXTENT 50

void glInit();
void reshape(int width, int height);
void display();
void setupMVPMatrices();

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glInit();

    glutMainLoop();

    return(0);
}

void glInit()
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(WINDOW_STARTX, WINDOW_STARTY);

    glutCreateWindow("Ray Tracer");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glPolygonMode(GL_FRONT, GL_FILL);
    
    glEnable(GL_DEPTH_TEST);

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    setupMVPMatrices();
}

void setupMVPMatrices()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-EXTENT, EXTENT, -EXTENT, EXTENT, -EXTENT, EXTENT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glutSwapBuffers();
}
