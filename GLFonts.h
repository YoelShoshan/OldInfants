#ifndef GLFONTS_H
#define GLFONTS_H

#pragma once

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

//GLuint	base;				// Base Display List For The Font Set

class CFonts
{
public:
void BuildFont(void);
void KillFont(void);
void glPrint(const char *fmt, ...);

private:
unsigned int	base;				// Base Display List For The Font Set

};




#endif //GLFONTS_H