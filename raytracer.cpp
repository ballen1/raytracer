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
static Plane plane;
static Camera cam;
static Light Light0;

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
    glutInitWindowSize(ScreenWidth, ScreenHeight);
    glutInitWindowPosition(WINDOW_STARTX, WINDOW_STARTY);

    glutCreateWindow("Ray Tracer");

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

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


    for (int j = 0; j < ScreenHeight; j++)
    {   
	for (int i = 0; i < ScreenWidth; i++)
	{
	    traceRay(pixel, i, j);
	    glColor3fv(pixel);
	    glVertex3f(i+0.5, j+0.5, 0);
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

    int sphereHit = -1;

    float t_hitPoint, t_currentHitPoint;
    
    t_hitPoint = 0;

    for (int k = 0; k < 3; k++)
    {
	if(sphereIntersection(cam.eye, dir, spheres[k], &t_currentHitPoint))
	{

	    if (t_hitPoint <= 0 || t_currentHitPoint <= t_hitPoint)
	    {
		t_hitPoint = t_currentHitPoint;
		sphereHit = k;
	    }

	}
    }


    if (sphereHit != -1)
    {
	pixel[0] = 0.0f;
	pixel[1] = 0.0f;
	pixel[2] = 0.0f;
	float intersectPoint[3] = {0.0, 0.0, 0.0};
	    
	scalarMultiply(t_hitPoint, dir, 3);
	vAdd(dir, cam.eye, 3, intersectPoint);

	float normal[3] = {0.0, 0.0, 0.0};
	float center[3] = {spheres[sphereHit].center.x, spheres[sphereHit].center.y, spheres[sphereHit].center.z};

	vSubtract(intersectPoint, center, 3, normal);
	normalize(normal, 3);

	// Ambient Lighting
	float ambientLighting[3];
	
	scalarMultiplyCopy(spheres[sphereHit].k_ambient, bg.ambientColor, 3, ambientLighting);

	vec3Mult(ambientLighting, spheres[sphereHit].materialColor, ambientLighting);

	vAdd(pixel, ambientLighting, 3, pixel);

	// Diffuse Lighting

	float NL = dotProduct(Light0.dir, normal);

	float diffuseLighting[3];
	    
	scalarMultiplyCopy(spheres[sphereHit].k_diffuse, Light0.color, 3, diffuseLighting);
	
	vec3Mult(diffuseLighting, spheres[sphereHit].materialColor, diffuseLighting);
    
	scalarMultiply(NL, diffuseLighting, 3);

	vAdd(pixel, diffuseLighting, 3, pixel);

/*
	// Specular Lighting
	float V[3] = {0.0, 0.0, 0.0};

	float scale = -2*dotProduct(normal, Light0.dir);
	scalarMultiplyCopy(scale, normal, 3, V);
       
	float R[3];
	vAdd(Light0.dir, V, 3, R);

	float specularLightingMultiplier = (spheres[sphereHit].k_specular) * pow(dotProduct(R, V), spheres[sphereHit].specParam);
	float specularLighting[3];

	scalarMultiplyCopy(specularLightingMultiplier, spheres[sphereHit].specularColor, 3, specularLighting);
	
	vAdd(pixel, specularLighting, 3, pixel);
*/

    }

    int planeHit = 0;

    if (sphereHit == -1)
    {
	planeHit = planeIntersection(cam.eye, dir, plane, &t_hitPoint);
	
	pixel[0] = plane.color[0];
	pixel[1] = plane.color[1];
	pixel[2] = plane.color[2];
    }
    
    if (sphereHit == -1  && !planeHit)
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
	float delta = sqrt(disct);

	float t1, t2;

	t1 = (-b - delta)/(2.0*a);
	t2 = (-b + delta)/(2.0*a);

	*hitPoint = t1 < t2 ? t1 : t2;

	return (1);
    }
    else
    {
	return (0);
    }

}

int planeIntersection(float eye[], float dir[], Plane plane, float* hitPoint)
{
    if (dir[1] >= 0.0)
    {
	return (0);
    }

    else
    {
	*hitPoint = -(eye[1]/dir[1]);
    }

}

void defineSceneObjects()
{
    // Define the camera
    cam.eye[0] = 300.0f;
    cam.eye[1] = 25.0f;
    cam.eye[2] = 2000.0f;
    
    cam.lookAt[0] = 300.0f;
    cam.lookAt[1] = 50.0f;
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

    // Define the plane
    plane.y = 0.0f;
    
    plane.color[0] = 0.8f;
    plane.color[1] = 0.8f;
    plane.color[2] = 0.8f;

    // Define the spheres

    spheres[0].center.x = 100.0f;
    spheres[0].center.y = 100.0f;
    spheres[0].center.z = 0.0f;
    spheres[0].radius = 100.0f;
    spheres[0].materialColor[0] = 1.0f;
    spheres[0].materialColor[1] = 0.0f;
    spheres[0].materialColor[2] = 0.0f;

    spheres[0].k_ambient = 0.2f;
    spheres[0].k_diffuse = 0.7f;
    spheres[0].k_specular = 0.03f;
    spheres[0].specParam = 5;

    spheres[1].center.x = 250.0f;
    spheres[1].center.y = 50.0f;
    spheres[1].center.z = 100.0f;
    spheres[1].radius = 50.0f;
    spheres[1].materialColor[0] = 0.0f;
    spheres[1].materialColor[1] = 1.0f;
    spheres[1].materialColor[2] = 0.0f;

    spheres[1].k_ambient = 0.2f;
    spheres[1].k_diffuse = 0.7f;
    spheres[1].k_specular = 0.03f;
    spheres[1].specParam = 5;

    spheres[2].center.x = 450.0f;
    spheres[2].center.y = 80.0f;
    spheres[2].center.z = 20.0f;
    spheres[2].radius = 75.0f;
    spheres[2].materialColor[0] = 0.0f;
    spheres[2].materialColor[1] = 0.0f;
    spheres[2].materialColor[2] = 1.0f;

    spheres[2].k_ambient = 0.2f;
    spheres[2].k_diffuse = 0.7f;
    spheres[2].k_specular = 0.01f;
    spheres[2].specParam = 5;

    Light0.color[0] = 1.0f;
    Light0.color[1] = 1.0f;
    Light0.color[2] = 1.0f;

    Light0.dir[0] = -150.0;
    Light0.dir[1] = 0.0;
    Light0.dir[2] = 250.0;
    
    normalize(Light0.dir, 3);

}
