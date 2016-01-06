
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
/* ---- Eigene Header einbinden ---- */
#include "terrain.h"
#include "logic.h"
#include "vector.h"
#include "types.h"

Terrain * G_TerrainList;
GLuint * G_TerrainIndices;

Terrain * G_TerrainCaustics;

Terrain * getTerrainList(void){
	return G_TerrainList;
}

Terrain * getTerrainCaustics(void) {
	return G_TerrainCaustics;
}

GLuint * getTerrainIndices (void) 
{
	return G_TerrainIndices;
}

void initTerrainIndices(void) {
	
	int i,j;
	int index = 0;
	for (i=0;i<TERRAIN_SIZE*TERRAIN_SIZE;i++) {		
		int atI = i % TERRAIN_SIZE;
		int atJ = i / TERRAIN_SIZE;
		
		if (atI == TERRAIN_SIZE-1 || atJ == TERRAIN_SIZE-1) {
			continue;
		}
		
		G_TerrainIndices[index]   = i;
		G_TerrainIndices[index+1] = i+1;
		G_TerrainIndices[index+2] = i+TERRAIN_SIZE+1;
		
		G_TerrainIndices[index+3] = i;
		G_TerrainIndices[index+4] = i+TERRAIN_SIZE+1;
		G_TerrainIndices[index+5] = i+TERRAIN_SIZE;
		
		index += 6;
	}
	
}

void calcTerrainNormals(void) {
	int i;
	for (i=0;i<TERRAIN_SIZE*TERRAIN_SIZE;i++) {
		int atI = i % TERRAIN_SIZE;
		int atJ = i / TERRAIN_SIZE;
		
		if (atI < 1 || atI >= TERRAIN_SIZE-1 || atJ < 1 || atJ >= TERRAIN_SIZE-1) {
			continue;
		}
		
		CGVector3D v1 = {G_TerrainList[i-TERRAIN_SIZE].x - G_TerrainList[i].x, G_TerrainList[i-TERRAIN_SIZE].y - G_TerrainList[i].y, G_TerrainList[i-TERRAIN_SIZE].z - G_TerrainList[i].z};
		CGVector3D v2 = {G_TerrainList[i-1].x - G_TerrainList[i].x, G_TerrainList[i-1].y - G_TerrainList[i].y, G_TerrainList[i-1].z - G_TerrainList[i].z};
		CGVector3D res1;
		crossProduct3D(res1, v1, v2);
		
		CGVector3D v3 = {G_TerrainList[i+TERRAIN_SIZE].x - G_TerrainList[i].x, G_TerrainList[i+TERRAIN_SIZE].y - G_TerrainList[i].y, G_TerrainList[i+TERRAIN_SIZE].z - G_TerrainList[i].z};
		CGVector3D v4 = {G_TerrainList[i+1].x - G_TerrainList[i].x, G_TerrainList[i+1].y - G_TerrainList[i].y, G_TerrainList[i+1].z - G_TerrainList[i].z};
		CGVector3D res2;
		crossProduct3D(res2, v3, v4);
		
		if (res1[1] < 0) {
			multiplyVectorScalar(res1, -1, &res1);
		}
		if (res2[1] < 0) {
			multiplyVectorScalar(res2, -1, &res2);
		}
		CGVector3D normal;
		addVectorVector(res1, res2, &normal);
		
		G_TerrainList[i].nx = normal[0];
		G_TerrainList[i].ny = normal[1];
		G_TerrainList[i].nz = normal[2];
		
		G_TerrainCaustics[i].nx = G_TerrainList[i].nx;
		G_TerrainCaustics[i].ny = G_TerrainList[i].ny;
		G_TerrainCaustics[i].nz = G_TerrainList[i].nz;
		
	}
	
}

/**
 * Kubische Interpolation.
 */
double interpolateCube(double v0, double v1, double v2, double v3, double x) {
	double p = (v3 - v2) - (v0 - v1);
	double q = (v0 - v1) - p;
	double r = v2 - v0;
	double s = v1;
	return p*pow(x, 3) + q*pow(x, 2) + r*x + s;
}

/**
 * Kosinus-Interpolation.
 */
double interpolateCos(double a, double b, double x) {
	double ft = x * 3.1415927;
	double f  = (1 - cos(ft)) * 0.5;
	return a*(1-f) + b*f;
}

/**
 * Erzeugt random-Noise-Wert zwischen 0..1
 */
double noise(int x, int y) {
	long n = x + y * 57;
	n = (n<<13) ^ n;
	return (1.0 - ((n * (n*n*15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}


/**
 * Erzeugt Noise... Etwas angepasst und smoothed.
 */
double smoothedNoise(float x, float y) {
	double corners 	= ( noise(x-1, y-1) + noise(x+1, y-1) + noise(x-1, y+1) + noise(x+1, y+1) ) / 16.0;
	double sides	= ( noise(x-1, y  ) + noise(x+1, y  ) + noise(x  , y-1) + noise(x  , y+1) ) / 8;
	double center	= noise(x,y) / 4;
	return corners + sides + center;
}

/**
 * Interpoliert das errechneten Noise, um perfekte fließende Übergänge zu erreichen (je nach Interpolationsart).
 */
double interpolatedNoise(float x, float y) {
	
	int intX 			= (int)x;
	double fractionalX 	= x - intX;
	int intY			= (int)y;
	double fractionalY 	= y - intY;
	
	double v1 = smoothedNoise (intX		, intY 		);
	double v2 = smoothedNoise (intX + 1	, intY 		);
	double v3 = smoothedNoise (intX		, intY + 1	);
	double v4 = smoothedNoise (intX + 1	, intY + 1	);
	
	double i1 = interpolateCos(v1, v2, fractionalX);
	double i2 = interpolateCos(v3, v4, fractionalX);
	
	return interpolateCos(i1, i2, fractionalY);
}

/**
 * Berechnet einen Perlin-Noise-Wert für eine beliebige Position auf dem Feld!
 */
double perlinNoise2D(float x, float y) {
	double 	total  	= 0.0;
	double 	p 		= PERSISTENCE;
	int 	n		= OCTAVES_COUNT;
	int i;
	
	for (i = 0;i<n;i++) {
		
		double frequency = pow(2, i);
		double amplitude = pow(p, i);
		
		total += interpolatedNoise (x * frequency, y * frequency) * amplitude;
		
	}
	
	return total;
}

double gauss(float x, float y, float offset) {
	float a = 3.0;
	float e = 2.71828182846;
	float spread = 25.0;
	
	x = x + offset;
	y = y + offset;
	
	float term = -(pow(x,2)/(2*spread) + pow(y,2)/(2*spread));
	return -a * pow(e, term);
}

void initTerrain(void) {
	
	int i,j;
	
	double min, max;
	
	G_TerrainList = calloc(TERRAIN_SIZE*TERRAIN_SIZE, sizeof(*G_TerrainList));
	G_TerrainCaustics = calloc(TERRAIN_SIZE*TERRAIN_SIZE, sizeof(*G_TerrainList));
	G_TerrainIndices = calloc(TERRAIN_SIZE*TERRAIN_SIZE*2*3, sizeof(*G_TerrainIndices));

	for (i=0;i<(TERRAIN_SIZE)*(TERRAIN_SIZE);i++) {
		int xCoord = i / TERRAIN_SIZE;
		int zCoord = i % TERRAIN_SIZE;
		
		G_TerrainList[i].x = (xCoord - TERRAIN_SIZE/2) * TERRAIN_DISTANCE;
		
		float noiseAtX = (float)xCoord/10.0;
		float noiseAtZ = (float)zCoord/10.0;
		
		G_TerrainList[i].y = 	(perlinNoise2D(noiseAtX, noiseAtZ)*TERRAIN_DIMENSION
								/* Die 0.8 ist ein manueller Versatz, damit die Wasserkante nicht sichtbar ist. */
								+ gauss(noiseAtX, noiseAtZ, -TERRAIN_SIZE*TERRAIN_DISTANCE*0.5*0.1*0.9) * GAUSS_DIMENSION
								- 2.0 * gauss(noiseAtX, noiseAtZ, 0.0) * GAUSS_DIMENSION) 
								+ 14.0;
		G_TerrainList[i].z = (zCoord - TERRAIN_SIZE/2) * TERRAIN_DISTANCE;
		
		G_TerrainList[i].s = (float)xCoord / (float)TERRAIN_SIZE;
		G_TerrainList[i].t = (float)zCoord / (float)TERRAIN_SIZE;
		
		G_TerrainList[i].nx = 0.0;
		G_TerrainList[i].ny = 1.0;
		G_TerrainList[i].nz = 0.0;
		
		min = G_TerrainList[i].y < min ? G_TerrainList[i].y : min;
		max = G_TerrainList[i].y > max ? G_TerrainList[i].y : max;
		
		G_TerrainCaustics[i].x = G_TerrainList[i].x;
		G_TerrainCaustics[i].y = G_TerrainList[i].y;
		G_TerrainCaustics[i].z = G_TerrainList[i].z;
		G_TerrainCaustics[i].nx = G_TerrainList[i].nx;
		G_TerrainCaustics[i].ny = G_TerrainList[i].ny;
		G_TerrainCaustics[i].nz = G_TerrainList[i].nz;
		G_TerrainCaustics[i].s = G_TerrainList[i].s;
		G_TerrainCaustics[i].t = G_TerrainList[i].t;		
		
	}
	
	printf ("Terrain-Height min: %f, max: %f\n", min, max);
	
	initTerrainIndices();
	calcTerrainNormals();
	
}
