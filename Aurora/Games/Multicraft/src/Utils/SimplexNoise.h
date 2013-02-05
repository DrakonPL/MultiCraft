#ifndef SimplexNoise_H
#define SimplexNoise_H

class SimplexNoise
{
public:
	SimplexNoise(void);
	SimplexNoise(int seed);
	~SimplexNoise(void);

	void setSeed(int seed);

	float sdnoise1( float x, float *dnoise_dx);
	float sdnoise2( float x, float y, float *dnoise_dx, float *dnoise_dy );
	float sdnoise3( float x, float y, float z,float *dnoise_dx, float *dnoise_dy, float *dnoise_dz );
	float sdnoise4( float x, float y, float z, float w,	float *dnoise_dx, float *dnoise_dy,	float *dnoise_dz, float *dnoise_dw);
};

#endif