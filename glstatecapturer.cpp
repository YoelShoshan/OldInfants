#include "glstatecapturer.h"
/*
//----------------------------------------------------------------------------------------------------------
CGLStateCapturer::CGLStateCapturer()
{
	//m_hMainWindow=g_GlobalPrefs.hWnd;
	m_hMainWindow=NULL;
}
//----------------------------------------------------------------------------------------------------------
CGLStateCapturer::~CGLStateCapturer()
{
}
//----------------------------------------------------------------------------------------------------------
void CGLStateCapturer::Capture()
{
	HWND hTarget=FindWindow(NULL,"GLStateReceiver");
	if (!hTarget) return;

	m_tGLState.GetValuesFromGL();
	//glGetBooleanv(GL_BLEND,(GLboolean*)&m_tGLState.bGL_BLEND);
	//glGetBooleanv(GL_DEPTH_TEST,(GLboolean*)&m_tGLState.bGL_DEPTH_TEST);
	//...

	
	PostMessage(hTarget,WM_USER+101,(WPARAM)m_hMainWindow,(LPARAM)&m_tGLState);
}
//----------------------------------------------------------------------------------------------------------*/