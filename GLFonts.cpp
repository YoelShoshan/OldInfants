
#include "GLFonts.h"
#include <Windows.h>
#include "NeHeGL.h"
#include <gl/GL.h>
//#include <stdarg.h>			// Header File For Variable Argument Routines
//<stdio.h> and <stdarg.h>
#include <stdio.h>

///////////////////////////
#include "ZedTracker.h"
///////////////////////////

extern GL_Window* g_pWindow; // Main Window

//// Fonts Rendering
void CFonts::BuildFont(void)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping

	base = glGenLists(96);								// Storage For 96 Characters

	/*font = CreateFont(	-24,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						//"Courier New");					// Font Name
						//"Arial Narrow Bold");					// Font Name
						//"Fixed Miriam Transparent");					// Font Name
						"Courier 10,12,15");					// Font Name
						//"Quake");					// Font Name*/


	font = CreateFont(	-24,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						//"Courier New");					// Font Name
						//"Arial Narrow Bold");					// Font Name
						//"Fixed Miriam Transparent");					// Font Name
						//"Elronmonospace");					// Font Name
						//"Megadeth");					// Font Name
						//"Monospace");					// Font Name
						"Monospace Bold");					// Font Name
						//"Monospace Oblique");					// Font Name



	
	oldfont = (HFONT)SelectObject(g_pWindow->hDC, font);           // Selects The Font We Want
	wglUseFontBitmaps(g_pWindow->hDC, 32, 96, base);				// Builds 96 Characters Starting At Character 32
	SelectObject(g_pWindow->hDC, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font
}

void CFonts::KillFont(void)									// Delete The Font List
{
	glDeleteLists(base, 96);							// Delete All 96 Characters
}

void CFonts::glPrint(const char *fmt, ...)					// Custom GL "Print" Routine
{
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base Character to 32
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
}
