#ifndef __TERRAIN_H__
#define __TERRAIN_H__
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

/* Maximale Ausdehnung (Mittig!) */
#define TERRAIN_SIZE	200
/* Wie groß das Noise abgebildet werden soll */
#define TERRAIN_DIMENSION	18.0
#define GAUSS_DIMENSION		10.0

#define TERRAIN_DISTANCE	1.0

#define PERSISTENCE		0.25
#define OCTAVES_COUNT	32

Terrain * getTerrainList(void);
void initTerrain(void);
Terrain * getTerrainCaustics(void);
GLuint * getTerrainIndices (void);

#endif

