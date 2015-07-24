#ifndef __WATER_H__
#define __WATER_H__
#define __DEBUG_GL_H__
/**
 * @file
 * Programmlogik und Datenhaltung
 *
 * @author Maurice Tollmien
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef MACOSX
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdarg.h>

#include "types.h"
#include "terrain.h"

#define WATER_HEIGHT_INIT	0
#define WATER_SPEED 		3
#define WATER_WIDTH			1
#define WATER_DISTORTION	(rand()%1000) / 4000.0

Water * getWaterList(void);
void initWater(void);
void calcWaterAttributes (double interval);
void pushSomeWaterDown(void);
void toggleWaterMovement(void);
GLuint * getWaterIndices (void);
void calcFixWaterPoints(Terrain * terrain);
void setRandomize(void);

#endif

