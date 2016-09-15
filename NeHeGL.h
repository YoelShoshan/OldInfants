/********************
*                   *
*   NeHeGL Header   *
*                   *
**********************************************************************************
*                                                                                *
*	You Need To Provide The Following Functions:                                 *
*                                                                                *
*	BOOL Initialize (GL_Window* window, Keys* keys);                             *
*		Performs All Your Initialization                                         *
*		Returns TRUE If Initialization Was Successful, FALSE If Not              *
*		'window' Is A Parameter Used In Calls To NeHeGL                          *
*		'keys' Is A Structure Containing The Up/Down Status Of keys              *
*                                                                                *
*	void Deinitialize (void);                                                    *
*		Performs All Your DeInitialization                                       *
*                                                                                *
*	void Update (DWORD milliseconds);                                            *
*		Perform Motion Updates                                                   *
*		'milliseconds' Is The Number Of Milliseconds Passed Since The Last Call  *
*		With Whatever Accuracy GetTickCount() Provides                           *
*                                                                                *
*	void Draw (void);                                                            *
*		Perform All Your Scene Drawing                                           *
*                                                                                *
*********************************************************************************/


#ifndef GL_FRAMEWORK__INCLUDED
#define GL_FRAMEWORK__INCLUDED

#pragma once

#include <Windows.h>
#include "NeHeGL.h"
#include "Defines.h"



#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)



// Keys
struct sKeys {										// Structure For Keyboard Stuff
	BOOL keyDown [256];								// Holds TRUE / FALSE For Each Key
};



// Application
class Application {									// Contains Information Vital To Applications
public:	
	HINSTANCE		hInstance;						// Application Instance
	const char*		className;						// Application ClassName

	BOOL RegisterWindowClass();
} ;


class GL_Window {									// Contains Information Vital To A Window
public:	
	sKeys*				keys;						// Key Structure
	HWND				hWnd;						// Window Handle
	HDC					hDC;						// Device Context
	HGLRC				hRC;						// Rendering Context
	//GL_WindowInit		init;						// Window Init
	BOOL				isVisible;					// Window Visible?
	DWORD				lastTickCount;				// Tick Counter

	Application*		application;				// Application Structure
	char*				title;						// Window Title
	int					width;						// Width
	int					height;						// Height
	int					bitsPerPixel;				// Bits Per Pixel
	BOOL				isFullScreen;				// FullScreen?
	/**/
	
	

	//Functions
	BOOL CreateWindowGL();
	BOOL DestroyWindowGL();
	void TerminateApplication();		// Terminate The Application
	void ToggleFullscreen();			// Toggle Fullscreen / Windowed Mode
	void ReshapeGL (int iwidth, int iheight);
		

	BOOL ChangeScreenResolution (int width, int height, int bitsPerPixel);	// Change The Screen Resolution


	// These Are The Function You Must Provide
	BOOL Initialize(bool bFullScreen);					// Performs All Your Initialization
	void Deinitialize (void);			// Performs All Your DeInitialization
	void Draw (void);					// Perform All Your Scene Drawing
	void Update (bool bFullScreen);					// Perform Motion Updates
};										// GL_Window



//void Update (void);					// Perform Motion Updates


#endif												// GL_FRAMEWORK__INCLUDED
