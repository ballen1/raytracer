#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include "raytracer.h"
#include "utilities.h"
#include <stdio.h>

void glInit();
void reshape(int width, int height);
void display();
void setupMVPMatrices();
void defineSceneObjects();
void traceRay(float* pixel, int i, int j);
void calculateCameraCoordinateSystem();
void viewportToWindow(int i, int j, float* result);
int sphereIntersection(float eye[], float dir[], Sphere sphere);

static Sphere spheres[3];
static Background bg;
static Camera cam;

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

    for (int j = 0; j < HEIGHT; j++)
    {
	for (int i = 0; i < WIDTH; i++)
	{
	    traceRay(pixel, i, j);
	    glColor3fv(pixel);
	    glVertex2i(i, j);
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

    float vecDir[3];
    vecDir[0] = (dir[0]*cam.u[0]) + (dir[1]*cam.v[0]) - (dir[2]*cam.w[0]);
    vecDir[1] = (dir[0]*cam.u[1]) + (dir[1]*cam.v[1]) - (dir[2]*cam.w[1]);
    vecDir[2] = (dir[0]*cam.u[2]) + (dir[1]*cam.v[2]) - (dir[2]*cam.w[2]);
    normalize(vecDir, 3);

    float sphereHit = 0;
    
    for (int k = 0; k < 3; k++)
    {
	if(sphereIntersection(cam.eye, vecDir, spheres[k]))
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
    cam.eye[0] = 300.0f;
    cam.eye[1] = 300.0f;
    cam.eye[2] = 1000.0f;
    
    cam.lookAt[0] = 300.0f;
    cam.lookAt[1] = 300.0f;
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

    spheres[0].center.x = 300.0f;
    spheres[0].center.y = 300.0f;
    spheres[0].center.z = 0.0f;
    spheres[0].radius = 50.0f;
    spheres[0].diffuseColor[0] = 1.0f;
    spheres[0].diffuseColor[1] = 0.0f;
    spheres[0].diffuseColor[2] = 0.0f;
    //spheres[0].specularColor[0] = ;
    //spheres[0].specularColor[1] = ;
    //spheres[0].specularColor[2] = ;

    spheres[1].center.x = 100.0f;
    spheres[1].center.y = 100.0f;
    spheres[1].center.z = -10.0f;
    spheres[1].radius = 25.0f;
    spheres[1].diffuseColor[0] = 0.0f;
    spheres[1].diffuseColor[1] = 1.0f;
    spheres[1].diffuseColor[2] = 0.0f;
    //spheres[1].specularColor[0] = ;
    //spheres[1].specularColor[1] = ;
    //spheres[1].specularColor[2] = ;

    spheres[2].center.x = 450.0f;
    spheres[2].center.y = 450.0f;
    spheres[2].center.z = 20.0f;
    spheres[2].radius = 75.0f;
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

void viewportToWindow(int i, int j, float* result)
{
    
    // Calculate u
    result[0] = i + (-WIDTH/2.0);

    // Calculate v
    result[1] = j + (-WIDTH/2.0);

    // Calculate w
    result[2] = cam.eye[2];

}

int sphereIntersection(float eye[], float dir[], Sphere sphere)
{
    float a, b, c;

    a = dotProduct(dir, dir);

    float rayStart[3];
    rayStart[0] = cam.eye[0] - sphere.center.x;
    rayStart[1] = cam.eye[1] - sphere.center.y;
    rayStart[2] = cam.eye[2] - sphere.center.z;

    b = 2*dotProduct(rayStart, dir);

    c = magnitude(rayStart, 3)*magnitude(rayStart, 3) - (sphere.radius*sphere.radius);

    float disct = b*b - 4*a*c;

    if (disct >= 0)
    {
	// TODO: calculate t to find exact point of intersection
	return (1);
    }
    else
    {
	return (0);
    }

}
