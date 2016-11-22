#ifndef RAYTRACER_H
#define RAYTRACER_H

#define WIDTH 800
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

    float materialColor[3];

    float k_ambient;
    float k_diffuse;
    float k_specular;

    float specParam;
};

struct Plane
{
    float y;
    float color[3];
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

struct Light
{
    float dir[3];
    float color[3];
};

void glInit();

void reshape(int width, int height);

void display();

void setupMVPMatrices();

void defineSceneObjects();

void traceRay(float* pixel, int i, int j);

void calculateCameraCoordinateSystem();

void viewportToWindow(int i, int j, float* result);

int sphereIntersection(float eye[], float dir[], Sphere sphere, float* hitPoint);

int planeIntersection(float eye[], float dir[], Plane plane, float* hitPoint);

#endif
