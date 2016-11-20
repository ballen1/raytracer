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

static GLUquadric* SphereQuadric = gluNewQuadric();

struct Vec3 
{
    float x;
    float y;
    float z;
};

struct Sphere 
{
    Vec3 center;
    float radius;
};

struct RectanglePlane
{
    Vec3 points[4];
};


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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(WINDOW_STARTX, WINDOW_STARTY);

    glutCreateWindow("Ray Tracer");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    glDisable(GL_DEPTH_TEST);

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
    glOrtho(0, EXTENT, 0, EXTENT, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glPushMatrix();
    
    
    glPopMatrix();

    glutSwapBuffers();

    glutPostRedisplay();
}
