#ifndef UTILITIES_H
#define UTILITIES_H

#include <math.h>

inline float magnitude(float vec[], int size)
{
    float mag = 0.0f;

    for (int i = 0; i < size; i++)
    {
	mag += vec[i]*vec[i];
    }

    return sqrt(mag);
}

inline void normalize(float* vec, int size)
{
    float mag = magnitude(vec, 3);

    if (mag != 0)
    {
	for (int i = 0; i < size; i++)
	{
	    vec[i] /= mag;
	}
    }
}

inline void crossProduct(float a[], float b[], float* result)
{
   result[0] = (a[1]*b[2] - a[2]*b[1]);
   result[1] = (a[2]*b[0] - a[0]*b[2]);
   result[2] = (a[0]*b[3] - a[3]*b[0]);
}

inline float dotProduct(float a[], float b[]) {
    return (a[0]*b[0] + a[1]*b[1] + a[2]*b[2]);
}

inline void vSubtract(float v1[], float v2[], int size, float* result)
{
    for (int i = 0; i < size; i++)
    {
	result[i] = v1[i] - v2[i];
    }
}

inline void vAdd(float v1[], float v2[], int size, float* result)
{
    for (int i = 0; i < size; i++)
    {
	result[i] = v1[i] + v2[i];
    }
}

inline void scalarMultiply(float s, float* vec, int size)
{
    for (int i = 0; i < size; i++)
    {
	vec[i] *= s;
    }
}

inline void scalarMultiplyCopy(float s, float vec[], int size, float* result)
{
    for (int i = 0; i < size; i++)
    {
	result[i] = s * vec[i];
    }
}

#endif
