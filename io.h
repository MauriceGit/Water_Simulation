#ifndef __IO_H__
#define __IO_H__
#define __DEBUG_GL_H__
/**
 * @file
 * CallBack-Funktionsköpfe
 *
 * @author Maurice Tollmien
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef MACOSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifdef MACOSX
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdarg.h>

int initAndStartIO (char *title, int width, int height);

#endif
