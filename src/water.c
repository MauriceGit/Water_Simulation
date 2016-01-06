
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
/* ---- Eigene Header einbinden ---- */
#include "water.h"
#include "particle.h"
#include "logic.h"
#include "vector.h"
#include "types.h"
#include "terrain.h"

Water * G_WaterList;
Water * G_TmpWaterList;
float ** G_V;

GLuint * G_WaterIndices;

int normalList = 1;
int G_IsMoving = 1;
int G_randomize = 0;

Water * getWaterList(void){
	return G_WaterList;
}

void calcWaterAttributes (double interval) {
	
	int i,j;
	double f;
	
	if (G_IsMoving) {
		
		for (i=0;i<WORLD_SIZE*WORLD_SIZE;i++) {		
			int atI = i % WORLD_SIZE;
			int atJ = i / WORLD_SIZE;
			
			if (0 && G_WaterList[i].fix && fabs(G_WaterList[i].y - WATER_HEIGHT_INIT) > 0.1) {
				printf ("Wasser: fix=%d, Y now=%f, Y init=%f\n", G_WaterList[i].fix, G_WaterList[i].y, (double)WATER_HEIGHT_INIT);
			}
			
			if (atI == 0 || atI == WORLD_SIZE-1 || atJ == 0 || atJ == WORLD_SIZE-1
				) {
				continue;
			}
			
			f = WATER_SPEED * WATER_SPEED * (	G_WaterList[i - WORLD_SIZE].y + 
												G_WaterList[i + WORLD_SIZE].y + 
												G_WaterList[i + 1].y + 
												G_WaterList[i - 1].y - 4 *
												G_WaterList[i].y
											) / WATER_WIDTH;
			
			G_V[atI][atJ] += f * interval;
			
			if (!G_WaterList[i].fix)
				G_TmpWaterList[i].y = G_WaterList[i].y + G_V[atI][atJ] * interval;
			else
				G_TmpWaterList[i].y = WATER_HEIGHT_INIT;
				
			if (G_randomize) {
				G_TmpWaterList[i].y = WATER_HEIGHT_INIT + WATER_DISTORTION - WATER_DISTORTION;
				G_WaterList[i].y = WATER_HEIGHT_INIT + WATER_DISTORTION - WATER_DISTORTION;
			}
			
			
			CGVector3D v1 = {G_WaterList[i-WORLD_SIZE].x - G_WaterList[i].x, G_WaterList[i-WORLD_SIZE].y - G_WaterList[i].y, G_WaterList[i-WORLD_SIZE].z - G_WaterList[i].z};
			CGVector3D v2 = {G_WaterList[i-1].x - G_WaterList[i].x, G_WaterList[i-1].y - G_WaterList[i].y, G_WaterList[i-1].z - G_WaterList[i].z};
			CGVector3D res1;
			crossProduct3D(res1, v1, v2);
			
			CGVector3D v3 = {G_WaterList[i+WORLD_SIZE].x - G_WaterList[i].x, G_WaterList[i+WORLD_SIZE].y - G_WaterList[i].y, G_WaterList[i+WORLD_SIZE].z - G_WaterList[i].z};
			CGVector3D v4 = {G_WaterList[i+1].x - G_WaterList[i].x, G_WaterList[i+1].y - G_WaterList[i].y, G_WaterList[i+1].z - G_WaterList[i].z};
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
			
			G_TmpWaterList[i].nx = normal[0];
			G_TmpWaterList[i].ny = normal[1];
			G_TmpWaterList[i].nz = normal[2];
			
			/* Caustics durch die Normalen des Wasser auf der Bodenfläche. */
			int waterTerrainOffset = (TERRAIN_SIZE - WORLD_SIZE)/2;
			Terrain * terrainCaustics = getTerrainCaustics();
			/*terrainCaustics[(atI + waterTerrainOffset) * TERRAIN_SIZE + (atJ + waterTerrainOffset)].nx = normal[0]; 
			terrainCaustics[(atI + waterTerrainOffset) * TERRAIN_SIZE + (atJ + waterTerrainOffset)].ny = normal[1]; 
			terrainCaustics[(atI + waterTerrainOffset) * TERRAIN_SIZE + (atJ + waterTerrainOffset)].nz = normal[2]; */
			
		}
		
		Water * tmp;
		tmp = G_TmpWaterList;
		G_TmpWaterList = G_WaterList;
		G_WaterList = tmp;
	}
	
	G_randomize = 0;
	
}

/**
 * Guckt, wo/ob Wasserpunkte einen Mindestabstand zu Landpunkten unterschreiten
 * und setzt diese auf fix, sodass quasi ein Wasserrand entsteht.
 */
void calcFixWaterPoints(Terrain * terrain) 
{
	int w,t;
	double eps = 1.0;
	
	for (t = 0; t < TERRAIN_SIZE*TERRAIN_SIZE; t++) {
		for (w = 0; w < WORLD_SIZE*WORLD_SIZE; w++) {
			
			if (fabs(terrain[t].x - G_WaterList[w].x) < eps && 
				fabs(terrain[t].y - G_WaterList[w].y) < eps && 
				fabs(terrain[t].z - G_WaterList[w].z) < eps
				) {
				G_WaterList[w].fix	= 1;
				G_WaterList[w].y 	= WATER_HEIGHT_INIT;
			}
		}
	}
}

void setRandomize(void) {
	G_randomize = 1;
}

void pushSomeWaterDown(void)
{
	
	int i,j;
	
	for (i=WORLD_SIZE/2-4;i<WORLD_SIZE/2+5;i++) {
		for (j=WORLD_SIZE/2-4;j<WORLD_SIZE/2+5;j++) {
			if (!G_WaterList[i*WORLD_SIZE+j].fix)
				G_WaterList[i*WORLD_SIZE+j].y -= 5.0;			
		}
	}
	
}

void toggleWaterMovement(void) 
{
	G_IsMoving = !G_IsMoving;
}

GLuint * getWaterIndices (void) 
{
	return G_WaterIndices;
}

void initWaterIndices(void) {
	
	int i,j;
	int index = 0;
	for (i=0;i<WORLD_SIZE*WORLD_SIZE;i++) {		
		int atI = i % WORLD_SIZE;
		int atJ = i / WORLD_SIZE;
		
		if (atI == WORLD_SIZE-1 || atJ == WORLD_SIZE-1) {
			continue;
		}
		
		G_WaterIndices[index]   = i;
		G_WaterIndices[index+1] = i+1;
		G_WaterIndices[index+2] = i+WORLD_SIZE+1;
		
		G_WaterIndices[index+3] = i;
		G_WaterIndices[index+4] = i+WORLD_SIZE+1;
		G_WaterIndices[index+5] = i+WORLD_SIZE;
		
		index += 6;			
	}
	
}

void initWater(void) {
	
	int i,j;
	
	G_V = calloc(WORLD_SIZE, sizeof(*G_V));
	G_WaterList = calloc(WORLD_SIZE*WORLD_SIZE, sizeof(*G_WaterList));
	G_TmpWaterList = calloc(WORLD_SIZE*WORLD_SIZE, sizeof(*G_TmpWaterList));
	G_WaterIndices = calloc(WORLD_SIZE*WORLD_SIZE*2*3, sizeof(*G_WaterIndices));
	
	for (i=0;i<WORLD_SIZE;i++) {
		G_V[i] = calloc(WORLD_SIZE, sizeof(**G_V));
	}
	
	for (i=0;i<WORLD_SIZE*WORLD_SIZE;i++) {
		int xCoord = i / WORLD_SIZE;
		int zCoord = i % WORLD_SIZE;
		
		G_WaterList[i].x = xCoord - WORLD_SIZE/2;
		G_WaterList[i].y = WATER_HEIGHT_INIT /*+ WATER_DISTORTION - WATER_DISTORTION*/;
		G_WaterList[i].z = zCoord - WORLD_SIZE/2;
		
		G_WaterList[i].s = (float)xCoord / (float)WORLD_SIZE;
		G_WaterList[i].t = (float)zCoord / (float)WORLD_SIZE;
		
		G_WaterList[i].nx = 0.0;
		G_WaterList[i].ny = 1.0;
		G_WaterList[i].nz = 0.0;
		
		G_WaterList[i].fix = 0;
		
		G_TmpWaterList[i].x = G_WaterList[i].x;
		G_TmpWaterList[i].y = G_WaterList[i].y;
		G_TmpWaterList[i].z = G_WaterList[i].z;
		
		G_TmpWaterList[i].s = G_WaterList[i].s;
		G_TmpWaterList[i].t = G_WaterList[i].t;		
		
		G_TmpWaterList[i].nx = G_WaterList[i].nx;		
		G_TmpWaterList[i].ny = G_WaterList[i].ny;		
		G_TmpWaterList[i].nz = G_WaterList[i].nz;	
		
		G_TmpWaterList[i].fix = 0;	
		
		G_V[xCoord][zCoord] = 0.0;
		
		
	}
	
	initWaterIndices();	
}
