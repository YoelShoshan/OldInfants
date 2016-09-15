#include "SoundManager.h"

#include <iostream>
#include <string.h>


#include "NeHeGL.h"
extern GL_Window* g_pWindow; // Main Window
#include "fmod_errors.h"
#include "fmod.h"

#include "Camera.h"
extern CCamera g_Camera1;

// FSOUND_3D_SetAttributes(int channel, const float *pos, const float *vel);
// FSOUND_3D_SetMinMaxDistance(int channel, float min, float max);

//DLL_API void            F_API FSOUND_3D_Listener_SetAttributes(const float *pos, const float *vel, float fx, float fy, float fz, float tx, float ty, float tz);


CSoundManager::CSoundManager()
{
	for (int i=0;i<SM_MAX_CHANNELS;i++)
	{
		m_ppChannels[i] = new int;
		(*m_ppChannels[i]) = i ;
	}


	m_pBackgroundMusic = NULL;

	m_bInitedCorrectly=false;
}
CSoundManager::~CSoundManager()
{

}

// init fmod a
bool CSoundManager::Init(void)
{
	char strMessage[255] = {0};


	float f = FSOUND_GetVersion();

	if (FSOUND_GetVersion() < FMOD_VERSION) // we need same dll version
	{
		sprintf(strMessage, "Wrong FMOD dll version!  You should be using FMOD %f", FMOD_VERSION);
		MessageBox(HWND_DESKTOP, strMessage, "Error", MB_OK);
		return false;		
	}
	
	FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
	FSOUND_SetMixer(FSOUND_MIXER_AUTODETECT);

	// Initialize the FMod Sound System with good quality sound and a global focus
	if (!FSOUND_Init(44100, 32, 0))
	{
		sprintf(strMessage, "%s", FMOD_ErrorString(FSOUND_GetError()));
		MessageBox(HWND_DESKTOP, strMessage, "Error", MB_OK);
		return false;
	}


	//FSOUND_3D_SetDistanceFactor(3.28f*8.f);
	//FSOUND_3D_SetDistanceFactor(3.28f*8.f*105.f);
	FSOUND_3D_SetDistanceFactor(3.28f*8.f*200.f);


	m_bInitedCorrectly=true;

	return true;
}



bool CSoundManager::LoadSample(const char* pcFileName,FSOUND_SAMPLE*& pSound)
{
	if (!m_bInitedCorrectly)
		return false;

	if(pSound!=NULL)
	{
		FSOUND_Sample_Free(pSound);
		pSound = NULL;
	}

	
	//pSound = FSOUND_Sample_Load(FSOUND_FREE, pcFileName, 0, 0, 0);
	pSound = FSOUND_Sample_Load(FSOUND_FREE, pcFileName, FSOUND_HW3D | FSOUND_FORCEMONO, 0, 0);
	FSOUND_Sample_SetMinMaxDistance(pSound, 4.0f, 10000.0f); 

	if (!pSound)
		return false;

	return true;
}

bool CSoundManager::PlaySample(FSOUND_SAMPLE*& pSound)
{
	if (!m_bInitedCorrectly)
		return false;


	int channel = FSOUND_PlaySound(FSOUND_FREE, pSound);
	FSOUND_SetVolume(channel, 255);	// FixMe
	//FSOUND_SetVolume(channel, 150);	// FixMe
	return true;
}

bool CSoundManager::PlaySample(FSOUND_SAMPLE*& pSound,int iVolume)
{
	if (!m_bInitedCorrectly)
		return false;

	int channel = FSOUND_PlaySound(FSOUND_FREE, pSound);
	FSOUND_SetVolume(channel, iVolume);
	return true;
}

bool CSoundManager::PlaySample3D(FSOUND_SAMPLE*& pSound,CVector3f& vPos,int iVolume)
{
	if (!m_bInitedCorrectly)
		return false;

	//FSOUND_Sample_SetMinMaxDistance(pSound, 4.0f, 10000.0f);    
	//FSOUND_Sample_SetMinMaxDistance(pSound, 20.0f, 100000.0f);    
	FSOUND_Sample_SetMinMaxDistance(pSound, 40.0f, 100000.0f);    
	int channel = FSOUND_PlaySound(FSOUND_FREE, pSound);
	FSOUND_SetVolume(channel, iVolume);
	float Pos[3];
	Pos[0] = vPos.v[0];
	Pos[1] = vPos.v[1];
	Pos[2] = -vPos.v[2];

	/*Pos[0] = 0;
	Pos[1] = 0;
	Pos[2] = 40;*/

	float vel[3] = { 0,0,0 };

	FSOUND_3D_SetAttributes(channel,&Pos[0],&vel[0]);	
	return true;

}


bool CSoundManager::FreeSound(FSOUND_SAMPLE*& pSound)
{
	if (!m_bInitedCorrectly)
		return false;

	if(pSound!=NULL)
	{
		FSOUND_Sample_Free(pSound);
		pSound = NULL;
	}
	return true;
}

// load and play a sound sample
bool CSoundManager::LoadPlaySound(char* pcFileName, FSOUND_SAMPLE*& pSound)
{
	if (!m_bInitedCorrectly)
		return false;

	if(pSound!=NULL)
	{
		FSOUND_Sample_Free(pSound);
		pSound = NULL;
	}


	
	// todo: i might need to fill the length parameter with actuall value...
	//(int index, const char *name_or_data, unsigned int mode, int offset, int length);
	pSound = FSOUND_Sample_Load(FSOUND_FREE, pcFileName, 0, 0, 0);
	
	int channel = FSOUND_PlaySound(FSOUND_FREE, pSound);
	FSOUND_SetVolume(channel, 255);		// FixMe
	return true;
}

// play looped music

bool CSoundManager::LoadStream(char* pcFileName, FSOUND_STREAM*& pStream)
{
	if (!m_bInitedCorrectly)
		return false;

	if(pStream)
	{
		FSOUND_Stream_Close(pStream);
		pStream = NULL;
	}

	pStream = FSOUND_Stream_Open(pcFileName, FSOUND_LOOP_NORMAL, 0,0);

	if (!pStream)
		return false;

	return true;
}

bool CSoundManager::FreeStream(FSOUND_STREAM*& pStream)
{
	if (!m_bInitedCorrectly)
		return false;

	if(pStream)
	{
		FSOUND_Stream_Close(pStream);
		pStream = NULL;
	}

	return true;
}




bool CSoundManager::StopStream(FSOUND_STREAM*& pStream)
{
	if (!m_bInitedCorrectly)
		return false;

	if (pStream)
		FSOUND_Stream_Stop(pStream);

	return true;
}

bool CSoundManager::PlayStream(FSOUND_STREAM*& pStream)
{
	if (!m_bInitedCorrectly)
		return false;

	int channel = FSOUND_Stream_Play(FSOUND_FREE, pStream);
	FSOUND_SetVolume(channel, 255);	// FixMe
	return true;
}
bool CSoundManager::PlayStream(FSOUND_STREAM*& pStream,int iVolume)
{
	if (!m_bInitedCorrectly)
		return false;

	int channel = FSOUND_Stream_Play(FSOUND_FREE, pStream);
	FSOUND_SetVolume(channel, iVolume);	// FixMe

	return true;
}

bool CSoundManager::PlayLoopedMusic(char* pcFileName, FSOUND_STREAM*& pStream, int* channel)
{
	if (!m_bInitedCorrectly)
		return false;

	
	if( (*channel) <0)
	{
		return false;
	}
	
	if(pStream!=NULL)
	{
		FSOUND_Stream_Close(pStream);
		pStream = NULL;
	}

	pStream = FSOUND_Stream_Open(pcFileName, 0, 0,0);
	
	*channel = FSOUND_Stream_Play(FSOUND_FREE, pStream);

	return true;
}

void CSoundManager::Update(void)
{
	if (!m_bInitedCorrectly)
		return;

	FSOUND_Update();
}

void CSoundManager::UpdateListenerPosition(void)
{
	if (!m_bInitedCorrectly)
		return;


	g_Camera1.m_PosZNegated.v[0] = g_Camera1.m_Pos.v[0];
	g_Camera1.m_PosZNegated.v[1] = g_Camera1.m_Pos.v[1];
	g_Camera1.m_PosZNegated.v[2] = -g_Camera1.m_Pos.v[2];

	/*g_Camera1.m_PosZNegated.v[0] = 0.f;
	g_Camera1.m_PosZNegated.v[1] = 0.f;
	g_Camera1.m_PosZNegated.v[2] = 0.f;*/

	g_Camera1.ReturnLookDirectionNormalized();

	g_Camera1.m_LookDirectionZNegated.v[0] = g_Camera1.m_LookDirection.v[0];
	g_Camera1.m_LookDirectionZNegated.v[1] = g_Camera1.m_LookDirection.v[1];
	g_Camera1.m_LookDirectionZNegated.v[2] = -g_Camera1.m_LookDirection.v[2];

	/*g_Camera1.m_LookDirectionZNegated.v[0] = 0;
	g_Camera1.m_LookDirectionZNegated.v[1] = 0;
	g_Camera1.m_LookDirectionZNegated.v[2] = 1;*/


	FSOUND_3D_Listener_SetAttributes(&g_Camera1.m_PosZNegated.v[0],NULL,
		g_Camera1.m_LookDirectionZNegated.v[0],g_Camera1.m_LookDirectionZNegated.v[1],g_Camera1.m_LookDirectionZNegated.v[2],
		0,1,0);

}



