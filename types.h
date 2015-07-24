#ifndef __TYPES_H__
#define __TYPES_H__

/**
 * @file
 * Typenschnittstelle.
 * Das Modul kapselt die Typdefinitionen und globalen Konstanten des Programms.
 *
 * @author Maurice Tollmien
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef MACOSX
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

/* ---- Eigene Konstanten */

#define REFRACTION		30

#define GROUND_HEIGHT	0.3

#define CAMERA_X        -50.1
#define CAMERA_Y        50.0
#define CAMERA_Z        75.0
#define CAMERA_SPEED    15.0

#define CAMERA_MOVEMENT_SPEED	4.0
#define CAMERA_ZOOM_SPEED 10.0

#define COUNTOWNTEX 2
#define WORLD_SIZE 150

#define SLICE_CNT	50

#define E			2.71828183
#define PI             3.14159265  
#define EPS		0.0001

/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS      120

/** Reihen des Images */
#define IMAGE_ROWS  128
#define IMAGE_COLS  128

#define SPHERE_MOVE_SPEED	-35
#define SPHERE_R	80.0
#define SPHERE_TRANSPARENCY		0.8
#define SPHERE_CNT	0

#define QUAD_CNT	0
#define QUAD_SIDE	60

#define RED                     0.7, 0.0, 0.0
#define BLUE                    0.0, 0.0, 0.7
#define GREEN                   0.0, 0.7, 0.0
#define BLACK                   0.0, 0.0, 0.0
#define WHITE                   1.0, 1.0, 1.0
#define GREY                    0.4, 0.4, 0.4 
#define YELLOW                  0.7, 0.7, 0.0

/* Text */
/** Textausgabe, wenn das SPIEel zu ende ist, weil kein Stein mehr verfuegbar ist */
#define HELP_OUTPUT_1       "====== U'r blinded by pure AWESOMENESS!!!!! ======"
#define HELP_OUTPUT_2       "|________________________________________________|"
#define HELP_OUTPUT_3       "q/Q    beendet das Spiel."
#define HELP_OUTPUT_4       "r        beginnt ein neues Spiel im Ausgangszustand."
#define HELP_OUTPUT_5       "h/H     oeffnet/schliesst den Hilfemodus."
#define HELP_OUTPUT_6       "+/-     Aendert die Anzahl der Quadrate auf dem Boden"
#define HELP_OUTPUT_7       "n/N     Kamera zum naechsten Partikel wechseln."
#define HELP_OUTPUT_8       "m/M   Kugeln in Bewegung setzten."
#define HELP_OUTPUT_9       "t/T    Textur wechseln."
#define HELP_OUTPUT_10       "p/P   Partikel einfrieren."
#define HELP_OUTPUT_11       "f/F    Ansicht der Partikel wechseln."
#define HELP_OUTPUT_12       "b      Kugeln bewegen sich nach Bezier."
#define HELP_OUTPUT_13       "B      Bezier-Kurven der Kugeln werden angezeigt."
#define HELP_OUTPUT_14       "s/S   Schatten zeichnen an/aus."
#define HELP_OUTPUT_15      "k/K    Kamera-Modus wechseln."
#define HELP_OUTPUT_16       "v/V   Verfolger-Modus an/aus."
#define HELP_OUTPUT_17       "x/X   Abstand fuer Bezier-Kontrollpunkte erhoehen."
#define HELP_OUTPUT_18       "y/Y   Abstand fuer Bezier-Kontrollpunkte verkleinern."
#define HELP_OUTPUT_19       "f1    Wireframe an/aus."
#define HELP_OUTPUT_20       "f2    Licht an/aus."
#define HELP_OUTPUT_21       "f3    Texturen an/aus."
#define HELP_OUTPUT_22       "Maus + rechte Maustaste    Abstand zum Mittelpunkt."
#define HELP_OUTPUT_23       "Maus + linke Maustaste    Bewegung im Raum."


/* Tastatur */
/** Keyboardtaste ESC definieren */
#define ESC     27
/** Keyboardtaste SPACE definieren */
#define SPACE   32

/** Datentyp fuer  den aktuellen SPIEelmodus */
enum e_gameIntiType { OFF, ON };
typedef enum e_gameIntiType InitType;

/** Mausereignisse. */
enum e_MouseEventType
{ mouseButton, mouseMotion, mousePassiveMotion };
/** Datentyp fuer Mausereignisse. */
typedef enum e_MouseEventType CGMouseEventType;

/** Mausereignisse. */
enum e_MouseInterpretType
{ NONE, MOVE, ZOOM};
/** Datentyp fuer Mausereignisse. */
typedef enum e_MouseInterpretType MouseInterpretType;

/** Datentyp fuer die Texturart */
enum e_textureType
{ NORMALTEXTURE, CHECKER, DIAGONAL };
/** Datentyp die Texturart */
typedef enum e_textureType textureType;

/** Datentyp fuer  den aktuellen SPIEelmodus */
enum e_gameModi { normal, help };
typedef enum e_gameModi GameMode;

/** Datentyp fuer die Verlaengerung/Verkuerzung des Kameravektors */
enum e_cameraVector { closer, further };
typedef enum e_cameraVector CameraVector;

/** Datentyp fuer die Richtungsaenderungen */
enum e_gameDirection {left, right, up, down};
typedef enum e_gameDirection Directions;

/** Punkt im 3D-Raum (homogene Koordinaten) */
typedef GLfloat CGPoint4f[4];
typedef GLfloat CGPoint3f[3];
typedef CGPoint3f CGColor;

/** Datentyp fuer einen Vektor */
typedef double Vector4D[4];
typedef Vector4D Punkt4D;

/** Vektor im 3D-Raum */
typedef GLfloat CGVector3D[3];

typedef double Vertex[6];

typedef int Movement[3];

typedef double VertexTexture[2];

enum e_particleType {line, sphere};
typedef enum e_particleType ParticleType;

typedef struct {
	GLfloat x,y,z;
	GLfloat s,t;
	GLfloat nx, ny, nz;
	GLint fix;
} Water;

typedef struct {
	GLfloat x,y,z;
	GLfloat s,t;
	GLfloat nx, ny, nz;
} Terrain;

/** Kugeln */
typedef struct X * Spheres;
struct X
{
	/* Position im Raum */
    CGPoint3f pos;
    /* Richtungsvektor, Geschwindigkeit in eine Richtung */
    CGVector3D newPos;   
    /* Farbe */
   CGVector3D color; 
    /* Bezier */
    Vertex * interpolatedPoints;
    /* Bezier */
    Vertex * interpolatedPointsAll;

    Vertex * controlPoints;
    /* Bezier t */
    double t;
};

/** Kugeln */
typedef struct Z * Quads;
struct Z
{
	/* Position im Raum */
    CGPoint3f pos;
};


/** Partikel */
typedef struct Y * Particle;
struct Y
{
	/* Position im Raum */
    CGPoint3f s;
    /* Geschwindigkeitsvektor */
    CGVector3D v;   
    /* Beschleunigungsvektor */
	CGVector3D a;
	/* Kraft - intern */
	CGVector3D f;
	/* Masse */
	double m;
	/* Noch was? */
	/* Upvektor */ 
	CGVector3D up;
};

#endif
