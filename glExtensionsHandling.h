#ifndef YOEL_GLHANDLINE_H
#define YOEL_GLHANDLING_H

#pragma once

#include <Windows.h>
#include <gl/GL.h>
#include "NeHeGL.h"
#include "glext.h"

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

extern PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
extern int g_iCardTexUnits; // num of tex units on current card


// this initializes some variables inside crenderer, returns false if CRITICAL extensions aren't supported
bool HandleExtensions(void);

// return false if the card doesn't support this
bool InitMultiTextureFunctionPointers(void);

// find out how many tex units the card has
void InitCardTexUnitsNum(void);

#endif //YOEL_GLHANDLING_H