#ifndef RAYTRACER_H
#define RAYTRACER_H

#define WIDTH 800
#define HEIGHT 600
#define WINDOW_STARTX 100
#define WINDOW_STARTY 100

#define SPHERE_COUNT 3

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

    float y_max;
    float y_min;

    int y_move;
};

struct Plane
{
    float y;
    float materialColor[3];
    
    float k_ambient;
    float k_diffuse;
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

int isInShadow(float point[], float light[], float normal[], int excludeSphere = -1);

void updateSpherePositions(Sphere* l_Sphere, int size);

void calculateParametricPoint(float t, float dir[], float origin[], float* result);

void addAmbientLighting(float k, float LightColor[], float materialColor[], float* pixel);

void addDiffuseLighting(float normal[], float lightDir[], float k,
                        float lightColor[], float materialColor[],
                        float* pixel);

void addSpecularLighting(float camOrigin[],
			 float intersectPoint[],
			 float normal[],
			 float lightDir[],
			 float lightTravelDir[],
			 float specularParameter,
			 float k,
			 float lightColor[],
			 float* pixel);

#endif
