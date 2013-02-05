#include "Interpolation.h"

float Interpolation::Cosine(float a,float b,float x)
{
	float ft = x * 3.1415927f;
	float f = (1.0f - cosf(ft)) * 0.5f;

	return  a*(1.0f-f) + b*f;
}

float Interpolation::Linear(float a,float b,float x)
{
	return  a*(1.0f-x) + b*x;
}