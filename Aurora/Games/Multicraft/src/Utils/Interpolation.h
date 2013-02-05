#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <math.h>

class Interpolation
{
public:
	static float Linear(float a, float b, float x);
	static float Cosine(float a, float b, float x);
};


#endif