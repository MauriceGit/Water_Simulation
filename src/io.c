

/**
 * @file
 * Hier sind alle CallBack-Funktionen
 *
 * @author Maurice Tollmien, Tilman Nedele
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#ifdef MACOSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "io.h"
#include "scene.h"
#include "logic.h"
#include "water.h"
#include "terrain.h"
#include "vector.h"
#include "imageLoader.h"

/* ---- Eigene Funktionen ---- */

/* Texturen und Framebuffer */
GLuint G_fboCam, G_fboRefl, G_fboRefr, G_TexCamera, G_TexCameraDepth, G_TexReflection, G_TexRefraction, G_TexImageGrass, G_TexImageSand, G_TexImageCaustics, G_TexImageRocks, G_TexImageSnow, G_TexImageForest, G_TexImageTundra;

/* Shader-ID's */
GLuint G_ShaderID, G_ShaderIDSimple, G_ShaderColor, G_ShaderIDSimpleD;

/* Geometrie-Buffer */
GLuint G_arrayBuffer, G_TexCube, G_cubeBuffer, G_SurfaceBuffer, G_WaterBuffer, G_WaterBufferIndex, G_ObjectsBuffer, G_TerrainBuffer, G_TerrainBufferIndex, G_CausticsBuffer;

/* Cube-Map */
GLuint G_TexCubeFront, G_TexCubeBack, G_TexCubeBottom, G_TexCubeTop, G_TexCubeLeft, G_TexCubeRight;

/* Shader-Variablen */
GLuint G_projMatrixLoc, G_viewMatrixLoc, G_sampler2dLoc, G_samplerDepth2dLoc, G_cameraPosLoc, G_colorIn, G_samplerCubeLoc;

int view = 1;
int G_Width = 1024;
int G_Height = 1024;
int G_fullScreen = 1;

GLfloat G_data[] = {
				-1.0, -1.0, 0.0,		0.0, 0.0,	
				1.0, -1.0, 0.0,			1.0, 0.0,	
				1.0, 1.0, 0.0,			1.0, 1.0,
				-1.0, -1.0, 0.0,		0.0, 0.0,
				1.0, 1.0, 0.0,			1.0, 1.0,
				-1.0, 1.0, 0.0,			0.0, 1.0
			};

float G_Surface[] = {
	-75, 0.0, -75,
	-75, 0.0, 75,
	75, 0.0, 75,
	-75, 0.0, -75,
	75, 0.0, 75,
	75, 0.0, -75
};

GLfloat G_Objects[] = {
	-30.0, -30.0, -30.0,	
	30.0, -30.0, -30.0,	
	30.0, 30.0, -30.0,
	-30.0, -30.0, -30.0,
	30.0, 30.0, -30.0,
	-30.0, 30.0, -30.0
};

float G_points[] = {
			  -100.0f,  100.0f, -100.0f,
			  -100.0f, -100.0f, -100.0f,
			   100.0f, -100.0f, -100.0f,
			   100.0f, -100.0f, -100.0f,
			   100.0f,  100.0f, -100.0f,
			  -100.0f,  100.0f, -100.0f,
			  
			  -100.0f, -100.0f,  100.0f,
			  -100.0f, -100.0f, -100.0f,
			  -100.0f,  100.0f, -100.0f,
			  -100.0f,  100.0f, -100.0f,
			  -100.0f,  100.0f,  100.0f,
			  -100.0f, -100.0f,  100.0f,
			  
			   100.0f, -100.0f, -100.0f, 
			   100.0f, -100.0f,  100.0f,
			   100.0f,  100.0f,  100.0f,
			   100.0f,  100.0f,  100.0f,
			   100.0f,  100.0f, -100.0f,
			   100.0f, -100.0f, -100.0f,
			   
			  -100.0f, -100.0f,  100.0f,
			  -100.0f,  100.0f,  100.0f,
			   100.0f,  100.0f,  100.0f,
			   100.0f,  100.0f,  100.0f,
			   100.0f, -100.0f,  100.0f,
			  -100.0f, -100.0f,  100.0f,
			  
			  -100.0f,  100.0f, -100.0f,
			   100.0f,  100.0f, -100.0f,
			   100.0f,  100.0f,  100.0f,
			   100.0f,  100.0f,  100.0f,
			  -100.0f,  100.0f,  100.0f,
			  -100.0f,  100.0f, -100.0f,
			  
			  -100.0f, -100.0f, -100.0f,
			  -100.0f, -100.0f,  100.0f,
			   100.0f, -100.0f, -100.0f,
			   100.0f, -100.0f, -100.0f,
			  -100.0f, -100.0f,  100.0f,
			   100.0f, -100.0f,  100.0f
			};


/**
 * Timer-Callback.
 * Initiiert Berechnung der aktuellen Position und Farben und anschliessendes
 * Neuzeichnen, setzt sich selbst erneut als Timer-Callback.
 * @param lastCallTime Zeitpunkt, zu dem die Funktion als Timer-Funktion
 *   registriert wurde (In).
 */
static void
cbTimer (int lastCallTime)
{
	/* Seit dem Programmstart vergangene Zeit in Millisekunden */
	int thisCallTime = glutGet (GLUT_ELAPSED_TIME);
    
	/* Seit dem letzten Funktionsaufruf vergangene Zeit in Sekunden */
	double interval = (double) (thisCallTime - lastCallTime) / 1000.0f;
		
	calcTimeRelatedStuff(interval);
			
	/* Wieder als Timer-Funktion registrieren */
	glutTimerFunc (1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

	/* Neuzeichnen anstossen */
	glutPostRedisplay ();
    
}

/**
 * Setzen der Projektionsmatrix.
 * Setzt die Projektionsmatrix unter Berücksichtigung des Seitenverhaeltnisses
 * des Anzeigefensters, sodass das Seitenverhaeltnisse der Szene unveraendert
 * bleibt und gleichzeitig entweder in x- oder y-Richtung der Bereich von -1
 * bis +1 zu sehen ist.
 * @param aspect Seitenverhaeltnis des Anzeigefensters (In).
 */
static void
setProjection (GLdouble aspect)
{
  /* Nachfolgende Operationen beeinflussen Projektionsmatrix */
  glMatrixMode (GL_PROJECTION);
  /* Matrix zuruecksetzen - Einheitsmatrix laden */
  glLoadIdentity ();
    
  {
      /* perspektivische Projektion */
      gluPerspective (90.0,     /* Oeffnungswinkel */
                      aspect,   /* Seitenverhaeltnis */
                      0.01,      /* nahe ClipPIEng-Ebene */
                      1000.0 /* ferne ClipPIEng-Ebene */ );
  }
}

static void drawColoredQuad(double r, double g, double b) {
	glDisable(GL_CULL_FACE);
	glUseProgram(G_ShaderColor);
		GLfloat mp[16], mv[16];
		glGetFloatv(GL_PROJECTION_MATRIX, mp);
		glGetFloatv(GL_MODELVIEW_MATRIX, mv);
		glUniformMatrix4fv(glGetUniformLocation(G_ShaderColor, "projMatrix"),  1, GL_FALSE, &mp[0]);
		glUniformMatrix4fv(glGetUniformLocation(G_ShaderColor, "viewMatrix"),  1, GL_FALSE, &mv[0]);
		
		GLfloat color[] = {r, g, b};
		GLfloat cam[] = {getCameraPosition(0), getCameraPosition(1), getCameraPosition(2)};
		glUniform3fv(glGetUniformLocation(G_ShaderColor, "colorIn"), 1, color);
		glUniform3fv(glGetUniformLocation(G_ShaderColor, "cameraPos"), 1, cam);
		
		glBindBuffer (GL_ARRAY_BUFFER, G_ObjectsBuffer);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		glDisableVertexAttribArray(0);
		glBindBuffer (GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glEnable(GL_CULL_FACE);
}

static void drawSimpleStuff(GLuint colorTex, int fromX, int fromY, int width, int height)   {
	glUseProgram(G_ShaderIDSimple);
		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport (fromX, fromY, width, height);		
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (90.0, 1, 5.1, 1000.0 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt (0,0,1,
		 0.0, 0.0, 0.0,
		 0.0, 1.0, 0.0);
	
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTex);
	
	GLfloat mp[16], mv[16];
	glGetFloatv(GL_PROJECTION_MATRIX, mp);
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	glUniformMatrix4fv(glGetUniformLocation(G_ShaderIDSimple, "projMatrix"),  1, GL_FALSE, &mp[0]);
	glUniformMatrix4fv(glGetUniformLocation(G_ShaderIDSimple, "viewMatrix"),  1, GL_FALSE, &mv[0]);
	
	glUniform1i(glGetUniformLocation(G_ShaderIDSimple, "texsampler"), 0);
	
	glBindBuffer (GL_ARRAY_BUFFER, G_arrayBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 3*sizeof(float));
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	
	glUseProgram(0);	
}

static void drawSimpleStuffShader(GLuint shader, GLuint colorTex, int fromX, int fromY, int width, int height)   {
	glUseProgram(shader);
		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport (fromX, fromY, width, height);		
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (90.0, 1, 5.1, 1000.0 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt (0,0,1,
		 0.0, 0.0, 0.0,
		 0.0, 1.0, 0.0);
	
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTex);
	
	GLfloat mp[16], mv[16];
	glGetFloatv(GL_PROJECTION_MATRIX, mp);
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	glUniformMatrix4fv(glGetUniformLocation(G_ShaderIDSimple, "projMatrix"),  1, GL_FALSE, &mp[0]);
	glUniformMatrix4fv(glGetUniformLocation(G_ShaderIDSimple, "viewMatrix"),  1, GL_FALSE, &mv[0]);
	
	glUniform1i(glGetUniformLocation(G_ShaderIDSimple, "texsampler"), 0);
	
	glBindBuffer (GL_ARRAY_BUFFER, G_arrayBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 3*sizeof(float));
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	
	glUseProgram(0);	
}

void drawTerrainComplete(void) {
	glUseProgram(G_ShaderIDSimple);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, G_TexImageGrass);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, G_TexImageSand);
	glActiveTexture(GL_TEXTURE0+2);
	glBindTexture(GL_TEXTURE_2D, G_TexImageForest);
	glActiveTexture(GL_TEXTURE0+3);
	glBindTexture(GL_TEXTURE_2D, G_TexImageRocks);
	glActiveTexture(GL_TEXTURE0+4);
	glBindTexture(GL_TEXTURE_2D, G_TexImageSnow);
	glActiveTexture(GL_TEXTURE0+5);
	glBindTexture(GL_TEXTURE_2D, G_TexImageTundra);
		
	GLfloat mp[16], mv[16];
	glGetFloatv(GL_PROJECTION_MATRIX, mp);
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	glUniformMatrix4fv(glGetUniformLocation(G_ShaderIDSimple, "projMatrix"),  1, GL_FALSE, &mp[0]);
	glUniformMatrix4fv(glGetUniformLocation(G_ShaderIDSimple, "viewMatrix"),  1, GL_FALSE, &mv[0]);
		 
	glUniform1i(glGetUniformLocation(G_ShaderIDSimple, "texSand") , 1);
	glUniform1i(glGetUniformLocation(G_ShaderIDSimple, "texGrass"), 0);
	glUniform1i(glGetUniformLocation(G_ShaderIDSimple, "texForest"), 2);
	glUniform1i(glGetUniformLocation(G_ShaderIDSimple, "texRocks"), 3);
	glUniform1i(glGetUniformLocation(G_ShaderIDSimple, "texSnow"), 4);
	glUniform1i(glGetUniformLocation(G_ShaderIDSimple, "texTundra"), 5);
	
	/* Wasser-Normalen für eigene Caustics-Berechnungen */
		glBindBuffer (GL_ARRAY_BUFFER, G_CausticsBuffer);
		glBufferData (GL_ARRAY_BUFFER, sizeof(*getTerrainCaustics())*TERRAIN_SIZE*TERRAIN_SIZE, getTerrainCaustics(), GL_STREAM_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 5*sizeof(GLfloat));
		
	/* Offset in X und Z-Richtung eventuell hier in die Shader übergeben... */
	
	glBindBuffer (GL_ARRAY_BUFFER, G_TerrainBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 3*sizeof(GLfloat));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 5*sizeof(GLfloat));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_TerrainBufferIndex);
	glDrawElements(GL_TRIANGLES, TERRAIN_SIZE*TERRAIN_SIZE*2*3, GL_UNSIGNED_INT, 0);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	
	glUseProgram(0);
	
}

void drawSceneToTexture(GLuint tex, GLfloat ex, GLfloat ey, GLfloat ez, GLfloat cx, GLfloat cy, GLfloat cz) {
	glBindFramebuffer(GL_FRAMEBUFFER, tex);	
	glClearColor(0.0, 1.0, 1.0, 0.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glViewport (0, 0, G_Width, G_Height);		
	setProjection (1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt (	ex, ey, ez,
		 cx, cy, cz,
		 0.0, 1.0, 0.0);

	glDisable(GL_TEXTURE_2D);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf ("Framebuffer ist nicht korrekt! 4\n");
	}

	/*drawSides();*/
	drawTerrainComplete();
	
}


void allocateMemoryStuff(GLuint * texID, GLuint * fbo) {
	
	glGenTextures(1, texID);
	glBindTexture(GL_TEXTURE_2D, *texID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	*/
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, G_Width, G_Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	
	glGenFramebuffers(1, fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);	
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texID, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void allocateMemoryStuffDepth(GLuint * texID, GLuint * texID2, GLuint * fbo) {
	
	glGenTextures(1, texID);
	glBindTexture(GL_TEXTURE_2D, *texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, G_Width, G_Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	
	glGenTextures(1, texID2);
	glBindTexture(GL_TEXTURE_2D, *texID2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, G_Width, G_Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	
	glGenFramebuffers(1, fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);	
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *texID2, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void loadCubeSide(GLuint * texID, GLenum side) {
	/*glBindTexture(GL_TEXTURE_CUBE_MAP, *texID);*/
	
	glTexImage2D(side, 0, GL_RGBA8, G_Width, G_Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);	
}

void appendTexToFB(GLuint * fbo, GLuint * texID, GLenum side) {
	glGenFramebuffers(1, fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);		
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, side, *texID, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void allocateMemoryStuffCube(GLuint * texID) {
	glActiveTexture (GL_TEXTURE0);
	glGenTextures(1, texID);
	
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, *texID);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
	
	loadCubeSide(texID, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z); /* front  */
	loadCubeSide(texID, GL_TEXTURE_CUBE_MAP_POSITIVE_Z); /* back   */
	loadCubeSide(texID, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y); /* bottom */
	loadCubeSide(texID, GL_TEXTURE_CUBE_MAP_POSITIVE_Y); /* top    */
	loadCubeSide(texID, GL_TEXTURE_CUBE_MAP_NEGATIVE_X); /* left   */
	loadCubeSide(texID, GL_TEXTURE_CUBE_MAP_POSITIVE_X); /* right  */
	
	appendTexToFB(&G_TexCubeFront, texID, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	appendTexToFB(&G_TexCubeBack,  texID, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	appendTexToFB(&G_TexCubeBottom,texID, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	appendTexToFB(&G_TexCubeTop,   texID, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	appendTexToFB(&G_TexCubeLeft,  texID, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	appendTexToFB(&G_TexCubeRight, texID, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf ("Framebuffer ist nicht korrekt 1!\n");
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderFBToCubeTexture(GLuint * fbo, GLenum side, double vX, double vY, double vZ) {
	/*==================================================================*/
	/* Eine Seite für die Cube-Textur rendern */
	/*==================================================================*/
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);	
	glClearColor(0.0, 1.0, 1.0, 0.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport (0, 0, G_Width, G_Height);		
	setProjection(1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt (0, 0, 0,
		 vX, vY, vZ,
		 0.0, fabs(vY) > 0.8 ? 0.0 : 1.0, vY > 0.8 ? 1.0 : (vY < -0.8 ? -1.0 : 0.0));

	glDisable(GL_TEXTURE_2D);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf ("Framebuffer ist nicht korrekt! 2\n");

	}
	
	/*drawColoredQuad(1,1,1);*/
	/*drawSides();*/
	
	/* RAUS, WEIL EIGENTLICH NUR DIE ENTFERNTE UMGEBUNG HIER WAHRGENOMMEN WERDEN SOLL UND  NICHT DIE KONKRETEN, DYNAMISCHEN OBJEKTE DER SCENE!!!!!!! */
	/*drawTerrainComplete();*/
}



/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer.
 */
static void
cbDisplay (void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0, 1.0, 1.0, 0.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*==================================================================*/
	/* Textur mit Kamera-Perspektive */
	/*==================================================================*/
	drawSceneToTexture(G_fboCam, getCameraPosition(0), getCameraPosition(1), getCameraPosition(2), 0.0, 0.0, 0.0);
	
	/*==================================================================*/
	/* Textur mit Reflection-Perspektive */
	/*==================================================================*/
	/*drawSceneToTexture(G_fboRefl, 0.0, 0.0, 0.0,
		 getCameraPosition(0)+(2*-getCameraPosition(0)), 
		 getCameraPosition(1)+(2*-getCameraPosition(1))+2*getCameraPosition(1), 
		 getCameraPosition(2)+(2*-getCameraPosition(2)));*/
	
	/*==================================================================*/
	/* Kamera-Textur rendern. */
	/*==================================================================*/
	if (0) {
		drawSimpleStuff(G_fboCam, 0, G_Height/2, G_Width/2, G_Height/2);
	}
	if (0) {
		drawSimpleStuffShader(G_ShaderIDSimpleD, G_TexCameraDepth, 0, 0, G_Width/2, G_Height/2);
	}
	
	/*==================================================================*/
	/* Reflection-Textur rendern */
	/*==================================================================*/
	if (0) {
		drawSimpleStuff(G_TexReflection, G_Width/2, G_Height/2, G_Width/2, G_Height/2);
	}

	/*==================================================================*/
	/* Spiegelfläche mit korrektem dynamischen Environment-Mapping (Reflection + Refraction) */
	/*==================================================================*/
	if (0) {			
		glUseProgram(G_ShaderID);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport (0, 0, G_Width/2, G_Height/2);		
			setProjection (1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt (getCameraPosition(0), getCameraPosition(1), getCameraPosition(2),
				 0.0, 0.0, 0.0,
				 0.0, 1.0, 0.0);
			
			glEnable(GL_TEXTURE_CUBE_MAP);
			glEnable(GL_TEXTURE_2D);
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, G_TexCube);
			
			GLfloat mp[16], mv[16];
			glGetFloatv(GL_PROJECTION_MATRIX, mp);
			glGetFloatv(GL_MODELVIEW_MATRIX, mv);
			glUniformMatrix4fv(glGetUniformLocation(G_ShaderID, "projMatrix"),  1, GL_FALSE, &mp[0]);
			glUniformMatrix4fv(glGetUniformLocation(G_ShaderID, "viewMatrix"),  1, GL_FALSE, &mv[0]);
			
			glUniform1i(G_sampler2dLoc, 0);
			
			GLfloat cam_pos[] = {getCameraPosition(0), getCameraPosition(1), getCameraPosition(2)};
			glUniform3fv(glGetUniformLocation(G_ShaderID, "cameraPos"), 1, cam_pos);
			
			glBindBuffer (GL_ARRAY_BUFFER, G_WaterBuffer);
			glBufferData (GL_ARRAY_BUFFER, sizeof(*getWaterList())*WORLD_SIZE*WORLD_SIZE, getWaterList(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat)+1*sizeof(GLint), 0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat)+1*sizeof(GLint), 5*sizeof(GLfloat));
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_WaterBufferIndex);
			
			/* Wasser. */
			glDrawElements(GL_TRIANGLES, WORLD_SIZE*WORLD_SIZE*2*3, GL_UNSIGNED_INT, 0);
			
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisable(GL_TEXTURE_CUBE_MAP);
			glDisable(GL_TEXTURE_2D);
			
		glUseProgram(0);
		
		/*drawSides();*/
		drawTerrainComplete();
		
		
	}
	
	/*==================================================================*/
	/* Spiegelfläche mit Screenspace-Reflections */
	/*==================================================================*/
	if (1) {			
		int toX = G_fullScreen ? G_Width : G_Width/2, 
			toZ = G_fullScreen ? G_Height : G_Height/2;
		glUseProgram(G_ShaderID);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport (0, 0, toX, toZ);
			setProjection (1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt (getCameraPosition(0), getCameraPosition(1), getCameraPosition(2),
				 0.0, 0.0, 0.0,
				 0.0, 1.0, 0.0);
			
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, G_TexCamera);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, G_TexCameraDepth);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, G_TexCube);
			
			GLfloat mp[16], mv[16];
			glGetFloatv(GL_PROJECTION_MATRIX, mp);
			glGetFloatv(GL_MODELVIEW_MATRIX, mv);
			glUniformMatrix4fv(glGetUniformLocation(G_ShaderID, "projMatrix"),  1, GL_FALSE, &mp[0]);
			glUniformMatrix4fv(glGetUniformLocation(G_ShaderID, "viewMatrix"),  1, GL_FALSE, &mv[0]);
			
			glUniform1i(G_sampler2dLoc, 1);
			glUniform1i(G_samplerDepth2dLoc, 0);
			glUniform1i(G_samplerCubeLoc, 2);
			
			GLfloat cam_pos[] = {getCameraPosition(0), getCameraPosition(1), getCameraPosition(2)};
			glUniform3fv(glGetUniformLocation(G_ShaderID, "cameraPos"), 1, cam_pos);
			
			/*glBindBuffer (GL_ARRAY_BUFFER, G_TerrainBuffer);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);*/
			
			glBindBuffer (GL_ARRAY_BUFFER, G_WaterBuffer);
			glBufferData (GL_ARRAY_BUFFER, sizeof(*getWaterList())*WORLD_SIZE*WORLD_SIZE, getWaterList(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat)+1*sizeof(GLint), 0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat)+1*sizeof(GLint), 5*sizeof(GLfloat));
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_WaterBufferIndex);
			
			/* Wasser. */
			glDrawElements(GL_TRIANGLES, WORLD_SIZE*WORLD_SIZE*2*3, GL_UNSIGNED_INT, 0);
			
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			
		glUseProgram(0);
		
		/*drawSides();*/
		drawTerrainComplete();
		drawFPS();
	}
	
	/*==================================================================*/
	/* Landschaft aus Perlin-Noise. */
	/*==================================================================*/
	if (!G_fullScreen) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport (G_Width/2, G_Height/2, G_Width/2, G_Height/2);
		setProjection (1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt (getCameraPosition(0), getCameraPosition(1), getCameraPosition(2),
			 0.0, 0.0, 0.0,
			 0.0, 1.0, 0.0);
			 
		drawTerrainComplete();
	}

	/*==================================================================*/
	/* Die Normale Scene auf der rechten Seite rendern. Echte Polygone etc. */
	/*==================================================================*/
	if (0) {
		glUseProgram(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport (0, G_Height/2, G_Width/2, G_Height/2);		
		setProjection (1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		gluLookAt (getCameraPosition(0), getCameraPosition(1), getCameraPosition(2),
			 0.0, 0.0, 0.0,
			 0.0, 1.0, 0.0);		
			
		glDisable(GL_TEXTURE_2D);
		
		drawSides();
		drawWaterSurface();		
	}	
	
	/* Alles fertig. */
	glutSwapBuffers ();
}

/**
 * Callback fuer Tastendruck.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
void cbKeyboard (unsigned char key, int x, int y)
{
	
	switch (key)
	{
		case 'q':
		case 'Q':
		case ESC:
			exit (0);
			break;
		case 'r':
        case 'R':
			/*initGame ();*/
			setRandomize();
			break;
		case 'h':
        case 'H':
            setHelpStatus(!getHelpStatus());
            break;
        case '+':
            incQuadCount();
            break;
        case '-':
            decQuadCount();
            break;
        case 'n':
		case 'N':
			pushSomeWaterDown();
            break;
        case 'w':
        case 'W':
			toggleWaterMovement();
			break;
        case 'm':
		case 'M':
			toggleSpheresMoving ();
            break;
		case 'p':
        case 'P':
            toggleParticleFreeze ();
            break;
		case 'f':
        case 'F':
            G_fullScreen = !G_fullScreen;
            break;
        case 's':
        case 'S':
            toggleShadows ();
            break;    
        case 'k':
        case 'K':
			printf ("k recognized.\n");
            startMovingBall ();
            break;   
        case 'v':
        case 'V':
            view = !view;
            break;  
            
	}	
}

/* Spezialtasten-Druck-Callback - wird ausgefuehrt, wenn Spezialtaste
* (F1 - F12, Links, Rechts, Oben, Unten, Bild-Auf, Bild-Ab, Pos1, Ende oder
* Einfuegen) gedrueckt wird */
void cbSpecial (int key, int x, int y)
{
	switch (key)
	{
        case GLUT_KEY_LEFT: 

			break;
		case GLUT_KEY_RIGHT:

			break;
		case GLUT_KEY_UP:
            setKey (1, 1);
			break;
		case GLUT_KEY_DOWN:
            setKey(0,1);
			break;
		case GLUT_KEY_F1:
            toggleWireframeMode();
			break;
        case GLUT_KEY_F2:
            setLightStatus(!getLightStatus());
            break;
		case GLUT_KEY_F3:
            setTextureStatus(!getTextureStatus());
			break;
        case GLUT_KEY_F4:

            break;
        case GLUT_KEY_F5:

            break;
        case GLUT_KEY_F6:

            break;
        case GLUT_KEY_F7:

            break;
        case GLUT_KEY_F8:

            break;
	}
}

/* Wird aufgerufen, wenn eine Spezialtaste wieder losgelassen wird1 */
void cbUpSpecial (int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_LEFT: 

			break;
		case GLUT_KEY_RIGHT:

			break;
		case GLUT_KEY_UP:
            setKey (1,0);
			break;
		case GLUT_KEY_DOWN:
            setKey (0,0);
			break;
	}
}

/**
 * Verarbeitung eines Mausereignisses.
 * Durch Bewegung der Maus bei gedrueckter Maustaste kann die aktuelle
 * Zeichenfarbe beeinflusst werden.
 * Falls Debugging aktiviert ist, wird jedes Mausereignis auf stdout
 * ausgegeben.
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param eventType Typ des Ereignisses.
 * @param button ausloesende Maustaste (nur bei Ereignissen vom Typ mouseButton).
 * @param buttonState Status der Maustaste (nur bei Ereignissen vom Typ mouseButton).
 */
static void
handleMouseEvent (int x, int y, CGMouseEventType eventType, int button, int buttonState)
{
    switch (eventType)
    {
        case mouseButton:
            switch (button)
            {
                case GLUT_LEFT_BUTTON:
                    setMouseEvent(MOVE,x,y);
                break;
                case GLUT_RIGHT_BUTTON:
                    setMouseEvent(ZOOM,x,y);
                break;
                default:
                  break;
            }
        break;
        default:
          break;
    }
    if (buttonState)
        setMouseEvent(NONE,x,y);

}

/**
 * Mouse-Button-Callback.
 * @param button Taste, die den Callback ausgeloest hat.
 * @param state Status der Taste, die den Callback ausgeloest hat.
 * @param x X-Position des Mauszeigers beim Ausloesen des Callbacks.
 * @param y Y-Position des Mauszeigers beim Ausloesen des Callbacks.
 */
static void
cbMouseButton (int button, int state, int x, int y)
{
	handleMouseEvent (x, y, mouseButton, button, state);
}

void mouseMovement(int x, int y) 
{
    
    if (getMouseEvent() == MOVE)
		setCameraMovement(x,y);
	
    if (getMouseEvent() == ZOOM)
        setCameraZoom(x,y);
}




/**
 * Callback fuer Aenderungen der Fenstergroesse.
 * Initiiert Anpassung der Projektionsmatrix an veraenderte Fenstergroesse.
 * @param w Fensterbreite (In).
 * @param h Fensterhoehe (In).
 */
static void
cbReshape (int w, int h)
{
  /* Das ganze Fenster ist GL-Anzeigebereich */
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);

  /* Anpassen der Projektionsmatrix an das Seitenverhaeltnis des Fensters */
  setProjection ((GLdouble) w / (GLdouble) h);
}

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void registerCallBacks (void)
{
    /* Mouse-Button-Callback (wird ausgefuehrt, wenn eine Maustaste
     *  gedrueckt oder losgelassen wird) */
    glutMouseFunc (cbMouseButton);
    
    glutMotionFunc(mouseMovement);
	
	/* Timer-Callback - wird einmalig nach msescs Millisekunden ausgefuehrt */
	glutTimerFunc (1000 / TIMER_CALLS_PS, /* msecs - bis Aufruf von func */
                 cbTimer,       /* func  - wird aufgerufen    */
                 glutGet (GLUT_ELAPSED_TIME));  /* value - Parameter, mit dem
                                                   func aufgerufen wird */

	/* Reshape-Callback - wird ausgefuehrt, wenn neu gezeichnet wird (z.B. nach
	* Erzeugen oder Groessenaenderungen des Fensters) */
	glutReshapeFunc (cbReshape);

	/* Display-Callback - wird an mehreren Stellen imlizit (z.B. im Anschluss an
	* Reshape-Callback) oder explizit (durch glutPostRedisplay) angestossen */
	glutDisplayFunc (cbDisplay);
	
	/* Tasten-Druck-Callback - wird ausgefuehrt, wenn eine Taste gedrueckt wird */
	glutKeyboardFunc (cbKeyboard);
	
	/* Spezialtasten-Druck-Callback - wird ausgefuehrt, wenn Spezialtaste
	* (F1 - F12, Links, Rechts, Oben, Unten, Bild-Auf, Bild-Ab, Pos1, Ende oder
	* Einfuegen) gedrueckt wird */
	glutSpecialFunc (cbSpecial);
	
    /* Wird aufgerufen, wenn eine Taste wieder losgelassen wird! */
	glutSpecialUpFunc(cbUpSpecial);
    
	/* Automat. Tastendruckwiederholung ignorieren */
	glutIgnoreKeyRepeat (1);
    
    /* Wenn die Fenstergröße verändert wird. */
    /*glutReshapeFunc(cgReshape);*/
}

int readFile (char * name, char ** buffer) {
	FILE *f = fopen(name, "rb");
	fseek(f, 0, SEEK_END);
	int pos = ftell(f);
	fseek(f, 0, SEEK_SET);

	(*buffer) = malloc(pos+1);
	fread(*buffer, pos-1, 1, f);
	(*buffer)[pos-1] = '\0';
	fclose(f);
}

GLuint loadShaders(char * vertexShader, char * fragmentShader){
 
    /* Create the shaders */
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
 
	/* Read Shader from File was here */
 
    GLint Result = GL_FALSE;
    int InfoLogLength;
 
    /* Compile Vertex Shader */
    printf("Compiling Vertex shader\n");
    char * VertexSourcePointer = NULL;
    readFile(vertexShader, &VertexSourcePointer);
    
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
 
    /* Check Vertex Shader */
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char * vertexShaderErrorMessage = calloc(InfoLogLength, sizeof(char));
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &(vertexShaderErrorMessage[0]));
    fprintf(stdout, "vertexShaderErrorMessage: %s\n", vertexShaderErrorMessage);
 
    /* Compile Fragment Shader */
    printf("Compiling Fragment shader\n");
    char * FragmentSourcePointer = NULL;
    readFile(fragmentShader, &FragmentSourcePointer);
    
    /*printf ("FragmentShader: \n\"%s\"\n", FragmentSourcePointer);*/
    
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
 
    /* Check Fragment Shader */
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char * fragmentShaderErrorMessage = calloc(InfoLogLength, sizeof(char));
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &(fragmentShaderErrorMessage[0]));
    fprintf(stdout, "fragmentShaderErrorMessage: %s\n", fragmentShaderErrorMessage);
 
    /*  Link the program */
    GLuint ProgramID = glCreateProgram();
    
    glAttachShader(ProgramID, VertexShaderID);
    
    glAttachShader(ProgramID, FragmentShaderID);
    
    glLinkProgram(ProgramID);
    
    /* Check the program */
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char * programErrorMessage = calloc(InfoLogLength, sizeof(char));
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &(programErrorMessage[0]));
    fprintf(stdout, "programErrorMessage: %s\n", programErrorMessage);
 
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
 
    return ProgramID;
}

int loadTextureImage(Image * image, char * name, GLuint * tex) {
	
	if (!imageLoad(name, image)) 
	{
		printf("Error reading image file");
		exit(1);
	}        
	
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->sizeX, image->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);	
}

/**
 * Initialisiert das Programm (inkl. I/O und OpenGL) und startet die
 * Ereignisbehandlung.
 * @param title Beschriftung des Fensters
 * @param width Breite des Fensters
 * @param height Hoehe des Fensters
 * @return ID des erzeugten Fensters, 0 im Fehlerfall
 */
int initAndStartIO (char *title, int width, int height)
{
	int windowID = 0;
	
	/* Kommandozeile immitieren */
	int argc = 1;
	char *argv = "cmd";
	
	G_Width = width;
	G_Height = height;

	/* Glut initialisieren */
	glutInit (&argc, &argv);
	
	/* FensterInitialisierung */
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	/* FensterGröße */
	glutInitWindowSize (G_Width, G_Height);
	/* FensterPosition */
	glutInitWindowPosition (0, 0);
	
	windowID = glutCreateWindow (title);
	
	if (windowID)
	{
		/* Hintergrund und so werden initialisiert (Farben) */
		if (initScene ())
		{
			G_ShaderID = loadShaders("waterVertexShader.vert", "waterFragmentShader.frag");
			G_ShaderIDSimple = loadShaders("textureVertexShader.vert", "textureFragmentShader.frag");
			G_ShaderIDSimpleD = loadShaders("textureVertexShader.vert", "textureDepthFragmentShader.frag");
			G_ShaderColor = loadShaders("colorVertexShader.vert", "colorFragmentShader.frag");
           
			registerCallBacks ();
			
			allocateMemoryStuffDepth(&G_TexCamera, &G_TexCameraDepth, &G_fboCam);
			allocateMemoryStuff(&G_TexReflection, &G_fboRefl);
			allocateMemoryStuff(&G_TexRefraction, &G_fboRefr);
			
			allocateMemoryStuffCube(&G_TexCube);

			G_sampler2dLoc  		= glGetUniformLocation(G_ShaderID, "texsampler");
			G_samplerDepth2dLoc  	= glGetUniformLocation(G_ShaderID, "texsamplerDepth");
			G_samplerCubeLoc	  	= glGetUniformLocation(G_ShaderID, "texsamplerCube");
			
			Image * imageSand, * imageGrass, * imageCaustics, * imageRocks, * imageSnow, * imageForest, * imageTundra;
			imageSand  = malloc(sizeof(Image));
			imageGrass = malloc(sizeof(Image));
			imageCaustics = malloc(sizeof(Image));
			imageRocks = malloc(sizeof(Image));
			imageSnow = malloc(sizeof(Image));
			imageForest = malloc(sizeof(Image));
			imageTundra = malloc(sizeof(Image));
			
			loadTextureImage(imageSand,  "sand1.bmp", &G_TexImageSand );
			loadTextureImage(imageGrass, "gras2.bmp", &G_TexImageGrass);
			loadTextureImage(imageCaustics, "caustics.bmp", &G_TexImageCaustics);
			loadTextureImage(imageRocks, "rocks.bmp", &G_TexImageRocks);
			loadTextureImage(imageSnow, "snow.bmp", &G_TexImageSnow);
			loadTextureImage(imageForest, "forest.bmp", &G_TexImageForest);
			loadTextureImage(imageTundra, "tundra.bmp", &G_TexImageTundra);
			 
			glGenBuffers(1, &G_arrayBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, G_arrayBuffer); 
			glBufferData(GL_ARRAY_BUFFER, sizeof(G_data), G_data, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers (1, &G_cubeBuffer);
			glBindBuffer (GL_ARRAY_BUFFER, G_cubeBuffer);
			glBufferData (GL_ARRAY_BUFFER, sizeof(G_points), &G_points, GL_STATIC_DRAW);
			glBindBuffer (GL_ARRAY_BUFFER, 0);
			
			glGenBuffers (1, &G_SurfaceBuffer);
			glBindBuffer (GL_ARRAY_BUFFER, G_SurfaceBuffer);
			glBufferData (GL_ARRAY_BUFFER, sizeof(G_Surface), &G_Surface, GL_STATIC_DRAW);
			glBindBuffer (GL_ARRAY_BUFFER, 0);
			
			/* Wasser */
			glGenBuffers (1, &G_WaterBuffer);
			glBindBuffer (GL_ARRAY_BUFFER, G_WaterBuffer);
			glBufferData (GL_ARRAY_BUFFER, sizeof(*getWaterList())*WORLD_SIZE*WORLD_SIZE, getWaterList(), GL_STREAM_DRAW);
			glBindBuffer (GL_ARRAY_BUFFER, 0);
			
			glGenBuffers (1, &G_WaterBufferIndex);
			glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, G_WaterBufferIndex);
			glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof(*getWaterIndices())*WORLD_SIZE*WORLD_SIZE*3*2, getWaterIndices(), GL_STREAM_DRAW);
			glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
			
			/* Terrain */
			glGenBuffers (1, &G_TerrainBuffer);
			glBindBuffer (GL_ARRAY_BUFFER, G_TerrainBuffer);
			glBufferData (GL_ARRAY_BUFFER, sizeof(*getTerrainList())*TERRAIN_SIZE*TERRAIN_SIZE, getTerrainList(), GL_STATIC_DRAW);
			glBindBuffer (GL_ARRAY_BUFFER, 0);
			
			glGenBuffers (1, &G_TerrainBufferIndex);
			glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, G_TerrainBufferIndex);
			glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof(*getTerrainIndices())*TERRAIN_SIZE*TERRAIN_SIZE*3*2, getTerrainIndices(), GL_STATIC_DRAW);
			glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
			
			/* Caustics --> Indices des Terrains nutzen! */
			glGenBuffers (1, &G_CausticsBuffer);
			glBindBuffer (GL_ARRAY_BUFFER, G_CausticsBuffer);
			glBufferData (GL_ARRAY_BUFFER, sizeof(*getTerrainCaustics())*TERRAIN_SIZE*TERRAIN_SIZE, getTerrainCaustics(), GL_STREAM_DRAW);
			glBindBuffer (GL_ARRAY_BUFFER, 0);
			
			glGenBuffers(1, &G_ObjectsBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, G_ObjectsBuffer); 
			glBufferData(GL_ARRAY_BUFFER, sizeof(G_Objects), G_Objects, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(0.0, 1.0, 1.0, 0.0);
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderFBToCubeTexture(&G_TexCubeFront, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0,  0,    -1);
			renderFBToCubeTexture(&G_TexCubeBack,  GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0,  0,    1);
			renderFBToCubeTexture(&G_TexCubeBottom,GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0,  -1,   0);
			renderFBToCubeTexture(&G_TexCubeTop,   GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0,  1,    0);
			renderFBToCubeTexture(&G_TexCubeLeft,  GL_TEXTURE_CUBE_MAP_NEGATIVE_X, -1, 0,    0);
			renderFBToCubeTexture(&G_TexCubeRight, GL_TEXTURE_CUBE_MAP_POSITIVE_X, 1,  0,    0);
			
			/* Die Endlosschleife wird angestoßen */
			glutMainLoop ();  
			windowID = 0;
			
			glDeleteFramebuffers(1, &G_fboCam);
			glDeleteTextures(1, &G_TexCamera);
            
		} 
		else
		{
			glutDestroyWindow (windowID);
			windowID = 0;
		}
	}
	return windowID;
}
