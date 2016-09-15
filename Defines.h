#ifndef YOEL_DEFINES_H
#define YOEL_DEFINES_H

#pragma once

////////////////////
// Variable types
////////////////////


#ifndef DWORD
#define DWORD unsigned long
#endif



#ifndef byte
#define byte unsigned char
#endif


#ifndef BOOL
#define BOOL boolean
#endif

#ifndef UINT
#define UINT unsigned int
#endif




///////////////////////
// Math
//////////////////////

//#define PI (3.1415926535897932384626433832795028841971693993751)
#define PI    (3.14159265358979)
//#define TWOPI (6.283185307179586476925286766558)
#define TWOPI (6.28318530717958)
//#define EPSILON (0.00000000001)
#define PIOVER180 (0.0174532925)
#define DEG2RAD( x ) ( (x) * PI / 180.0f )
#define RAD2DEG( x ) ( (180.f) * x /PI   )

#define RANDOM_NUM        ((float)rand()/(RAND_MAX+1))


////////////////////////
// Memory
///////////////////////

#ifndef NULL
 #define NULL 0
#endif

////////////////////////////////////////////////////////////
// safe delete operations - only deletes if it is NOT null
////////////////////////////////////////////////////////////

// a safe delete for single
#define SafeDelete(x) { if (x) delete x; x=NULL; };

// a safe delete for an array
#define SafeDeleteArray(x) {if (x!=NULL) delete[] x; x=NULL;}



/////////////////////////
// Debug
////////////////////////

#define BREAKPOINT(x)    if (x) __asm int 3

#endif // YOEL_DEFINES_H
