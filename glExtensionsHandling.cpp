#include "glExtensionsHandling.h"

#include <Windows.h>
#include <gl/GL.h>
#include "NeHeGL.h"
#include "glext.h"

extern GL_Window* g_pWindow; // Main Window


///////////////////////////
#include "ZedTracker.h"
///////////////////////////



// this initializes some variables inside crenderer, returns false if CRITICAL extensions aren't supported
bool HandleExtensions(void)
{

	//FIXME: what about the last extension? it doesn't have " " at the end of it, so it wouldn't be found
	// (maybe this is handled and it DOES have a space in the end of it)
	const char* extensionList = (const char *)glGetString(GL_EXTENSIONS);

	//g_FileManager.WriteStringToFile(extensionList,"extensionlist.txt"); // debug

	//////////////////////
	// Critical extensions
	//////////////////////

	/*
	if (strstr(extensionList,"GL_REALLY_IMPORTANT_ARB ") == NULL)
	{
	MessageBox(g_pWindow->hWnd, "Your video card doesn't support blahblah", "Error", MB_OK);
	return false;
	}
	*/
	
	
	//////////////////////////
	// Non-Critical extensions
	//////////////////////////

	if (strstr(extensionList, "GL_EXT_texture_env_combine ") == NULL)
	{
		MessageBox(g_pWindow->hWnd, "Your video card doesn't support texture env combine", "Error", MB_OK);
	}

	if (strstr(extensionList, "GL_EXT_texture_filter_anisotropic ") == NULL)
	{
		MessageBox(g_pWindow->hWnd, "Your video card doesn't support anisotropic texture filtering", "Error", MB_OK);
	}

	
	if (strstr(extensionList, "GL_ARB_texture_cube_map ") == NULL)
	{
		MessageBox(g_pWindow->hWnd, "Your video card doesn't the GL_ARB_texture_cube_map extension", "Error", MB_OK);		
	}

	return true;
}

// return false if the card doesn't support this
// TODO: since my engine probably works with 1 tex unit maybe I should allow this?
// it couldn't hirt...
bool InitMultiTextureFunctionPointers(void)
{
	// Initialize the multitexturing function pointers
	glActiveTextureARB		 = (PFNGLACTIVETEXTUREARBPROC)	 wglGetProcAddress(	"glActiveTextureARB");
	glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)   wglGetProcAddress("glClientActiveTextureARB");

	// Here we make sure that the functions were loaded properly
	if(!glActiveTextureARB || !glClientActiveTextureARB)
	{
		// Display an error message and quit
		MessageBox(g_pWindow->hWnd, "Your video card doesn't support multitexturing", "Error", MB_OK);
		return false;
	}

/*	glPointParameterfEXT = (PFNGLPOINTPARAMETERFEXTPROC) wglGetProcAddress("glPointParameterfEXT");
	glPointParameterfvEXT = (PFNGLPOINTPARAMETERFVEXTPROC)wglGetProcAddress("glPointParameterfvEXT");

	if (!glPointParameterfEXT || !glPointParameterfvEXT)
	{
		//blah...
	}*/

	return true;
}

void InitCardTexUnitsNum(void)
{
	char* strUnitsNum;
	strUnitsNum = new char[4];
	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &g_iCardTexUnits);
/*	sprintf(strUnitsNum,"%d",g_iCardTexUnits);
		
	g_LogFile.OutPutPlainText("Number of texture units supported:","3","000000",false);
	g_LogFile.OutPutPlainText(strUnitsNum,"3","009900",false);
	g_LogFile.OutPutPlainText(".","3","009900",true);*/

	SafeDeleteArray(strUnitsNum);
}
