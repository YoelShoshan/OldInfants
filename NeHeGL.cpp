/***********************************************
*                                              *
*    Jeff Molofee's Revised OpenGL Basecode    *
*  Huge Thanks To Maxwell Sayles & Peter Puck  *
*            http://nehe.gamedev.net           *
*                     2001                     *
*                                              *
***********************************************/

#include "NeHeGL.h"													// Header File For The Windows Library
#include <gl\gl.h>			

#include "Input.h"


#include "Singleton.h"

#include <stdio.h>

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

extern char* g_strWindowTitle;
int  g_iTempInt=0;

#include "Input.h"

#include <windows.h>

#include "GUI.h"
extern CGUIManager* g_pGUIManager;

extern char defaultIP[16];

#define WM_TOGGLEFULLSCREEN (WM_USER+1)									// Application Define Message For Toggling

static BOOL g_isProgramLooping;											// Window Creation Loop, For FullScreen/Windowed Toggle																		// Between Fullscreen / Windowed Mode
static BOOL g_createFullScreen;											// If TRUE, Then Create Fullscreen

void GL_Window::TerminateApplication ()							// Terminate The Application
{
	PostMessage (this->hWnd, WM_QUIT, 0, 0);							// Send A WM_QUIT Message
	g_isProgramLooping = FALSE;											// Stop Looping Of The Program
}

void GL_Window::ToggleFullscreen ()								// Toggle Fullscreen/Windowed
{
	PostMessage (this->hWnd, WM_TOGGLEFULLSCREEN, 0, 0);				// Send A WM_TOGGLEFULLSCREEN Message
}


void GL_Window::ReshapeGL (int iwidth, int iheight)
					 
{
	this->width		=	iwidth;
	this->height	=	iheight;
	
	glViewport (0, 0, (GLsizei)(iwidth), (GLsizei)(iheight));				// Reset The Current Viewport


	if (st_Input::Inst())
	{
		st_Input::Inst()->SetScreenWidth( (DWORD) iwidth);
		st_Input::Inst()->SetScreenHeight( (DWORD) iheight);
	}

	if (g_pGUIManager)
		g_pGUIManager->Init(FALSE);
	

/*
	glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity ();													// Reset The Projection Matrix
	
	g_fFrustumZStart=1.f;
	g_fFrustumZEnd=1000.f;
	g_fFOV=45.f;
	
	//gluPerspective (g_fFOV, (GLfloat)(iwidth)/(GLfloat)(iheight),
		g_fFrustumZStart, g_fFrustumZEnd);			// Calculate The Aspect Ratio Of The Window
	

	gluPerspective (45.f, (GLfloat)(iwidth)/(GLfloat)(iheight),
		1.f, 75.f);			// Calculate The Aspect Ratio Of The Window


	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix
	glLoadIdentity ();													// Reset The Modelview Matrix*/




}

BOOL GL_Window::ChangeScreenResolution (int width, int height, int bitsPerPixel)	// Change The Screen Resolution
{
	DEVMODE dmScreenSettings;											// Device Mode
	ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));					// Make Sure Memory Is Cleared
	dmScreenSettings.dmSize				= sizeof (DEVMODE);				// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth		= width;						// Select Screen Width
	dmScreenSettings.dmPelsHeight		= height;						// Select Screen Height
	dmScreenSettings.dmBitsPerPel		= bitsPerPixel;					// Select Bits Per Pixel
	dmScreenSettings.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	if (ChangeDisplaySettings (&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		return FALSE;													// Display Change Failed, Return False
	}
	return TRUE;														// Display Change Was Successful, Return True
}

BOOL GL_Window::CreateWindowGL ()									// This Code Creates Our OpenGL Window
{
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;							// Define Our Window Style
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;						// Define The Window's Extended Style

	PIXELFORMATDESCRIPTOR pfd =											// pfd Tells Windows How We Want Things To Be
	{
		sizeof (PIXELFORMATDESCRIPTOR),									// Size Of This Pixel Format Descriptor
		1,																// Version Number
		PFD_DRAW_TO_WINDOW	|											// Format Must Support Window
		PFD_SUPPORT_OPENGL	|											// Format Must Support OpenGL
		PFD_STEREO			|										// Only supported by few Cards (such as ASUS and ELSA GeForce series, and GF Quadro series) with Stereo-glasses
		PFD_DOUBLEBUFFER,												// Must Support Double Buffering
		PFD_TYPE_RGBA,													// Request An RGBA Format
		this->bitsPerPixel,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,												// Color Bits Ignored
		0,																// No Alpha Buffer
		0,																// Shift Bit Ignored
		0,																// No Accumulation Buffer
		0, 0, 0, 0,														// Accumulation Bits Ignored
		16,																// 16Bit Z-Buffer (Depth Buffer)  
		0,																// No Stencil Buffer
		0,																// No Auxiliary Buffer
		PFD_MAIN_PLANE,													// Main Drawing Layer
		0,																// Reserved
		0, 0, 0															// Layer Masks Ignored
	};

	RECT windowRect = {0, 0, this->width, this->height};	// Define Our Window Coordinates

	GLuint PixelFormat;													// Will Hold The Selected Pixel Format

	if (this->isFullScreen == TRUE)								// Fullscreen Requested, Try Changing Video Modes
	{
		if (this->ChangeScreenResolution (this->width, this->height, this->bitsPerPixel) == FALSE)
		{
			// Fullscreen Mode Failed.  Run In Windowed Mode Instead
			MessageBox (HWND_DESKTOP, "Mode Switch Failed.\nRunning In Windowed Mode.", "Error", MB_OK | MB_ICONEXCLAMATION);
			this->isFullScreen = FALSE;							// Set isFullscreen To False (Windowed Mode)
		}
		else															// Otherwise (If Fullscreen Mode Was Successful)
		{
			windowStyle = WS_POPUP;										// Set The WindowStyle To WS_POPUP (Popup Window)
			windowExtendedStyle |= WS_EX_TOPMOST;						// Set The Extended Window Style To WS_EX_TOPMOST
		}																// (Top Window Covering Everything Else)
	}
	else																// If Fullscreen Was Not Selected
	{
		// Adjust Window, Account For Window Borders
		AdjustWindowRectEx (&windowRect, windowStyle, 0, windowExtendedStyle);
	}

	// Create The OpenGL Window
	this->hWnd = CreateWindowEx (windowExtendedStyle,					// Extended Style
								   this->application->className,	// Class Name
								   this->title,					// Window Title
								   windowStyle,							// Window Style
								   0, 0,								// Window X,Y Position
								   //640, 0,								// Window X,Y Position
								   windowRect.right - windowRect.left,	// Window Width
								   windowRect.bottom - windowRect.top,	// Window Height
								   HWND_DESKTOP,						// Desktop Is Window's Parent
								   0,									// No Menu
								   this->application->hInstance, // Pass The Window Instance
								   this);

	if (this->hWnd == 0)												// Was Window Creation A Success?
	{
		return FALSE;													// If Not Return False
	}

	this->hDC = GetDC (this->hWnd);									// Grab A Device Context For This Window
	if (this->hDC == 0)												// Did We Get A Device Context?
	{
		// Failed
		DestroyWindow (this->hWnd);									// Destroy The Window
		this->hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	PixelFormat = ChoosePixelFormat (this->hDC, &pfd);				// Find A Compatible Pixel Format
	if (PixelFormat == 0)												// Did We Find A Compatible Format?
	{
		// Failed
		ReleaseDC (this->hWnd, this->hDC);							// Release Our Device Context
		this->hDC = 0;												// Zero The Device Context
		DestroyWindow (this->hWnd);									// Destroy The Window
		this->hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	if (SetPixelFormat (this->hDC, PixelFormat, &pfd) == FALSE)		// Try To Set The Pixel Format
	{
		// Failed
		ReleaseDC (this->hWnd, this->hDC);							// Release Our Device Context
		this->hDC = 0;												// Zero The Device Context
		DestroyWindow (this->hWnd);									// Destroy The Window
		this->hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	this->hRC = wglCreateContext (this->hDC);						// Try To Get A Rendering Context
	if (this->hRC == 0)												// Did We Get A Rendering Context?
	{
		// Failed
		ReleaseDC (this->hWnd, this->hDC);							// Release Our Device Context
		this->hDC = 0;												// Zero The Device Context
		DestroyWindow (this->hWnd);									// Destroy The Window
		this->hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	// Make The Rendering Context Our Current Rendering Context
	if (wglMakeCurrent (this->hDC, this->hRC) == FALSE)
	{
		// Failed
		wglDeleteContext (this->hRC);									// Delete The Rendering Context
		this->hRC = 0;												// Zero The Rendering Context
		ReleaseDC (this->hWnd, this->hDC);							// Release Our Device Context
		this->hDC = 0;												// Zero The Device Context
		DestroyWindow (this->hWnd);									// Destroy The Window
		this->hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}


	ShowWindow (this->hWnd, SW_NORMAL);								// Make The Window Visible

	this->isVisible = TRUE;											// Set isVisible To True

	this->ReshapeGL (this->width, this->height);	

	ZeroMemory (this->keys, sizeof (sKeys));							// Clear All Keys

	this->lastTickCount = GetTickCount ();							// Get Tick Count

	return TRUE;														// Window Creating Was A Success
																		// Initialization Will Be Done In WM_CREATE
}

BOOL GL_Window::DestroyWindowGL ()								// Destroy The OpenGL Window & Release Resources
{
	if (this->hWnd != 0)												// Does The Window Have A Handle?
	{	
		if (this->hDC != 0)											// Does The Window Have A Device Context?
		{
			wglMakeCurrent (this->hDC, 0);							// Set The Current Active Rendering Context To Zero
			if (this->hRC != 0)										// Does The Window Have A Rendering Context?
			{
				wglDeleteContext (this->hRC);							// Release The Rendering Context
				this->hRC = 0;										// Zero The Rendering Context

			}
			ReleaseDC (this->hWnd, this->hDC);						// Release The Device Context
			this->hDC = 0;											// Zero The Device Context
		}
		DestroyWindow (this->hWnd);									// Destroy The Window
		this->hWnd = 0;												// Zero The Window Handle
	}

	if (this->isFullScreen)										// Is Window In Fullscreen Mode
	{
		ChangeDisplaySettings (NULL,0);									// Switch Back To Desktop Resolution
		//ShowCursor (TRUE);												// Show The Cursor
	}	
	return TRUE;														// Return True
}

// Process Window Message Callbacks
LRESULT CALLBACK WindowProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Get The Window Context
	GL_Window* window = (GL_Window*)(GetWindowLong (hWnd, GWL_USERDATA));

	switch (uMsg)														// Evaluate Window Message
	{
		case WM_SYSCOMMAND:												// Intercept System Commands
		{
			switch (wParam)												// Check System Calls
			{
				case SC_SCREENSAVE:										// Screensaver Trying To Start?
				case SC_MONITORPOWER:									// Monitor Trying To Enter Powersave?
				return 0;												// Prevent From Happening
			}
			break;														// Exit
		}
		return 0;														// Return

		case WM_CREATE:													// Window Creation
		{
			CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);			// Store Window Structure Pointer
			window = (GL_Window*)(creation->lpCreateParams);
			SetWindowLong (hWnd, GWL_USERDATA, (LONG)(window));
		}
		return 0;														// Return

		case WM_CLOSE:													// Closing The Window
			window->TerminateApplication();								// Terminate The Application
		return 0;														// Return

		case WM_SIZE:													// Size Action Has Taken Place
			switch (wParam)												// Evaluate Size Action
			{
				case SIZE_MINIMIZED:									// Was Window Minimized?
					window->isVisible = FALSE;							// Set isVisible To False
				return 0;												// Return

				case SIZE_MAXIMIZED:									// Was Window Maximized?
					window->isVisible = TRUE;							// Set isVisible To True
					window->ReshapeGL (LOWORD (lParam), HIWORD (lParam));		// Reshape Window - LoWord=Width, HiWord=Height
				return 0;												// Return

				case SIZE_RESTORED:										// Was Window Restored?
					window->isVisible = TRUE;							// Set isVisible To True
					window->ReshapeGL (LOWORD (lParam), HIWORD (lParam));		// Reshape Window - LoWord=Width, HiWord=Height
				return 0;												// Return
			}
		break;															// Break

/*		case WM_KEYDOWN:												// Update Keyboard Buffers For Keys Pressed

			if ((wParam >= 0) && (wParam <= 255))						// Is Key (wParam) In A Valid Range?
			{
				window->keys->keyDown [wParam] = TRUE;					// Set The Selected Key (wParam) To True
				return 0;												// Return
			}
		break;															// Break

		case WM_KEYUP:													// Update Keyboard Buffers For Keys Released
			if ((wParam >= 0) && (wParam <= 255))						// Is Key (wParam) In A Valid Range?
			{
				window->keys->keyDown [wParam] = FALSE;					// Set The Selected Key (wParam) To False
				return 0;												// Return-
			}
		break;															// Break*/

		case WM_LBUTTONDOWN:
			if (st_Input::Inst())
			{
				st_Input::Inst()->AcquireMouse();
				st_Input::Inst()->AcquireKeboard();
				//ShowCursor(false);
			}
		break;

		case WM_TOGGLEFULLSCREEN:										// Toggle FullScreen Mode On/Off
			g_createFullScreen = (g_createFullScreen == TRUE) ? FALSE : TRUE;
			PostMessage (hWnd, WM_QUIT, 0, 0);
		break;															// Break
	}

	return DefWindowProc (hWnd, uMsg, wParam, lParam);					// Pass Unhandled Messages To DefWindowProc
}

BOOL Application::RegisterWindowClass ()						// Register A Window Class For This Application.
{																		// TRUE If Successful
	// Register A Window Class
	WNDCLASSEX windowClass;												// Window Class
	ZeroMemory (&windowClass, sizeof (WNDCLASSEX));						// Make Sure Memory Is Cleared
	windowClass.cbSize			= sizeof (WNDCLASSEX);					// Size Of The windowClass Structure
	windowClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraws The Window For Any Movement / Resizing
	windowClass.lpfnWndProc		= (WNDPROC)(WindowProc);				// WindowProc Handles Messages
	windowClass.hInstance		= this->hInstance;				// Set The Instance
	windowClass.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE);			// Class Background Brush Color
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	windowClass.lpszClassName	= this->className;				// Sets The Applications Classname
	if (RegisterClassEx (&windowClass) == 0)							// Did Registering The Class Fail?
	{
		// NOTE: Failure, Should Never Happen
		MessageBox (HWND_DESKTOP, "RegisterClassEx Failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;													// Return False (Failure)
	}
	return TRUE;														// Return True (Success)
}

// Program Entry (WinMain)
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application			application;									// Application Structure
	GL_Window			window;											// Window Structure
	sKeys				keys;											// Key Structure
	BOOL				isMessagePumpActive;							// Message Pump Active?
	MSG					msg;											// Window Message Structure
	DWORD				tickCount;										// Used For The Tick Counter
	
	// Fill Out Application Data
	application.className = "OpenGL";									// Application Class Name
	application.hInstance = hInstance;									// Application Instance
	
	
	// Fill Out Window
	ZeroMemory (&window, sizeof (GL_Window));							// Make Sure Memory Is Zeroed
	window.keys					= &keys;								// Window Key Structure
	window.application		= &application;							// Window Application
	/**/
	//window.title			= "YOL Engine";			// Window Title
	
	//sprintf(g_strWindowTitle,"blah blah");
	//++g_iTempInt;
	//sprintf(g_strWindowTitle,"blah blah %f",g_fFPS);
	//sprintf(g_strWindowTitle,"YOL Engine");

	g_strWindowTitle = new char[256];

	if (strlen(lpCmdLine)<17 && lpCmdLine[0]!=NULL)
		strcpy(defaultIP,lpCmdLine);		
	else
		strcpy(defaultIP,"");	   //home	
		//strcpy(defaultIP,"127.0.0.1");	   //home	
	    //strcpy(defaultIP,"66.75.230.132"); //moav	
		//strcpy(defaultIP,"62.128.49.64"); //dad 
		//strcpy(defaultIP,"83.228.167.70"); //m0rbac		
		//strcpy(defaultIP,"217.236.166.40"); //dTag
		
	

	sprintf(g_strWindowTitle,"Crimson Engine.");
	
	
	window.title			= g_strWindowTitle;

	window.width			= 1440;									// Window Width
	window.height			= 900;									// Window Height

	//window.width			= 1280;									// Window Width
	//window.height			= 1024;									// Window Height

	window.bitsPerPixel	= 16;									// Bits Per Pixel
	//window.bitsPerPixel	= 32;									// Bits Per Pixel
	window.isFullScreen	= TRUE;									// Fullscreen? (Set To TRUE)

	ZeroMemory (&keys, sizeof (sKeys));									// Zero keys Structure

	// Ask The User If They Want To Start In FullScreen Mode?
	if (MessageBox (HWND_DESKTOP, "Fullscreen Mode?", "Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		window.isFullScreen = FALSE; // Windowed Mode
	}
	else
	{
		window.isFullScreen = TRUE;	// FullScreen Mode
	}

	//window.isFullScreen = FALSE; // Windowed Mode


	// Register A Class For Our Window To Use
	if (application.RegisterWindowClass () == FALSE)					// Did Registering A Class Fail?
	{
		// Failure
		MessageBox (HWND_DESKTOP, "Error Registering Window Class!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;														// Terminate Application
	}

	g_isProgramLooping = TRUE;											// Program Looping Is Set To TRUE
	g_createFullScreen = window.isFullScreen;						// g_createFullScreen Is Set To User Default
	while (g_isProgramLooping)											// Loop Until WM_QUIT Is Received
	{
		// Create A Window
		window.isFullScreen = g_createFullScreen;					// Set Init Param Of Window Creation To Fullscreen?
		if (window.CreateWindowGL () == TRUE)							// Was Window Creation Successful?
		{
			// At This Point We Should Have A Window That Is Setup To Render OpenGL
			if (window.Initialize (window.isFullScreen) == FALSE)					// Call User Intialization
			{			
				// Failure
				window.TerminateApplication ();							// Close Window, This Will Handle The Shutdown
			}
			else														// Otherwise (Start The Message Pump)
			{	// Initialize was a success
				isMessagePumpActive = TRUE;								// Set isMessagePumpActive To TRUE
				

				while (isMessagePumpActive == TRUE)						// While The Message Pump Is Active
				{
					// Success Creating Window.  Check For Window Messages
					if (PeekMessage (&msg, window.hWnd, 0, 0, PM_REMOVE) != 0)
					{
						// Check For WM_QUIT Message
						if (msg.message != WM_QUIT)						// Is The Message A WM_QUIT Message?
						{
							DispatchMessage (&msg);						// If Not, Dispatch The Message
						}
						else											// Otherwise (If Message Is WM_QUIT)
						{
							isMessagePumpActive = FALSE;				// Terminate The Message Pump
						}
					}
					else												// If There Are No Messages
					{
						if (window.isVisible == FALSE)					// If Window Is Not Visible
						{
							WaitMessage ();								// Application Is Minimized Wait For A Message
						}
						else											// If Window Is Visible
						{
							// Process Application Loop
							window.Update (window.isFullScreen);	// Update Input/AI/CD/Physics
							window.Draw ();		// Draw everything
						}
					}
				}														// Loop While isMessagePumpActive == TRUE
			}															// If (Initialize (...

			// Application Is Finished
			window.Deinitialize ();											// User Defined DeInitialization

			window.DestroyWindowGL ();									// Destroy The Active Window
		}
		else															// If Window Creation Failed
		{
			// Error Creating Window
			MessageBox (HWND_DESKTOP, "Error Creating OpenGL Window", "Error", MB_OK | MB_ICONEXCLAMATION);
			g_isProgramLooping = FALSE;									// Terminate The Loop
		}
	}																	// While (isProgramLooping)

	UnregisterClass (application.className, application.hInstance);		// UnRegister Window Class
	return 0;
}																		// End Of WinMain()
