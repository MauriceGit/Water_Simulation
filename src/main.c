/**
 * @file
 * ein erster Versuch, eine eigene Datei zu erstellen...
 * @author Maurice Tollmien.
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#ifdef MACOSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "io.h"
#include "scene.h" 
#include "types.h"



/**
 * Hauptprogramm.
 * Initialisiert Fenster, Anwendung und Callbacks, startet glutMainLoop.
 * @param argc Anzahl der Kommandozeilenparameter (In).
 * @param argv Kommandozeilenparameter (In).
 * @return Rueckgabewert im Fehlerfall ungleich Null.
 */
int
main (int argc, char **argv)
{
	srand (time (0));
	if (!initAndStartIO ("nen Programm", 1080, 1080))
	{
		fprintf (stderr, "Initialisierung fehlgeschlagen!\n");
		return 1;
	} 
	return 0;
}
