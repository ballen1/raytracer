#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <math.h>
#include <stdio.h>

#define WIDTH 600
#define HEIGHT 600
#define WINDOW_STARTX 100
#define WINDOW_STARTY 100

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
    float diffuseColor[3];
    float specularColor[3];
};

struct RectanglePlane
{
    Vec3 points[4];
};

struct Background
{
    float ambientColor[3];
    float backgroundColor[4];
};

struct Camera
{
    float eye[3];
    float lookAt[3];
    float up[3];

    float u[3];
    float v[3];
    float w[3];
};

static Sphere spheres[3];
static Background bg;
static Camera cam;

void glInit();
void reshape(int width, int height);
void display();
void setupMVPMatrices();
void defineSceneObjects();
void traceRay(float* pixel, int i, int j);
void calculateCameraCoordinateSystem();
void normalize(float* vec, int size);
float magnitude(float vec[], int size);
void crossProduct(float a[], float b[], float* result);
float dotProduct(float a[], float b[]);
void viewportToWindow(int i, int j, float* result);
int sphereIntersection(float eye[], float dir[], Sphere sphere);

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glInit();

    defineSceneObjects();

    glutMainLoop();

    return(0);
}

void glInit()
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
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
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glPushMatrix();
    
    glBegin(GL_POINTS);

    float pixel[3];

    for (int i = 0; i < HEIGHT; i++)
    {
	for (int j = 0; j < WIDTH; j++)
	{
	    traceRay(pixel, i, j);
	    glColor3fv(pixel);
	    glVertex3f(i, j , 0);
	}
    }
    
    glEnd();

    glPopMatrix();

    glutSwapBuffers();

    glutPostRedisplay();
}

void traceRay(float* pixel, int i, int j) {

    float dir[3];

    viewportToWindow(i, j, dir);

    float sphereHit = 0;

    for (int k = 0; k < 3; k++)
    {
	if(sphereIntersection(cam.eye, dir, spheres[k]))
	{
	    sphereHit = 1;
	    pixel[0] = spheres[k].diffuseColor[0];
	    pixel[1] = spheres[k].diffuseColor[1];
	    pixel[2] = spheres[k].diffuseColor[2];
	    break;
	}
    }

    if (!sphereHit)
    {
	pixel[0] = bg.backgroundColor[0];
	pixel[1] = bg.backgroundColor[1];
	pixel[2] = bg.backgroundColor[2];
    }
}

void defineSceneObjects()
{
    // Define the camera
    cam.eye[0] = 0.0f;
    cam.eye[1] = 10.0f;
    cam.eye[2] = -5.0f;
    
    cam.lookAt[0] = 0.0f;
    cam.lookAt[1] = 0.0f;
    cam.lookAt[2] = 0.0f;

    cam.up[0] = 0.0f;
    cam.up[1] = 1.0f;
    cam.up[2] = 0.0f;

    // Define the coordinate systems for the camera based on its position/lookAt definitions
    calculateCameraCoordinateSystem();
    
    // Define overall background properties
    bg.backgroundColor[0] = 0.0f;
    bg.backgroundColor[1] = 0.0f;
    bg.backgroundColor[2] = 0.0f;
    bg.backgroundColor[3] = 1.0f;

    bg.ambientColor[0] = 1.0f;
    bg.ambientColor[1] = 1.0f;
    bg.ambientColor[2] = 1.0f;

    // Define the spheres

    spheres[0].center.x = 10.0f;
    spheres[0].center.y = 10.0f;
    spheres[0].center.z = 10.0f;
    spheres[0].radius = 5.0f;
    spheres[0].diffuseColor[0] = 1.0f;
    spheres[0].diffuseColor[1] = 0.0f;
    spheres[0].diffuseColor[2] = 0.0f;
    //spheres[0].specularColor[0] = ;
    //spheres[0].specularColor[1] = ;
    //spheres[0].specularColor[2] = ;

    spheres[1].center.x = -10.0f;
    spheres[1].center.y = 10.0f;
    spheres[1].center.z = -10.0f;
    spheres[1].radius = 5.0f;
    spheres[1].diffuseColor[0] = 0.0f;
    spheres[1].diffuseColor[1] = 1.0f;
    spheres[1].diffuseColor[2] = 0.0f;
    //spheres[1].specularColor[0] = ;
    //spheres[1].specularColor[1] = ;
    //spheres[1].specularColor[2] = ;

    spheres[2].center.x = 0.0f;
    spheres[2].center.y = 5.0f;
    spheres[2].center.z = 20.0f;
    spheres[2].radius = 5.0f;
    spheres[2].diffuseColor[0] = 0.0f;
    spheres[2].diffuseColor[1] = 0.0f;
    spheres[2].diffuseColor[2] = 1.0f;
    //spheres[2].specularColor[0] = ;
    //spheres[2].specularColor[1] = ;
    //spheres[2].specularColor[2] = ;

}

void calculateCameraCoordinateSystem()
{
    
    cam.w[0] = -(cam.lookAt[0] - cam.eye[0]);
    cam.w[1] = -(cam.lookAt[1] - cam.eye[1]);
    cam.w[2] = -(cam.lookAt[2] - cam.eye[2]);

    normalize(cam.w, 3);

    crossProduct(cam.up, cam.w, cam.u);
    normalize(cam.u, 3);

    crossProduct(cam.w, cam.u, cam.v);
    normalize(cam.v, 3);

}

void normalize(float* vec, int size)
{

    // TODO: Switch this to magnitude function
    float magnitude_sqrt = 0.0f;
    
    for (int i = 0; i < size; i++)
    {
	magnitude_sqrt += vec[i]*vec[i];
    }

    magnitude_sqrt = sqrt(magnitude_sqrt);

    if (magnitude_sqrt != 0)
    {
	for (int i = 0; i < size; i++)
	{
	    vec[i] /= magnitude_sqrt;
	}
    }
}

float magnitude(float vec[], int size)
{
    float mag = 0.0f;

    for (int i = 0; i < size; i++)
    {
	mag += vec[i]*vec[i];
    }

    return sqrt(mag);
}

void crossProduct(float a[], float b[], float* result)
{
    result[0] = (a[1]*b[2] - a[2]*b[1]);
    result[1] = (a[2]*b[0] - a[0]*b[2]);
    result[2] = (a[0]*b[3] - a[3]*b[0]);
}

float dotProduct(float a[], float b[])
{
    return (a[0]*b[0] + a[1]*b[1] + a[2]*b[2]);
}

void viewportToWindow(int i, int j, float* result)
{
    
    // Calculate u
    result[0] = i;

    // Calculate v
    result[1] = j;

    // Calculate w
    result[2] = -cam.eye[2];

}

int sphereIntersection(float eye[], float dir[], Sphere sphere)
{
    float a, b, c;

    a = magnitude(eye, 3)*magnitude(eye,3);
    b = dotProduct(eye, dir);
    c = magnitude(dir, 3)*magnitude(dir, 3);

    float disct = b*b - a*c;

    if (disct > 0)
    {
	// TODO: calculate t to find exact point of intersection
	return (1);
    }
    else
    {
	return (0);
    }

}
