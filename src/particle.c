

/**
 * @file
 * Hier ist die Datenhaltung und Programmlogik
 *
 * @author Tilman Nedele, Maurice Tollmien
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#ifdef MACOSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "particle.h"
#include "logic.h"
#include "vector.h"
#include "types.h"

/** Globale Variablen */

/** Liste mit Partikeln */
Particle * G_Particles;
/** Anzahl der Partikel */
int G_ParticleCnt = PARTICLE_CNT;

/** Init-Wert der Länge der Feder ohne Dehnung */
double G_StructureSpringLengthInit;
double G_SheerSpringLengthInit;
double G_BendingSpringLengthInit;


/**
 * Gibt die Liste mit Partikeln zurück
 */
Particle * getParticleList (void)
{
	return G_Particles;
}

/**
 * Getter für ein Partikel an der Position i
 * @return das Partikel
 */
Particle getParticleAt(int i)
{
	return G_Particles[i];
}

/**
 * Gibt die Anzahl der Partikel zurück.
 */
int getParticleCnt (void)
{
	return G_ParticleCnt;
}

/**
 * Setzt die Partikelanzahl
 */
void setParticleCnt (int i)
{
    G_ParticleCnt = i;
}

void calcSpringForce (Particle p1, Particle p2, double initLength, double k, CGVector3D * res)
{
	CGVector3D tmp = {0.0,0.0,0.0};
	double len = 0.0;
	
	/* Tatsächlicher Abstand beider Punkte */
	subtractVectorVector (p1->s, p2->s, &tmp);
	/* Länge des Abstandes */
	len  = vectorLength3D (tmp);
	/* Bildung des Einheitsvektors in die Richtung der Kraft */
	divideVectorScalar (tmp, len, &tmp);
	/* Abweichung der Initialen Länge berechnen */
	len  = len - initLength;
	
	if (abs(len) < EPS_LEN) 
		len = 0.0;
	
	/* Abweichung mit Federkonstante multiplizieren */
	multiplyVectorScalar (tmp, len * -k, &tmp);
	
	if (vectorLength3D(tmp) <= EPS_F) 
		multiplyVectorScalar (tmp, 0, &tmp);
		
	/* NEU! */
	if (vectorLength3D(tmp) > MAX_F) {
		multiplyVectorScalar (tmp, 0.0001, &tmp);
	}
	
		
	multiplyVectorScalar (tmp, -1, &tmp);
		
	(*res)[0] = tmp[0];
	(*res)[1] = tmp[1];
	(*res)[2] = tmp[2];
	
}

void correctConcreteParticleSymmetric (Particle p, Particle p2, double initValue)
{
	CGVector3D tmp = {0.0,0.0,0.0};
	double len, tooMuch;
	double moveLen;
	
	subtractVectorVector (p->s, p2->s, &tmp);	
	len = vectorLength3D (tmp);
	
	tooMuch = 1.0 - (len / initValue);

	/* Grenze überschritten! Feder zu lang oder kurz! */
	if ((double)(abs(tooMuch * 1000.0)/1000.0) > SPRING_EXPANSION_MAX) {
		if (len > initValue) {
			moveLen = initValue * SPRING_EXPANSION_MAX + initValue;
		} else {
			moveLen = -initValue * SPRING_EXPANSION_MAX + initValue;
		}

		/* Wenn der Vektor verkürzt werden soll, ist das Ergebnis positiv! */
		moveLen = (len - moveLen) / 2.0;
			
		/* normierter Richtungsvektor in Richtung des Partikels p */
		divideVectorScalar (tmp, len, &tmp);
		
		/* Vektor zeigt Richtung Partikel p, wenn er verkürzt werden soll! */
		multiplyVectorScalar (tmp, moveLen, &tmp);
		
		addVectorVector (p2->s, tmp, &(p2->s));
		multiplyVectorScalar (tmp, -1, &tmp);
		addVectorVector (p->s, tmp, &p->s);		
	}
	
}
 
void correctConcreteParticleP2 (Particle p, Particle p2, double initValue)
{
	CGVector3D tmp = {0.0,0.0,0.0};
	double len, tooMuch;
	double moveLen;
	
	subtractVectorVector (p->s, p2->s, &tmp);	
	len = vectorLength3D (tmp);
	
	tooMuch = 1.0 - (len / initValue);
	/* Grenze überschritten! Feder zu lang oder kurz! */
	if ((double)(abs(tooMuch * 1000.0)/1000.0) > SPRING_EXPANSION_MAX) {
		if (len > initValue) {
			moveLen = initValue * SPRING_EXPANSION_MAX + initValue;
		} else {
			moveLen = -initValue * SPRING_EXPANSION_MAX + initValue;
		}
		
		/* Wenn der Vektor verkürzt werden soll, ist das Ergebnis positiv! */
		moveLen = len - moveLen;
			
		/* normierter Richtungsvektor in Richtung des Partikels p */
		divideVectorScalar (tmp, len, &tmp);
		
		/* Vektor zeigt Richtung Partikel p, wenn er verkürzt werden soll! */
		multiplyVectorScalar (tmp, moveLen, &tmp);
		
		addVectorVector (p2->s, tmp, &(p2->s));
	}
	
}
 
void correctSpringLengthPosition(Particle p, int i, int j)
{
	int sqrtLen = (int)(sqrt(G_ParticleCnt)+0.5);
	
	/** ================== STRUKTURFEDERN ===================== */
	if (1) {
		/* Untere Feder */
		if (i > 0) {				
			/*Particle p2 = G_Particles[(i-1)*sqrtLen+j];
			correctConcreteParticleSymmetric (p, p2, G_StructureSpringLengthInit);*/
		}
		
		/* Obere Feder */
		if (i < sqrtLen-1) {		
			Particle p2 = G_Particles[(i+1)*sqrtLen+j];		
			
			if ((i == sqrtLen-2 && j == sqrtLen-1) || (i == sqrtLen-2 && j == 0)) {
				correctConcreteParticleP2(p2, p, G_StructureSpringLengthInit);
			} else {
				correctConcreteParticleSymmetric (p, p2, G_StructureSpringLengthInit);
			}
		}
		
		/* Rechte Feder */
		if (j < sqrtLen-1) {
			Particle p2 = G_Particles[i*sqrtLen+(j+1)];		
			correctConcreteParticleSymmetric (p, p2, G_StructureSpringLengthInit);
			/*if (i == sqrtLen-1 && j == sqrtLen-2) {
				correctConcreteParticleP2(p2, p, G_StructureSpringLengthInit);
			} else {
				correctConcreteParticleSymmetric (p, p2, G_StructureSpringLengthInit);
			}*/
		}
		
		/* Linke Feder */
		if (j > 0) {
			Particle p2 = G_Particles[i*sqrtLen+(j-1)];		
			correctConcreteParticleSymmetric (p, p2, G_StructureSpringLengthInit);
			/*if (i == sqrtLen-1 && j == 1) {
				correctConcreteParticleP2(p2, p, G_StructureSpringLengthInit);
			} else {
				correctConcreteParticleSymmetric (p, p2, G_StructureSpringLengthInit);
			}*/
		}
	}
	
	/** ==================== SCHERFEDERN ====================== */	
	
	if (1) {
		
		/* Links oben */
		if (i < sqrtLen-1 && j > 0) {					
			Particle p2 = G_Particles[(i+1)*sqrtLen+(j-1)];
			
			if (i == sqrtLen-2 && j == 1) {
				correctConcreteParticleP2(p2, p, G_SheerSpringLengthInit);	
			} else {
				correctConcreteParticleSymmetric (p, p2, G_SheerSpringLengthInit);
			}
		}
		
		/* Rechts oben */
		if (i < sqrtLen-1 && j < sqrtLen-1) {					
			Particle p2 = G_Particles[(i+1)*sqrtLen+(j+1)];
			
			if (i == sqrtLen-2 && j == sqrtLen-2) {
				correctConcreteParticleP2(p2, p, G_SheerSpringLengthInit);
			} else {
				correctConcreteParticleSymmetric (p, p2, G_SheerSpringLengthInit);
			}
		}
		
		if (i > 0 && j > 0) {					
			Particle p2 = G_Particles[(i-1)*sqrtLen+(j-1)];
			correctConcreteParticleSymmetric (p, p2, G_SheerSpringLengthInit);
		}
		
		if (i > 0 && j < sqrtLen-1) {					
			Particle p2 = G_Particles[(i-1)*sqrtLen+(j+1)];
			correctConcreteParticleSymmetric (p, p2, G_SheerSpringLengthInit);
		}
		
		
	}

}

double calcWindForce (CGVector3D va, CGVector3D vb, CGVector3D vc, CGVector3D wind) {
	CGVector3D a = {0.0,0.0,0.0};
	CGVector3D b = {0.0,0.0,0.0};
	CGVector3D c = {0.0,0.0,0.0};
	CGVector3D norm = {0.0,0.0,0.0};
	double la, lb, lc, s, f;
	
	subtractVectorVector (vb, va, &a);
	subtractVectorVector (vc, vb, &b);
	subtractVectorVector (va, vc, &c);
	
	la = vectorLength3D (a);
	lb = vectorLength3D (b);
	lc = vectorLength3D (c);
	
	s = (la + lb + lc ) / 2.0;
	
	f = sqrt (s * (s-la) * (s-lb) * (s-lc));
	
	crossProduct3D (norm, a, b);
	
	return scalarProduct (norm, wind) * f * WIND_FORCE;
	
}

/**
 * Berechnet mitHilfe von Euler die neuen Attribute der Partikel
 * 
 * @param spheres - Liste aller Kugeln
 * @param sphereCnt - Anzahl aller Kugeln
 * @param interval - Zeit
 * @param persecutorParticleMode - Verfolgermodus
 */
void calcParticleAttributesEuler (Spheres * spheres, int sphereCnt, double interval,int persecutorParticleMode, int cameraParticlePos)
{
	int i,j,k;
	int sqrtLen = (int)(sqrt(G_ParticleCnt)+0.5);
		
	for (i=0;i<sqrtLen;i++) {
		
		for (j=0;j<sqrtLen;j++) {
		
			Particle p = G_Particles[i*sqrtLen +j];
			CGVector3D tmp = {0.0,0.0,0.0};
			CGVector3D fComplete = {0.0,0.0,0.0};
			CGVector3D fInt = {0.0,0.0,0.0};
			CGVector3D fIntNeighbours = {0.0,0.0,0.0};
			CGVector3D fWindForce = {0.0,0.0,0.0};
			CGVector3D fExt = {0.0,G,0.0};
			double factor = 1.0;
			CGVector3D wind = WIND_DIR;
			CGVector3D tmp2 = {0.0,0.0,0.0};
		
		
			/* Tuch an zwei Ecken aufhängen. */
			if (!(i >= sqrtLen-2 /*   || (i == sqrtLen-1 && j == sqrtLen-1) || (i == sqrtLen-1 && j == 0)  */  )) {
				
				
				
				/** ================== STRUKTURFEDERN ===================== */
				
				/* Linke Feder */
				if (i > 0) {					
					calcSpringForce (G_Particles[(i-1)*sqrtLen+j], p, G_StructureSpringLengthInit, K_STRUCTURE, &tmp);	
					addVectorVector (fInt, tmp, &fInt);
					
					divideVectorScalar (G_Particles[(i-1)*sqrtLen+j]->f, 5, &tmp);
					addVectorVector (fIntNeighbours, tmp, &fIntNeighbours);
				}
				
				/* Rechte Feder */
				if (i < sqrtLen-1) {
					calcSpringForce (G_Particles[(i+1)*sqrtLen+j], p, G_StructureSpringLengthInit, K_STRUCTURE, &tmp);
					addVectorVector (fInt, tmp, &fInt);
					
					divideVectorScalar (G_Particles[(i+1)*sqrtLen+j]->f, 5, &tmp);
					addVectorVector (fIntNeighbours, tmp, &fIntNeighbours);
				}
				
				/* Untere Feder */
				if (j > 0) {
					calcSpringForce (G_Particles[i*sqrtLen+(j-1)], p, G_StructureSpringLengthInit, K_STRUCTURE, &tmp);
					addVectorVector (fInt, tmp, &fInt);
					
					divideVectorScalar (G_Particles[i*sqrtLen+(j-1)]->f, 5, &tmp);
					addVectorVector (fIntNeighbours, tmp, &fIntNeighbours);
				}
				
				/* Obere Feder */
				if (j < sqrtLen-1) {
					calcSpringForce (G_Particles[i*sqrtLen+(j+1)], p, G_StructureSpringLengthInit, K_STRUCTURE, &tmp);
					addVectorVector (fInt, tmp, &fInt);
					
					divideVectorScalar (G_Particles[i*sqrtLen+(j+1)]->f, 5, &tmp);
					addVectorVector (fIntNeighbours, tmp, &fIntNeighbours);
				}
				
				/** ==================== SCHERFEDERN ====================== */
				
				/* Links unten */
				if (i > 0 && j > 0) {					
					calcSpringForce (G_Particles[(i-1)*sqrtLen+(j-1)], p, G_SheerSpringLengthInit, K_SHEER, &tmp);		
					addVectorVector (fInt, tmp, &fInt);
				}
				
				if (i > 0 && j < sqrtLen-1) {					
					calcSpringForce (G_Particles[(i-1)*sqrtLen+(j+1)], p, G_SheerSpringLengthInit, K_SHEER, &tmp);		
					addVectorVector (fInt, tmp, &fInt);
				}
				
				if (i < sqrtLen-1 && j > 0) {					
					calcSpringForce (G_Particles[(i+1)*sqrtLen+(j-1)], p, G_SheerSpringLengthInit, K_SHEER, &tmp);
					addVectorVector (fInt, tmp, &fInt);
				}
				
				if (i < sqrtLen-1 && j < sqrtLen-1) {					
					calcSpringForce (G_Particles[(i+1)*sqrtLen+(j+1)], p, G_SheerSpringLengthInit, K_SHEER, &tmp);	
					addVectorVector (fInt, tmp, &fInt);
				}
				
				/** ================== BEUGUNGSFEDERN ===================== */
				
				if (i > 1) {					
					calcSpringForce (G_Particles[(i-2)*sqrtLen+j], p, G_BendingSpringLengthInit, K_BEND, &tmp);	
					addVectorVector (fInt, tmp, &fInt);
				}
				
				if (i < sqrtLen-2) {
					calcSpringForce (G_Particles[(i+2)*sqrtLen+j], p, G_BendingSpringLengthInit, K_BEND, &tmp);
					addVectorVector (fInt, tmp, &fInt);
				}
				
				if (j > 1) {
					calcSpringForce (G_Particles[i*sqrtLen+(j-2)], p, G_BendingSpringLengthInit, K_BEND, &tmp);
					addVectorVector (fInt, tmp, &fInt);
				}
				
				if (j < sqrtLen-2) {
					calcSpringForce (G_Particles[i*sqrtLen+(j+2)], p, G_BendingSpringLengthInit, K_BEND, &tmp);
					addVectorVector (fInt, tmp, &fInt);
				}
				
				/*divideVectorScalar (fInt, 5, &fInt);
				addVectorVector (fInt, fIntNeighbours, &fInt);*/
				
				if (vectorLength3D(fInt) <= F_MIN) {
					multiplyVectorScalar (fInt, 0, &fInt);
				}
				
				p->f[0] = fInt[0];
				p->f[1] = fInt[1];
				p->f[2] = fInt[2];
				
				/* Kollision mit Wind :-) */						
				if (i == 0 || i == sqrtLen-1) {
					factor *= WIND_EDGE_FACTOR;
				}
				
				if (j == 0 || j == sqrtLen-1) {
					factor *= WIND_EDGE_FACTOR;
				}
					
				if (i > 0) {
					if (j > 0) {
						/* 8 */
						multiplyVectorScalar (wind, calcWindForce(G_Particles[(i)*sqrtLen+(j)]->s, G_Particles[(i)*sqrtLen+(j-1)]->s, G_Particles[(i-1)*sqrtLen+(j-1)]->s, wind), &tmp2);
						addVectorVector (fWindForce, tmp2, &fWindForce);
						/* 1 */
						multiplyVectorScalar (wind, calcWindForce(G_Particles[(i)*sqrtLen+(j)]->s, G_Particles[(i-1)*sqrtLen+(j-1)]->s, G_Particles[(i-1)*sqrtLen+(j)]->s, wind), &tmp2);
						addVectorVector (fWindForce, tmp2, &fWindForce);
					}
					if (j < sqrtLen-1) {
						/* 2 */
						multiplyVectorScalar (wind, calcWindForce(G_Particles[(i)*sqrtLen+(j)]->s, G_Particles[(i-1)*sqrtLen+(j)]->s, G_Particles[(i-1)*sqrtLen+(j+1)]->s, wind), &tmp2);
						addVectorVector (fWindForce, tmp2, &fWindForce);
						/* 3 */
						multiplyVectorScalar (wind, calcWindForce(G_Particles[(i)*sqrtLen+(j)]->s, G_Particles[(i-1)*sqrtLen+(j+1)]->s, G_Particles[(i)*sqrtLen+(j+1)]->s, wind), &tmp2);
						addVectorVector (fWindForce, tmp2, &fWindForce);
					}
				}
				
				if (i < sqrtLen-1) {
					if (j < sqrtLen-1) {
						/* 4 */
						multiplyVectorScalar (wind, calcWindForce(G_Particles[(i)*sqrtLen+(j)]->s, G_Particles[(i)*sqrtLen+(j+1)]->s, G_Particles[(i+1)*sqrtLen+(j+1)]->s, wind), &tmp2);
						addVectorVector (fWindForce, tmp2, &fWindForce);
						/* 5 */
						multiplyVectorScalar (wind, calcWindForce(G_Particles[(i)*sqrtLen+(j)]->s, G_Particles[(i+1)*sqrtLen+(j+1)]->s, G_Particles[(i+1)*sqrtLen+(j)]->s, wind), &tmp2);
						addVectorVector (fWindForce, tmp2, &fWindForce);
					}
					if (j > 0) {
						/* 6 */
						multiplyVectorScalar (wind, calcWindForce(G_Particles[(i)*sqrtLen+(j)]->s, G_Particles[(i+1)*sqrtLen+(j)]->s, G_Particles[(i+1)*sqrtLen+(j-1)]->s, wind), &tmp2);
						addVectorVector (fWindForce, tmp2, &fWindForce);
						/* 7 */
						multiplyVectorScalar (wind, calcWindForce(G_Particles[(i)*sqrtLen+(j)]->s, G_Particles[(i+1)*sqrtLen+(j-1)]->s, G_Particles[(i)*sqrtLen+(j-1)]->s, wind), &tmp2);
						addVectorVector (fWindForce, tmp2, &fWindForce);
					}
				}
				
				multiplyVectorScalar (fWindForce, factor, &fWindForce);
				
				/* Kräfte */
				addVectorVector (fInt, fExt, &fComplete);
				addVectorVector (fComplete, fWindForce, &fComplete);
				
				/* Beschleunigung */
				p->a[0] = (-LAMDA * p->v[0] + fComplete[0]) / p->m;
				p->a[1] = (-LAMDA * p->v[1] + fComplete[1]) / p->m;
				p->a[2] = (-LAMDA * p->v[2] + fComplete[2]) / p->m;
				
				/* Geschwindigkeit */
				multiplyVectorScalar (p->a, interval, &tmp);
				addVectorVector (p->v, tmp, &p->v);
				
				if (vectorLength3D(p->v) > MAX_V) {
					/*printf ("V = %f\n", vectorLength3D(p->v));*/
					divideVectorScalar (p->v, vectorLength3D(p->v), &(p->v));
					multiplyVectorScalar(p->v, MAX_V, &(p->v));
				}
				
				/* Ort */
				multiplyVectorScalar (p->v, interval, &tmp);
				addVectorVector (p->s, tmp, &p->s);
				
				/* Up */
				crossProduct3D (tmp, p->v, p->a);
				crossProduct3D (tmp, tmp, p->v);
				
				if (vectorLength3D(tmp) > 0.1)
				{
					p->up[0] = tmp[0];
					p->up[1] = tmp[1];
					p->up[2] = tmp[2];
				}
				
				/* Eventuelle Anpassung der Entfernung zwischen Partikeln, wenn die Feder zu lang oder zu klein wird! */
			
				correctSpringLengthPosition(p, i, j);
				
				/* Kollision mit Boden */
				if (p->s[1] <= -WORLD_SIZE/2.0 + 0.5) {
					p->s[1] = -WORLD_SIZE/2.0 + 0.5;
					multiplyVectorScalar(p->v, 0, &(p->v));
				}
				
				/* Kollision mit Kugel(n) */
				for (k=0;k<SPHERE_CNT;k++) {
					Spheres s = getSphereList()[k];
					double dist = 0.0;
					
					subtractVectorVector(p->s, s->pos, &tmp);
					dist = vectorLength3D(tmp);
					
					if (dist < SPHERE_R+4.5) {
						divideVectorScalar(tmp, dist, &tmp);
						
						multiplyVectorScalar(tmp, SPHERE_R-dist+4.5, &tmp);
						
						addVectorVector(p->s, tmp, &(p->s));
						p->v[0] = 0;
						p->v[1] = 0;
						p->v[2] = 0;
						
					}
					
				}
				
				/* Kollision mit Quad(s) */
				for (k=0;k<QUAD_CNT;k++) {
					Quads s = getQuadList()[k];
					
					if (	p->s[1] <= s->pos[1]+QUAD_SIDE/2+2.5
						&&	p->s[0] <= s->pos[0]+QUAD_SIDE/2+2.5
						&& 	p->s[0] >= s->pos[0]-QUAD_SIDE/2-2.5
						&& 	p->s[2] <= s->pos[2]+QUAD_SIDE/2+2.5
						&& 	p->s[2] >= s->pos[2]-QUAD_SIDE/2-2.5
						) {
							
						p->s[1] = s->pos[1]+QUAD_SIDE/2+2.5;
						multiplyVectorScalar(p->v, 0, &(p->v));
					}	
				}
				
			} else {
				Particle p2 = G_Particles[(i-1)*sqrtLen+j];		
						
				correctConcreteParticleP2(p, p2, G_StructureSpringLengthInit);
				
			}
			
		}
        
	}
	
}


/**
 * Fügt ein Partikel der Liste hinzu.
 */
void addParticle (void)
{
    Particle p = malloc (sizeof(**G_Particles));
    
    G_Particles = realloc (G_Particles, sizeof(*G_Particles) * ++G_ParticleCnt);
    
    p->s[0] = rand()%(int)(5*WORLD_SIZE)/20.0 - rand()%(int)(5*WORLD_SIZE)/20.0;
    p->s[1] = rand()%(int)(5*WORLD_SIZE)/20.0 - rand()%(int)(5*WORLD_SIZE)/20.0;
    p->s[2] = rand()%(int)(5*WORLD_SIZE)/20.0 - rand()%(int)(5*WORLD_SIZE)/20.0;
    
    p->v[0] = 0.1;
    p->v[1] = 0.0;
    p->v[2] = 0.0;
    
    p->a[0] = 0.0;
    p->a[1] = 0.0;
    p->a[2] = 0.0;
    
    p->up[0] = 0.0;
    p->up[1] = 0.0;
    p->up[2] = 0.0;
    
    G_Particles[G_ParticleCnt-1] = p;
}

/**
 * Löscht das letzte Partikel
 */
 void deleteParticle (void)
 {
    if (G_ParticleCnt >= 1) 
    {
        free(G_Particles[G_ParticleCnt-1]);
        G_Particles = realloc (G_Particles, sizeof(*G_Particles) * --G_ParticleCnt);
    }
 }

/**
 * Initialisiert die Partikel.
 */
void initParticles(void)
{
	int i,j;
	G_Particles = malloc (sizeof(*G_Particles) * G_ParticleCnt * G_ParticleCnt);
	
	for (i=0;i<(int)(sqrt(G_ParticleCnt)+0.5);i++) {
		for (j=0;j<(int)(sqrt(G_ParticleCnt)+0.5);j++) {
			
			Particle p = malloc (sizeof(**G_Particles));
			
			p->s[0] = PARTICLE_HEIGHT_INIT ;
			p->s[1] = i/((sqrt(G_ParticleCnt)))*WORLD_SIZE - WORLD_SIZE/2+0;
			p->s[2] = j/((sqrt(G_ParticleCnt)))*WORLD_SIZE - WORLD_SIZE/2;
			
			/*p->s[0] = i/((sqrt(G_ParticleCnt)))*WORLD_SIZE - WORLD_SIZE/2;
			p->s[1] = PARTICLE_HEIGHT_INIT;
			p->s[2] = j/((sqrt(G_ParticleCnt)))*WORLD_SIZE - WORLD_SIZE/2;*/
			
			p->v[0] = 0.0;
			p->v[1] = 0.0;
			p->v[2] = 0.0;
			
			p->a[0] = 0.0;
			p->a[1] = 0.0;
			p->a[2] = 0.0;
			
			p->up[0] = 0.0;
			p->up[1] = 0.0;
			p->up[2] = 0.0;
			
			p->m = PARTICLE_M;
			p->f[0] = 0.0;
			p->f[1] = 0.0;
			p->f[2] = 0.0;
			
			G_Particles[i*(int)(sqrt(G_ParticleCnt)+0.5) +j] = p;
		}
	}
	
	G_StructureSpringLengthInit = abs((i+1)/((sqrt(G_ParticleCnt)))*WORLD_SIZE - i/((sqrt(G_ParticleCnt)))*WORLD_SIZE);
	G_SheerSpringLengthInit = sqrt(pow(G_StructureSpringLengthInit, 2) + pow(G_StructureSpringLengthInit, 2));
	G_BendingSpringLengthInit = 2 * G_StructureSpringLengthInit;
	
	printf ("Strukturfeder-Länge: %f, Scherfeder-Länge: %f, Beugungsfeder-Länge: %f\n", G_StructureSpringLengthInit, G_SheerSpringLengthInit, G_BendingSpringLengthInit);
}




















