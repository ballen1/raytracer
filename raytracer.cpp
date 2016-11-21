#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include "raytracer.h"
#include "utilities.h"
#include <stdio.h>

static Sphere spheres[3];
static Background bg;
static Camera cam;

static int ScreenWidth;
static int ScreenHeight;

int main(int argc, char* argv[])
{

    ScreenWidth = WIDTH;
    ScreenHeight = HEIGHT;

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
    ScreenWidth = width;
    ScreenHeight = height;
    glViewport(0, 0, ScreenWidth, ScreenHeight);
    setupMVPMatrices();
}

void setupMVPMatrices()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, ScreenWidth, 0, ScreenHeight, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glPushMatrix();

    glBegin(GL_POINTS);

    float pixel[3];

    for (int j = 0; j <= ScreenHeight; j++)
    {
	for (int i = 0; i <= ScreenWidth; i++)
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

    float v2w[3];

    viewportToWindow(i, j, v2w);

    float dir[3];

    dir[0] = (v2w[0]*cam.u[0]) + (v2w[1]*cam.v[0]) - (v2w[2]*cam.w[0]);
    dir[1] = (v2w[0]*cam.u[1]) + (v2w[1]*cam.v[1]) - (v2w[2]*cam.w[1]);
    dir[2] = (v2w[0]*cam.u[2]) + (v2w[1]*cam.v[2]) - (v2w[2]*cam.w[2]);

    normalize(dir, 3);

    int sphereHit = 0;
    float t_hitPoint;

    for (int k = 0; k < 3; k++)
    {
	if(sphereIntersection(cam.eye, dir, spheres[k], &t_hitPoint))
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
    result[0] = (1.0)*(i)*(WIDTH)/(ScreenWidth) + (-WIDTH/2.0);

    // Calculate v
   
    result[1] = (1.0)*(j)*(HEIGHT)/(ScreenHeight) + (-HEIGHT/2.0);

    // Calculate w
    result[2] = cam.eye[2];

}

int sphereIntersection(float eye[], float dir[], Sphere sphere, float* hitPoint)
{
    float a, b, c;

    a = dotProduct(dir, dir);

    float rayOrigin[3];

    rayOrigin[0] = cam.eye[0];
    rayOrigin[1] = cam.eye[1];
    rayOrigin[2] = cam.eye[2];

    rayOrigin[0] -= sphere.center.x;
    rayOrigin[1] -= sphere.center.y;
    rayOrigin[2] -= sphere.center.z;

    b = 2*dotProduct(rayOrigin, dir);

    c = magnitude(rayOrigin, 3)*magnitude(rayOrigin, 3) - (sphere.radius*sphere.radius);

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
