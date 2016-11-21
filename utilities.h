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

#endif
