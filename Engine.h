#ifndef YOEL_ENGINE_H
#define YOEL_ENGINE_H

#pragma once

#include "Texture.h"
#include "Math3d.h"


#define st_Engine CSingleton<CEngine>
#define st_EngineI CSingleton<CEngine>::Inst()



#define  ENGINE_MESSAGE_NEW_GAME      (1<<1)
#define  ENGINE_MESSAGE_SAVE_GAME     (1<<2)
#define  ENGINE_MESSAGE_LOAD_GAME     (1<<3)
#define  ENGINE_MESSAGE_OPTIONS_MENU  (1<<4)
#define  ENGINE_MESSAGE_QUIT          (1<<5)
#define  ENGINE_MESSAGE_SAVED_GAME_00 (1<<6)
#define  ENGINE_MESSAGE_SAVED_GAME_01 (1<<7)
#define  ENGINE_MESSAGE_SAVED_GAME_02 (1<<8)
#define  ENGINE_MESSAGE_SAVED_GAME_03 (1<<9)
#define  ENGINE_MESSAGE_SAVED_GAME_04 (1<<10)
#define  ENGINE_MESSAGE_SAVED_GAME_05 (1<<11)
#define  ENGINE_MESSAGE_LOAD_GAME_00 (1<<12)
#define  ENGINE_MESSAGE_LOAD_GAME_01 (1<<13)
#define  ENGINE_MESSAGE_LOAD_GAME_02 (1<<14)
#define  ENGINE_MESSAGE_LOAD_GAME_03 (1<<15)
#define  ENGINE_MESSAGE_LOAD_GAME_04 (1<<16)
#define  ENGINE_MESSAGE_LOAD_GAME_05 (1<<17)
#define  ENGINE_MESSAGE_CAST_A_SPELL (1<<18)
#define  ENGINE_MESSAGE_ADVANTAGE_WEAPON     (1<<19)
#define  ENGINE_MESSAGE_ADVANTAGE_EXTRALIFE  (1<<20)
#define  ENGINE_MESSAGE_ADVANTAGE_AMMO       (1<<21)
#define  ENGINE_MESSAGE_START_NETWORK_SERVER (1<<22)
#define  ENGINE_MESSAGE_START_NETWORK_CLIENT (1<<23)
#define  ENGINE_MESSAGE_SEND_CHAT_MESSAGE    (1<<24)
#define  ENGINE_MESSAGE_STOP_CONNECTION     (1<<25)
//#define  ENGINE_MESSAGE_END_GAME (1<<18)

#include "GUI.h"
extern CGUIManager* g_pGUIManager;


///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

class CEngine
{
public:
	////////////////
	// Constructer
	////////////////
	CEngine() {};

	////////////////
	// Destructer
	////////////////
	~CEngine();

	/////////////////
	// InitGL()
	////////////////
	void InitGL(void);

	////////////////
	// Init()
	////////////////
	bool Init(void);

	////////////////
	// Delete()
	////////////////
	void Delete(void);

	void AddEngineMessage(DWORD dwAdd) { m_dwEngineMessages|=dwAdd;};
	

	void AddGUIMessage(DWORD dwADD) {g_pGUIManager->m_dwGUIMessages|=dwADD;};

	void HandleMessages(void);

	

/*	//////////////////////
	// DebugGetLookAtAngles()
	// (FROM (0,0,1)!)
	//////////////////////
	void DebugGetLookAtAngles(CVector3f& const to,float& xAng, float& yAng);*/



	///////////////////
	// debug counters
	///////////////////

	void ResetDebugCounters(void)
	{
		m_iMapModelsDrawn=0;
		m_iSurfacesDrawn=0;
	}

	int m_iMapModelsDrawn;
	int m_iSurfacesDrawn;
	int m_iFrameCounter;

	int m_iCameraClusterNum;
	int m_iCameraLeafNum;

	float m_fHeightAboveGround;

	int   m_iGameState; // 0 - in the board, 1 - in the FPS, 2 - other stuff like menu etc.
	bool  m_bSwitchingGameState;


	DWORD m_dwEngineMessages;

	///////////////////

	float m_fEntireDrawTimerS;
	float m_fEntireDrawTimerD;

	float m_fEntireUpdateTimerS;
	float m_fEntireUpdateTimerD;

	float m_fDrawPart1S;
	float m_fDrawPart1D;

	float m_fDrawPart11S;
	float m_fDrawPart11D;

	float m_fDrawPart12S;
	float m_fDrawPart12D;

	float m_fDrawPart2S;
	float m_fDrawPart2D;

	float m_fDrawPart3S;
	float m_fDrawPart3D;

	float m_fDrawPart4S;
	float m_fDrawPart4D;


};




#endif // YOEL_ENGINE_H





