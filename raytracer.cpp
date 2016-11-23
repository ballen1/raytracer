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

    updateSpherePositions(spheres, 3);

    glutPostRedisplay();
}

void traceRay(float* pixel, int i, int j) {

    // Clear the pixel

    pixel[0] = 0.0f;
    pixel[1] = 0.0f;
    pixel[2] = 0.0f;

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

    for (int k = 0; k < SPHERE_COUNT; k++)
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

    // If a sphere was hit, the sphereHit will be set with the index of the sphere

    if (sphereHit != -1)
    {

	float intersectPoint[3];
	    	
	calculateParametricPoint(t_hitPoint, dir, cam.eye, intersectPoint);
	
	float normal[3];
	float LightDir[3];

	scalarMultiplyCopy(-1.0, Light0.dir, 3, LightDir);

	float center[3] = {spheres[sphereHit].center.x,
			   spheres[sphereHit].center.y,
			   spheres[sphereHit].center.z};

	vSubtract(intersectPoint, center, 3, normal);
	normalize(normal, 3);

	// Ambient Lighting

	addAmbientLighting(spheres[sphereHit].k_ambient, Light0.color,
			   spheres[sphereHit].materialColor, pixel);

	if (!isInShadow(intersectPoint, LightDir, normal, sphereHit))
	{

	    // Diffuse Lighting

	    addDiffuseLighting(normal, LightDir, spheres[sphereHit].k_diffuse,
			       Light0.color, spheres[sphereHit].materialColor,
			       pixel);

	    // Specular Lighting
	    float V[3];

	    V[0] = (cam.eye[0] - intersectPoint[0]);
	    V[1] = (cam.eye[1] - intersectPoint[1]);
	    V[2] = (cam.eye[2] - intersectPoint[2]);

	    normalize(V, 3);

	    float intermediate[3];
	    float dot;

	    dot = dotProduct(normal, LightDir)*2.0;
	    scalarMultiplyCopy(dot, normal, 3, intermediate);

	    float R[3];

	    vAdd(intermediate, Light0.dir, 3, R);

	    float VR = dotProduct(R, V);

	    if (VR >= 0)
	    {
		float specularMultiplier = pow(VR, spheres[sphereHit].specParam) * spheres[sphereHit].k_specular;

		float specularLighting[3];
		scalarMultiplyCopy(specularMultiplier, Light0.color, 3, specularLighting);

		vAdd(pixel, specularLighting, 3, pixel);
	    }	    
	}
    }

    int planeHit = 0;

    if (sphereHit == -1)
    {
	planeHit = planeIntersection(cam.eye, dir, plane, &t_hitPoint);
	
	float intersectPoint[3];
	    
	calculateParametricPoint(t_hitPoint, dir, cam.eye, intersectPoint);

	float normal[3];
	float LightDir[3];

	scalarMultiplyCopy(-1.0, Light0.dir, 3, LightDir);

	normal[0] = 0.0f;
	normal[1] = 1.0f;
	normal[2] = 0.0f;
	
	// Ambient Lighting
	
	addAmbientLighting(plane.k_ambient, Light0.color, plane.materialColor, pixel);

	if (!isInShadow(intersectPoint, LightDir, normal))
	{
	    // Diffuse Lighting
	    	    
	    addDiffuseLighting(normal, LightDir, plane.k_diffuse,
			       Light0.color, plane.materialColor,
			       pixel);

	}
    }
    
    if (sphereHit == -1  && !planeHit)
    {
	pixel[0] = bg.backgroundColor[0];
	pixel[1] = bg.backgroundColor[1];
	pixel[2] = bg.backgroundColor[2];
    }
}

void calculateParametricPoint(float t, float dir[], float origin[], float* result)
{
    scalarMultiplyCopy(t, dir, 3, result);

    vAdd(result, origin, 3, result);    
}

void addAmbientLighting(float k, float LightColor[], float materialColor[], float* pixel)
{

    float ambientLighting[3];

    scalarMultiplyCopy(k, LightColor, 3, ambientLighting);

    vec3Mult(ambientLighting, materialColor, ambientLighting);

    vAdd(pixel, ambientLighting, 3, pixel);

}

void addDiffuseLighting(float normal[], float lightDir[], float k,
			float lightColor[], float materialColor[],
			float* pixel)
{
    
    float NL = dotProduct(lightDir, normal);

    float diffuseLighting[3];

    scalarMultiplyCopy(k, lightColor, 3, diffuseLighting);
    vec3Mult(diffuseLighting, materialColor, diffuseLighting);
    scalarMultiply(NL, diffuseLighting, 3);

    vAdd(pixel, diffuseLighting, 3, pixel);

}

int isInShadow(float point[], float light[], float normal[], int excludeSphere)
{

    int shadow = 0;
    
    float direction[3];

    vAdd(light, point, 3, direction);

    if (dotProduct(normal, light) >= 0)
    {
	for (int i = 0; i < SPHERE_COUNT; i++)
	{
	    float hitPoint;
	    if (i != excludeSphere && sphereIntersection(point, light, spheres[i], &hitPoint))
	    {
		if (hitPoint > 1)
		{

		    shadow = 1;
		    break;
		}
	    }
	}
    }
    
    return (shadow);
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

    rayOrigin[0] = eye[0] - sphere.center.x;
    rayOrigin[1] = eye[1] - sphere.center.y;
    rayOrigin[2] = eye[2] - sphere.center.z;

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
	return(1);
    }

}

void updateSpherePositions(Sphere* l_Sphere, int size)
{

    for (int i = 0; i < size; i++)
    {
	l_Sphere[i].center.y += l_Sphere[i].y_move;
	
	if ((l_Sphere[i].y_move > 0 && l_Sphere[i].center.y >= l_Sphere[i].y_max) ||
	    (l_Sphere[i].y_move < 0 && l_Sphere[i].center.y <= l_Sphere[i].y_min))
	{
	    l_Sphere[i].y_move = -l_Sphere[i].y_move;
	}
    }

}

void defineSceneObjects()
{
    // Define the camera
    cam.eye[0] = 300.0f;
    cam.eye[1] = 300.0f;
    cam.eye[2] = 1500.0f;
    
    cam.lookAt[0] = 300.0f;
    cam.lookAt[1] = 150.0f;
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

    // Define the plane
    plane.y = 0.0f;
    
    plane.materialColor[0] = 0.8f;
    plane.materialColor[1] = 0.8f;
    plane.materialColor[2] = 0.8f;

    plane.k_ambient = 0.3f;
    plane.k_diffuse = 1.0f;

    // Define the spheres

    spheres[0].center.x = 100.0f;
    spheres[0].center.y = 100.0f;
    spheres[0].center.z = 100.0f;
    spheres[0].radius = 50.0f;
    spheres[0].y_max = 300.0f;
    spheres[0].y_min = 100.0f;
    spheres[0].y_move = 6;
    spheres[0].materialColor[0] = 1.0f;
    spheres[0].materialColor[1] = 0.0f;
    spheres[0].materialColor[2] = 0.0f;

    spheres[0].k_ambient = 0.4f;
    spheres[0].k_diffuse = 0.7f;
    spheres[0].k_specular = 0.9f;
    spheres[0].specParam = 10;

    spheres[1].center.x = 250.0f;
    spheres[1].center.y = 101.0f;
    spheres[1].center.z = 0.0f;
    spheres[1].radius = 100.0f;
    spheres[1].y_max = 450.0f;
    spheres[1].y_min = 101.0f;
    spheres[1].y_move = 2;
    spheres[1].materialColor[0] = 0.0f;
    spheres[1].materialColor[1] = 1.0f;
    spheres[1].materialColor[2] = 0.0f;

    spheres[1].k_ambient = 0.15f;
    spheres[1].k_diffuse = 0.7f;
    spheres[1].k_specular = 0.7f;
    spheres[1].specParam = 5;

    spheres[2].center.x = 400.0f;
    spheres[2].center.y = 80.0f;
    spheres[2].center.z = 200.0f;
    spheres[2].radius = 75.0f;
    spheres[2].y_max = 250.0f;
    spheres[2].y_min = 80.0f;
    spheres[2].y_move = 3;
    spheres[2].materialColor[0] = 0.0f;
    spheres[2].materialColor[1] = 0.0f;
    spheres[2].materialColor[2] = 1.0f;

    spheres[2].k_ambient = 0.4f;
    spheres[2].k_diffuse = 0.7f;
    spheres[2].k_specular = 0.8f;
    spheres[2].specParam = 20;

    Light0.color[0] = 1.0f;
    Light0.color[1] = 1.0f;
    Light0.color[2] = 1.0f;

    Light0.dir[0] = 150.0;
    Light0.dir[1] = -150.0;
    Light0.dir[2] = -50.0;
    
    normalize(Light0.dir, 3);

}
