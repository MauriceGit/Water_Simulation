#ifndef __PARTICLE_H__
#define __PARTICLE_H__
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

#define PARTICLE_HEIGHT_INIT	40	
#define PARTICLE_CNT	0
#define PARTICLE_LEN	1.0
#define PARTICLE_M		1.0
#define K_WEAK			1.0
#define K_WEAK_INTERPOL	0.05
#define K_V				1.5
#define GAUSS			10000.0
#define CAMERA_PARTICLE_MULT 2.5

Particle * getParticleList (void);
int getParticleCnt (void);
void initParticles(void);
void calcParticleAttributesEuler (Spheres * spheres, int sphereCnt, double interval,int persecutorParticleMode, int cameraParticlePos);
void addParticle (void); 
void deleteParticle (void);
Particle getParticleAt(int i);

#endif
