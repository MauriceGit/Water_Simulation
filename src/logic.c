
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
#include <stringOutput.h>
#include <assert.h>

#ifdef MACOSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "vector.h"
#include "scene.h"
#include "types.h"
#include "water.h"
#include "terrain.h"

/* Globale Variablen */
/** Licht an/aus */
InitType G_Light = ON;
/** Hilfe an/aus */
InitType G_Help  = OFF;
/** Textur an/aus */
InitType G_Texture = ON;
/** Mausbewegung zoom/move/none */
MouseInterpretType G_Mouse;
/** Kameraposition */
CGVector3D G_CameraPosition = {CAMERA_X,CAMERA_Y,CAMERA_Z};
/** Position der Maus */
Movement G_MouseMove = {0.0,0.0,0.0};
/** Startpunkt der Mausbewegung */
CGVector3D G_LastMouseCenter = {0.0,0.0,0.0};
/** Anzahl der Quadrate für den Boden */
int G_QuadCount = 0;
/** Welche Textur soll angezeigt werden */
int G_TextureCount = 5;
/** Liste aller Kugeln */
Spheres * G_Spheres;

int G_BallMoving = 0;

Quads * G_Quads;
/** Sollen sich die Kugeln bewegen? */
int G_SpheresMoving = 0;
/** Ob die Partikel neu berechnet werden */
int G_ParticleFreeze = 0;
/** Wie sollen die Partikel gezeichnet werden? */
ParticleType G_ParticleType = line;
/** Schatten zeichnen? */
int G_DrawShadows = 0;
/** Partikel hinzufügen? */
int G_UpDownKeys[2] = {0,0};
/** Kamer auf einem Partikel? */
int G_ParticleCamera = 0;
/** Position des Partikel in der Liste mit der Kamera */
int G_CameraParticlePos = 0;
/** Verfolgermodus? */
int G_PersecutorParticle = 0;
/** UP-Vektor anzeigen? */
int G_ShowUpVector = 0;
/** FPS */
int G_FPS = 0;
/** FPS counter */
double G_Counter = 0.0-EPS;

/* ------- GETTER / SETTER ------- */

int getFPS(void)
{
	return G_FPS;
}

/**
 * Setzt die Ansicht des Up-Vektors
 */
void toggleShowUpVector (void)
{
	G_ShowUpVector = !G_ShowUpVector;
}

/**
 * Getter für Ansicht des Up-Vektors
 * @return 0 / 1
 */
int getShowUpVector (void)
{
	return G_ShowUpVector;
}

/**
 * Getter für die Position des Partikel mit der Kamera
 */
int getCameraParticlePos() {
    return G_CameraParticlePos;
}

/**
 * Ändert den Verfolger-Modus
 */
void togglePersecutorParticle (void)
{
    G_PersecutorParticle = !G_PersecutorParticle;
}

/**
 * Ändert den Kameramodus.
 */
void toggleParticleCamera (void)
{
    G_ParticleCamera = !G_ParticleCamera;
}

/**
 * Ob der Verfolgermodus gesetzt ist
 */
int getParticleCamera (void)
{
    return G_ParticleCamera;
}

void startMovingBall(void)
{
	G_BallMoving = G_BallMoving == 0 ? 1 : 0;
}

/**
 * Setzt, ob ein Up/Down-Key gedrückt ist.
 * key == 0 = down
 * key == 1 = up
 * value = gesetzt/nicht gesetzt.
 */
void setKey (int key, int value)
{
    if (key < 2)
        G_UpDownKeys[key] = value;
}

/**
 * Ob Schatten gezeichnet werden sollen, oder nicht.
 */
void toggleShadows(void)
{
	G_DrawShadows = !G_DrawShadows;
}

/**
 * Ob Schatten gezeichnet werden sollen, oder nicht.
 */
int getDrawShadows(void)
{
	return G_DrawShadows;
}

/**
 * Ändert den Typ der Partikel
 */
void toggleParticleType (void)
{
	if (G_ParticleType + 1 > sphere)
		G_ParticleType = 0;
	else 
		G_ParticleType++;
}

/**
 * Welcher Typ die Partikel sind
 */
ParticleType getParticleType (void)
{
	return G_ParticleType;
}

/**
 * Ob die Partikel sich bewegen oder nicht.
 */
void toggleParticleFreeze (void)
{
	G_ParticleFreeze = !G_ParticleFreeze;
	if (G_SpheresMoving)
		G_SpheresMoving = 0;
}

/**
 * Gibt eine Sphere zurück.
 */
Spheres * getSphereList (void) 
{
	return G_Spheres;
}

/**
 * Gibt eine Sphere zurück.
 */
Quads * getQuadList (void) 
{
	return G_Quads;
}

/**
 * Ändert den Wert des Boolean, ob sich die Kugeln bewegen dürfen.
 */
void toggleSpheresMoving (void)
{
	G_SpheresMoving = !G_SpheresMoving;
}

/**
 * Get-Function für den Status des Lichts
 * @return Status des Lichts
 */
InitType getLightStatus() {
    return G_Light;
}

/**
 * Set-Function für den Status des Lichts
 * @param Status des Lichts
 */
void setLightStatus(InitType state) {
    G_Light = state;
}

void toggleLight (void)
{
	G_Light = !G_Light;
}

/**
 * Get-Function für den Status der Hilfe
 * @return Status der Hilfe
 */
InitType getHelpStatus() {
    return G_Help;
}

/**
 * Set-Function für den Status der Hilfe
 * @param Status der Hilfe
 */
void setHelpStatus(InitType state) {
    G_Help = state;
}

/**
 * Get-Function für den Status der Texturen
 * @return Status der Texturen
 */
InitType getTextureStatus() {
    return G_Texture;
}

/**
 * Set-Function für den Status der Textur
 * @param Status der Textur
 */
void setTextureStatus(InitType state) {
    G_Texture = state;
}

/**
 * Set-Function für den Status der Maus
 * @param Status der Maus
 */
void setMouseEvent(MouseInterpretType state,int x, int y) {
    G_MouseMove[0] = 0.0;
    G_MouseMove[2] = 0.0;
    G_LastMouseCenter[0] = x;
    G_LastMouseCenter[2] = y;
    
    
    G_Mouse = state;
}

/**
 * Get-Function für den Status der Maus
 * @return Status der Maus
 */
MouseInterpretType getMouseEvent() {
    return G_Mouse;
}

/**
 * Gibt die Kamerakoordinate zurück.
 */
double getCameraPosition (int axis)
{
    if (axis >= 0 && axis < 3)
        return G_CameraPosition[axis];
        
    return 0.0;
}



/**
 * Erhöht die Größe der Unterteilungen um den Faktor 2
 */
void incQuadCount() {
    if (G_QuadCount < 9)
        ++G_QuadCount;
}

/**
 * Verkleinert die Größe der Unterteilungen um den Divisor 2
 */
void decQuadCount() {
    if (G_QuadCount > 0)
        --G_QuadCount;
}

/**
 * Rekursionstiefe für die Bodenfläche
 */
int getQuadCount() {
    return G_QuadCount;
}

/**
 * Bewegt die Kamera auf einem Kugelradius um den Szenenmittelpunkt.
 */
void setCameraMovement(int x,int y)
{
	CGVector3D tmp;
	double factor, radius = vectorLength3D(G_CameraPosition);
	
	tmp[0] = G_CameraPosition[0];
	tmp[1] = G_CameraPosition[1];
	tmp[2] = G_CameraPosition[2];
	
	G_MouseMove[0] = x-G_LastMouseCenter[0];
	G_MouseMove[2] = y-G_LastMouseCenter[2];
	
	G_LastMouseCenter[0] = x;
	G_LastMouseCenter[2] = y;
	
	/* Bewegung um Y-Achse: */
	G_CameraPosition[0] = cos(-G_MouseMove[0]*PI/180.0/CAMERA_MOVEMENT_SPEED)*tmp[0] + sin(-G_MouseMove[0]*PI/180.0/CAMERA_MOVEMENT_SPEED)*tmp[2];
	G_CameraPosition[2] = -sin(-G_MouseMove[0]*PI/180.0/CAMERA_MOVEMENT_SPEED)*tmp[0] + cos(-G_MouseMove[0]*PI/180.0/CAMERA_MOVEMENT_SPEED)*tmp[2];
	
	/* Bewegung oben/unten */
	G_CameraPosition[1] += G_MouseMove[2]/(CAMERA_MOVEMENT_SPEED/2)*(vectorLength3D(G_CameraPosition)/100.0);
	factor = 1.0 / (vectorLength3D(G_CameraPosition) / radius);
	multiplyVectorScalar (G_CameraPosition, factor, &G_CameraPosition);
		
}

/**
 * Verlängert/verkürzt den Vektor zur Kamera.
 */
void setCameraZoom(int x,int y)
{
	double factor = 1.0 + (CAMERA_ZOOM_SPEED / 1000.0) * ((G_MouseMove[2] < 0.0) ? -1.0 : 1.0);
	
	G_MouseMove[0] = x-G_LastMouseCenter[0];
	G_MouseMove[2] = y-G_LastMouseCenter[2];
	
	G_LastMouseCenter[0] = x;
	G_LastMouseCenter[2] = y;
	
	G_CameraPosition[0] *= factor;
	G_CameraPosition[1] *= factor;
	G_CameraPosition[2] *= factor;
}

/* ------- BERECHNUNGEN ------- */

/**
 * Berechnet die Bewegung der Kugeln
 * @param interval - Zeit
 */
void moveSpheres (double interval)
{
	int i;
	
	for (i=0;i<SPHERE_CNT && G_BallMoving;i++)
	{
		Spheres s = getSphereList()[i];
	
		s->pos[0] += SPHERE_MOVE_SPEED*interval;
		s->pos[1] += 0.0;
		s->pos[2] += 0.0;
		
	}
}

/**
 * Berechnet alle Funktionen, die vom interval abhängig sind
 * @param interval - Zeit
 */
void calcTimeRelatedStuff (double interval)
{
    moveSpheres(interval);
	
	if (!G_ParticleFreeze)
    {
        calcWaterAttributes(interval);
    }
    
    /* Keine Partikel hinzufügen oder löschen! */
    /*
    if (G_UpDownKeys[0])
    {
		for (i=0;i<100;i++) {
			deleteParticle ();
		}
    } else if (G_UpDownKeys[1]) {
		for (i=0;i<100;i++) {
			addParticle ();
		}
    }
    */
    
    G_Counter += interval;
    
    if (G_Counter >= 1.0)
		G_Counter = 0.0 -EPS;
    
    if (G_Counter <= 0.0)
		G_FPS = (int) 1.0/interval;
}

/**
 * Berechnet anhand der Rekursionstiefe die Anzahl der Quadrate auf dem Boden
 * @param p1,p2,p3,p4 - Die Ecken des Quadrats
 * @param rek - Rekursionstiefe
 * @param color - die Farbe der Quadrate
 * 
 */
void drawRecursiveQuad(CGPoint3f p1, CGPoint3f p2, CGPoint3f p3, CGPoint3f p4, int rek, int color)
{
    CGPoint3f middle0 = {0.0,0.0,0.0},
              middle1 = {0.0,0.0,0.0},
              middle2 = {0.0,0.0,0.0},
              middle3 = {0.0,0.0,0.0},
              center = {0.0,0.0,0.0};

    center[0] = (p1[0] + p3[0]) / 2;
    center[2] = (p1[2] + p3[2]) / 2;

    middle0[0] = (p1[0] + p2[0]) / 2;
    middle0[2] = (p1[2] + p2[2]) / 2;

    middle1[0] = (p2[0] + p3[0]) / 2;
    middle1[2] = (p2[2] + p3[2]) / 2;

    middle2[0] = (p3[0] + p4[0]) / 2;
    middle2[2] = (p3[2] + p4[2]) / 2;


    middle3[0] = (p4[0] + p1[0]) / 2;
    middle3[2] = (p4[2] + p1[2]) / 2;

    if (rek > 0)
    {
      drawRecursiveQuad(center,middle0,p2,middle1, rek-1, color);
      drawRecursiveQuad(center,middle1,p3,middle2, rek-1, !color);
      drawRecursiveQuad(center,middle2,p4,middle3, rek-1, color);
      drawRecursiveQuad(center,middle3,p1,middle0, rek-1, !color);


    } else {
        drawVertex(p4,p3,p2,p1, color);
    }
}

/* ------- INIT ------- */

/**
 * Initialisiert die Kamera.
 */
void initCameraPosition ()
{
    G_CameraPosition[0] = CAMERA_X;
    G_CameraPosition[1] = CAMERA_Y;
    G_CameraPosition[2] = CAMERA_Z;
}

/**
 * Initialisiert die Kugeln.
 */
void initSpheres(void)
{
	int i;
	G_Spheres = malloc (sizeof(*G_Spheres) * SPHERE_CNT);
	
	for (i=0;i<SPHERE_CNT;i++) 
	{
		
		Spheres sphere = malloc (sizeof(**G_Spheres));
		
        sphere->interpolatedPoints = NULL;
        sphere->interpolatedPointsAll = NULL;
        sphere->controlPoints = NULL;
        sphere->t = 0.0;
        
		sphere->pos[0] = rand() % (int)WORLD_SIZE/2 - rand() % (int)WORLD_SIZE/2+150;
		sphere->pos[1] = GROUND_HEIGHT-WORLD_SIZE/2.0+0.1 + SPHERE_R + 10;
		sphere->pos[2] = rand() % (int)WORLD_SIZE/2 - rand() % (int)WORLD_SIZE/2;
		
		sphere->newPos[0] = 0.0;
		sphere->newPos[1] = 0.0;
		sphere->newPos[2] = 0.0;
		
		sphere->color[0] = rand()%255;
		sphere->color[1] = rand()%255;
		sphere->color[2] = rand()%255;
		
		G_Spheres[i] = sphere;
	}
}

/**
 * Initialisiert die Quads.
 */
void initQuads(void)
{
	int i;
	G_Quads = malloc (sizeof(*G_Quads) * QUAD_CNT);
	
	for (i=0;i<QUAD_CNT;i++) 
	{
		
		Quads quads = malloc (sizeof(**G_Quads));
		
		quads->pos[0] = -40;
		quads->pos[1] = GROUND_HEIGHT-WORLD_SIZE/2.0+0.1+QUAD_SIDE/2 ;
		quads->pos[2] = -40;
		
		G_Quads[i] = quads;
	}
}

/**
 * Hier findet die komplette Initialisierung des kompletten SPIEeles statt.
 * Inklusive der Datenhaltung und des SPIEelfeldes.
 */
void initGame ()
{   
    initCameraPosition();
    initTerrain();
    initWater();
    calcFixWaterPoints(getTerrainList());
    /*setRandomize();*/
}

