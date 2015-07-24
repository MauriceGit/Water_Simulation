
/**
 * @file
 * Hier wird vll gezeichnet...
 *
 * @author Maurice Tollmien, Tilman Nedele
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stringOutput.h>

#ifdef MACOSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <GL/glu.h>

/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "logic.h"
#include "types.h"
#include "water.h"
#include "terrain.h"
#include "vector.h"


/* ---- Globale Variablen für die Farben der Wände ------ */
float G_R;
float G_G;
float G_B;
float G_Alpha;
float G_Shininess;
int G_Emission;


/* ---- Funktionen ---- */

/**
 * Wird aufgerufen, wenn "h" gedrückt worden ist.
 * Gibt einen Text auf einem schwarzen Hintergrund auf dem Bildschirm aus
 */
void printHelp (void)
{
    /* Textfarbe */
    GLfloat textColor[3] = { 1.0f, 0.5f, 0.5f };

    drawString (0.2f, 0.1f, textColor, HELP_OUTPUT_1);
    drawString (0.2f, 0.125f, textColor, HELP_OUTPUT_2);
    drawString (0.2f, 0.148f, textColor, HELP_OUTPUT_3);
    drawString (0.2f, 0.171f, textColor, HELP_OUTPUT_4);
    drawString (0.2f, 0.194f, textColor, HELP_OUTPUT_5);
    drawString (0.2f, 0.217f, textColor, HELP_OUTPUT_6);
    drawString (0.2f, 0.240f, textColor, HELP_OUTPUT_7);
    drawString (0.2f, 0.263f, textColor, HELP_OUTPUT_8);
    drawString (0.2f, 0.286f, textColor, HELP_OUTPUT_9);
    drawString (0.2f, 0.311f, textColor, HELP_OUTPUT_10);
    drawString (0.2f, 0.336f, textColor, HELP_OUTPUT_11);
    drawString (0.2f, 0.361f, textColor, HELP_OUTPUT_12);
    drawString (0.2f, 0.386f, textColor, HELP_OUTPUT_13);
    drawString (0.2f, 0.411f, textColor, HELP_OUTPUT_14);
    drawString (0.2f, 0.436f, textColor, HELP_OUTPUT_15);
    drawString (0.2f, 0.461f, textColor, HELP_OUTPUT_16);
    drawString (0.2f, 0.486f, textColor, HELP_OUTPUT_17);
    drawString (0.2f, 0.505f, textColor, HELP_OUTPUT_18);
    drawString (0.2f, 0.530f, textColor, HELP_OUTPUT_19);
    drawString (0.2f, 0.555f, textColor, HELP_OUTPUT_20);
    drawString (0.2f, 0.580f, textColor, HELP_OUTPUT_21);
    drawString (0.2f, 0.605f, textColor, HELP_OUTPUT_22);
    drawString (0.2f, 0.630f, textColor, HELP_OUTPUT_23);

}



/**
 * Setzt global eine Farbe!
 * @param color1,color2,color3 - RGB
 * @param alpha
 * @param shininess
 * @param emission
 */
void setColor (float color1, float color2, float color3, float alpha, float shininess, int emission)
{
    float tmp[] = {0.0, 0.0, 0.0};
    tmp[0] = color1;
    tmp[1] = color2;
    tmp[2] = color3;
        
    {
        /* Material */
        float matAmbient[] = {0.0, 0.0, 0.0, 0.0};
        float matBlack[]= {0.0, 0.0, 0.0, 0.0};
        float matDiffuse[] = {0.0, 0.0, 0.0, 0.0};
        float matSpecular[] = {0.0, 0.0, 0.0, 0.0};
        float matShininess[] = { 0.0 };
        
        matShininess[0] = shininess;
        
        matAmbient[0] = tmp[0];
        matAmbient[1] = tmp[1];
        matAmbient[2] = tmp[2];
        matAmbient[3] = alpha;
        
        matSpecular[0] = tmp[0];
        matSpecular[1] = tmp[1];
        matSpecular[2] = tmp[2];
        matSpecular[3] = alpha;
        
        matDiffuse[0] = tmp[0] - 0.7;
        matDiffuse[1] = tmp[1] - 0.7;
        matDiffuse[2] = tmp[2] - 0.7;
        matDiffuse[3] = alpha;

        /* Setzen der Material-Parameter */
        glMaterialfv (GL_FRONT, GL_AMBIENT, matAmbient);
        glMaterialfv (GL_FRONT, GL_DIFFUSE, matDiffuse);
        glMaterialfv (GL_FRONT, GL_SPECULAR, matSpecular);
        glMaterialfv (GL_FRONT, GL_SHININESS, matShininess);
        if (emission)
            glMaterialfv (GL_FRONT, GL_EMISSION, matSpecular);
        else
            glMaterialfv (GL_FRONT, GL_EMISSION, matBlack);
    }
}

/**
 * Zeichnet Kugel mit mehr Parametern.
 * @param sliceCount - Anzahl der Unterteilungen
 * @param r - Radius
 */
void drawSphere (int sliceCount, double r)
{
	GLUquadricObj * body;
    
    body = gluNewQuadric ();
    if (body == NULL)
        exit (1);

    /* Normalen fuer Quadrics berechnen lassen */
    gluQuadricNormals (body, GLU_SMOOTH);
    
    gluSphere (body, r, sliceCount, sliceCount);

    gluDeleteQuadric (body);
}

/**
 * Zeichnet ein Quadrat mit der Kantenlaenge 1, das aus mehreren kleineren
 * Quadraten zusammen gesetzt ist.
 * @param subDivs Anzahl der Unterteilungsschritte der Kanten.
 */
static void
drawSquare (GLint subDivs)
{
  int x, y;

  for (y = 0; y < subDivs + 1; y++)
    {
		glNormal3f (0.0f, 0.0f, 1.0f);
		glBegin (GL_QUAD_STRIP);
		
		for (x = 0; x <= subDivs + 1; x++)
		{
		/* Texturkoordinate,
		gilt nicht, wenn Texturkoordinaten automatisch generiert werden. */
		/*glTexCoord2f (x / (subDivs + 1.0f), y / (subDivs + 1.0f));*/
		glVertex3f (-0.5f + x / (subDivs + 1.0f),
				   0.5f - y / (subDivs + 1.0f),
				   0.0f);

		/* Texturkoordinate,
		gilt nicht, wenn Texturkoordinaten automatisch generiert werden. */
		/*glTexCoord2f (x / (subDivs + 1.0f), (y + 1) / (subDivs + 1.0f));*/
		glVertex3f (-0.5f +    x    / (subDivs + 1.0f),
				   0.5f - (y + 1) / (subDivs + 1.0f),
				   0.0f);
		}
      glEnd ();
    }

}

/**
 * Zeichnet einen Wuerfel mit Kantenlaenge 1.
 * @param subDivs Anzahl der Unterteilungsschritte der Kanten.
 */
static void
drawCube (unsigned int subDivs, GLfloat radius)
{
  /* Frontflaeche */
  glPushMatrix ();
  {
    glTranslatef (0.0f, 0.0f, radius);
    drawSquare (subDivs);
  }
  glPopMatrix ();

  /* rechte Seitenflaeche */
  glPushMatrix ();
  {
    glRotatef (90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef (0.0f, 0.0f, radius);
    drawSquare (subDivs);
 }
  glPopMatrix ();

  /* Rueckseitenflaeche */
  glPushMatrix ();
  {
    glRotatef (180.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef (0.0f, 0.0f, radius);
    drawSquare (subDivs);
  }
  glPopMatrix ();


  /* linke Seitenflaeche */
  glPushMatrix ();
  {
    glRotatef (270.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef (0.0f, 0.0f, radius);
    drawSquare (subDivs);
  }
  glPopMatrix ();

  /* Deckelflaeche */
  glPushMatrix ();
  {
    glRotatef (-90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef (0.0f, 0.0f, radius);
    drawSquare (subDivs);
  }
  glPopMatrix ();

  /* Bodenflaeche */
  /*glPushMatrix ();
  {
    glRotatef (90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef (0.0f, 0.0f, radius);
    drawSquare (subDivs);
  }
  glPopMatrix ();*/
}

/**
 * Zeichnet ein Quadrat mit den Ecken p1,p2,p3,p4 in der Farbe color=1/0
 * @param p1,p2,p3,p4 - Die Ecken
 * @param color - schwarz = 0 / weiß = 1
 */
void drawVertex(CGPoint3f p1, CGPoint3f p2, CGPoint3f p3, CGPoint3f p4, int color)
{
     if (color)
        setColor(1.0,1.0,1.0,G_Alpha,G_Shininess,G_Emission);
    else
        setColor(G_R,G_G,G_B,G_Alpha,G_Shininess,G_Emission);
      glBegin (GL_QUADS);
      {
        glVertex3f(p1[0],p1[1],p1[2]);
        glVertex3f(p2[0],p2[1],p2[2]);
        glVertex3f(p3[0],p3[1],p3[2]);
        glVertex3f(p4[0],p4[1],p4[2]);
      }
      glEnd();
}

/**
 * Zeichnet den Boden
 */
void drawBottom(void)
{
    CGPoint3f p1 = {0.0,GROUND_HEIGHT,0.0},
          p2 = {WORLD_SIZE,GROUND_HEIGHT,0.0},
          p3 = {WORLD_SIZE,GROUND_HEIGHT,WORLD_SIZE},
          p4 = {0.0,GROUND_HEIGHT,WORLD_SIZE};

	
	/*glDisable (GL_DEPTH_TEST);*/
    /*glDisable(GL_CULL_FACE);*/
    
    glPushMatrix ();
    {
        glTranslatef(-(WORLD_SIZE/2), -WORLD_SIZE/2.0+0.1,-(WORLD_SIZE/2));
		/*setColor (0.0, 0.0, 0.0, 1.0, 40, 1);*/
        drawRecursiveQuad(p1,p2,p3,p4,getQuadCount(),0);
    }
    glPopMatrix ();
    
    /*glEnable(GL_CULL_FACE);*/
    /*glEnable (GL_DEPTH_TEST);*/
}

/**
 * Zeichnet die Wände
 */
void drawSides(void) 
{	
	glPushMatrix();
	{
		G_R = 1.0;
		G_G = 0.0;
		G_B = 0.0;
		G_Alpha = 1.0;
		G_Shininess = 40;
		G_Emission = 1;
		glRotatef(90,1,0,0);
		drawBottom();
	}
	glPopMatrix();
	glPushMatrix();
	{
		G_R = 0.0;
		G_G = 1.0;
		G_B = 0.0;
		G_Alpha = 1.0;
		G_Shininess = 40;
		G_Emission = 1;
		glRotatef(90,1,0,0);
		glRotatef(180,0,0,1);
		drawBottom();
	}
	glPopMatrix();
	glPushMatrix();
	{
		G_R = 0.0;
		G_G = 0.0;
		G_B = 1.0;
		G_Alpha = 1.0;
		G_Shininess = 40;
		G_Emission = 1;
		glRotatef(90,1,0,0);
		glRotatef(90,0,0,1);
		drawBottom();
	}
	glPopMatrix();
	glPushMatrix();
	{
		G_R = 0.5;
		G_G = 0.5;
		G_B = 0.5;
		G_Alpha = 1.0;
		G_Shininess = 40;
		G_Emission = 1;
		glRotatef(90,1,0,0);
		glRotatef(90,0,0,-1);
		drawBottom();
	}
	glPopMatrix();
	
	/* Oben! */
	glPushMatrix();
	{
		G_R = 1.0;
		G_G = 1.0;
		G_B = 0.0;
		G_Alpha = 1.0;
		G_Shininess = 40;
		G_Emission = 1;
		glRotatef(180,1,0,0);
		drawBottom();
	}
	glPopMatrix();
	
	/* Unten! */
	glPushMatrix();
	{
		G_R = 0.0;
		G_G = 0.0;
		G_B = 0.0;
		G_Alpha = 1.0;
		G_Shininess = 40;
		G_Emission = 1;
		glRotatef(360,1,0,0);
		drawBottom();
	}
	glPopMatrix();
	
}


/**
 * Zeichnet die Welt
 */
void drawWorld() {


    glDisable(GL_CULL_FACE);
    glPushMatrix();
    {
		setColor (WHITE, 1.0, 30.0, 0);
        /*glTranslatef(0,(30.0f)/2,0);*/
        glScalef(WORLD_SIZE,WORLD_SIZE,WORLD_SIZE);
        drawCube(20,0.5f);
    }
    glPopMatrix();
  glEnable(GL_CULL_FACE);
}

/**
 * Zeichnet die beiden Referenzkugeln
 */
void drawSpheres(void)
{
	int i;
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
		for (i=0;i<SPHERE_CNT;i++)
		{
			glPushMatrix();
			{
				Spheres s = getSphereList()[i];
				glTranslatef(s->pos[0],s->pos[1],s->pos[2]);
			
				setColor (s->color[0],s->color[1],s->color[2], SPHERE_TRANSPARENCY, 0.0, 1);
				glCullFace (GL_FRONT);
				drawSphere (SLICE_CNT, SPHERE_R);
				glCullFace (GL_BACK);
				drawSphere (SLICE_CNT, SPHERE_R);
			}
			glPopMatrix();
		}
	
	glDisable (GL_BLEND);
	
}

void drawQuads(void) 
{
	
	int i;
	
	for (i=0;i<QUAD_CNT;i++)
	{
		glPushMatrix();
		{
			Quads s = getQuadList()[i];
			glTranslatef(s->pos[0],s->pos[1],s->pos[2]);
			glScalef(QUAD_SIDE, QUAD_SIDE, QUAD_SIDE);
			
			drawCube (1, 0.5);
			
		}
		glPopMatrix();
	}

}

/**
 * Malt ein übergebenes Partikel in den Ursprung
 */
void drawOneParticle(Particle p, int suspensionPoint)
{  
	if (getParticleType() == sphere) {  
		if (suspensionPoint) 
			setColor (GREEN, 1.0, 0, 1);
		else
			setColor (RED, 1.0, 0, 1);
		
		glPushMatrix();
		{
			glTranslatef(p->s[0],p->s[1],p->s[2]);
			drawSphere (4, 0.8);
		}
		glPopMatrix();
				
		setColor (WHITE, 1.0, 0, 1);
	}
			
}

/**
 * Malt die Normale --> Up-Vektor des Partikels
 */
void drawParticleNormal (Particle p)
{
    
    glBegin (GL_LINES);
    {
        CGVector3D up;
        /*printf ("scene: up: %f, %f, %f\n", p->up[0],p->up[1],p->up[2]);*/
        up[0]=p->up[0];
        up[1]=p->up[1];
        up[2]=p->up[2];
        normVector3D(up);
        glVertex3f(p->s[0],p->s[1],p->s[2]);
        glVertex3f(up[0]+p->s[0],up[1]+p->s[1],up[2]+p->s[2]);
    }
    glEnd ();
}

void drawFPS (void)
{
	GLfloat textColor[3] = { 1.0f, 1.0f, 1.0f };
	char * string = calloc ((strlen ("FPS = ") + 4), sizeof(char));
	sprintf(string, "FPS = %d", getFPS());
	
	drawString (0.6, 0.1, textColor, string);
	
	free (string);
}

void drawWaterSurface(void) 
{
	Water * water = getWaterList();
	int i,j;
	
	/* Punkte anzeigen. */
	glPointSize(1.0);
	for (i=0;i<WORLD_SIZE*WORLD_SIZE;i++) {
		glBegin (GL_POINTS);
			glVertex3f(water[i].x, water[i].y, water[i].z);
		glEnd();
	}
	
	
	
}

void drawTerrainSurface(void)
{
	Terrain * terrain = getTerrainList();
	GLuint * indices = getTerrainIndices();
	int i,j;
	
	/* Punkte anzeigen. */
	if (0) {
		glPointSize(4.0);
		for (i=0;i<TERRAIN_SIZE*TERRAIN_SIZE;i++) {
			glBegin (GL_POINTS);
				glVertex3f(terrain[i].x, terrain[i].y, terrain[i].z);
			glEnd();
		}
	}
	/* Linestrip. */
	if (1) {
		
		for (i=0;i<TERRAIN_SIZE*TERRAIN_SIZE*3*2;i+=3) {
			glBegin(GL_TRIANGLES);
				glVertex3f(terrain[indices[i]].x, terrain[indices[i]].y, terrain[indices[i]].z);
				glVertex3f(terrain[indices[i+1]].x, terrain[indices[i+1]].y, terrain[indices[i+1]].z);
				glVertex3f(terrain[indices[i+2]].x, terrain[indices[i+2]].y, terrain[indices[i+2]].z);
			glEnd();
		}
		
	}
}
	

/**
 * Zeichen-Funktion.
 * Stellt die Szenerie rund um die Rennstrecke dar.
 */
void drawScene (void)
{
    CGPoint4f 	lightPos2 = { 0.0f, 20.0f, 0.0f, 1.0f };
       
    CGPoint4f 	lightPos1 = { 0.0f, 5.0f, 10.0f, 1.0f };
    CGPoint3f 	lightDir1  = { 0.0, -5.0, -10.0 };
    

    glLightfv (GL_LIGHT2, GL_POSITION, lightPos2);

    glLightfv (GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, lightDir1);
	
    setColor (BLACK, 1.0, 0.0, 0);
	
    if (!getHelpStatus()) 
    {
        if (getLightStatus()){
			/*glEnable (GL_LIGHT1);*/
            glEnable (GL_LIGHT2);
        } else {
			glDisable (GL_LIGHT1);
			glDisable (GL_LIGHT2);
        }

        /* Zeichnet den Boden*/
        G_R = 0.0;
		G_G = 0.0;
		G_B = 0.0;
		G_Alpha = 1.0;
		G_Shininess = 40;
		G_Emission = 1;
        drawBottom();
        
        drawSides();
        
        /* Zeichnet die beiden Kugeln */
        drawSpheres();
        
        drawQuads();
                
        /* Zeichnet die Partikel */
        /*drawParticles();
        
        drawParticleCloth();*/
        
        drawWaterSurface();
            
        drawFPS ();
        
                
    } else {
        printHelp();
    }
    
}

/**
 * Initialisierung der Lichtquellen.
 * Setzt Eigenschaften der Lichtquellen (Farbe, Oeffnungswinkel, ...)
 */
static void initLight (void)
{

    /* Farbe der zweiten Lichtquelle */
	CGPoint4f lightColor1[3] =
	{ {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f,
														   1.0f}
	};
    
    /* Farbe der ersten Lichtquelle */
    CGPoint4f lightColor2[3] =
    { {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f,
                                                           1.0f}
    };
    
	/* Oeffnungswinkel der zweiten Lichtquelle */
	GLdouble lightCutoff1 = 90.0f;
	/* Lichtverteilung im Lichtkegel der zweiten Lichtquelle */
	GLdouble lightExponent1 = 20.0f;
    
    float globalAmbientLight[] = {0.3, 0.3, 0.3, 1.0};
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);
    
    /* Farbe der zweiten Lichtquelle setzen */
	glLightfv (GL_LIGHT1, GL_AMBIENT, lightColor1[0]);
	glLightfv (GL_LIGHT1, GL_DIFFUSE, lightColor1[1]);
	glLightfv (GL_LIGHT1, GL_SPECULAR, lightColor1[2]);
    
    /* Spotlight-Eigenschaften der zweiten Lichtquelle setzen */
	glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, lightCutoff1);
	glLightf (GL_LIGHT1, GL_SPOT_EXPONENT, lightExponent1);
    
    /* Farbe der zweiten Lichtquelle setzen */
	glLightfv (GL_LIGHT2, GL_AMBIENT, lightColor2[0]);
	glLightfv (GL_LIGHT2, GL_DIFFUSE, lightColor2[1]);
	glLightfv (GL_LIGHT2, GL_SPECULAR, lightColor2[2]);
	
	/* Spotlight-Eigenschaften der zweiten Lichtquelle setzen */
	glLightf (GL_LIGHT2, GL_SPOT_CUTOFF, lightCutoff1);
	glLightf (GL_LIGHT2, GL_SPOT_EXPONENT, lightExponent1);
}

/**
 * Bei SPIEelbegin wird das SPIEelfeld komplett initialisiert
 * mit einem Hintergrund, einer Zeichenfarbe, Linienbreite.
 * Außerdem wird die Datenhaltung initialisiert (siehe initField (), initStones ()).
 * @return Ob ein Fehler aufgetreten ist.
 */
int initScene (void)
{
	glEnable (GL_DEPTH_TEST);
	glCullFace (GL_BACK);
	glEnable (GL_CULL_FACE);
	glEnable (GL_NORMALIZE);
	glEnable (GL_LIGHTING);
	initLight ();
	initGame ();

	return 1;
}



/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void
toggleWireframeMode (void)
{
    /* Flag: Wireframe: ja/nein */
    static GLboolean wireframe = GL_FALSE;

    /* Modus wechseln */
    wireframe = !wireframe;

    if (wireframe)
        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

