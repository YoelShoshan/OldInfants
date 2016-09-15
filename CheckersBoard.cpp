#include "CheckersBoard.h"

#include <assert.h>
#include <crtdbg.h>

#include "Engine.h"

#include "Q3AMap.h"
extern CQuake3BSP* g_pQuake3Map;

#include "NeHeGL.h"
extern GL_Window* g_pWindow; // Main Window

#include "Sky.h"
extern CSky*     g_pSky;

#include <gl/GL.h>

extern int g_iPlayerScore;

#include "Renderer.h"

#include "VertexBuffer.h"
extern CVertexBuffer * g_pvbCharacterFrame;

#include "cgslShader.h"
extern CcgslShader g_cgslShader;

#include "Engine.h"

extern CPlayerClient *g_pCurrentlyProcessedPlayerClient;
extern CPlayerClient *g_pPlayerClient1;
extern CPlayerClient *g_pPlayerClient2; // g_pCurrentlyProcessedPlayerClient only hold the CURRENTLY tested clientplayer


extern CVector3f g_vBoardRayCollision;

extern CTexture* g_pPleaseWaitTex;

#include "ParticleSystem.h"

//#include <fstream.h>
#include <fstream>

#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>

#include "GUI.h"
//CGUIWindow* g_pWindowMainMenuGUI;
extern CGUIManager* g_pGUIManager;


// sound manager
#include "SoundManager.h"
extern CSoundManager g_SoundManager;

#include "Camera.h"
extern CCamera g_Camera1;  // the global camera

#include "Communication.h"
extern CCommunication g_Communication;

#include "LogFile.h"
extern CLogFile g_LogFile;

void CCheckersBoard::InitSounds(void)
{
	m_ppBoardSoundSamples = new FSOUND_SAMPLE*[CB_SOUND_SAMPLES_MAX];
	for (int i=0;i<CB_SOUND_SAMPLES_MAX;i++)
		m_ppBoardSoundSamples[i]=NULL;


	g_SoundManager.LoadSample("ceSounds/jumppad.wav",m_ppBoardSoundSamples[CB_SOUND_SAMPLES_SELECT_UNIT]);
	g_SoundManager.LoadSample("ceSounds/jumppad.wav",m_ppBoardSoundSamples[CB_SOUND_SAMPLES_SELECT_DESTINATION]);
	g_SoundManager.LoadSample("ceSounds/jumppad.wav",m_ppBoardSoundSamples[CB_SOUND_SAMPLES_BURN]);
	g_SoundManager.LoadSample("ceSounds/jumppad.wav",m_ppBoardSoundSamples[CB_SOUND_SAMPLES_COMBO]);
	g_SoundManager.LoadSample("sound/feedback/prepare.wav",m_ppBoardSoundSamples[CB_SOUND_SAMPLES_PREPARE_TO_FIGHT]);
	g_SoundManager.LoadSample("sound/feedback/fight.wav",m_ppBoardSoundSamples[CB_SOUND_SAMPLES_FIGHT]);
	



	m_ppBackgroundStreams = new FSOUND_STREAM*[CB_SOUND_STREAM_MAX];
	for (int i=0;i<CB_SOUND_STREAM_MAX;i++)
		m_ppBackgroundStreams[i]=NULL;

	/*

	#define CB_SOUND_STREAM_INTRO            1
#define CB_SOUND_STREAM_BOARD            2
#define CB_SOUND_STREAM_PRE_BATTLE       3
#define CB_SOUND_STREAM_BATTLE           4
#define CB_SOUND_STREAM_WIN              5
#define CB_SOUND_STREAM_MAX              6

	*/


	g_SoundManager.LoadStream("ceSounds/KineTiK-Deep_Blue.mp3",m_ppBackgroundStreams[CB_SOUND_STREAM_INTRO]);
	//g_SoundManager.LoadStream("ceSounds/KineTiK-Spirits Of The Night.mp3",m_ppBackgroundStreams[CB_SOUND_STREAM_BOARD]);
	g_SoundManager.LoadStream("ceSounds/KineTiK-Deep_Blue.mp3",m_ppBackgroundStreams[CB_SOUND_STREAM_BOARD]);
	
	g_SoundManager.LoadStream("ceSounds/techno.mp3",m_ppBackgroundStreams[CB_SOUND_STREAM_PRE_BATTLE]);
	g_SoundManager.LoadStream("ceSounds/techno.mp3",m_ppBackgroundStreams[CB_SOUND_STREAM_BATTLE]);
	g_SoundManager.LoadStream("ceSounds/KineTiK-Fire & Ice.mp3",m_ppBackgroundStreams[CB_SOUND_STREAM_WIN]);

}

void CCheckersBoard::StopBackgroundMusic(void)
{
	for (int i=0;i<CB_SOUND_STREAM_MAX;i++)
		g_SoundManager.StopStream(m_ppBackgroundStreams[i]);

}

class CGUIManager;
CCheckersBoard::CCheckersBoard()
{

	// sounds

	m_bPlayingWinSong=false;

	
	//
	m_iAdvantagePAST = ADVANTAGE_WEAPON;
	m_iAdvantageFUTURE = ADVANTAGE_WEAPON;

	m_iTurnNum=0;
	m_iCharmedUnitsOwnedByPastNum=0;
	m_iCharmedUnitsOwnedByFutureNum=0;

	m_iParalisedUnitsOwnedByPastNum=0;
	m_iParalisedUnitsOwnedByFutureNum=0;

	m_bShakeBoard=false;

	m_bSpellAlreadyEffected=false;
	m_bSpellParticlesStarted=false;

	m_iChosenSpellTargetNum=0;

	m_bPlayersChanged=false;

	m_iPlayerTypePAST = PLAYER_TYPE_HUMAN;
	m_iPlayerTypeFUTURE = PLAYER_TYPE_PC;


	// fill spells

	// SWITCH PLACES OWN
	m_Spells[SPELL_TYPE_SWITCH_PLACES_OWN].m_iSpell = SPELL_TYPE_SWITCH_PLACES_OWN;
	m_Spells[SPELL_TYPE_SWITCH_PLACES_OWN].m_iEffects = SPELL_EFFECTS_SELF_TWO_REST_OF_OWN_ARMY;
	m_Spells[SPELL_TYPE_SWITCH_PLACES_OWN].m_iRange = 1000;
	m_Spells[SPELL_TYPE_SWITCH_PLACES_OWN].m_iWay = SPELL_WAY_DOESNT_MATTER;

	// SWITCH PLACES ENEMY
	m_Spells[SPELL_TYPE_SWITCH_PLACES_ENEMY].m_iSpell = SPELL_TYPE_SWITCH_PLACES_OWN;
	m_Spells[SPELL_TYPE_SWITCH_PLACES_ENEMY].m_iEffects = SPELL_EFFECTS_ENEMY_TWO;
	m_Spells[SPELL_TYPE_SWITCH_PLACES_ENEMY].m_iRange = 1000;
	m_Spells[SPELL_TYPE_SWITCH_PLACES_ENEMY].m_iWay = SPELL_WAY_DOESNT_MATTER;

	// SWITCH PLACES
	m_Spells[SPELL_TYPE_FIREBALL].m_iSpell = SPELL_TYPE_FIREBALL;
	m_Spells[SPELL_TYPE_FIREBALL].m_iEffects = SPELL_EFFECTS_ENEMY_UNIT;
	m_Spells[SPELL_TYPE_FIREBALL].m_iRange = 1000;
	m_Spells[SPELL_TYPE_FIREBALL].m_iWay = SPELL_WAY_DIRECT_LINE_OF_SIGHT;

	// SPELL_TYPE_SHOCKWAVE
	m_Spells[SPELL_TYPE_SHOCKWAVE].m_iSpell = SPELL_TYPE_SHOCKWAVE;
	m_Spells[SPELL_TYPE_SHOCKWAVE].m_iEffects = SPELL_EFFECTS_AUTO;
	m_Spells[SPELL_TYPE_SHOCKWAVE].m_iRange = 2;
	m_Spells[SPELL_TYPE_SHOCKWAVE].m_iWay = SPELL_WAY_DOESNT_MATTER;

	// SPELL_TYPE_CHARM
	m_Spells[SPELL_TYPE_CHARM].m_iSpell = SPELL_TYPE_CHARM;
	m_Spells[SPELL_TYPE_CHARM].m_iEffects = SPELL_EFFECTS_ENEMY_UNIT;
	m_Spells[SPELL_TYPE_CHARM].m_iRange = 1000; // perhaps i SHOULD make it depend on distance
	m_Spells[SPELL_TYPE_CHARM].m_iWay = SPELL_WAY_DOESNT_MATTER;


	// SPELL_TYPE_PARALYZE
	m_Spells[SPELL_TYPE_PARALYSE].m_iSpell = SPELL_TYPE_PARALYSE;
	m_Spells[SPELL_TYPE_PARALYSE].m_iEffects = SPELL_EFFECTS_ENEMY_UNIT;
	m_Spells[SPELL_TYPE_PARALYSE].m_iRange = 1000; // perhaps i SHOULD make it depend on distance
	m_Spells[SPELL_TYPE_PARALYSE].m_iWay = SPELL_WAY_DOESNT_MATTER;

    //SPELL_TYPE_HEAL
	m_Spells[SPELL_TYPE_HEAL].m_iSpell = SPELL_TYPE_HEAL;
	m_Spells[SPELL_TYPE_HEAL].m_iEffects = SPELL_EFFECTS_SELF_ONE_REST_OF_OWN_ARMY;
	m_Spells[SPELL_TYPE_HEAL].m_iRange = 1000; // perhaps i SHOULD make it depend on distance
	m_Spells[SPELL_TYPE_HEAL].m_iWay = SPELL_WAY_DOESNT_MATTER;



	PreInit();

}

CCheckersBoard::~CCheckersBoard()
{
	Destroy();
	for (int i=0;i<CB_SOUND_SAMPLES_MAX;i++)
		g_SoundManager.FreeSound(m_ppBoardSoundSamples[i]);

	for (int i=0;i<CB_SOUND_STREAM_MAX;i++)
		g_SoundManager.FreeStream(m_ppBackgroundStreams[i]);
}

void CCheckersBoard::PreInit(void)
{	
	m_iTurnNum=0;
	m_iCharmedUnitsOwnedByPastNum=0;
	m_iCharmedUnitsOwnedByFutureNum=0;
	m_iParalisedUnitsOwnedByPastNum=0;
	m_iParalisedUnitsOwnedByFutureNum=0;

	m_bShakeBoard=false;
	m_bSpellAlreadyEffected=false;
	m_bSpellParticlesStarted=false;
	Init(false);
	m_iBoardGameState=GAME_BOARD_STATE_MENU;
	//TriggerNewGameParticles();


	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
			BoardsSquaresUnits[i][j].m_iIndexToPlayersArray = -1;

	m_iChosenSpellTargetNum=0;
}

void CCheckersBoard::Destroy(void)
{
	for (int i=0;i<MAX_UNITS_NUM;i++)
	{
		SafeDelete(m_ppGameUnits[i]);
	}

	SafeDeleteArray(m_ppGameUnits);
}

void CCheckersBoard::Init(bool bTriggerParticleSystems)
{

	bool bStandaloneVersion=true;

	m_iTurnNum=0;
	m_iCharmedUnitsOwnedByPastNum=0;
	m_iCharmedUnitsOwnedByFutureNum=0;
	m_iParalisedUnitsOwnedByPastNum=0;
	m_iParalisedUnitsOwnedByFutureNum=0;
	m_bShakeBoard=false;
	m_bSpellAlreadyEffected=false;
	m_bSpellParticlesStarted=false;

	m_iChosenSpellTargetNum=0;

	if (bTriggerParticleSystems)
		TriggerNewGameParticles();
	


	m_iCurrentSaveGame = 0;
	    
	m_fGamePlaySpeed = 1.f;

	m_iSelectedUnitExperiencePoints = 0;
    m_iSelectedUnitHitPoints = 0;

	m_iComboMovesNum = 0;
	m_ComboMovesVector = new tCoord[COMBO_MOVES_MAXIMUM_NUM];

	m_bInComboState=false;

	m_bCanSelectedUnitKill=false;

	m_iAnimationState = WALK_ANIMATION_FIRST_TURN;

	m_fAnimationT = 0.f;

	m_iBoardGameState=GAME_BOARD_STATE_MENU;

	m_iCurrentPlayerSide = ARMY_SIDE_PAST;


	caPlayerInfoLine[0]=NULL;


	BoardsSquaresAccesablity[0][0] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[2][0] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[4][0] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[6][0] = UNACCESABLE_SQUARE; // which means unaccable square

	BoardsSquaresAccesablity[1][1] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[3][1] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[5][1] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[7][1] = UNACCESABLE_SQUARE; // which means unaccable square

	BoardsSquaresAccesablity[0][2] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[2][2] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[4][2] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[6][2] = UNACCESABLE_SQUARE; // which means unaccable square

	BoardsSquaresAccesablity[1][3] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[3][3] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[5][3] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[7][3] = UNACCESABLE_SQUARE; // which means unaccable square

	BoardsSquaresAccesablity[0][4] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[2][4] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[4][4] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[6][4] = UNACCESABLE_SQUARE; // which means unaccable square

	BoardsSquaresAccesablity[1][5] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[3][5] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[5][5] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[7][5] = UNACCESABLE_SQUARE; // which means unaccable square

	BoardsSquaresAccesablity[0][6] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[2][6] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[4][6] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[6][6] = UNACCESABLE_SQUARE; // which means unaccable square

	BoardsSquaresAccesablity[1][7] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[3][7] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[5][7] = UNACCESABLE_SQUARE; // which means unaccable square
	BoardsSquaresAccesablity[7][7] = UNACCESABLE_SQUARE; // which means unaccable square

	/*****/

	BoardsSquaresAccesablity[1][0] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[3][0] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[5][0] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[7][0] = ACCESABLE_SQUARE; // which means accable square

	BoardsSquaresAccesablity[0][1] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[2][1] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[4][1] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[6][1] = ACCESABLE_SQUARE; // which means accable square

	BoardsSquaresAccesablity[1][2] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[3][2] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[5][2] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[7][2] = ACCESABLE_SQUARE; // which means accable square

	BoardsSquaresAccesablity[0][3] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[2][3] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[4][3] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[6][3] = ACCESABLE_SQUARE; // which means accable square

	BoardsSquaresAccesablity[1][4] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[3][4] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[5][4] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[7][4] = ACCESABLE_SQUARE; // which means accable square

	BoardsSquaresAccesablity[0][5] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[2][5] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[4][5] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[6][5] = ACCESABLE_SQUARE; // which means accable square

	BoardsSquaresAccesablity[1][6] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[3][6] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[5][6] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[7][6] = ACCESABLE_SQUARE; // which means accable square

	BoardsSquaresAccesablity[0][7] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[2][7] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[4][7] = ACCESABLE_SQUARE; // which means accable square
	BoardsSquaresAccesablity[6][7] = ACCESABLE_SQUARE; // which means accable square


	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			BoardsSquaresUnits[i][j].m_iIndexToPlayersArray = -1;
			BoardsSquaresUnits[i][j].m_dwState = UNIT_NOT_SELECTED;
			BoardsSquaresUnits[i][j].m_iUnitType = UNIT_TYPE_SOLDIER;
			BoardsSquaresUnits[i][j].m_bParalised = false;
			//BoardsSquaresUnits[i][j].m_bTriggerDeath=false;
		}

	BoardsSquaresUnits[1][0].m_iIndexToPlayersArray = 0;
	BoardsSquaresUnits[3][0].m_iIndexToPlayersArray = 1;
	BoardsSquaresUnits[5][0].m_iIndexToPlayersArray = 2;
	BoardsSquaresUnits[7][0].m_iIndexToPlayersArray = 3;


	BoardsSquaresUnits[0][1].m_iIndexToPlayersArray = 4;
	BoardsSquaresUnits[2][1].m_iIndexToPlayersArray = 5;
	BoardsSquaresUnits[4][1].m_iIndexToPlayersArray = 6;
	BoardsSquaresUnits[6][1].m_iIndexToPlayersArray = 7;

	BoardsSquaresUnits[1][2].m_iIndexToPlayersArray = 8;
	BoardsSquaresUnits[3][2].m_iIndexToPlayersArray = 9;
	BoardsSquaresUnits[5][2].m_iIndexToPlayersArray = 10;
	BoardsSquaresUnits[7][2].m_iIndexToPlayersArray = 11;

	

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<3;j++)
		{
			BoardsSquaresUnits[i][j].m_iArmySide = ARMY_SIDE_PAST;
		}


	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=SQUARES_NUM_Y-1;j>SQUARES_NUM_Y-4;j--)
		{
			BoardsSquaresUnits[i][j].m_iArmySide = ARMY_SIDE_FUTURE;
		}


	////////////////////////////////////////


	BoardsSquaresUnits[0][5].m_iIndexToPlayersArray = 12;
	BoardsSquaresUnits[2][5].m_iIndexToPlayersArray = 13;
	BoardsSquaresUnits[4][5].m_iIndexToPlayersArray = 14;
	BoardsSquaresUnits[6][5].m_iIndexToPlayersArray = 15;	

	BoardsSquaresUnits[1][6].m_iIndexToPlayersArray = 16;
	BoardsSquaresUnits[3][6].m_iIndexToPlayersArray = 17;
	BoardsSquaresUnits[5][6].m_iIndexToPlayersArray = 18;
	BoardsSquaresUnits[7][6].m_iIndexToPlayersArray = 19;

	BoardsSquaresUnits[0][7].m_iIndexToPlayersArray = 20;
	BoardsSquaresUnits[2][7].m_iIndexToPlayersArray = 21;
	BoardsSquaresUnits[4][7].m_iIndexToPlayersArray = 22;
	BoardsSquaresUnits[6][7].m_iIndexToPlayersArray = 23;

	

	





	fRotateXAng = 0.f;
	fRotateYAng = 0.f;

	m_ppGameUnits = new CEntity*[MAX_UNITS_NUM];

	for (int i=0;i<MAX_UNITS_NUM;i++)
	{
		m_ppGameUnits[i] = new CEntityPlayer;
	}


	m_iNumOfUnits=0;



	// remember that i need to make the board vertex buffer with a shader sort, because it screws up the bones character
	// because of sort values

	// major     orbb     doom    crash     grunt     hunter
	
//////////// FUTURE SIDE //////////////////
	
	
	// first line

	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/klesk/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/klesk/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (-3.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (4.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE   ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_PARALYSE);

	m_iNumOfUnits++;

	//assert(_CrtCheckMemory());

	

	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/razor/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/razor/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (-1.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (4.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_CHARM);
	m_iNumOfUnits++;

	

	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/crash/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/crash/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (1.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (4.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_SHOCKWAVE);
	//((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_CHARM);
	m_iNumOfUnits++;

	
	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (3.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (4.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_HEAL);
	m_iNumOfUnits++;




// second line


	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (-4.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (3.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_SWITCH_PLACES_ENEMY);
	m_iNumOfUnits++;


	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (-2.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (3.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_SWITCH_PLACES_ENEMY);
	m_iNumOfUnits++;


	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (0.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (3.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_SWITCH_PLACES_OWN);
	m_iNumOfUnits++;


	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (2.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (3.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_SWITCH_PLACES_OWN);
	m_iNumOfUnits++;


// third line



	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/sarge/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/sarge/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	//((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/magdalena/");
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (-3.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (2.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_FIREBALL);
	m_iNumOfUnits++;

	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/sarge/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/sarge/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (-1.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (2.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_FIREBALL);
	m_iNumOfUnits++;

	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/sarge/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/sarge/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (1.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (2.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_FIREBALL);
	m_iNumOfUnits++;


	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/sarge/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/sarge/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (3.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (2.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_FIREBALL);
	m_iNumOfUnits++;


	for (int i=0;i<4;i++)
	{
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->SetPlayerWeapon(3);
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f;
	}

	for (int i=4;i<8;i++)
	{
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->SetPlayerWeapon(2);
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f;
	}

	for (int i=8;i<12;i++)
	{
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->SetPlayerWeapon(1);
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f;
	}


	////////////////////// PAST SIDE ////////////////////////////

	// third line



	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/visor/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/visor/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (-4.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-1.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_CHARM);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
	m_iNumOfUnits++;

	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/visor/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/visor/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (-2.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-1.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_CHARM);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
	m_iNumOfUnits++;

	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/visor/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/visor/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (0.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-1.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_SHOCKWAVE);
	//((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_PARALYSE);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
	m_iNumOfUnits++;


	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/visor/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/visor/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (2.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-1.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_SHOCKWAVE);
	//((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_PARALYSE);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
	m_iNumOfUnits++;
		

	// second line


	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (-3.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-2.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_FIREBALL);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
	m_iNumOfUnits++;

	//assert(_CrtCheckMemory());


	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (-1.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-2.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_FIREBALL);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
	m_iNumOfUnits++;


	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (1.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-2.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_FIREBALL);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
	m_iNumOfUnits++;


	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (3.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-2.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_FIREBALL);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
	m_iNumOfUnits++;


	// first line

	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (-4.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-3.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE   ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_SWITCH_PLACES_OWN);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
	m_iNumOfUnits++;

	//assert(_CrtCheckMemory());

	

	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (-2.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-3.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_SWITCH_PLACES_OWN);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
	m_iNumOfUnits++;

	

	
	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (0.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-3.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_SWITCH_PLACES_ENEMY);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
	m_iNumOfUnits++;

	


	if (bStandaloneVersion)
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/major/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/major/");
	}
	else
	{
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/grunt/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/grunt/");
	}
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (2.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-3.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->SetSpellType(SPELL_TYPE_SWITCH_PLACES_ENEMY);
	((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
	m_iNumOfUnits++;









	for (int i=12;i<16;i++)
	{
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->SetPlayerWeapon(1);
		//((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f;
	}

	for (int i=16;i<20;i++)
	{
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->SetPlayerWeapon(2);
		//((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f;
	}

	for (int i=20;i<24;i++)
	{
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->SetPlayerWeapon(3);
		//((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f;
	}



	///// load all the "queens"


	for (int i=24;i<36;i++)
	{
		if (bStandaloneVersion)
			((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/sarge/");
		else
			((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/xaero/");
		
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/xaero/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (2.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-3.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
		m_iNumOfUnits++;
	}

	for (int i=36;i<48;i++)
	{
		if (bStandaloneVersion)
			((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/visor/");
		else
			((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_pPlayerModel->Load("models/players/uriel/");
		
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->LoadSounds("sound/player/uriel/");
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->GetPhysicsModel()->SetSoundSamples(((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_pPlayerClient->m_ppSoundSamples);
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->ForcePosition(CVector3f( (2.f*BOARD_SQUARE_SIZE)+HALF_BOARD_SQUARE_SIZE,35.f, (-3.f* BOARD_SQUARE_SIZE)-HALF_BOARD_SQUARE_SIZE  ));
		((CEntityPlayer*)m_ppGameUnits[m_iNumOfUnits])->m_iAbsoluteEntitiesIndex = m_iNumOfUnits;
		m_iNumOfUnits++;
	}






	for (int i=24;i<36;i++)
	{
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->SetPlayerWeapon(2);
	}

	for (int i=36;i<48;i++)
	{
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->SetPlayerWeapon(3);
	}


	for (int i=0;i<m_iNumOfUnits;i++)
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->m_bReviveUponDeath = false;




	// particle system stuff

	//m_ParticleSystem.Init(PARTICLE_SYSTEM_TYPE_FIRE,400);
	//m_ParticleSystem.Init(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,90);
	//m_ParticleSystem.Init(PARTICLE_SYSTEM_TYPE_FIRE,1);


	
	/*StopBackgroundMusic();
	g_SoundManager.PlayStream(m_ppBackgroundStreams[CB_SOUND_STREAM_BOARD]);*/


	g_Camera1.m_bIgnoreThisFrame=true;
}


bool CCheckersBoard::CheckIfPlayerWon(int& iSide)
{
	bool bPlayerWon=false;

	int m_iPastUnits=0;
	int m_iFutureUnits=0;

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			int iIndex = BoardsSquaresUnits[i][j].m_iIndexToPlayersArray;

			if (iIndex<0)
				continue;

			if (BoardsSquaresUnits[i][j].m_iArmySide==ARMY_SIDE_FUTURE)
				m_iFutureUnits++;
			else
				m_iPastUnits++;
		}


	// past won
	if ( (m_iFutureUnits==0) && (m_iPastUnits>0))
	{
		m_iBoardGameState=GAME_BOARD_STATE_MENU;

		for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			int iIndex = BoardsSquaresUnits[i][j].m_iIndexToPlayersArray;

			if (iIndex<0)
				continue;
			m_ppGameUnits[iIndex]->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kGesturing;
		}

		iSide = ARMY_SIDE_PAST;

		if (!m_bPlayingWinSong)
		{
			m_bPlayingWinSong=true;
			
			StopBackgroundMusic();
			g_SoundManager.PlayStream(m_ppBackgroundStreams[CB_SOUND_STREAM_WIN]);
		}

		return true;
	}
	else
	// future won
	if ( (m_iPastUnits==0) && (m_iFutureUnits>0))
	{

		m_iBoardGameState=GAME_BOARD_STATE_MENU;

		for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			int iIndex = BoardsSquaresUnits[i][j].m_iIndexToPlayersArray;

			if (iIndex<0)
				continue;
			m_ppGameUnits[iIndex]->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kGesturing;			
		}

		iSide = ARMY_SIDE_FUTURE;

		if (!m_bPlayingWinSong)
		{
			m_bPlayingWinSong=true;		
			StopBackgroundMusic();
			g_SoundManager.PlayStream(m_ppBackgroundStreams[CB_SOUND_STREAM_WIN]);
		}
		return true;
	}

	m_bPlayingWinSong=false;
	return false;
}

void CCheckersBoard::GoBackToSelectUnit(void)
{
	m_iBoardGameState = GAME_BOARD_STATE_CHOOSE_UNIT;
	m_iAnimationState = 0;

}

CVector3f CCheckersBoard::GetMiddleOfSquare(int x,int y)
{

	x = (SQUARES_NUM_X-1) - x ;

	float fX = 	(-4.f*BOARD_SQUARE_SIZE)+((float)x)*BOARD_SQUARE_SIZE + HALF_BOARD_SQUARE_SIZE;
    float fZ =  (-4.f*BOARD_SQUARE_SIZE)+((float)y)*BOARD_SQUARE_SIZE + HALF_BOARD_SQUARE_SIZE;

	return CVector3f(fX,10.f,fZ);
}



void CCheckersBoard::UpdateGameUnits()
{

	if (m_iBoardGameState==GAME_BOARD_STATE_CHOOSE_UNIT)
	{
		m_iSelectedUnitExperiencePoints=-1;
		 m_iSelectedUnitHitPoints=-1;

		CGUIWindow* pWin = g_pGUIManager->m_pBoardCharacterMenu;
		CGUIImage* pImage;
		for (int i=0;i<pWin->m_iCurrentElementNum-1;i++)
		{
			pImage = (CGUIImage*) pWin->GetElementNum(i);
			pImage->Deactivate();
		}

		pImage = (CGUIImage*) pWin->GetElementNum(0);
		pImage->Activate();	
	}

	


	bool bBreak=false;

	int iDummy;

	if (m_iBoardGameState==GAME_BOARD_STATE_CHOOSE_UNIT)
		CheckIfPlayerWon(iDummy);
	

	if (m_iBoardGameState==GAME_BOARD_STATE_CHOOSE_TARGET_COMBO)
		m_fComboMoveTimeLeft-=st_Timer::Inst()->GetDeltaTimeFAKE();


	// if combo time count down finished don't allow any more selections
	if ((m_fComboMoveTimeLeft<=0.f) &&(m_iBoardGameState==GAME_BOARD_STATE_CHOOSE_TARGET_COMBO))
	{
		m_iBoardGameState = GAME_BOARD_STATE_SHOWING_COMBO_ANIMATION;
		m_iPlaceInComboMoveAnimation = 0;
				
		m_iSubComboFromX=m_iChosenUnitX;
		m_iSubComboFromY=m_iChosenUnitY;

		m_iSubComboToX=m_ComboMovesVector[0].x;
		m_iSubComboToY=m_ComboMovesVector[0].y;
	}

			

	if (m_iBoardGameState==GAME_BOARD_STATE_CHOOSE_TARGET)
	{
		// we have selected a unit

		if (CanUnitKill(m_iChosenUnitX,m_iChosenUnitY))
			m_bCanSelectedUnitKill=true;
		else
			m_bCanSelectedUnitKill=false;

		/*int iTempIndex = BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray;

		if (iTempIndex>=0)
			m_iSelectedUnitExperiencePoints = m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iExperiencePoints;*/

		
	}

	if (m_iBoardGameState==GAME_BOARD_STATE_MENU)
	{
		int iArmySide;

		if (CheckIfPlayerWon(iArmySide))
		{
			if (iArmySide==ARMY_SIDE_PAST)
				strcpy(&caPlayerInfoLine[0],"PAST WON! Press ""New Game"" To Restart!");
			else
				strcpy(&caPlayerInfoLine[0],"FUTURE WON! Press ""New Game"" To Restart!");
		}
		else
			strcpy(&caPlayerInfoLine[0],"Press ""New Game"" To Start The Battle!");



		for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			int iIndex = BoardsSquaresUnits[i][j].m_iIndexToPlayersArray;

			if (iIndex<0)
				continue;

			((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(i,j)+CVector3f(0.f,25.f,0.f));			
					m_ppGameUnits[iIndex]->Update();
		}


		

		return;
	}

	//assert(_CrtCheckMemory());

	

	/////
	if (m_iCurrentPlayerSide==ARMY_SIDE_PAST)
		strcpy(&caPlayerInfoLine[0],"Past Turn");
	else
		strcpy(&caPlayerInfoLine[0],"Future Turn");

	/////

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			if (bBreak)
				break;

			int iIndex = BoardsSquaresUnits[i][j].m_iIndexToPlayersArray;

			if (iIndex<0 || iIndex>1000)
				continue;

			// if finished dying

			if (m_iBoardGameState!=GAME_BOARD_STATE_CHOOSE_TARGET_COMBO)
			if ( ((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_bFinishedDying )
			{
				// remove from board				
				BoardsSquaresUnits[i][j].m_iIndexToPlayersArray = -1;



				//////
				//m_ParticleSystem.SetDie();
				////
			}
			else
			{
				bool bJustSwitchedAnimationPhase = false;



				// spell spell spell spell spell spell spell spell spell spell spell spell
				if (m_iBoardGameState==GAME_BOARD_STATE_SHOWING_SPELL_ANIMATION &&
					(i==m_iChosenUnitX) && (j==m_iChosenUnitY))
				{
				

					if (m_iAnimationState==SPELL_ANIMATION_PRE_SPELL || m_iAnimationState==SPELL_ANIMATION_SPELL)
					{
						if (m_fAnimationT>=m_fTimeForThisAnimationT)
						m_fAnimationT=m_fTimeForThisAnimationT;					
					}
					else
					if (m_fAnimationT>=1.f)
						m_fAnimationT=1.f;


					float fAddToY;
					if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
						fAddToY=0.f;
					else
						fAddToY=180.f;


					// skip rotating if it doesn't make sense
					
					int iCurrentSpellType = ((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->m_iSpellType;

					// if shockwave update the position (or else shake effect will accumulate...)
					if (iCurrentSpellType==SPELL_TYPE_SHOCKWAVE)
						((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->ForcePosition(GetMiddleOfSquare(i,j)+CVector3f(0.f,25.f,0.f));			


					bool bSkipTurning=false;

					if (m_Spells[iCurrentSpellType].m_iWay==SPELL_WAY_DOESNT_MATTER)
					{
						if (m_iAnimationState==SPELL_ANIMATION_FIRST_TURN)
						{
							bSkipTurning=true;
							m_fAnimationT=1.f;
						}
						else
						if (m_iAnimationState==SPELL_ANIMATION_SECOND_TURN)
						{
							m_fAnimationT=1.f;
						}
					}
					


					//////////

						if (m_iAnimationState==SPELL_ANIMATION_FIRST_TURN && (!bSkipTurning))
						{

							if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray>=0)
								((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kTurning;			

							if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType==UNIT_TYPE_QUEEN)
							{
								if ( (m_iSpellCastUponTarget1X>m_iChosenUnitX) && (m_iSpellCastUponTarget1Y>m_iChosenUnitY))
								{
									// if it was "equal" 180.f
									if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
									{
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f-m_fAnimationT*45.f;
									}
									else
									{
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = m_fAnimationT*(180.f-45.f);
									}
								}
								else
								if ( (m_iSpellCastUponTarget1X<m_iChosenUnitX) && (m_iSpellCastUponTarget1Y>m_iChosenUnitY))
								{
									// if it was "equal" 180.f
									if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
									{
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f+m_fAnimationT*45.f;
									}
									else
									{
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = m_fAnimationT*(180.f+45.f);
	
									}
								}
								else
								if ( (m_iSpellCastUponTarget1X>m_iChosenUnitX) && (m_iSpellCastUponTarget1Y<m_iChosenUnitY))
								{
									// if it was "equal" 180.f
									if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
									{
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f-m_fAnimationT*(180-45.f);
									}
									else
									{
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = m_fAnimationT*(45.f);
	
									}
								}
								else
								if ( (m_iSpellCastUponTarget1X<m_iChosenUnitX) && (m_iSpellCastUponTarget1Y<m_iChosenUnitY))
								{								
								
									// if it was "equal" 180.f
									if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
									{
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f+m_fAnimationT*(180-45.f);
									}
									else
									{
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = -m_fAnimationT*(45.f);
									}
								}
							

							}
							else // soldier
							{
	
								if (m_iChosenUnitX>m_iSpellCastUponTarget1X)
									m_iComboMoveDirection = KILL_DIRECTION_LEFT;
								else
									m_iComboMoveDirection = KILL_DIRECTION_RIGHT;
	

								if  (m_iComboMoveDirection==KILL_DIRECTION_LEFT)
								{
									if (m_iCurrentPlayerSide==ARMY_SIDE_PAST)
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+m_fAnimationT*45.f;
									else
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-m_fAnimationT*45.f;
								}
								else
								{							
									if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+m_fAnimationT*45.f;
									else
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-m_fAnimationT*45.f;
								}
							}

					}
					else
					if (m_iAnimationState==SPELL_ANIMATION_PRE_SPELL)
					{

						// make the unit that casts the spell use the cast_a_spell animation
						// currently i'm using the gesturing animation

						if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray>=0)
								((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kGesturing;			

					}
					else
					if (m_iAnimationState==SPELL_ANIMATION_SPELL)
					{
						// make the unit that casts the spell get back to idle
						if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray>=0)
								((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kIdle;			


						// updating moving particle systems

						if (iCurrentSpellType==SPELL_TYPE_FIREBALL)
						{
							CVector3f vDir;
							vDir = GetMiddleOfSquare(m_iSpellCastUponTarget1X,m_iSpellCastUponTarget1Y) - GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY);
							vDir.Normalize();
							CVector3f v1 = GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY) + (vDir*10.f)+CVector3f(0.f,33.f,0.f);
							v1+=(vDir * ((m_fAnimationT)*280.f));
							m_pMagicPS1->SetParticlePos(&v1,false);
						}
						



					}
					else
					if (m_iAnimationState==SPELL_ANIMATION_SECOND_TURN)
					{
						
						if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType==UNIT_TYPE_QUEEN)
						{
						
						if ( (m_iSpellCastUponTarget1X>m_iChosenUnitX) && (m_iSpellCastUponTarget1Y>m_iChosenUnitY))
							{							
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f-45.f+m_fAnimationT*45.f;								
							}
							else
							if ( (m_iSpellCastUponTarget1X<m_iChosenUnitX) && (m_iSpellCastUponTarget1Y>m_iChosenUnitY))
							{
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f+45.f-m_fAnimationT*45.f;
							}
							else
							if ( (m_iSpellCastUponTarget1X>m_iChosenUnitX) && (m_iSpellCastUponTarget1Y<m_iChosenUnitY))
							{
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 45.f-m_fAnimationT*45.f;
								
							}
							else
							if ( (m_iSpellCastUponTarget1X<m_iChosenUnitX) && (m_iSpellCastUponTarget1Y<m_iChosenUnitY))
							{
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = -45.f+m_fAnimationT*45.f;						
							}


						}
						else // a soldier
						{
								if  (m_iComboMoveDirection==KILL_DIRECTION_RIGHT)
								{
									if (m_iCurrentPlayerSide==ARMY_SIDE_PAST)
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-45.f+m_fAnimationT*45.f;
									else
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+45.f-m_fAnimationT*45.f;
								}
								else
								{							
									if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-45.f+m_fAnimationT*45.f;
									else
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+45.f-m_fAnimationT*45.f;
								}
						}

					}




					// an animation ended

					/*if (m_iAnimationState==SPELL_ANIMATION_PRE_SPELL)
					{
						if (m_fTimeForThisAnimationT>=1.f)
						m_fTimeForThisAnimationT=1.f;					
					}*/


					// starting the particle system coming from the spell caster 700 ms before end of cast_a_spell animation
					
					if (m_iAnimationState==SPELL_ANIMATION_PRE_SPELL )
						if (m_fAnimationT>m_fTimeForThisAnimationT-0.7f)
						//if(!m_pMagicPS1)
						if (m_bSpellParticlesStarted==false)
						{

							if (iCurrentSpellType==SPELL_TYPE_FIREBALL)
							{
								m_bSpellParticlesStarted=true;

								CVector3f vDir;

								vDir = GetMiddleOfSquare(m_iSpellCastUponTarget1X,m_iSpellCastUponTarget1Y) - GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY);
								vDir.Normalize();
							
								CVector3f v1 = GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY) + (vDir*15.f)+CVector3f(0.f,43.f,0.f);
								CParticleSystem* pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE_FASTER,400,
									&v1,false,CVector3f(0.f,0.f,0.f),0.f);
								pPS->SetDelayedStart(700);
							
								float fFireBallTime;
	
								/**/

								// compute how much time for the gesture animation
								int iDifference = abs(m_iSpellCastUponTarget1X-m_iChosenUnitX);

								fFireBallTime=((float) iDifference)*0.25f;
								fFireBallTime+=0.7f;
								/**/



								pPS->SetTimeToDeath(fFireBallTime*1000.f);

								m_pMagicPS1 = pPS;
							}
							/*else
							if (iCurrentSpellType==SPELL_TYPE_SWITCH_PLACES)
							{							

								CVector3f vBackForward;
								//(0.f,0.f,-1.f);
								if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray>=0)
									if (m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_fAngY>10.f)
										vBackForward.Set(0.f,0.f,-1.f);
									else
										vBackForward.Set(0.f,0.f,1.f);



								CVector3f v1 = GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY) + (vBackForward*15.f)+CVector3f(0.f,43.f,0.f);


								CParticleSystem* pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,400,
									&v1,false,CVector3f(0.f,0.f,0.f),0.f);
								//pPS->SetDelayedStart(700);																								
							}*/
						}
					

					if (!m_bSpellAlreadyEffected)
					if (m_iAnimationState==SPELL_ANIMATION_SPELL )
						if (m_fAnimationT>0.5f)
						{
							m_bSpellAlreadyEffected=true;

							if (iCurrentSpellType==SPELL_TYPE_SWITCH_PLACES_OWN || iCurrentSpellType==SPELL_TYPE_SWITCH_PLACES_ENEMY)
							{
								int iTemp = BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray;
								BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray = BoardsSquaresUnits[m_iSpellCastUponTarget2X][m_iSpellCastUponTarget2Y].m_iIndexToPlayersArray;
								BoardsSquaresUnits[m_iSpellCastUponTarget2X][m_iSpellCastUponTarget2Y].m_iIndexToPlayersArray = iTemp;

								iTemp = BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iExperiencePointsBACKUP;
								BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iExperiencePointsBACKUP = BoardsSquaresUnits[m_iSpellCastUponTarget2X][m_iSpellCastUponTarget2Y].m_iExperiencePointsBACKUP;
								BoardsSquaresUnits[m_iSpellCastUponTarget2X][m_iSpellCastUponTarget2Y].m_iExperiencePointsBACKUP = iTemp;								


								iTemp = BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iUnitType;
								BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iUnitType = BoardsSquaresUnits[m_iSpellCastUponTarget2X][m_iSpellCastUponTarget2Y].m_iUnitType;
								BoardsSquaresUnits[m_iSpellCastUponTarget2X][m_iSpellCastUponTarget2Y].m_iUnitType = iTemp;
							}


						}


					if ( ( (m_iAnimationState==SPELL_ANIMATION_PRE_SPELL || m_iAnimationState==SPELL_ANIMATION_SPELL) && m_fAnimationT>=m_fTimeForThisAnimationT) ||
						(m_iAnimationState!=SPELL_ANIMATION_PRE_SPELL & m_iAnimationState!=SPELL_ANIMATION_SPELL &&m_fAnimationT>=1.f))
					{
						

						if (m_iAnimationState==SPELL_ANIMATION_FIRST_TURN)
						{

							m_bSpellAlreadyEffected=false;

							m_iAnimationState=SPELL_ANIMATION_PRE_SPELL;
							m_fAnimationT=0.f;
							
							/*if (iCurrentSpellType==SPELL_TYPE_FIREBALL)
							{*/
							
							// compute how much time for the gesture animation
							// and make the pre-spell stage to last that much.
							int iTempIndex = BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray;
							int iAnimationIndex=m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_pPlayerModel->m_pUpper->GetAnimationByID(TORSO_GESTURE);
							
							m_fTimeForThisAnimationT = m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_pPlayerModel->m_pUpper->m_pMD3Shared->animations[iAnimationIndex].framesPerSecond;
							m_fTimeForThisAnimationT = 1.f / m_fTimeForThisAnimationT;

							int iFramesNum = (m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_pPlayerModel->m_pUpper->m_pMD3Shared->animations[iAnimationIndex].endFrame-
								m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_pPlayerModel->m_pUpper->m_pMD3Shared->animations[iAnimationIndex].startFrame);
							m_fTimeForThisAnimationT*=((float)iFramesNum);									

							/*}
							else
							if (iCurrentSpellType==SPELL_TYPE_SWITCH_PLACES)
							{
								m_fTimeForThisAnimationT=1.f;
							}*/
						}
						else
						if (m_iAnimationState==SPELL_ANIMATION_PRE_SPELL)
						{

							m_iAnimationState=SPELL_ANIMATION_SPELL;
							m_fAnimationT=0.f;

							if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray>=0)
								((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kIdle;			

							if (iCurrentSpellType==SPELL_TYPE_PARALYSE)
							{
								if (m_iCurrentPlayerSide==ARMY_SIDE_PAST)
								{
									m_ParalisedUnitsOwnedByPAST[m_iParalisedUnitsOwnedByPastNum].iTurnParalisedNum = m_iTurnNum;
									m_ParalisedUnitsOwnedByPAST[m_iParalisedUnitsOwnedByPastNum].coord.x = m_iSpellCastUponTarget1X;
									m_ParalisedUnitsOwnedByPAST[m_iParalisedUnitsOwnedByPastNum].coord.y = m_iSpellCastUponTarget1Y;
									m_ParalisedUnitsOwnedByPAST[m_iParalisedUnitsOwnedByPastNum].iPlayerIndex = BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray;

									m_iParalisedUnitsOwnedByPastNum++;
								}
								else
								{
									m_ParalisedUnitsOwnedByFUTURE[m_iParalisedUnitsOwnedByFutureNum].iTurnParalisedNum = m_iTurnNum;
									m_ParalisedUnitsOwnedByFUTURE[m_iParalisedUnitsOwnedByPastNum].coord.x = m_iSpellCastUponTarget1X;
									m_ParalisedUnitsOwnedByFUTURE[m_iParalisedUnitsOwnedByPastNum].coord.y = m_iSpellCastUponTarget1Y;
									m_ParalisedUnitsOwnedByFUTURE[m_iParalisedUnitsOwnedByPastNum].iPlayerIndex = BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray;
									m_iParalisedUnitsOwnedByFutureNum++;
								}

								m_fTimeForThisAnimationT=1.f;

								CVector3f* pv;
								pv = new CVector3f[9];	

								pv[0].Set(GetMiddleOfSquare(m_iSpellCastUponTarget1X,m_iSpellCastUponTarget1Y));


								CParticleSystem* pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,250,&pv[0],false,CVector3f(0.f,10.f,0.f),0.f);
								pPS->SetTimeToDeath(2000.f);

								pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,250,&pv[0],false,CVector3f(0.f,50.f,0.f),0.f);
								pPS->SetTimeToDeath(2000.f);


								SafeDeleteArray(pv);
							}
							else
							if (iCurrentSpellType==SPELL_TYPE_CHARM)
							{
								m_fTimeForThisAnimationT=1.f;

								CVector3f* pv;
								pv = new CVector3f[9];	

								pv[0].Set(GetMiddleOfSquare(m_iSpellCastUponTarget1X,m_iSpellCastUponTarget1Y));


								CParticleSystem* pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,250,&pv[0],false,CVector3f(0.f,10.f,0.f),0.f);
								pPS->SetTimeToDeath(2000.f);

								pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,250,&pv[0],false,CVector3f(0.f,50.f,0.f),0.f);
								pPS->SetTimeToDeath(2000.f);


								SafeDeleteArray(pv);


								// add to the arrays

								if (m_iCurrentPlayerSide==ARMY_SIDE_PAST)
								{
									m_CharmedUnitsOwnedByPAST[m_iCharmedUnitsOwnedByPastNum].iTurnCharmedNum = m_iTurnNum;
									m_CharmedUnitsOwnedByPAST[m_iCharmedUnitsOwnedByPastNum].iPlayerIndex = BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray;

									m_iCharmedUnitsOwnedByPastNum++;
								}
								else
								{
									m_CharmedUnitsOwnedByFUTURE[m_iCharmedUnitsOwnedByFutureNum].iTurnCharmedNum = m_iTurnNum;
									m_CharmedUnitsOwnedByFUTURE[m_iCharmedUnitsOwnedByPastNum].iPlayerIndex = BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray;
									m_iCharmedUnitsOwnedByFutureNum++;
								}
								
								
							}
							else
							if (iCurrentSpellType==SPELL_TYPE_HEAL)
							{
								m_fTimeForThisAnimationT=1.f;

								CVector3f* pv;
								pv = new CVector3f[9];	

								pv[0].Set(GetMiddleOfSquare(m_iSpellCastUponTarget1X,m_iSpellCastUponTarget1Y));


								CParticleSystem* pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,250,&pv[0],false,CVector3f(0.f,10.f,0.f),0.f);
								pPS->SetTimeToDeath(2000.f);

								pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,250,&pv[0],false,CVector3f(0.f,50.f,0.f),0.f);
								pPS->SetTimeToDeath(2000.f);


								SafeDeleteArray(pv);																						
							}
							else
							if (iCurrentSpellType==SPELL_TYPE_SHOCKWAVE)
							{								
								m_fTimeForThisAnimationT = 1.f;
								m_bShakeBoard=true;
							}
							else
							if (iCurrentSpellType==SPELL_TYPE_FIREBALL)
							{
								int iDifference = abs(m_iSpellCastUponTarget1X-m_iChosenUnitX);
								m_fTimeForThisAnimationT=((float) iDifference)*0.25f;
							}
							else
							if (iCurrentSpellType==SPELL_TYPE_SWITCH_PLACES_OWN || iCurrentSpellType==SPELL_TYPE_SWITCH_PLACES_ENEMY)
							{
								m_fTimeForThisAnimationT=2.f;

								CVector3f* pv;
								pv = new CVector3f[9];	

								pv[0].Set(GetMiddleOfSquare(m_iSpellCastUponTarget1X,m_iSpellCastUponTarget1Y));
								pv[1].Set(GetMiddleOfSquare(m_iSpellCastUponTarget2X,m_iSpellCastUponTarget2Y));



								CParticleSystem* pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,250,&pv[0],false,CVector3f(0.f,10.f,0.f),0.f);
								pPS->SetTimeToDeath(2000.f);

								pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,250,&pv[0],false,CVector3f(0.f,50.f,0.f),0.f);
								pPS->SetTimeToDeath(2000.f);

								pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,250,&pv[1],false,CVector3f(0.f,10.f,0.f),0.f);
								pPS->SetTimeToDeath(2000.f);

								pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,250,&pv[1],false,CVector3f(0.f,50.f,0.f),0.f);
								pPS->SetTimeToDeath(2000.f);


								SafeDeleteArray(pv);
							}

							m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray]->m_pPlayerClient->m_iExperiencePoints-=3;
							m_iSelectedUnitExperiencePoints = m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray]->m_pPlayerClient->m_iExperiencePoints;
							
						}
						else
						if (m_iAnimationState==SPELL_ANIMATION_SPELL) // spell effect ended
						{

							m_bShakeBoard=false;



							if (iCurrentSpellType==SPELL_TYPE_HEAL)
							{
								if (BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray)
								{
									m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife+=75;
									if (m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife>100)
										m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife=100;
								}

							}
							else
							if (iCurrentSpellType==SPELL_TYPE_PARALYSE)
							{
								BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_bParalised=true;
							}
							else
							if (iCurrentSpellType==SPELL_TYPE_CHARM)
							{

								if (BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iArmySide==ARMY_SIDE_PAST)
									BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iArmySide=ARMY_SIDE_FUTURE;
								else
									BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iArmySide=ARMY_SIDE_PAST;

								if (BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iUnitType==UNIT_TYPE_SOLDIER)
								{
									m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_fAngY+=180.f;

									if (m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_fAngY>=360.f)
										m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_fAngY-=360.f;
								}

							}
							if (iCurrentSpellType==SPELL_TYPE_SHOCKWAVE)
							{

								for (int x=0;x<SQUARES_NUM_X;x++)
									for (int y=0;y<SQUARES_NUM_Y;y++)
									if (IsOdd(x+y))
									if (BoardsSquaresUnits[x][y].m_iIndexToPlayersArray>=0)
									if ((x!= m_iChosenUnitX) ||(y!= m_iChosenUnitY))
									{
									
									// closest
									if (abs(m_iChosenUnitX-x)==1 && abs(m_iChosenUnitY-y)==1)
										m_ppGameUnits[BoardsSquaresUnits[x][y].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife-=50;

									// midrange
									if (abs(m_iChosenUnitX-x)==0 && abs(m_iChosenUnitY-y)==2)
										m_ppGameUnits[BoardsSquaresUnits[x][y].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife-=25;

									// midrange
									if (abs(m_iChosenUnitX-x)==2 && abs(m_iChosenUnitY-y)==0)
										m_ppGameUnits[BoardsSquaresUnits[x][y].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife-=25;


									// farthest
									if (abs(m_iChosenUnitX-x)==2 && abs(m_iChosenUnitY-y)==2)
										m_ppGameUnits[BoardsSquaresUnits[x][y].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife-=12;

									if (m_ppGameUnits[BoardsSquaresUnits[x][y].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife<1)
									{
										m_ppGameUnits[BoardsSquaresUnits[x][y].m_iIndexToPlayersArray]->m_pPlayerClient->Die();
										m_ppGameUnits[BoardsSquaresUnits[x][y].m_iIndexToPlayersArray]->m_pPlayerClient->PlaySound(CLIENT_SOUND_SAMPLE_DEATH);
									}

									}
								
							}
							else
							if (iCurrentSpellType==SPELL_TYPE_FIREBALL)
							{
								if (BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray>=0)
								{
									m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_bBurning = true;

									m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife-=75;

									if (m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife<1)
									{
										m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->Die();
										m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->PlaySound(CLIENT_SOUND_SAMPLE_DEATH);
									}
									else
										m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->PlaySound(CLIENT_SOUND_SAMPLE_PAIN_75);
								}

								/// make the other player burn			
			
								CVector3f* pv;
								pv = new CVector3f[9];					
					
								pv[0].Set(GetMiddleOfSquare(m_iSpellCastUponTarget1X,m_iSpellCastUponTarget1Y) + CVector3f(HALF_BOARD_SQUARE_SIZE/2.f,0.f,HALF_BOARD_SQUARE_SIZE/2.f));					
								pv[1].Set(GetMiddleOfSquare(m_iSpellCastUponTarget1X,m_iSpellCastUponTarget1Y) + CVector3f(-HALF_BOARD_SQUARE_SIZE*0.3f,10.f,HALF_BOARD_SQUARE_SIZE*0.3f));
								pv[2].Set(GetMiddleOfSquare(m_iSpellCastUponTarget1X,m_iSpellCastUponTarget1Y) + CVector3f(HALF_BOARD_SQUARE_SIZE*0.3f,10.f,-HALF_BOARD_SQUARE_SIZE*0.3f));
								pv[3].Set(GetMiddleOfSquare(m_iSpellCastUponTarget1X,m_iSpellCastUponTarget1Y) + CVector3f(-HALF_BOARD_SQUARE_SIZE*0.3f,10.f,-HALF_BOARD_SQUARE_SIZE*0.3f));


								CParticleSystem* pPS;							

								int iAnimationIndex=m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_pUpper->GetAnimationByID(BOTH_DEATH1);

								float fDeathAnimationTime = 
									(1.f / m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_pUpper->m_pMD3Shared->animations[iAnimationIndex].framesPerSecond);


								int iFramesNum = (m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_pUpper->m_pMD3Shared->animations[iAnimationIndex].endFrame-
													m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_pUpper->m_pMD3Shared->animations[iAnimationIndex].startFrame);

								fDeathAnimationTime*=((float)iFramesNum)*1000.f;

									

								// set the unit on fire
								pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,90, &(m_ppGameUnits[BoardsSquaresUnits[m_iSpellCastUponTarget1X][m_iSpellCastUponTarget1Y].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_vBonePosition),true,CVector3f(0.f,0.f,0.f),0.f);
								pPS->SetTimeToDeath(fDeathAnimationTime);

								// set the square on fire
								pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,50,&pv[0],false,CVector3f(0.f,0.f,0.f),0.f);
								pPS->SetTimeToDeath(fDeathAnimationTime);
					
								pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,50,&pv[1],false,CVector3f(0.f,0.f,0.f),0.f);
								pPS->SetTimeToDeath(fDeathAnimationTime);

								pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,50,&pv[2],false,CVector3f(0.f,0.f,0.f),0.f);
								pPS->SetTimeToDeath(fDeathAnimationTime);

								pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,50,&pv[3],false,CVector3f(0.f,0.f,0.f),0.f);
								pPS->SetTimeToDeath(fDeathAnimationTime); 

								SafeDeleteArray(pv);

							}
							else
							if (iCurrentSpellType==SPELL_TYPE_SWITCH_PLACES_OWN || iCurrentSpellType==SPELL_TYPE_SWITCH_PLACES_ENEMY)
							{
								//
							}

							
							
							m_iAnimationState=SPELL_ANIMATION_SECOND_TURN;
							m_fAnimationT=0.f;


							           								
						}
						else
						if (m_iAnimationState==SPELL_ANIMATION_SECOND_TURN)
						{							
							// change player side
							if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
								m_iCurrentPlayerSide=ARMY_SIDE_PAST;
							else
								m_iCurrentPlayerSide=ARMY_SIDE_FUTURE;	

							m_iTurnNum++;
							TimeHealUnits();

							m_iBoardGameState = GAME_BOARD_STATE_CHOOSE_UNIT;
							ClearBoardSelections();

							m_iAnimationState=0;
							m_fAnimationT=0.f;

							m_bSpellParticlesStarted=false;
						}

						m_fAnimationT=0.f;
					}


					
					m_fAnimationT+= st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;				

				}
				else
				// combo combo combo combo combo combo combo combo combo combo combo combo
				if (m_iBoardGameState==GAME_BOARD_STATE_SHOWING_COMBO_ANIMATION &&
					(i==m_iChosenUnitX) && (j==m_iChosenUnitY))
				{

					if (m_fAnimationT>=1.f)
						m_fAnimationT=1.f;


					CVector3f vAnimatedPosition;

					static float fAddToY;
					if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
						fAddToY=0.f;
					else
						fAddToY=180.f;

					if (m_iAnimationState==WALK_ANIMATION_FIRST_TURN)
					{

						//***//
						m_iSubComboFromX = m_iChosenUnitX;
						m_iSubComboFromY = m_iChosenUnitY;

						m_iSubComboToX = m_ComboMovesVector[0].x;
						m_iSubComboToY = m_ComboMovesVector[0].y;

						if (m_iSubComboFromX>m_iSubComboToX)
							m_iComboMoveDirection = KILL_DIRECTION_LEFT;
						else
							m_iComboMoveDirection = KILL_DIRECTION_RIGHT;

						((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kTurning;			

												

						//if (m_iWalkAnimationRotatingDirection==ROTATING_DIRECTION_RIGHT)
						//if (m_iComboMoveDirection==ROTATING_DIRECTION_RIGHT)
						
						//((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-m_fAnimationT*45.f;
						//((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+m_fAnimationT*45.f;
						

						if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType==UNIT_TYPE_QUEEN)
						{
							if ( (m_iSubComboToX>m_iSubComboFromX) && (m_iSubComboToY>m_iSubComboFromY))
							{
								// if it was "equal" 180.f
								if (BoardsSquaresUnits[m_iSubComboFromX][m_iSubComboFromY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f-m_fAnimationT*45.f;
								}
								else
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = m_fAnimationT*(180.f-45.f);

								}
							}
							else
							if ( (m_iSubComboToX<m_iSubComboFromX) && (m_iSubComboToY>m_iSubComboFromY))
							{
								// if it was "equal" 180.f
								if (BoardsSquaresUnits[m_iSubComboFromX][m_iSubComboFromY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f+m_fAnimationT*45.f;
								}
								else
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = m_fAnimationT*(180.f+45.f);

								}
							}
							else
							if ( (m_iSubComboToX>m_iSubComboFromX) && (m_iSubComboToY<m_iSubComboFromY))
							{
								// if it was "equal" 180.f
								if (BoardsSquaresUnits[m_iSubComboFromX][m_iSubComboFromY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f-m_fAnimationT*(180-45.f);
								}
								else
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = m_fAnimationT*(45.f);

								}
							}
							else
							if ( (m_iSubComboToX<m_iSubComboFromX) && (m_iSubComboToY<m_iSubComboFromY))
							{
								// if it was "equal" 180.f
								if (BoardsSquaresUnits[m_iSubComboFromX][m_iSubComboFromY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f+m_fAnimationT*(180-45.f);
								}
								else
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = -m_fAnimationT*(45.f);

								}
							}
							

						}
						else // soldier
						{
							if  (m_iComboMoveDirection==KILL_DIRECTION_LEFT)
							{
								if (m_iCurrentPlayerSide==ARMY_SIDE_PAST)
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+m_fAnimationT*45.f;
								else
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-m_fAnimationT*45.f;
							}
							else
							{							
								if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+m_fAnimationT*45.f;
								else
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-m_fAnimationT*45.f;
							}
						}

						

					}
					else
					if (m_iAnimationState==WALK_ANIMATION_WALK)
					{
						//((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kWalking;			
						((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kRunning;			

						CVector3f vStart,vEnd;

						vStart = GetMiddleOfSquare(m_iSubComboFromX,m_iSubComboFromY)+CVector3f(0.f,25.f,0.f);
						vEnd = GetMiddleOfSquare(m_iSubComboToX,m_iSubComboToY)+CVector3f(0.f,25.f,0.f);
					

						vAnimatedPosition = vStart+((vEnd-vStart)*m_fAnimationT);


						if ( (m_iSubComboToX>m_iSubComboFromX) && (m_iSubComboToY>m_iSubComboFromY))
							((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180-45.f;
						else
						if ( (m_iSubComboToX<m_iSubComboFromX) && (m_iSubComboToY>m_iSubComboFromY))
							((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180+45.f;
						else
						if ( (m_iSubComboToX>m_iSubComboFromX) && (m_iSubComboToY<m_iSubComboFromY))
							((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 45.f;
						else
						if ( (m_iSubComboToX<m_iSubComboFromX) && (m_iSubComboToY<m_iSubComboFromY))
							((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = -45.f;

												

						// THIS WAS IT
						/*if (m_iComboMoveDirection==KILL_DIRECTION_LEFT)
						{
							if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-45.f;
							else
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+45.f;
						}
						else
						{
							if (m_iCurrentPlayerSide==ARMY_SIDE_PAST)
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-45.f;
							else
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+45.f;							
						}*/
					}
					else
					if (m_iAnimationState==WALK_ANIMATION_SECOND_TURN)
					{
						((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kTurning;			


					if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType==UNIT_TYPE_QUEEN)
					{

						if (m_iComboMovesNum>1)
						{
							m_iSubComboFromX = m_ComboMovesVector[m_iComboMovesNum-2].x;
							m_iSubComboFromY = m_ComboMovesVector[m_iComboMovesNum-2].y;

							m_iSubComboToX = m_ComboMovesVector[m_iComboMovesNum-1].x;
							m_iSubComboToY = m_ComboMovesVector[m_iComboMovesNum-1].y;
						}
						else
						{
							m_iSubComboFromX = m_iChosenUnitX;
							m_iSubComboFromY = m_iChosenUnitY;

							m_iSubComboToX = m_ComboMovesVector[m_iComboMovesNum-1].x;
							m_iSubComboToY = m_ComboMovesVector[m_iComboMovesNum-1].y;

						}
						

						if ( (m_iSubComboToX>m_iSubComboFromX) && (m_iSubComboToY>m_iSubComboFromY))
							{							
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f-45.f+m_fAnimationT*45.f;								
							}
							else
							if ( (m_iSubComboToX<m_iSubComboFromX) && (m_iSubComboToY>m_iSubComboFromY))
							{
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f+45.f-m_fAnimationT*45.f;
							}
							else
							if ( (m_iSubComboToX>m_iSubComboFromX) && (m_iSubComboToY<m_iSubComboFromY))
							{
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 45.f-m_fAnimationT*45.f;							
							}
							else
							if ( (m_iSubComboToX<m_iSubComboFromX) && (m_iSubComboToY<m_iSubComboFromY))
							{
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = -45.f+m_fAnimationT*45.f;						
							}


					}
						else // a soldier
						{
								if  (m_iComboMoveDirection==KILL_DIRECTION_RIGHT)
								{
									if (m_iCurrentPlayerSide==ARMY_SIDE_PAST)
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-45.f+m_fAnimationT*45.f;
									else
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+45.f-m_fAnimationT*45.f;
								}
								else
								{							
									if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-45.f+m_fAnimationT*45.f;
									else
										((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+45.f-m_fAnimationT*45.f;
								}
						}
					}


													

					if (m_fAnimationT>=1.f) // sub animation ended
					{
						if (m_iAnimationState==WALK_ANIMATION_FIRST_TURN)
						{
							m_fAnimationT=0.f;
							m_iAnimationState=WALK_ANIMATION_WALK;
							bJustSwitchedAnimationPhase=true;


							//***//

							m_iSubComboFromX = m_iChosenUnitX;
							m_iSubComboFromY = m_iChosenUnitY;

							m_iSubComboToX = m_ComboMovesVector[0].x;
							m_iSubComboToY = m_ComboMovesVector[0].y;

							if (m_iSubComboFromX>m_iSubComboToX)
									m_iComboMoveDirection = KILL_DIRECTION_LEFT;
								else
									m_iComboMoveDirection = KILL_DIRECTION_RIGHT;
						}
						else
						if (m_iAnimationState==WALK_ANIMATION_WALK)
						{

							if (m_iPlaceInComboMoveAnimation>m_iComboMovesNum-2) // animation done
							{
								m_iAnimationState=WALK_ANIMATION_SECOND_TURN;
								bJustSwitchedAnimationPhase=true;
																
							}
							else
							{											
								m_iSubComboToX = m_ComboMovesVector[m_iPlaceInComboMoveAnimation+1].x;
								m_iSubComboToY = m_ComboMovesVector[m_iPlaceInComboMoveAnimation+1].y;

								m_iSubComboFromX = m_ComboMovesVector[m_iPlaceInComboMoveAnimation].x;
								m_iSubComboFromY = m_ComboMovesVector[m_iPlaceInComboMoveAnimation].y;

								//
								int iIndexTEMP = BoardsSquaresUnits[ (m_iSubComboFromX+m_iSubComboToX)/2][(m_iSubComboFromY+m_iSubComboToY)/2].m_iIndexToPlayersArray;
								/*if (iIndexTEMP>=0)
									((CEntityPlayer*)m_ppGameUnits[iIndexTEMP])->m_pPlayerClient->Die();*/
								//

								if (m_iSubComboFromX>m_iSubComboToX)
									m_iComboMoveDirection = KILL_DIRECTION_LEFT;
								else
									m_iComboMoveDirection = KILL_DIRECTION_RIGHT;

								m_iPlaceInComboMoveAnimation++;
							}
							
							//bJustSwitchedAnimationPhase=true;
							m_fAnimationT=0.f;													
							
						}
						else
						if (m_iAnimationState==WALK_ANIMATION_SECOND_TURN)
						{
							m_fAnimationT=0.f;
							m_iAnimationState=WALK_ANIMATION_FIRST_TURN; // reinit it

							m_iBoardGameState = GAME_BOARD_STATE_CHOOSE_UNIT;

							int iDummy;
							if (CheckIfPlayerWon(iDummy))
								m_iBoardGameState = GAME_BOARD_STATE_MENU;

							st_Engine::Inst()->m_bSwitchingGameState=true;
							bJustSwitchedAnimationPhase=true;

							

							((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kIdle;			

							// change player side
							if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
								m_iCurrentPlayerSide=ARMY_SIDE_PAST;
							else
								m_iCurrentPlayerSide=ARMY_SIDE_FUTURE;

							m_iTurnNum++;
							TimeHealUnits();


							// copy over the chosen unit to final destination

								BoardsSquaresUnits[m_ComboMovesVector[m_iPlaceInComboMoveAnimation].x][m_ComboMovesVector[m_iPlaceInComboMoveAnimation].y].m_iArmySide =
									BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iArmySide;

								BoardsSquaresUnits[m_ComboMovesVector[m_iPlaceInComboMoveAnimation].x][m_ComboMovesVector[m_iPlaceInComboMoveAnimation].y].m_iIndexToPlayersArray =
									BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray;

								BoardsSquaresUnits[m_ComboMovesVector[m_iPlaceInComboMoveAnimation].x][m_ComboMovesVector[m_iPlaceInComboMoveAnimation].y].m_iUnitType =
									BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType;

												
								// add experience points to the unit
								int iIndexTEMP = BoardsSquaresUnits[m_ComboMovesVector[m_iPlaceInComboMoveAnimation].x][m_ComboMovesVector[m_iPlaceInComboMoveAnimation].y].m_iIndexToPlayersArray;

								m_ppGameUnits[iIndexTEMP]->m_pPlayerClient->m_iExperiencePoints+=m_iComboMovesNum;

								// delete the starting place


								if (m_ComboMovesVector[m_iComboMovesNum-1].x!=m_iChosenUnitX ||
									m_ComboMovesVector[m_iComboMovesNum-1].y!=m_iChosenUnitY)
								{
									BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray=-1;
									BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType=0;
								}

								// eliminate all the enemies between


								/*BoardsSquaresUnits[ (m_ComboMovesVector[0].x + m_iChosenUnitX )/2  ][(m_ComboMovesVector[0].y + m_iChosenUnitY )/2].m_iIndexToPlayersArray = -1;

								for (int i=0;i<m_iComboMovesNum-1;i++)								
								{
									if ( ((m_ComboMovesVector[i].x + m_ComboMovesVector[i+1].x )/2 !=m_iChosenUnitX) &&
										((m_ComboMovesVector[i].y + m_ComboMovesVector[i+1].y )/2 !=m_iChosenUnitY))
										BoardsSquaresUnits[ (m_ComboMovesVector[i].x + m_ComboMovesVector[i+1].x )/2  ][(m_ComboMovesVector[i].y + m_ComboMovesVector[i+1].y )/2].m_iIndexToPlayersArray = -1;
								}*/

								


							ClearBoardSelections();
						}                     

					} // if (sub animation ended)


					if (!bJustSwitchedAnimationPhase)
						m_fAnimationT+= st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f*m_fGamePlaySpeed;
						//m_fAnimationT+= st_Timer::Inst()->GetDeltaTimeFAKE()*0.002f;
						//m_fAnimationT+= st_Timer::Inst()->GetDeltaTimeFAKE()*0.003f;
					else
						m_fAnimationT=0.f;


					
					if (bJustSwitchedAnimationPhase)
					{
						if (m_iAnimationState==WALK_ANIMATION_WALK)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY)+CVector3f(0.f,25.f,0.f));			
						else
						if (m_iAnimationState==WALK_ANIMATION_SECOND_TURN)
						{
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(m_ComboMovesVector[m_iComboMovesNum-1].x,m_ComboMovesVector[m_iComboMovesNum-1].y)+CVector3f(0.f,25.f,0.f));			
							vAnimatedPosition = GetMiddleOfSquare(m_iSubComboToX,m_iSubComboToY)+CVector3f(0.f,25.f,0.f);
						}
					}
					else
					{
						if (m_iAnimationState==WALK_ANIMATION_FIRST_TURN)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY)+CVector3f(0.f,25.f,0.f));			
						else
						if (m_iAnimationState==WALK_ANIMATION_WALK)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(vAnimatedPosition);			
						else
						if (m_iAnimationState==WALK_ANIMATION_SECOND_TURN)
						{
							vAnimatedPosition = GetMiddleOfSquare(m_iSubComboToX,m_iSubComboToY)+CVector3f(0.f,25.f,0.f);
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(m_ComboMovesVector[m_iComboMovesNum-1].x,m_ComboMovesVector[m_iComboMovesNum-1].y)+CVector3f(0.f,25.f,0.f));			
						}
						//((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(vAnimatedPosition);							
					}

					
					
					
				
					//m_ppGameUnits[iIndex]->Update();
					
				}
				else
				// walk walk walk walk walk walk walk walk walk walk walk walk walk walk walk walk walk 
				if (m_iBoardGameState==GAME_BOARD_STATE_SHOWING_WALK_ANIMATION &&
					(i==m_iChosenUnitX) && (j==m_iChosenUnitY))
				{					

					if (m_fAnimationT>=1.f)
						m_fAnimationT=1.f;


					CVector3f vAnimatedPosition;

					static float fAddToY;
					if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
						fAddToY=0.f;
					else
						fAddToY=180.f;

					if (m_iAnimationState==WALK_ANIMATION_FIRST_TURN)
					{
						((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kTurning;			

						
						if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType==UNIT_TYPE_QUEEN)
						{
							if ( (m_iTargetX>m_iChosenUnitX) && (m_iTargetY>m_iChosenUnitY))
							{
								// if it was "equal" 180.f
								if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f-m_fAnimationT*45.f;
								}
								else
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = m_fAnimationT*(180.f-45.f);

								}
							}
							else
							if ( (m_iTargetX<m_iChosenUnitX) && (m_iTargetY>m_iChosenUnitY))
							{
								// if it was "equal" 180.f
								if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f+m_fAnimationT*45.f;
								}
								else
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = m_fAnimationT*(180.f+45.f);

								}
							}
							else
							if ( (m_iTargetX>m_iChosenUnitX) && (m_iTargetY<m_iChosenUnitY))
							{
								// if it was "equal" 180.f
								if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f-m_fAnimationT*(180-45.f);
								}
								else
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = m_fAnimationT*(45.f);

								}
							}
							else
							if ( (m_iTargetX<m_iChosenUnitX) && (m_iTargetY<m_iChosenUnitY))
							{
								// if it was "equal" 180.f
								if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f+m_fAnimationT*(180-45.f);
								}
								else
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = -m_fAnimationT*(45.f);

								}
							}
							

						}
						else // soldier
						{
							if (m_iWalkAnimationRotatingDirection==ROTATING_DIRECTION_LEFT)
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+m_fAnimationT*45.f;
							else
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-m_fAnimationT*45.f;						
						}


					}
					else
					if (m_iAnimationState==WALK_ANIMATION_WALK)
					{
						//((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kWalking;			
						((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kRunning;			

						CVector3f vStart,vEnd;

						vStart = GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY)+CVector3f(0.f,25.f,0.f);
						vEnd = GetMiddleOfSquare(m_iTargetX,m_iTargetY)+CVector3f(0.f,25.f,0.f);
					

						vAnimatedPosition = vStart+((vEnd-vStart)*m_fAnimationT);
					}
					else
					if (m_iAnimationState==WALK_ANIMATION_SECOND_TURN)
					{
						((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kTurning;			

						if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType==UNIT_TYPE_QUEEN)
						{
							if ( (m_iTargetX>m_iChosenUnitX) && (m_iTargetY>m_iChosenUnitY))
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f-45.f+m_fAnimationT*45.f;
							else
							if ( (m_iTargetX<m_iChosenUnitX) && (m_iTargetY>m_iChosenUnitY))
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f+45-m_fAnimationT*45.f;
							else
							if ( (m_iTargetX>m_iChosenUnitX) && (m_iTargetY<m_iChosenUnitY))
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 45-m_fAnimationT*45.f;
							else
							if ( (m_iTargetX<m_iChosenUnitX) && (m_iTargetY<m_iChosenUnitY))
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = -45+m_fAnimationT*45.f;


						}
						else // soldier
						{

							if (m_iWalkAnimationRotatingDirection==ROTATING_DIRECTION_RIGHT)
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-45.f+m_fAnimationT*45.f;
							else
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+45.f-m_fAnimationT*45.f;
						}
					}


													

					if (m_fAnimationT>=1.f) // sub animation ended
					{
						if (m_iAnimationState==WALK_ANIMATION_FIRST_TURN)
						{
							m_fAnimationT=0.f;
							m_iAnimationState=WALK_ANIMATION_WALK;
							bJustSwitchedAnimationPhase=true;
						}
						else
						if (m_iAnimationState==WALK_ANIMATION_WALK)
						{
							// switch back player animation to kIdle
							
							m_iAnimationState=WALK_ANIMATION_SECOND_TURN;
							bJustSwitchedAnimationPhase=true;

							m_fAnimationT=0.f;
							
						}
						else
						if (m_iAnimationState==WALK_ANIMATION_SECOND_TURN)
						{
							m_fAnimationT=0.f;
							m_iAnimationState=WALK_ANIMATION_FIRST_TURN; // reinit it

							m_iBoardGameState = GAME_BOARD_STATE_CHOOSE_UNIT;
							st_Engine::Inst()->m_bSwitchingGameState=true;
							bJustSwitchedAnimationPhase=true;

							((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kIdle;			

							// change player side
							if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
								m_iCurrentPlayerSide=ARMY_SIDE_PAST;
							else
								m_iCurrentPlayerSide=ARMY_SIDE_FUTURE;

							m_iTurnNum++;
							TimeHealUnits();



							BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_iIndexToPlayersArray = BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray;
							BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_iArmySide = BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iArmySide;

							BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_iUnitType = BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType;
				

							BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iArmySide = -1;
							BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray = -1;

							ClearBoardSelections();
						}                     

					} // if (sub animation ended)


					if (!bJustSwitchedAnimationPhase)
					{
						
						if ((m_iAnimationState==WALK_ANIMATION_WALK) && (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType==UNIT_TYPE_QUEEN))
						{
							int iDifferenceX = abs(m_iTargetX-m_iChosenUnitX);
							m_fAnimationT+= st_Timer::Inst()->GetDeltaTimeFAKE()*0.003f / ((float) iDifferenceX) * m_fGamePlaySpeed;
						}
						else // regular soldier
							m_fAnimationT+= st_Timer::Inst()->GetDeltaTimeFAKE()*0.002f * m_fGamePlaySpeed;
					}
					else
						m_fAnimationT=0.f;


					if (bJustSwitchedAnimationPhase)
					{
						if (m_iAnimationState==WALK_ANIMATION_WALK)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY)+CVector3f(0.f,25.f,0.f));			
						else
						if (m_iAnimationState==WALK_ANIMATION_SECOND_TURN)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(m_iTargetX,m_iTargetY)+CVector3f(0.f,25.f,0.f));			
					}
					else
					{
						if (m_iAnimationState==WALK_ANIMATION_FIRST_TURN)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY)+CVector3f(0.f,25.f,0.f));			
						else
						if (m_iAnimationState==WALK_ANIMATION_WALK)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(vAnimatedPosition);			
						else
						if (m_iAnimationState==WALK_ANIMATION_SECOND_TURN)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(m_iTargetX,m_iTargetY)+CVector3f(0.f,25.f,0.f));			
						//((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(vAnimatedPosition);							
					}

					
				
					//m_ppGameUnits[iIndex]->Update();
					
				}
				else // fight fight fight fight fight fight fight fight fight fight fight 
				if (m_iBoardGameState==GAME_BOARD_STATE_SHOWING_FIGHT_ANIMATION &&
					(i==m_iChosenUnitX) && (j==m_iChosenUnitY))
				{					

					if (m_fAnimationT>=1.f)
						m_fAnimationT=1.f;
                    

					CVector3f vAnimatedPosition;

					static float fAddToY;
					if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
						fAddToY=0.f;
					else
						fAddToY=180.f;

					if (m_iAnimationState==FIGHT_ANIMATION_FIRST_TURN)
					{
						((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kTurning;			

						
						if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType==UNIT_TYPE_QUEEN)
						{
							if ( (m_iTargetX>m_iChosenUnitX) && (m_iTargetY>m_iChosenUnitY))
							{
								// if it was "equal" 180.f
								if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f-m_fAnimationT*45.f;
								}
								else
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = m_fAnimationT*(180.f-45.f);

								}
							}
							else
							if ( (m_iTargetX<m_iChosenUnitX) && (m_iTargetY>m_iChosenUnitY))
							{
								// if it was "equal" 180.f
								if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f+m_fAnimationT*45.f;
								}
								else
								{
									//((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = m_fAnimationT*(180.f+45.f);
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = m_fAnimationT*(-45.f-90.f);

								}
							}
							else
							if ( (m_iTargetX>m_iChosenUnitX) && (m_iTargetY<m_iChosenUnitY))
							{
								// if it was "equal" 180.f
								if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f-m_fAnimationT*(180-45.f);
								}
								else
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = m_fAnimationT*(45.f);

								}
							}
							else
							if ( (m_iTargetX<m_iChosenUnitX) && (m_iTargetY<m_iChosenUnitY))
							{
								// if it was "equal" 180.f
								if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP<181.f && BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP>179.f)
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f+m_fAnimationT*(180-45.f);
									//((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = -45.f+45.f*m_fAnimationT;
								}
								else
								{
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = -m_fAnimationT*(45.f);
									//((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = -45.f+45.f*m_fAnimationT;
								}
							}
							

						}
						else // soldier
						{					
							if (m_iWalkAnimationRotatingDirection==ROTATING_DIRECTION_LEFT)
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+m_fAnimationT*45.f;
							else
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-m_fAnimationT*45.f;
						}

					}
					else
					if (m_iAnimationState==FIGHT_ANIMATION_WALK)
					{
						((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kWalking;			
						//((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kRunning;			

						CVector3f vStart,vEnd;

						vStart = GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY)+CVector3f(0.f,25.f,0.f);
						vEnd = GetMiddleOfSquare(m_iTargetX,m_iTargetY)+CVector3f(0.f,25.f,0.f);
					
						int iDistance = abs(m_iTargetX-m_iChosenUnitX);

						float fMultiply = 0.1f + ((float)iDistance)*0.1f;

						if (fMultiply>0.9f)
							fMultiply=0.9f;


						vAnimatedPosition = vStart+((vEnd-vStart)*(m_fAnimationT*fMultiply));
					}
					else
					if (m_iAnimationState==FIGHT_ANIMATION_SECOND_TURN)
					{
						((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kTurning;			
						
						if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType==UNIT_TYPE_QUEEN)
						{
							if ( (m_iTargetX>m_iChosenUnitX) && (m_iTargetY>m_iChosenUnitY))
							{
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f-45.f+m_fAnimationT*45.f;
							}
							else
							if ( (m_iTargetX<m_iChosenUnitX) && (m_iTargetY>m_iChosenUnitY))
							{
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f+45.f-m_fAnimationT*45.f;
							}
							else
							if ( (m_iTargetX>m_iChosenUnitX) && (m_iTargetY<m_iChosenUnitY))
							{
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = 45.f-m_fAnimationT*(45.f);
							}
							else
							if ( (m_iTargetX<m_iChosenUnitX) && (m_iTargetY<m_iChosenUnitY))
							{
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = -45.f+m_fAnimationT*(45.f);
							}
							

						}
						else // soldier
						{
							if (m_iWalkAnimationRotatingDirection==ROTATING_DIRECTION_RIGHT)
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY-45.f+m_fAnimationT*45.f;
							else
								((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_pPlayerModel->m_fAngY = fAddToY+45.f-m_fAnimationT*45.f;
						}
					}
					else
					if (m_iAnimationState==FIGHT_ANIMATION_POST_FIGHT_WALK)
					{
						((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kWalking;			

						((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kWalking;			

						CVector3f vStart,vEnd;

						vStart = GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY)+CVector3f(0.f,25.f,0.f);
						vEnd = GetMiddleOfSquare(m_iTargetX,m_iTargetY)+CVector3f(0.f,25.f,0.f);
					

						vAnimatedPosition = vStart+((vEnd-vStart)*(m_fAnimationT*0.5f))+((vEnd-vStart)*(0.5f));

					}



					// if fight ended
					if ( (m_iAnimationState==FIGHT_ANIMATION_POST_FIGHT_WALK) && (m_fAnimationT==0.f))
					{

						StopBackgroundMusic();
						g_SoundManager.PlayStream(m_ppBackgroundStreams[CB_SOUND_STREAM_BOARD]);

						///////

							int iDifferenceX,iDifferenceY;
									iDifferenceX = m_iTargetX - m_iChosenUnitX;
									iDifferenceY = m_iTargetY - m_iChosenUnitY;

									if (iDifferenceX>0)
										iDifferenceX = -1;
									else
										iDifferenceX = 1;

									if (iDifferenceY>0)
										iDifferenceY = -1;
									else
										iDifferenceY = 1;

									m_iFightAgainstX = iDifferenceX+m_iTargetX;
									m_iFightAgainstY = iDifferenceY+m_iTargetY;

						///////

						CVector3f v1;

						v1 = GetMiddleOfSquare(m_iFightAgainstX,m_iFightAgainstY) + CVector3f(0.f,3.f,0.f);
						CParticleSystem* pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_SMOKE,30,
							&v1,false,CVector3f(0.f,0.f,0.f),0.f);
						pPS->SetTimeToDeath(1000.f);

						v1 = GetMiddleOfSquare(m_iFightAgainstX,m_iFightAgainstY) + CVector3f(0.f,3.f,0.f) + CVector3f(-HALF_BOARD_SQUARE_SIZE*0.4f,0.f,-HALF_BOARD_SQUARE_SIZE*0.4f);
						pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_SMOKE,30,
							&v1,false,CVector3f(0.f,0.f,0.f),0.f);
						pPS->SetTimeToDeath(1000.f);

						v1 = GetMiddleOfSquare(m_iFightAgainstX,m_iFightAgainstY) + CVector3f(0.f,3.f,0.f) + CVector3f(HALF_BOARD_SQUARE_SIZE*0.4f,0.f,HALF_BOARD_SQUARE_SIZE*0.4f);
						pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_SMOKE,30,
							&v1,false,CVector3f(0.f,0.f,0.f),0.f);
						pPS->SetTimeToDeath(1000.f);

						v1 = GetMiddleOfSquare(m_iFightAgainstX,m_iFightAgainstY) + CVector3f(0.f,3.f,0.f) + CVector3f(-HALF_BOARD_SQUARE_SIZE*0.4f,0.f,HALF_BOARD_SQUARE_SIZE*0.4f);
						pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_SMOKE,30,
							&v1,false,CVector3f(0.f,0.f,0.f),0.f);
						pPS->SetTimeToDeath(1000.f);

						v1 = GetMiddleOfSquare(m_iFightAgainstX,m_iFightAgainstY) + CVector3f(0.f,3.f,0.f) + CVector3f(HALF_BOARD_SQUARE_SIZE*0.4f,0.f,-HALF_BOARD_SQUARE_SIZE*0.4f);
						pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_SMOKE,30,
							&v1,false,CVector3f(0.f,0.f,0.f),0.f);
						pPS->SetTimeToDeath(1000.f);





						for (int i=0;i<m_iNumOfUnits;i++)
						{
							if (((CEntityPlayer*)m_ppGameUnits[i]))
								if (((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient)
									((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->m_bReviveUponDeath = false;
						}

						// after fight ended, i put the correct army weapon to each team
						
						/*if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray>=0)
						{
							if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iArmySide==ARMY_SIDE_PAST)							
								((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->SetPlayerWeapon(2);
							else
								((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->SetPlayerWeapon(3);
				
						}*/


						
						iDifferenceX = m_iTargetX - m_iChosenUnitX;
						iDifferenceY = m_iTargetY - m_iChosenUnitY;

						if (iDifferenceX>0)
							iDifferenceX = -1;
						else
							iDifferenceX = 1;

						if (iDifferenceY>0)
							iDifferenceY = -1;
						else
							iDifferenceY = 1;


						/*if (BoardsSquaresUnits[iDifferenceX + m_iTargetX][iDifferenceY + m_iTargetY].m_iIndexToPlayersArray>=0)
						{
							if (BoardsSquaresUnits[iDifferenceX + m_iTargetX][iDifferenceY + m_iTargetY].m_iArmySide==ARMY_SIDE_PAST)
								((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[iDifferenceX + m_iTargetX][iDifferenceY + m_iTargetY].m_iIndexToPlayersArray])->m_pPlayerClient->SetPlayerWeapon(2);
							else
								((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[iDifferenceX + m_iTargetX][iDifferenceY + m_iTargetY].m_iIndexToPlayersArray])->m_pPlayerClient->SetPlayerWeapon(3);

						}*/


						/*if (BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_iIndexToPlayersArray>=0)
						{
							if (BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_iArmySide==ARMY_SIDE_PAST)
								((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_iIndexToPlayersArray])->m_pPlayerClient->SetPlayerWeapon(2);
							else
								((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_iIndexToPlayersArray])->m_pPlayerClient->SetPlayerWeapon(3);

						}*/

						



						if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray>=0)
						{
							((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->m_pPlayerModel->m_fAngY =
								BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP;

							((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->m_pPlayerModel->m_fAngX = 0;
						}

						if (BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_iIndexToPlayersArray>=0)
						{
							((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_iIndexToPlayersArray])->m_pPlayerClient->m_pPlayerModel->m_fAngY =
								BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_fYAngBACKUP;

							((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_iIndexToPlayersArray])->m_pPlayerClient->m_pPlayerModel->m_fAngX = 0;
						}

						if (BoardsSquaresUnits[iDifferenceX + m_iTargetX][iDifferenceY + m_iTargetY].m_iIndexToPlayersArray>=0)
						{
							((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[iDifferenceX + m_iTargetX][iDifferenceY + m_iTargetY].m_iIndexToPlayersArray])->m_pPlayerClient->m_pPlayerModel->m_fAngY =
								BoardsSquaresUnits[iDifferenceX + m_iTargetX][iDifferenceY + m_iTargetY].m_fYAngBACKUP;

							((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[iDifferenceX + m_iTargetX][iDifferenceY + m_iTargetY].m_iIndexToPlayersArray])->m_pPlayerClient->m_pPlayerModel->m_fAngX = 0;
						}


						//pc vs. pc

							

						// human vs. pc || pc vs. human


						// current player won
						if ( (m_iCurrentPlayerSide==ARMY_SIDE_PAST && m_iPlayerTypePAST==PLAYER_TYPE_HUMAN) ||
							(m_iCurrentPlayerSide==ARMY_SIDE_FUTURE && m_iPlayerTypeFUTURE==PLAYER_TYPE_HUMAN)
							|| (m_iPlayerTypePAST==PLAYER_TYPE_PC &&  m_iPlayerTypeFUTURE==PLAYER_TYPE_PC))												
						{
							if (g_iPlayerScore>0 || 
								(g_iPlayerScore>0 &&m_iPlayerTypePAST==PLAYER_TYPE_PC &&  m_iPlayerTypeFUTURE==PLAYER_TYPE_PC)) 
							{

								m_iSelectedUnitExperiencePoints++;

								if ( BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType== UNIT_TYPE_QUEEN)
								{
									int iDifferenceX,iDifferenceY;
									iDifferenceX = m_iTargetX - m_iChosenUnitX;
									iDifferenceY = m_iTargetY - m_iChosenUnitY;

									if (iDifferenceX>0)
										iDifferenceX = -1;
									else
										iDifferenceX = 1;

									if (iDifferenceY>0)
										iDifferenceY = -1;
									else
										iDifferenceY = 1;


									// here i should also kill the enemy (and free his models/client)
									BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray=-1;


									// add experience points to the winner
									int iTempIndex = BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray;
									m_iTempIndex=iTempIndex;
									m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iExperiencePoints++;

			
								}
								else // regular soldier
								{								
									// here i should also kill the enemy (and free his models/client)
									BoardsSquaresUnits[m_iFightAgainstX][m_iFightAgainstY].m_iIndexToPlayersArray=-1;

									// add experience points to the winner
									int iTempIndex = BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray;
									m_iTempIndex=iTempIndex;
									m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iExperiencePoints++;

								}

							}
							else  // past side lost
							{

								BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray=-1;

								ClearBoardSelections();
								GoBackToSelectUnit();
								if (CheckIfPlayerWon(iDummy))
								m_iBoardGameState = GAME_BOARD_STATE_MENU;

								// change player side
								if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
									m_iCurrentPlayerSide=ARMY_SIDE_PAST;
								else
									m_iCurrentPlayerSide=ARMY_SIDE_FUTURE;

								m_iTurnNum++;
								TimeHealUnits();


								// add experience points to the winner
								if ( BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType== UNIT_TYPE_QUEEN)
								{
									int iDifferenceX,iDifferenceY;
									iDifferenceX = m_iTargetX - m_iChosenUnitX;
									iDifferenceY = m_iTargetY - m_iChosenUnitY;

									if (iDifferenceX>0)
										iDifferenceX = -1;
									else
										iDifferenceX = 1;

									if (iDifferenceY>0)
										iDifferenceY = -1;
									else
										iDifferenceY = 1;

									
									//BoardsSquaresUnits[m_iChosenUnitX+iDifferenceX][m_iChosenUnitY+iDifferenceY]

									int iTempIndex = BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray;
									m_iTempIndex=iTempIndex;

									m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iExperiencePoints++;


								}
								else
								{
									int iTempIndex = BoardsSquaresUnits[m_iFightAgainstX][m_iFightAgainstY].m_iIndexToPlayersArray;
									m_iTempIndex=iTempIndex;

									if (iTempIndex>=0)
										m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iExperiencePoints++;

								}
							}
						}
						else // current player lost
						if ( (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE && m_iPlayerTypePAST==PLAYER_TYPE_HUMAN) ||
							(m_iCurrentPlayerSide==ARMY_SIDE_PAST && m_iPlayerTypeFUTURE==PLAYER_TYPE_HUMAN) )
						{
							if (g_iPlayerScore>0) 
							{
								BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray=-1;
								ClearBoardSelections();
								GoBackToSelectUnit();					
								if (CheckIfPlayerWon(iDummy))
								m_iBoardGameState = GAME_BOARD_STATE_MENU;

								// change player side
								if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
									m_iCurrentPlayerSide=ARMY_SIDE_PAST;
								else
									m_iCurrentPlayerSide=ARMY_SIDE_FUTURE;

								m_iTurnNum++;
								TimeHealUnits();

								// add experience points to the winner

								if ( BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType== UNIT_TYPE_QUEEN)
								{
									int iDifferenceX,iDifferenceY;
									iDifferenceX = m_iTargetX - m_iChosenUnitX;
									iDifferenceY = m_iTargetY - m_iChosenUnitY;

									if (iDifferenceX>0)
										iDifferenceX = -1;
									else
										iDifferenceX = 1;

									if (iDifferenceY>0)
										iDifferenceY = -1;
									else
										iDifferenceY = 1;

									int iTempIndex = BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray;
									m_iTempIndex=iTempIndex;

									if (iTempIndex>=0)
										m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iExperiencePoints++;

									/*((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray])->m_pPlayerClient->InitStates();
									((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray])->m_pPlayerClient->m_bReviveUponDeath=false;
									((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray])->m_pPlayerClient->m_pPlayerModel->InitAnimationStates();*/

								}
								else
								{
									int iTempIndex = BoardsSquaresUnits[m_iFightAgainstX][m_iFightAgainstY].m_iIndexToPlayersArray;
									m_iTempIndex=iTempIndex;

									if (iTempIndex>=0)
										m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iExperiencePoints++;


									/*((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iFightAgainstX][m_iFightAgainstY].m_iIndexToPlayersArray])->m_pPlayerClient->InitStates();
									((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iFightAgainstX][m_iFightAgainstY].m_iIndexToPlayersArray])->m_pPlayerClient->m_bReviveUponDeath=false;
									((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iFightAgainstX][m_iFightAgainstY].m_iIndexToPlayersArray])->m_pPlayerClient->m_pPlayerModel->InitAnimationStates();*/

								}
							}
							else // past side lost
							{
								int iDifferenceX,iDifferenceY;
									iDifferenceX = m_iTargetX - m_iChosenUnitX;
									iDifferenceY = m_iTargetY - m_iChosenUnitY;

									if (iDifferenceX>0)
										iDifferenceX = -1;
									else
										iDifferenceX = 1;

									if (iDifferenceY>0)
										iDifferenceY = -1;
									else
										iDifferenceY = 1;

								// here i should also kill the enemy (and free his models/client)
								BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray=-1;																

								// add experience points to the winner

								int iTempIndex = BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray;
								m_iTempIndex=iTempIndex;
								m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iExperiencePoints++;


								/*((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->InitStates();
								((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->m_bReviveUponDeath=false;
								((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->m_pPlayerModel->InitAnimationStates();*/
							}
						}
						
					}
							



					g_iPlayerScore=0;
													

					if (m_fAnimationT>=1.f) // sub animation ended
					{
						if (m_iAnimationState==FIGHT_ANIMATION_FIRST_TURN)
						{
							m_fAnimationT=0.f;
							m_iAnimationState=FIGHT_ANIMATION_WALK;
							bJustSwitchedAnimationPhase=true;
						}
						else
						if (m_iAnimationState==FIGHT_ANIMATION_WALK)
						{

							StopBackgroundMusic();
							g_SoundManager.PlayStream(m_ppBackgroundStreams[CB_SOUND_STREAM_PRE_BATTLE],120);
							//g_SoundManager.PlaySample(m_ppBoardSoundSamples[CB_SOUND_SAMPLES_PREPARE_TO_FIGHT],50);
							g_SoundManager.PlaySample3D(m_ppBoardSoundSamples[CB_SOUND_SAMPLES_PREPARE_TO_FIGHT],g_Camera1.m_Pos,200);

							// switch back player animation to kIdle
							
							//m_iAnimationState=FIGHT_ANIMATION_SECOND_TURN;
							m_iAnimationState=FIGHT_ANIMATION_POST_FIGHT_WALK;
							bJustSwitchedAnimationPhase=true;

							m_fAnimationT=0.f;


							/**/ // go into the FPS battle

							st_Engine::Inst()->m_iGameState=1; // make it a fps game
							//st_Engine::Inst()->m_bSwitchingGameState = true;

							// here i should put some loading screen

							/****************/

							float fCenterX = (float)((g_pWindow->width)/2);
							float fCenterY = (float)((g_pWindow->height)/2);

							float fTempX = (float)((g_pWindow->height)/3);
							float fTempY = (float)((g_pWindow->height)/5);

							CVector2f vCenter(fCenterX,fCenterY);

							CVector2f vec1(-fTempX,-fTempY);
							CVector2f vec2(fTempX,-fTempY);
							CVector2f vec3(fTempX,fTempY);
							CVector2f vec4(-fTempX,fTempY);

							vec1+=vCenter;
							vec2+=vCenter;
							vec3+=vCenter;
							vec4+=vCenter;

							//st_Renderer::Inst()->DebugDraw2DQuadInitAllOpenGLStuff(g_pPleaseWaitTex,vec1,vec2,vec3,vec4,false);

							/*glMatrixMode(GL_MODELVIEW);
							glLoadIdentity();
							st_Renderer::Inst()->RenderVB(g_pvbCharacterFrame,false,true); 
							st_Renderer::Inst()->DrawAnchored2DQuad(NULL,ANCHOR_X_MIDDLE | ANCHOR_Y_MIDDLE,256,128,0,0);*/


							int iDifferenceX,iDifferenceY;
							iDifferenceX = m_iTargetX - m_iChosenUnitX;
							iDifferenceY = m_iTargetY - m_iChosenUnitY;

							if (iDifferenceX>0)
								iDifferenceX = -1;
							else
								iDifferenceX = 1;
							if (iDifferenceY>0)
								iDifferenceY = -1;
							else
								iDifferenceY = 1;




							///////// actually draw the picture to some back buffer	
							//glFlush ();		// Flush The GL Rendering Pipeline

							///////// wait for vsinc and swap buffers
							SwapBuffers (g_pWindow->hDC);					// Swap Buffers (Double Buffering)



							/*************/


							SafeDelete(g_pQuake3Map);


							g_pSky->FreeSideTextures();


							g_pQuake3Map = new CQuake3BSP;
							

							if (m_iTargetY+iDifferenceY==0)
							{
								//restore restore restore restore restore restore 
								//g_pQuake3Map->LoadBSP("maps/q3dm0.bsp");
								g_pQuake3Map->LoadBSP("maps/q3dm17.bsp");						


								//g_pQuake3Map->LoadBSP("maps/q3dm17.bsp");
								//g_pQuake3Map->LoadBSP("maps/shortcircuit.bsp");
							
								g_pQuake3Map->SetMapFileName("maps/q3dm17.bsp");
								//g_pQuake3Map->SetMapFileName("shortcircuit.bsp");
							}
							else
							if (m_iTargetY+iDifferenceY==1)
							{
								g_pQuake3Map->LoadBSP("maps/q3dm1.bsp");
								//g_pQuake3Map->LoadBSP("maps/q3dm17.bsp");
								//g_pQuake3Map->LoadBSP("maps/shortcircuit.bsp");
							
								g_pQuake3Map->SetMapFileName("maps/q3dm1.bsp");
								//g_pQuake3Map->SetMapFileName("shortcircuit.bsp");
							}
							else
							if (m_iTargetY+iDifferenceY==2)
							{
								//g_pQuake3Map->LoadBSP("maps/q3dm2.bsp");
								g_pQuake3Map->LoadBSP("maps/q3dm17.bsp");
								//g_pQuake3Map->LoadBSP("maps/shortcircuit.bsp");
							
								//g_pQuake3Map->SetMapFileName("maps/q3dm2.bsp");
								g_pQuake3Map->SetMapFileName("maps/q3dm17.bsp");
								//g_pQuake3Map->SetMapFileName("shortcircuit.bsp");
							}
							else
							if (m_iTargetY+iDifferenceY==3)
							{
								//g_pQuake3Map->LoadBSP("maps/q3dm3.bsp");
								//g_pQuake3Map->LoadBSP("maps/q3dm7.bsp");
								g_pQuake3Map->LoadBSP("maps/q3dm1.bsp");

								//g_pQuake3Map->LoadBSP("maps/shortcircuit.bsp");
							
								g_pQuake3Map->SetMapFileName("maps/q3dm1.bsp");
								//g_pQuake3Map->SetMapFileName("shortcircuit.bsp");
							}
							else
							if (m_iTargetY+iDifferenceY==4)
							{
								//g_pQuake3Map->LoadBSP("maps/q3dm4.bsp");
								g_pQuake3Map->LoadBSP("maps/q3dm17.bsp");
								//g_pQuake3Map->LoadBSP("maps/shortcircuit.bsp");
							
								g_pQuake3Map->SetMapFileName("maps/q3dm17.bsp");
								//g_pQuake3Map->SetMapFileName("shortcircuit.bsp");
							}
							else
							if (m_iTargetY+iDifferenceY==5)
							{
								//g_pQuake3Map->LoadBSP("maps/q3dm5.bsp");
								g_pQuake3Map->LoadBSP("maps/q3dm1.bsp");
								//g_pQuake3Map->LoadBSP("maps/shortcircuit.bsp");
							
								g_pQuake3Map->SetMapFileName("maps/q3dm1.bsp");
								//g_pQuake3Map->SetMapFileName("shortcircuit.bsp");
							}
							else
							if (m_iTargetY+iDifferenceY==6)
							{
								//g_pQuake3Map->LoadBSP("maps/q3dm6.bsp");
								g_pQuake3Map->LoadBSP("maps/q3dm17.bsp");
								//g_pQuake3Map->LoadBSP("maps/shortcircuit.bsp");
							
								g_pQuake3Map->SetMapFileName("maps/q3dm17.bsp");
								//g_pQuake3Map->SetMapFileName("shortcircuit.bsp");
							}
							else
							if (m_iTargetY+iDifferenceY==7)
							{
								g_pQuake3Map->LoadBSP("maps/q3dm1.bsp");
								//g_pQuake3Map->LoadBSP("maps/q3dm17.bsp");
								//g_pQuake3Map->LoadBSP("maps/shortcircuit.bsp");
							
								g_pQuake3Map->SetMapFileName("maps/q3dm1.bsp");
								//g_pQuake3Map->SetMapFileName("shortcircuit.bsp");
							}


							
							


							st_Renderer::Inst()->InitSkyVBs();	


							

								// here i should also kill the enemy (and free his models/client)
								//BoardsSquaresUnits[m_iTargetX+iDifferenceX]


							BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP = ((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->m_pPlayerModel->m_fAngY;
							BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_fYAngBACKUP = ((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray])->m_pPlayerClient->m_pPlayerModel->m_fAngY;

							((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->m_bReviveUponDeath=true;
							((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray])->m_pPlayerClient->m_bReviveUponDeath=true;


							if (m_iPlayerTypePAST==PLAYER_TYPE_HUMAN && (m_iPlayerTypeFUTURE==PLAYER_TYPE_PC || m_iPlayerTypeFUTURE==PLAYER_TYPE_NETWORK))
							{
								if (m_iCurrentPlayerSide==ARMY_SIDE_PAST)
									g_pQuake3Map->FillDynamicEntities(
										((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient,
										((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray])->m_pPlayerClient
										);
								else
									g_pQuake3Map->FillDynamicEntities(
										((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray])->m_pPlayerClient,
										((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient									
										);
							}
							else
							if ((m_iPlayerTypePAST==PLAYER_TYPE_PC || m_iPlayerTypePAST==PLAYER_TYPE_NETWORK) && m_iPlayerTypeFUTURE==PLAYER_TYPE_HUMAN)
							{
								if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
									g_pQuake3Map->FillDynamicEntities(
										((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient,
										((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray])->m_pPlayerClient
										);
								else
									g_pQuake3Map->FillDynamicEntities(
										((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray])->m_pPlayerClient,
										((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient									
										);
							}
							else
							if (m_iPlayerTypePAST==PLAYER_TYPE_PC && m_iPlayerTypeFUTURE==PLAYER_TYPE_PC)
							{
								// current the player is the one that attacked
								g_pQuake3Map->FillDynamicEntities(
									((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient,
									((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iTargetX+iDifferenceX][m_iTargetY+iDifferenceY].m_iIndexToPlayersArray])->m_pPlayerClient
									);
								
							}



							
							// send player advantages

							if (g_Communication.GetAlreadyInited())
							{
								if (m_iPlayerTypePAST==PLAYER_TYPE_HUMAN)
								{
									if (m_iAdvantagePAST==ADVANTAGE_WEAPON)
										g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_WEAPON,0,0,NULL);
									else
									if (m_iAdvantagePAST==ADVANTAGE_EXTRALIFE)
										g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_EXTRALIFE,0,0,NULL);
									else
									if (m_iAdvantagePAST==ADVANTAGE_AMMO)
										g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_AMMO,0,0,NULL);
								}
								else
								if (m_iPlayerTypeFUTURE==PLAYER_TYPE_HUMAN)
								{
									if (m_iAdvantageFUTURE==ADVANTAGE_WEAPON)
										g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_WEAPON,0,0,NULL);
									else
									if (m_iAdvantageFUTURE==ADVANTAGE_EXTRALIFE)
										g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_EXTRALIFE,0,0,NULL);
									else
									if (m_iAdvantageFUTURE==ADVANTAGE_AMMO)
										g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_AMMO,0,0,NULL);
								}
							}


							/*g_pPlayerClient1->InitStates();
							g_pPlayerClient2->InitStates();*/


							/*g_pPlayerClient1->PutPlayerInStartPos();
				
							g_pPlayerClient2->AdvanceStartPosition();
							g_pPlayerClient2->PutPlayerInStartPos();*/


							g_cgslShader.DisableProfiles();

							bBreak=true;

							
							StopBackgroundMusic();
							g_SoundManager.PlayStream(m_ppBackgroundStreams[CB_SOUND_STREAM_BATTLE],120);
							//g_SoundManager.PlaySample(m_ppBoardSoundSamples[CB_SOUND_SAMPLES_FIGHT],50);
							g_SoundManager.PlaySample3D(m_ppBoardSoundSamples[CB_SOUND_SAMPLES_FIGHT],g_Camera1.m_Pos,200);
							

							// only let the chat menu appear/be active
							g_pGUIManager->ChangeToFPSBattlesGUIMode();
						}
						else						
						if (m_iAnimationState==FIGHT_ANIMATION_POST_FIGHT_WALK)
						{
							m_fAnimationT=0.f;
							m_iAnimationState=FIGHT_ANIMATION_SECOND_TURN; 
						}
						else
						if (m_iAnimationState==FIGHT_ANIMATION_SECOND_TURN)
						{
							m_fAnimationT=0.f;
							m_iAnimationState=FIGHT_ANIMATION_FIRST_TURN; // reinit it

							m_iBoardGameState = GAME_BOARD_STATE_CHOOSE_UNIT;
							bJustSwitchedAnimationPhase=true;

							((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = kIdle;			

							// change player side
							if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
								m_iCurrentPlayerSide=ARMY_SIDE_PAST;
							else
								m_iCurrentPlayerSide=ARMY_SIDE_FUTURE;

							m_iTurnNum++;
							TimeHealUnits();



							BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_iIndexToPlayersArray = BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray;
							BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_iArmySide = BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iArmySide;
							BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_fYAngBACKUP = BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP;
							BoardsSquaresUnits[m_iTargetX][m_iTargetY].m_iUnitType = BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType;
				

							BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iArmySide = -1;
							BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray = -1;

							ClearBoardSelections();



							///

							if (CheckIfPlayerWon(iDummy))
								m_iBoardGameState = GAME_BOARD_STATE_MENU;

						}                     

					} // if (sub animation ended)

					if (bBreak)
						break;


					if (!bJustSwitchedAnimationPhase)
					{
						if (m_iAnimationState==FIGHT_ANIMATION_WALK)
							m_fAnimationT+= st_Timer::Inst()->GetDeltaTimeFAKE()*0.0013f * m_fGamePlaySpeed;
						else
							m_fAnimationT+= st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f * m_fGamePlaySpeed;
					}
					else
						m_fAnimationT=0.f;


					if (bJustSwitchedAnimationPhase)
					{
						if (m_iAnimationState==FIGHT_ANIMATION_WALK)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY)+CVector3f(0.f,25.f,0.f));			
						else
						if (m_iAnimationState==FIGHT_ANIMATION_SECOND_TURN)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(m_iTargetX,m_iTargetY)+CVector3f(0.f,25.f,0.f));			
						else
						if (m_iAnimationState==FIGHT_ANIMATION_POST_FIGHT_WALK)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(m_iTargetX,m_iTargetY)+CVector3f(0.f,25.f,0.f));			
					}
					else
					{
						if (m_iAnimationState==FIGHT_ANIMATION_FIRST_TURN)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY)+CVector3f(0.f,25.f,0.f));			
						else
						if (m_iAnimationState==FIGHT_ANIMATION_WALK)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(vAnimatedPosition);			
						else
						if (m_iAnimationState==FIGHT_ANIMATION_POST_FIGHT_WALK)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(vAnimatedPosition);			
						else
						if (m_iAnimationState==FIGHT_ANIMATION_SECOND_TURN)
							((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(m_iTargetX,m_iTargetY)+CVector3f(0.f,25.f,0.f));			
						
						//((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(vAnimatedPosition);							
					}

					
				
					//m_ppGameUnits[iIndex]->Update();
					
				}				
				else // if not the currently controled unit
				{
					if (m_iBoardGameState==GAME_BOARD_STATE_SHOWING_COMBO_ANIMATION)
					{
						if (m_iAnimationState==WALK_ANIMATION_FIRST_TURN)
						{
							/*

							int iDifferenceX = m_ComboMovesVector[0].x-m_iChosenUnitX;
							int iDifferenceY = m_ComboMovesVector[0].y-m_iChosenUnitY;

							if (iDifferenceX>0)
								iDifferenceX=-1;
							else
								iDifferenceX=+1;

							if (iDifferenceY>0)
								iDifferenceY=-1;
							else
								iDifferenceY=+1;


							//int iTempIndex = BoardsSquaresUnits[ (m_ComboMovesVector[0].x + m_iChosenUnitX )/2  ][(m_ComboMovesVector[0].y +  m_iChosenUnitY )/2].m_iIndexToPlayersArray;
							int iTempIndex = BoardsSquaresUnits[ m_ComboMovesVector[0].x + iDifferenceX ][m_ComboMovesVector[0].y + iDifferenceY].m_iIndexToPlayersArray;

							if (iTempIndex>=0)
							{
								//((CEntityPlayer*)m_ppGameUnits[iTempIndex])->ForcePosition(GetMiddleOfSquare(m_ComboMovesVector[0].x + iDifferenceX,m_ComboMovesVector[0].y + iDifferenceY)+CVector3f(0.f,25.f-m_fAnimationT*70.f,0.f));
								//((CEntityPlayer*)m_ppGameUnits[iTempIndex])->ForcePosition(GetMiddleOfSquare(m_ComboMovesVector[0].x + iDifferenceX,m_ComboMovesVector[0].y + iDifferenceY)+CVector3f(0.f,25.f-m_fAnimationT*10.f,0.f));
								// here i change y-=blah 
								
								
								
								//
								//((CEntityPlayer*)m_ppGameUnits[iTempIndex])->m_pPlayerClient->Die();
								//
							}
							//m_ppGameUnits[iTempIndex].m_iIndexToPlayersArray = -2;

							for (int i=0;i<m_iComboMovesNum-1;i++)								
								{
									int iDifferenceX = m_ComboMovesVector[i+1].x-m_ComboMovesVector[i].x;
									int iDifferenceY = m_ComboMovesVector[i+1].y-m_ComboMovesVector[i].y;

									if (iDifferenceX>0)
										iDifferenceX=-1;
									else
										iDifferenceX=+1;

									if (iDifferenceY>0)
										iDifferenceY=-1;
									else
										iDifferenceY=+1;

									
									int iTempIndex = BoardsSquaresUnits[  m_ComboMovesVector[i+1].x+iDifferenceX ][m_ComboMovesVector[i+1].y+iDifferenceY].m_iIndexToPlayersArray;
									if (iTempIndex>=0)
									{
										//
										//((CEntityPlayer*)m_ppGameUnits[iTempIndex])->ForcePosition(GetMiddleOfSquare( m_ComboMovesVector[i+1].x+iDifferenceX,m_ComboMovesVector[i+1].y+iDifferenceY)+CVector3f(0.f,25.f-m_fAnimationT*70.f,0.f));
										//((CEntityPlayer*)m_ppGameUnits[iTempIndex])->m_pPlayerClient->Die();
										//
									}
									
									
								}*/
						}
						else
						if (m_iAnimationState==WALK_ANIMATION_WALK || m_iAnimationState==WALK_ANIMATION_SECOND_TURN)
						{
							int iTempIndex = BoardsSquaresUnits[ (m_ComboMovesVector[0].x + m_iChosenUnitX )/2  ][(m_ComboMovesVector[0].y +  m_iChosenUnitY )/2].m_iIndexToPlayersArray;

							/*if (iTempIndex>=0)
								((CEntityPlayer*)m_ppGameUnits[iTempIndex])->ForcePosition(GetMiddleOfSquare((m_ComboMovesVector[0].x + m_iChosenUnitX )/2,(m_ComboMovesVector[0].y + m_iChosenUnitY )/2)+CVector3f(0.f,25.f-70.f,0.f));*/

							//((CEntityPlayer*)m_ppGameUnits[iTempIndex])->m_pPlayerClient->Die();

							for (int i=0;i<m_iComboMovesNum-1;i++)								
								{
									//BoardsSquaresUnits[ (m_ComboMovesVector[i].x + m_ComboMovesVector[i+1].x )/2  ][(m_ComboMovesVector[i].y + m_ComboMovesVector[i+1].y )/2].m_iIndexToPlayersArray = -1;

									/**/
									/*int iTempIndex = BoardsSquaresUnits[  (m_ComboMovesVector[i].x + m_ComboMovesVector[i+1].x )/2 ][(m_ComboMovesVector[i].y + m_ComboMovesVector[i+1].y )/2].m_iIndexToPlayersArray;
									if (iTempIndex>=0)
										((CEntityPlayer*)m_ppGameUnits[iTempIndex])->ForcePosition(GetMiddleOfSquare( (m_ComboMovesVector[i].x + m_ComboMovesVector[i+1].x )/2,(m_ComboMovesVector[i].y + m_ComboMovesVector[i+1].y )/2)+CVector3f(0.f,25.f-70.f,0.f));
									//((CEntityPlayer*)m_ppGameUnits[iTempIndex])->m_pPlayerClient->Die();*/
									/**/
								}



							
							int iDifferenceX = m_iSubComboToX-m_iSubComboFromX;
							int iDifferenceY = m_iSubComboToY-m_iSubComboFromY;

							if (iDifferenceX>0)
								iDifferenceX=-1;
							else
								iDifferenceX=+1;

							if (iDifferenceY>0)
								iDifferenceY=-1;
							else
								iDifferenceY=+1;

							/**/
							
							

							/**/

							if (m_iAnimationState==WALK_ANIMATION_WALK)
							{
							int iIndex = BoardsSquaresUnits[  m_iSubComboToX+iDifferenceX ][m_iSubComboToY+iDifferenceY].m_iIndexToPlayersArray;
							if (iIndex>=0)
							{
								if (!((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_bDying && !((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->m_bStartDie)
								{

									m_iSelectedUnitExperiencePoints++;

									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->Die();
									((CEntityPlayer*)m_ppGameUnits[iIndex])->m_pPlayerClient->PlaySound(CLIENT_SOUND_SAMPLE_DEATH);

									CVector3f v2 = GetMiddleOfSquare(m_iSubComboToX+iDifferenceX,m_iSubComboToY+iDifferenceY);
									CVector3f v1;

									v1 =  v2+ CVector3f(0.f,3.f,0.f);
									CParticleSystem* pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_SMOKE,30,
										&v1,false,CVector3f(0.f,0.f,0.f),0.f);
									pPS->SetTimeToDeath(1000.f);

									v1 = v2 + CVector3f(0.f,3.f,0.f) + CVector3f(-HALF_BOARD_SQUARE_SIZE*0.4f,0.f,-HALF_BOARD_SQUARE_SIZE*0.4f);
									pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_SMOKE,30,
										&v1,false,CVector3f(0.f,0.f,0.f),0.f);
									pPS->SetTimeToDeath(1000.f);

									v1 = v2 + CVector3f(0.f,3.f,0.f) + CVector3f(HALF_BOARD_SQUARE_SIZE*0.4f,0.f,HALF_BOARD_SQUARE_SIZE*0.4f);
									pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_SMOKE,30,
										&v1,false,CVector3f(0.f,0.f,0.f),0.f);
									pPS->SetTimeToDeath(1000.f);

									v1 = v2 + CVector3f(0.f,3.f,0.f) + CVector3f(-HALF_BOARD_SQUARE_SIZE*0.4f,0.f,HALF_BOARD_SQUARE_SIZE*0.4f);
									pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_SMOKE,30,
										&v1,false,CVector3f(0.f,0.f,0.f),0.f);
									pPS->SetTimeToDeath(1000.f);

									v1 = v2 + CVector3f(0.f,3.f,0.f) + CVector3f(HALF_BOARD_SQUARE_SIZE*0.4f,0.f,-HALF_BOARD_SQUARE_SIZE*0.4f);
									pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_SMOKE,30,
										&v1,false,CVector3f(0.f,0.f,0.f),0.f);
									pPS->SetTimeToDeath(1000.f);
								}
								
							}
							}
						}
					}
					

					

					((CEntityPlayer*)m_ppGameUnits[iIndex])->ForcePosition(GetMiddleOfSquare(i,j)+CVector3f(0.f,25.f,0.f));			
					//m_ppGameUnits[iIndex]->Update();
					
				}
			}

		}

		//assert(_CrtCheckMemory());


		if (bBreak)
			return;

		///

		for (int i=0;i<m_iNumOfUnits;i++)
			if (m_ppGameUnits[i])
				m_ppGameUnits[i]->Update();
		


		/******/

		// Check if a unit should become a queen

		int iArmySide;

		bool bStop=false;

		for (int i=0;i<SQUARES_NUM_X;i++)
			for (int j=0;j<SQUARES_NUM_Y;j++)
			{
				// if a part of the charmed then stop

				for (int k=0;k<m_iCharmedUnitsOwnedByPastNum;k++)
					if (m_CharmedUnitsOwnedByPAST[k].iPlayerIndex==BoardsSquaresUnits[i][j].m_iIndexToPlayersArray)
					{
						bStop=true;
						break;
					}

				if (bStop)
					continue;


				for (int k=0;k<m_iCharmedUnitsOwnedByFutureNum;k++)
					if (m_CharmedUnitsOwnedByFUTURE[k].iPlayerIndex==BoardsSquaresUnits[i][j].m_iIndexToPlayersArray)
					{
						bStop=true;
						break;
					}

				if (bStop)
					continue;

				if (BoardsSquaresUnits[i][j].m_iUnitType!=UNIT_TYPE_SOLDIER) // if already a queen
					continue;

				int iIndex = BoardsSquaresUnits[i][j].m_iIndexToPlayersArray;

				if (iIndex<0)
					continue;
				
				iArmySide = BoardsSquaresUnits[i][j].m_iArmySide;
				if (iArmySide==ARMY_SIDE_PAST)
				{
					if (j==SQUARES_NUM_Y-1) // we are in the last raw
					{
						int iSavedExperiencePoints = m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_iExperiencePoints;

						// make it a queen
						BoardsSquaresUnits[i][j].m_iIndexToPlayersArray+=24;						
						BoardsSquaresUnits[i][j].m_iUnitType = UNIT_TYPE_QUEEN;

						m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_fAngY = 0.f;							

						m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_iExperiencePoints = iSavedExperiencePoints;

						// make a small magical particle system appear
						CVector3f v1 = GetMiddleOfSquare(i,j) + CVector3f(0.f,3.f,-8.f);
						CParticleSystem* pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,100,&v1,false,CVector3f(0.f,0.f,0.f),0.f);
						pPS->SetTimeToDeath(2000.f);

						v1 = GetMiddleOfSquare(i,j) + CVector3f(0.f,13.f,-8.f);
						pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,100,&v1,false,CVector3f(0.f,0.f,0.f),0.f);
						pPS->SetTimeToDeath(2000.f);

						v1 = GetMiddleOfSquare(i,j) + CVector3f(0.f,23.f,0.f);
						pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,100,&v1,false,CVector3f(0.f,0.f,0.f),0.f);
						pPS->SetTimeToDeath(2000.f);

						v1 = GetMiddleOfSquare(i,j) + CVector3f(0.f,33.f,0.f);
						pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,100,&v1,false,CVector3f(0.f,0.f,0.f),0.f);
						pPS->SetTimeToDeath(2000.f);
					}
				}
				else // ARMY_SIDE_FUTURE
				{
					if (j==0) // we are in the last raw
					{
						int iSavedExperiencePoints = m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_iExperiencePoints;

						// make it a queen
						BoardsSquaresUnits[i][j].m_iIndexToPlayersArray+=24;						
						BoardsSquaresUnits[i][j].m_iUnitType = UNIT_TYPE_QUEEN;

						m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_fAngY = 180.f;							

						m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_iExperiencePoints = iSavedExperiencePoints;

						// make a small magical particle system appear
						CVector3f v1 = GetMiddleOfSquare(i,j) + CVector3f(0.f,3.f,8.f);
						CParticleSystem* pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,100,&v1,false,CVector3f(0.f,0.f,0.f),0.f);
						pPS->SetTimeToDeath(2000.f);

						v1 = GetMiddleOfSquare(i,j) + CVector3f(0.f,13.f,8.f);
						pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,100,&v1,false,CVector3f(0.f,0.f,0.f),0.f);
						pPS->SetTimeToDeath(2000.f);

						v1 = GetMiddleOfSquare(i,j) + CVector3f(0.f,23.f,0.f);
						pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,100,&v1,false,CVector3f(0.f,0.f,0.f),0.f);
						pPS->SetTimeToDeath(2000.f);

						v1 = GetMiddleOfSquare(i,j) + CVector3f(0.f,33.f,0.f);
						pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,100,&v1,false,CVector3f(0.f,0.f,0.f),0.f);
						pPS->SetTimeToDeath(2000.f);
					}

				}
				

			}


		/******/


		// shake if needed

		static CVector3f vTempPlayerPos;

		if (m_bShakeBoard)
		{
			for (int i=0;i<SQUARES_NUM_X;i++)
				for (int j=0;j<SQUARES_NUM_Y;j++)
					if (IsOdd(i+j))
						if (BoardsSquaresUnits[i][j].m_iIndexToPlayersArray>=0)
						{						
							vTempPlayerPos = ((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray])->m_pPlayerClient->m_pPlayerModel->m_vWorldPosition;
							((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray])->ForcePosition(vTempPlayerPos+m_vShakeSum);			
						}
		}


		UpdateCharmed();
		UpdateParalised();


		UpdateAIBoard();

		m_CheckersAI.FillCurrentPossibleMoves(ARMY_SIDE_PAST);
		m_CheckersAI.FillCurrentPossibleMoves(ARMY_SIDE_FUTURE);

		// check if there are no possible moves for current player
		// and switch back to other player if yes.

		if (AreAllBoardAnimationsDone())
		{
			if (m_iCurrentPlayerSide==ARMY_SIDE_PAST)
			{
				if (m_CheckersAI.m_iCurrentPossibleMovesPAST<1)
				{
					m_iCurrentPlayerSide=ARMY_SIDE_FUTURE;
					m_iTurnNum++;
					TimeHealUnits();
				}
			}
			else
			{
				if (m_CheckersAI.m_iCurrentPossibleMovesFUTURE<1)
				{
					m_iCurrentPlayerSide=ARMY_SIDE_PAST;
					m_iTurnNum++;
					TimeHealUnits();
				}
			}
		}
		
}

void CCheckersBoard::CopyBoardToTemp(void)
{
	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			BoardsSquaresUnitsTEMP[i][j].m_iIndexToPlayersArray = BoardsSquaresUnits[i][j].m_iIndexToPlayersArray;
			BoardsSquaresUnitsTEMP[i][j].m_dwState = BoardsSquaresUnits[i][j].m_dwState;
			BoardsSquaresUnitsTEMP[i][j].m_iArmySide = BoardsSquaresUnits[i][j].m_iArmySide;
			BoardsSquaresUnitsTEMP[i][j].m_iUnitType = BoardsSquaresUnits[i][j].m_iUnitType;
			BoardsSquaresUnitsTEMP[i][j].m_fYAngBACKUP = BoardsSquaresUnits[i][j].m_fYAngBACKUP;
			//BoardsSquaresUnitsTEMP[i][j].m_bTriggerDeath = BoardsSquaresUnits[i][j].m_bTriggerDeath;
			
			
			
		}

}

void CCheckersBoard::RestoreBoardFromTemp(void)
{
	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			BoardsSquaresUnits[i][j].m_iIndexToPlayersArray = BoardsSquaresUnitsTEMP[i][j].m_iIndexToPlayersArray;
			BoardsSquaresUnits[i][j].m_dwState = BoardsSquaresUnitsTEMP[i][j].m_dwState;
			BoardsSquaresUnits[i][j].m_iArmySide = BoardsSquaresUnitsTEMP[i][j].m_iArmySide;
			BoardsSquaresUnits[i][j].m_iUnitType = BoardsSquaresUnitsTEMP[i][j].m_iUnitType;
			BoardsSquaresUnits[i][j].m_fYAngBACKUP = BoardsSquaresUnitsTEMP[i][j].m_fYAngBACKUP;
			//BoardsSquaresUnits[i][j].m_bTriggerDeath = BoardsSquaresUnitsTEMP[i][j].m_bTriggerDeath;

			
		}

}


void CCheckersBoard::ChooseUnit(int x, int y)
{
	if (!AreAllBoardAnimationsDone())
		return;

	if (BoardsSquaresUnits[x][y].m_bParalised)
		return;

	//MessageBox(0,"1","ChooseUnit",MB_OK);

	if (BoardsSquaresUnits[x][y].m_iArmySide!=m_iCurrentPlayerSide)
		return;

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			if ( IsOdd(i+j))
			if ( (i==x) && (j==y))
			{
								
				if (m_iChosenUnitX==x && m_iChosenUnitY==y)
				{
					BoardsSquaresUnits[i][j].m_dwState = UNIT_NOT_SELECTED;
					m_iBoardGameState = GAME_BOARD_STATE_CHOOSE_UNIT;

					m_iChosenUnitX=-1;
					m_iChosenUnitY=-1;

					return;
				}

				
				// networking
				if ( (m_iPlayerTypeFUTURE==PLAYER_TYPE_NETWORK && m_iCurrentPlayerSide==ARMY_SIDE_PAST) || 					
					(m_iPlayerTypePAST==PLAYER_TYPE_NETWORK && m_iCurrentPlayerSide==ARMY_SIDE_FUTURE) )
					g_Communication.SendBoardMessage(0,x,y,NULL);
	
				

				BoardsSquaresUnits[i][j].m_dwState = UNIT_SELECTED;
				m_iChosenUnitX = i;
				m_iChosenUnitY = j;

				

				m_iBoardGameState = GAME_BOARD_STATE_CHOOSE_TARGET;

				int iTempIndex = BoardsSquaresUnits[i][j].m_iIndexToPlayersArray;

				if (iTempIndex>=0)
				{
					m_iSelectedUnitExperiencePoints = m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iExperiencePoints;
					m_iSelectedUnitHitPoints = m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iStartLife;

					CGUIWindow* pWin = g_pGUIManager->m_pBoardCharacterMenu;
					CGUIImage* pImage;
					for (int i=0;i<pWin->m_iCurrentElementNum-1;i++)
					{
						pImage = (CGUIImage*) pWin->GetElementNum(i);
						pImage->Deactivate();
					}


					// here i need to put the correct image

					if (m_iCurrentPlayerSide==ARMY_SIDE_PAST)
						pImage = (CGUIImage*) pWin->GetElementNum(2);
					else
						pImage = (CGUIImage*) pWin->GetElementNum(1);

					pImage->Activate();	

					//g_SoundManager.PlaySample(m_ppBoardSoundSamples[CB_SOUND_SAMPLES_SELECT_UNIT],50);
					g_SoundManager.PlaySample3D(m_ppBoardSoundSamples[CB_SOUND_SAMPLES_SELECT_UNIT],GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY),170);
				}
			}
			else
				BoardsSquaresUnits[i][j].m_dwState = UNIT_NOT_SELECTED;

		}
}

void CCheckersBoard::ClearBoardSelections(void)
{
	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
			BoardsSquaresUnits[i][j].m_dwState = UNIT_NOT_SELECTED;
}


bool CCheckersBoard::AreAllBoardAnimationsDone(void)
{
	/// check if we are free to choose target (all animations ended)
	int iIndex;

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			iIndex = BoardsSquaresUnits[i][j].m_iIndexToPlayersArray;
			if (iIndex>=0) // if contains a unit
			{
				// this part checks if there is ANY animation that is still being updated that we should wait for it to end
				
				// a dying animation
				if (m_ppGameUnits[iIndex]->m_pPlayerClient->m_bDying)
				{

					// i should really put next line somewhere else...

					///
					//m_ParticleSystem.SetParticlePos(m_ppGameUnits[iIndex]->m_pPlayerClient->m_pPlayerModel->m_vBonePosition);
					///

					return false; // wait for it to finish
				}
			}

		}


		if (st_PSManagerI->IsThereAnyActiveSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE))
			return false;

		if (st_PSManagerI->IsThereAnyActiveSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE))
			return false;


		if (st_PSManagerI->IsThereAnyActiveSystem(PARTICLE_SYSTEM_TYPE_SMOKE))
			return false;





		if (m_iBoardGameState==GAME_BOARD_STATE_CHOOSE_TARGET_COMBO ||
			m_iBoardGameState==GAME_BOARD_STATE_CHOOSE_TARGET ||
			m_iBoardGameState==GAME_BOARD_STATE_CHOOSE_TARGET_SPELL ||
			m_iBoardGameState==GAME_BOARD_STATE_CHOOSE_UNIT	||
			m_iBoardGameState==GAME_BOARD_STATE_MENU)
			return true;

		return false;

}

void CCheckersBoard::TryAllSquaresAsTarget(void)
{
	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
			if (IsOdd(i+j))
				ChooseTarget(i,j);
}

bool CCheckersBoard::IsSpellPossible(int iSpellType)
{

	if (m_Spells[iSpellType].m_iEffects==SPELL_EFFECTS_AUTO)
	{
		return true;
	}
	else
	if (m_Spells[iSpellType].m_iEffects==SPELL_EFFECTS_SELF_TWO_REST_OF_OWN_ARMY)
	{
		int iOwnArmyCount=0;
		
		for (int i=0;i<SQUARES_NUM_X;i++)
			for (int j=0;j<SQUARES_NUM_Y;j++)
				if (IsOdd(i+j)) // a playable square
					if (BoardsSquaresUnits[i][j].m_iIndexToPlayersArray>=0)
						if (BoardsSquaresUnits[i][j].m_iArmySide == m_iCurrentPlayerSide)
						{
							iOwnArmyCount++;
							if (iOwnArmyCount>2)
								return true;
						}


		return false;
	}
	else
	if (m_Spells[iSpellType].m_iEffects==SPELL_EFFECTS_ENEMY_TWO)
	{
		int iEnemyCount=0;
		
		for (int i=0;i<SQUARES_NUM_X;i++)
			for (int j=0;j<SQUARES_NUM_Y;j++)
				if (IsOdd(i+j)) // a playable square
					if (BoardsSquaresUnits[i][j].m_iIndexToPlayersArray>=0)
						if (BoardsSquaresUnits[i][j].m_iArmySide != m_iCurrentPlayerSide)
						{
							iEnemyCount++;
							if (iEnemyCount>1)
								return true;
						}

		return false;
	}
	else
	if (m_Spells[iSpellType].m_iEffects==SPELL_EFFECTS_SELF_ONE_REST_OF_OWN_ARMY)
	{
		int iEnemyCount=0;
		
		for (int i=0;i<SQUARES_NUM_X;i++)
			for (int j=0;j<SQUARES_NUM_Y;j++)
				if (IsOdd(i+j)) // a playable square
					if (BoardsSquaresUnits[i][j].m_iIndexToPlayersArray>=0)
						if (BoardsSquaresUnits[i][j].m_iArmySide != m_iCurrentPlayerSide)
						{
							iEnemyCount++;
							if (iEnemyCount>0)
								return true;
						}

		return false;
	}	
	else
	if (m_Spells[iSpellType].m_iEffects==SPELL_EFFECTS_ENEMY_UNIT)
	{
		int iEnemyCount=0;
		
		for (int i=0;i<SQUARES_NUM_X;i++)
			for (int j=0;j<SQUARES_NUM_Y;j++)
				if (IsOdd(i+j)) // a playable square
					if (BoardsSquaresUnits[i][j].m_iIndexToPlayersArray>=0)
						if (BoardsSquaresUnits[i][j].m_iArmySide != m_iCurrentPlayerSide)
						{
							iEnemyCount++;
							if (iEnemyCount>0)
								return true;
						}

		return false;
	}




}

void CCheckersBoard::ChooseSpellTarget(int x,int y)
{
	if (!AreAllBoardAnimationsDone())
		return;

	

	int iCurrentSpell;

	if (m_iChosenUnitX>=0 && m_iChosenUnitY>=0) // if a square was chosen
	if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray>=0) // if contains a live unit
	{
		iCurrentSpell = ((CEntityPlayer*)m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray])->m_pPlayerClient->m_iSpellType;



	// networking
	//if (m_Spells[iCurrentSpell].m_iEffects!=SPELL_EFFECTS_AUTO)
	if ( (m_iPlayerTypeFUTURE==PLAYER_TYPE_NETWORK && m_iCurrentPlayerSide==ARMY_SIDE_PAST) || 					
		(m_iPlayerTypePAST==PLAYER_TYPE_NETWORK && m_iCurrentPlayerSide==ARMY_SIDE_FUTURE) )
		g_Communication.SendBoardMessage(0,x,y,NULL);

	// if selected target is same as player then cancel all selection
	//if (m_Spells[iCurrentSpell].m_iEffects!=SPELL_EFFECTS_AUTO) // HERE HERE HERE
	if (x==m_iChosenUnitX && y==m_iChosenUnitY)
	{
		// debug debug debug
		m_iChosenUnitX=-1;
		m_iChosenUnitY=-1;
		
		m_iBoardGameState = GAME_BOARD_STATE_CHOOSE_UNIT;
		ClearBoardSelections();
		return;
	}
	
	}

	// a playable square
	if (!IsOdd(x+y)) // a diagonal line
		return;
	
	



	if (m_Spells[iCurrentSpell].m_iWay==SPELL_WAY_DIRECT_LINE_OF_SIGHT)
	{		
		int iDifferenceX = x-m_iChosenUnitX;
		int iDifferenceY = y-m_iChosenUnitY;

		if (abs(iDifferenceX)!=abs(iDifferenceY))
			return;

		iDifferenceX/=abs(iDifferenceX);
		iDifferenceY/=abs(iDifferenceY);

		
		
		bool bOK=true;

		for (int i=1;i<abs(x-m_iChosenUnitX)-1;i++)
			if (BoardsSquaresUnits[m_iChosenUnitX+iDifferenceX*i][m_iChosenUnitY+iDifferenceY*i].m_iIndexToPlayersArray>=0)
				bOK=false;

		if (!bOK)
			return;
	}
	else
	if (m_Spells[iCurrentSpell].m_iWay==SPELL_WAY_DOESNT_MATTER)
	{
		// do nothing :)
	}


	/*// SPELL_TYPE_SHOCKWAVE
	m_Spells[SPELL_TYPE_FIREBALL].m_iSpell = SPELL_TYPE_SHOCKWAVE;
	m_Spells[SPELL_TYPE_FIREBALL].m_iEffects = SPELL_EFFECTS_AUTO;
	m_Spells[SPELL_TYPE_FIREBALL].m_iRange = 3;
	m_Spells[SPELL_TYPE_FIREBALL].m_iWay = SPELL_WAY_DOESNT_MATTER;*/



	if (m_Spells[iCurrentSpell].m_iEffects==SPELL_EFFECTS_AUTO)
	{
		// do nothing
	}
	else
	if (m_Spells[iCurrentSpell].m_iEffects==SPELL_EFFECTS_ENEMY_UNIT)
	{

		// if empty then return;
		if (BoardsSquaresUnits[x][y].m_iIndexToPlayersArray<0)
			return;

		// if same army then return;
		if (BoardsSquaresUnits[x][y].m_iArmySide==m_iCurrentPlayerSide)
			return;


		m_iSpellCastUponTarget1X = x;
		m_iSpellCastUponTarget1Y = y;

	}
	else
	if (m_Spells[iCurrentSpell].m_iEffects==SPELL_EFFECTS_SELF_ONE_REST_OF_OWN_ARMY)
	{

		// if empty then return;
		if (BoardsSquaresUnits[x][y].m_iIndexToPlayersArray<0)
			return;

		// if different army then return;
		if (BoardsSquaresUnits[x][y].m_iArmySide!=m_iCurrentPlayerSide)
			return;

		/*// if same unit then return;
		if (m_iChosenUnitX==x && m_iChosenUnitY==y)
			return;*/

		m_iSpellCastUponTarget1X = x;
		m_iSpellCastUponTarget1Y = y;

	}	
	else // self two rest of own army
	if (m_Spells[iCurrentSpell].m_iEffects==SPELL_EFFECTS_SELF_TWO_REST_OF_OWN_ARMY)
	{
		// if empty then return;
		if (BoardsSquaresUnits[x][y].m_iIndexToPlayersArray<0)
			return;

		// if other army then return;
		if (BoardsSquaresUnits[x][y].m_iArmySide!=m_iCurrentPlayerSide)
			return;

		// is same as casting unit then return
		if (x==m_iChosenUnitX && y==m_iChosenUnitY)
			return;

		if (m_iChosenSpellTargetNum==0)
		{
			// fill in the first one
			m_iSpellCastUponTarget1X = x;
			m_iSpellCastUponTarget1Y = y;						
			m_iChosenSpellTargetNum=1;
			BoardsSquaresUnits[x][y].m_dwState = UNIT_SELECTED;
			return;
		}
		else
		if (m_iChosenSpellTargetNum==1)
		{
			if ( (x!=m_iSpellCastUponTarget1X) ||(y!=m_iSpellCastUponTarget1Y))
			{
				// fill in the second one
				m_iSpellCastUponTarget2X = x;
				m_iSpellCastUponTarget2Y = y;						
				m_iChosenSpellTargetNum=0;

				BoardsSquaresUnits[x][y].m_dwState = UNIT_SELECTED;
			}
			else // pressed again the same one
			{
				BoardsSquaresUnits[x][y].m_dwState = UNIT_NOT_SELECTED;
				m_iChosenSpellTargetNum=0;
				return;
			}
			
		}

	}
	else // self two enemy army
	if (m_Spells[iCurrentSpell].m_iEffects==SPELL_EFFECTS_ENEMY_TWO)
	{
		// if empty then return;
		if (BoardsSquaresUnits[x][y].m_iIndexToPlayersArray<0)
			return;

		// if same army then return;
		if (BoardsSquaresUnits[x][y].m_iArmySide==m_iCurrentPlayerSide)
			return;


		if (m_iChosenSpellTargetNum==0)
		{
			// fill in the first one
			m_iSpellCastUponTarget1X = x;
			m_iSpellCastUponTarget1Y = y;						
			m_iChosenSpellTargetNum=1;
			BoardsSquaresUnits[x][y].m_dwState = UNIT_SELECTED;
			return;
		}
		else
		if (m_iChosenSpellTargetNum==1)
		{
			if ( (x!=m_iSpellCastUponTarget1X) ||(y!=m_iSpellCastUponTarget1Y))
			{
				// fill in the second one
				m_iSpellCastUponTarget2X = x;
				m_iSpellCastUponTarget2Y = y;						
				m_iChosenSpellTargetNum=0;

				BoardsSquaresUnits[x][y].m_dwState = UNIT_SELECTED;
			}
			else // pressed again the same one
			{
				BoardsSquaresUnits[x][y].m_dwState = UNIT_NOT_SELECTED;
				m_iChosenSpellTargetNum=0;
				return;
			}
			
		}

	}


		
	m_iCurrentSpellCast = SPELL_TYPE_SWITCH_PLACES_OWN;



	m_iBoardGameState = GAME_BOARD_STATE_SHOWING_SPELL_ANIMATION;
	m_iAnimationState = SPELL_ANIMATION_FIRST_TURN;

	BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP = 
		m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_fAngY;

	m_fAnimationT=0.f;

	// i really need to make some array or sth for this...
	m_pMagicPS1=NULL;
	m_pMagicPS2=NULL;
	m_pMagicPS3=NULL;
	m_pMagicPS4=NULL;

}

void CCheckersBoard::ChooseTarget(int x, int y)
{

	/* // original original original
	// networking
	if ( (m_iPlayerTypeFUTURE==PLAYER_TYPE_NETWORK && m_iCurrentPlayerSide==ARMY_SIDE_PAST) || 					
		(m_iPlayerTypePAST==PLAYER_TYPE_NETWORK && m_iCurrentPlayerSide==ARMY_SIDE_FUTURE) )
		g_Communication.SendBoardMessage(0,x,y,NULL,false);
		*/

	if (!AreAllBoardAnimationsDone())
		return;


	// watch this
	m_fAnimationT=0.f;

	if (m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET_COMBO)
	{
		// networking
	if ( (m_iPlayerTypeFUTURE==PLAYER_TYPE_NETWORK && m_iCurrentPlayerSide==ARMY_SIDE_PAST) || 					
		(m_iPlayerTypePAST==PLAYER_TYPE_NETWORK && m_iCurrentPlayerSide==ARMY_SIDE_FUTURE) )
		g_Communication.SendBoardMessage(0,x,y,NULL);

		int iStartX,iStartY;

		iStartX = m_ComboMovesVector[m_iComboMovesNum-1].x;
		iStartY = m_ComboMovesVector[m_iComboMovesNum-1].y;

		if (IsKillMoveInTempBoard(iStartX,iStartY,x,y))
		{
			//g_SoundManager.PlaySample(m_ppBoardSoundSamples[CB_SOUND_SAMPLES_SELECT_DESTINATION],50);
			g_SoundManager.PlaySample3D(m_ppBoardSoundSamples[CB_SOUND_SAMPLES_SELECT_UNIT],GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY),170);


		
			//CopyBoardToTemp();

			/****/
			// i know that [x][y] is empty because IsKillMove returned true

			// simluate the future situation on the temp board(after the kill)
		
			// put the attacking unit in the final destination
			BoardsSquaresUnitsTEMP[x][y].m_iArmySide=m_iCurrentPlayerSide;
			BoardsSquaresUnitsTEMP[x][y].m_iIndexToPlayersArray=BoardsSquaresUnitsTEMP[iStartX][iStartY].m_iIndexToPlayersArray; // fake it
			BoardsSquaresUnitsTEMP[x][y].m_iUnitType = BoardsSquaresUnitsTEMP[iStartX][iStartY].m_iUnitType;
			BoardsSquaresUnitsTEMP[x][y].m_fYAngBACKUP = BoardsSquaresUnitsTEMP[iStartX][iStartY].m_fYAngBACKUP;

			// remove that attacking unit from the start position
			BoardsSquaresUnitsTEMP[iStartX][iStartY].m_iIndexToPlayersArray=-1;

			int iDifferenceX = x-iStartX;
			int iDifferenceY = y-iStartY;

			if (iDifferenceX>0)
				iDifferenceX=-1;
			else
				iDifferenceX=+1;

			if (iDifferenceY>0)
				iDifferenceY=-1;
			else
				iDifferenceY=+1;

			// remove that defending unit

			// ORIGINAL ORIGINAL ORIGINAL ORIGINAL 
			//BoardsSquaresUnitsTEMP[(iStartX+x)/2][(iStartY+y)/2].m_iIndexToPlayersArray=-1;	
			BoardsSquaresUnitsTEMP[x+iDifferenceX][y+iDifferenceY].m_iIndexToPlayersArray=-1;	

			
			m_bInComboState = true;

				m_iBoardGameState = GAME_BOARD_STATE_CHOOSE_TARGET_COMBO;
				m_iAnimationState = 0;

				
				m_ComboMovesVector[m_iComboMovesNum].x = x;
				m_ComboMovesVector[m_iComboMovesNum].y = y;
				m_iComboMovesNum++;

				BoardsSquaresUnits[x][y].m_dwState = UNIT_TARGET;

			if (CanUnitKillInTempBoard(x,y)) // if the same unit STILL can eat than stay in combo mode
			{

				m_fComboMoveTimeLeft = COMBO_MOVE_TIME;

				return;
			}
			else // combo options ended
			{
				m_iBoardGameState = GAME_BOARD_STATE_SHOWING_COMBO_ANIMATION;
				m_iPlaceInComboMoveAnimation = 0;
				
				m_iSubComboFromX=m_iChosenUnitX;
				m_iSubComboFromY=m_iChosenUnitY;

				m_iSubComboToX=m_ComboMovesVector[0].x;
				m_iSubComboToY=m_ComboMovesVector[0].y;

				BoardsSquaresUnits[m_iSubComboFromX][m_iSubComboFromY].m_fYAngBACKUP = 
					m_ppGameUnits[BoardsSquaresUnits[m_iSubComboFromX][m_iSubComboFromY].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_fAngY;
			}
			
		}
		return;
	}
	
	
	// i wish to skip this in networking

	
	if (BoardsSquaresUnits[x][y].m_iIndexToPlayersArray>=0)
		if (BoardsSquaresUnits[x][y].m_iArmySide==m_iCurrentPlayerSide)
		{

			if (g_Communication.GetAlreadyInited())
			// networking
				if ( (m_iPlayerTypeFUTURE==PLAYER_TYPE_NETWORK && m_iCurrentPlayerSide==ARMY_SIDE_PAST) || 					
					(m_iPlayerTypePAST==PLAYER_TYPE_NETWORK && m_iCurrentPlayerSide==ARMY_SIDE_FUTURE) )
					if (x!=m_iChosenUnitX || y!=m_iChosenUnitY)
						g_Communication.SendBoardMessage(0,x,y,NULL);

			
			//if ((x!=m_iChosenUnitX || y!=m_iChosenUnitY) || !g_Communication.GetAlreadyInited())
			if ((x!=m_iChosenUnitX || y!=m_iChosenUnitY))
				ChooseUnit(x,y);						

			return;
		}


	// networking
	if ( (m_iPlayerTypeFUTURE==PLAYER_TYPE_NETWORK && m_iCurrentPlayerSide==ARMY_SIDE_PAST) || 					
		(m_iPlayerTypePAST==PLAYER_TYPE_NETWORK && m_iCurrentPlayerSide==ARMY_SIDE_FUTURE) )
		g_Communication.SendBoardMessage(0,x,y,NULL);

	// if unit can kill, force it to kill
	if (CanUnitKill(m_iChosenUnitX,m_iChosenUnitY))
	{
		if (!IsKillMove(m_iChosenUnitX,m_iChosenUnitY,x,y))
			return;
	}
			


	//MessageBox(0,"1","ChooseTarget - WORKS",MB_OK);

	// a simple walk

	if (IsFreeMove(m_iChosenUnitX,m_iChosenUnitY,x,y))
		{			
			BoardsSquaresUnits[x][y].m_dwState = UNIT_TARGET;
			m_iBoardGameState = GAME_BOARD_STATE_SHOWING_WALK_ANIMATION ;

			m_iTargetX = x;
			m_iTargetY = y;

			BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP = 
				m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_fAngY;

			if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType==UNIT_TYPE_SOLDIER)
			{

				//g_SoundManager.PlaySample(m_ppBoardSoundSamples[CB_SOUND_SAMPLES_SELECT_DESTINATION],50);
				g_SoundManager.PlaySample3D(m_ppBoardSoundSamples[CB_SOUND_SAMPLES_SELECT_UNIT],GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY),170);

				if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iArmySide==ARMY_SIDE_PAST)
				{
					if (m_iTargetX>m_iChosenUnitX)
						m_iWalkAnimationRotatingDirection = ROTATING_DIRECTION_RIGHT;
					else
						m_iWalkAnimationRotatingDirection = ROTATING_DIRECTION_LEFT;
				}
				else
				{
					if (m_iTargetX>m_iChosenUnitX)
						m_iWalkAnimationRotatingDirection = ROTATING_DIRECTION_LEFT;
					else
						m_iWalkAnimationRotatingDirection = ROTATING_DIRECTION_RIGHT;
				}
			}


			// if free move - then check if there are units that should be burned

			/**/
			sMoveAI currentMove;
			currentMove.vFrom.x=m_iChosenUnitX;
			currentMove.vFrom.y=m_iChosenUnitY;
			currentMove.vTo.x=m_iTargetX;
			currentMove.vTo.y=m_iTargetY;
			MOVES_VECTOR shouldBurn;
			m_CheckersAI.GetUnitsThatShouldBeBurned(m_iCurrentPlayerSide,currentMove,shouldBurn);

			int iTempIndex=-1;

			for (int b=0;b<shouldBurn.size();b++)
			{
				iTempIndex = BoardsSquaresUnits[shouldBurn[b].vFrom.x][shouldBurn[b].vFrom.y].m_iIndexToPlayersArray;

				if (iTempIndex>=0)
				{	
					m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_bBurning = true;
					m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_pPlayerModel->SetDelayedDeath(2000.f);
					m_ppGameUnits[iTempIndex]->m_pPlayerClient->Die();
					//m_ppGameUnits[iTempIndex]->m_pPlayerClient->PlaySound(CLIENT_SOUND_SAMPLE_DEATH);// should be burn here
					m_ppGameUnits[iTempIndex]->m_pPlayerClient->PlaySound(CLIENT_SOUND_SAMPLE_PAIN_100);// should be burn here
					// which is 2 seconds burn and then die

					/*
					m_ParticleSystem.Init(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,90);
					m_ParticleSystem.SetParticlePos(m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_pPlayerModel->m_vBonePosition);
					m_ParticleSystem.SetRevive();*/

					CVector3f* pv;
					pv = new CVector3f[9];

					
					
					pv[0].Set(GetMiddleOfSquare(shouldBurn[b].vFrom.x,shouldBurn[b].vFrom.y) + CVector3f(HALF_BOARD_SQUARE_SIZE/2.f,0.f,HALF_BOARD_SQUARE_SIZE/2.f));

					

					pv[1].Set(GetMiddleOfSquare(shouldBurn[b].vFrom.x,shouldBurn[b].vFrom.y) + CVector3f(-HALF_BOARD_SQUARE_SIZE/2.f,0.f,HALF_BOARD_SQUARE_SIZE/2.f));
					pv[2].Set(GetMiddleOfSquare(shouldBurn[b].vFrom.x,shouldBurn[b].vFrom.y) + CVector3f(HALF_BOARD_SQUARE_SIZE/2.f,0.f,-HALF_BOARD_SQUARE_SIZE/2.f));
					pv[3].Set(GetMiddleOfSquare(shouldBurn[b].vFrom.x,shouldBurn[b].vFrom.y) + CVector3f(-HALF_BOARD_SQUARE_SIZE/2.f,0.f,-HALF_BOARD_SQUARE_SIZE/2.f));

					pv[4].Set(GetMiddleOfSquare(shouldBurn[b].vFrom.x,shouldBurn[b].vFrom.y) + CVector3f(HALF_BOARD_SQUARE_SIZE*0.9f,0.f,HALF_BOARD_SQUARE_SIZE*0.9f));
					pv[5].Set(GetMiddleOfSquare(shouldBurn[b].vFrom.x,shouldBurn[b].vFrom.y) + CVector3f(-HALF_BOARD_SQUARE_SIZE*0.9f,0.f,HALF_BOARD_SQUARE_SIZE*0.9f));
					pv[6].Set(GetMiddleOfSquare(shouldBurn[b].vFrom.x,shouldBurn[b].vFrom.y) + CVector3f(HALF_BOARD_SQUARE_SIZE*0.9f,0.f,-HALF_BOARD_SQUARE_SIZE*0.9f));
					pv[7].Set(GetMiddleOfSquare(shouldBurn[b].vFrom.x,shouldBurn[b].vFrom.y) + CVector3f(-HALF_BOARD_SQUARE_SIZE*0.9f,0.f,-HALF_BOARD_SQUARE_SIZE*0.9f));

					pv[8].Set(GetMiddleOfSquare(shouldBurn[b].vFrom.x,shouldBurn[b].vFrom.y));


					CParticleSystem* pPS;



					//

					int iAnimationIndex=m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_pPlayerModel->m_pUpper->GetAnimationByID(BOTH_DEATH1);

					float fDeathAnimationTime = 
						(1.f / m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_pPlayerModel->m_pUpper->m_pMD3Shared->animations[iAnimationIndex].framesPerSecond);


					int iFramesNum = (m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_pPlayerModel->m_pUpper->m_pMD3Shared->animations[iAnimationIndex].endFrame-
											m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_pPlayerModel->m_pUpper->m_pMD3Shared->animations[iAnimationIndex].startFrame);

					fDeathAnimationTime*=((float)iFramesNum)*1000.f;

					
						

					// set the unit on fire
					pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,90, &(m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_pPlayerModel->m_vBonePosition),true,CVector3f(0.f,0.f,0.f),1000.f);
					pPS->SetTimeToDeath(fDeathAnimationTime+1000.f);
					
					// set the square on fire
					pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,50,&pv[0],false,CVector3f(0.f,0.f,0.f),0.f);
					pPS->SetTimeToDeath(fDeathAnimationTime+3000.f);
					
					pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,50,&pv[1],false,CVector3f(0.f,0.f,0.f),0.f);
					pPS->SetTimeToDeath(fDeathAnimationTime+3000.f);

					pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,50,&pv[2],false,CVector3f(0.f,0.f,0.f),0.f);
					pPS->SetTimeToDeath(fDeathAnimationTime+3000.f);

					pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,50,&pv[3],false,CVector3f(0.f,0.f,0.f),0.f);
					pPS->SetTimeToDeath(fDeathAnimationTime+3000.f);

					pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,50,&pv[4],false,CVector3f(0.f,0.f,0.f),0.f);
					pPS->SetTimeToDeath(fDeathAnimationTime+3000.f);

					pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,50,&pv[5],false,CVector3f(0.f,0.f,0.f),0.f);
					pPS->SetTimeToDeath(fDeathAnimationTime+3000.f);

					pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,50,&pv[6],false,CVector3f(0.f,0.f,0.f),0.f);
					pPS->SetTimeToDeath(fDeathAnimationTime+3000.f);

					pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,50,&pv[7],false,CVector3f(0.f,0.f,0.f),0.f);
					pPS->SetTimeToDeath(fDeathAnimationTime+3000.f);

					pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,50,&pv[8],false,CVector3f(0.f,0.f,0.f),0.f);
					pPS->SetTimeToDeath(fDeathAnimationTime+3000.f);
					

					SafeDeleteArray(pv);

					/*st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,40, &(m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_pPlayerModel->m_vBonePosition),CVector3f(-2.f,5.f,2.f));
					st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE,40, &(m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_pPlayerModel->m_vBonePosition),CVector3f(2.f,-5.f,-2.f));*/
					

					

				}
			}

			/**/
		}
	else
	if (IsKillMove(m_iChosenUnitX,m_iChosenUnitY,x,y))
	{

		//g_SoundManager.PlaySample(m_ppBoardSoundSamples[CB_SOUND_SAMPLES_SELECT_DESTINATION],50);
		g_SoundManager.PlaySample3D(m_ppBoardSoundSamples[CB_SOUND_SAMPLES_SELECT_UNIT],GetMiddleOfSquare(m_iChosenUnitX,m_iChosenUnitY),170);


		BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP = 
				m_ppGameUnits[BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_fAngY;

		/*if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType==UNIT_TYPE_QUEEN)	
			MessageBox(0,"1","Queen Attack Move",MB_OK);*/

		CopyBoardToTemp();

		/****/
		// i know that [x][y] is empty because IsKillMove returned true

		// simluate the future situation (after the kill)
		
		// put the attacking unit in the final destination
		BoardsSquaresUnitsTEMP[x][y].m_iArmySide=m_iCurrentPlayerSide;
		BoardsSquaresUnitsTEMP[x][y].m_iIndexToPlayersArray=BoardsSquaresUnitsTEMP[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray; // fake it
		BoardsSquaresUnitsTEMP[x][y].m_fYAngBACKUP =BoardsSquaresUnitsTEMP[m_iChosenUnitX][m_iChosenUnitY].m_fYAngBACKUP;
		BoardsSquaresUnitsTEMP[x][y].m_iUnitType=BoardsSquaresUnitsTEMP[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType;

		// remove that attacking unit from the start position
		int iSaveAttackerIndex = BoardsSquaresUnitsTEMP[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray;
		BoardsSquaresUnitsTEMP[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray=-1;


		int iSaveDefenderIndex;

		// remove that defending unit
		if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iUnitType==UNIT_TYPE_QUEEN)
		{
			int iDifferenceX = x-m_iChosenUnitX;
			int iDifferenceY = y-m_iChosenUnitY;

			if (iDifferenceX>0)
				iDifferenceX=-1;
			else
				iDifferenceX=+1;

			if (iDifferenceY>0)
				iDifferenceY=-1;
			else
				iDifferenceY=+1;


			iSaveDefenderIndex = BoardsSquaresUnitsTEMP[x+iDifferenceX ][y+iDifferenceY].m_iIndexToPlayersArray;
			BoardsSquaresUnitsTEMP[x+iDifferenceX][y+iDifferenceY].m_iIndexToPlayersArray=-1;

		}
		else // soldier unit
		{
			iSaveDefenderIndex = BoardsSquaresUnitsTEMP[ (m_iChosenUnitX+x)/2][(m_iChosenUnitY+y)/2].m_iIndexToPlayersArray;
			BoardsSquaresUnitsTEMP[(m_iChosenUnitX+x)/2][(m_iChosenUnitY+y)/2].m_iIndexToPlayersArray=-1;
		}

		if (CanUnitKillInTempBoard(x,y))
		{
			// means that we are in combo situation

			m_bInComboState = true;

			m_fComboMoveTimeLeft = COMBO_MOVE_TIME;

			m_iBoardGameState = GAME_BOARD_STATE_CHOOSE_TARGET_COMBO;
			m_iAnimationState = 0;


			m_iComboMovesNum=0;
			m_ComboMovesVector[m_iComboMovesNum].x = x;
			m_ComboMovesVector[m_iComboMovesNum].y = y;
			m_iComboMovesNum++;


			BoardsSquaresUnits[x][y].m_dwState = UNIT_TARGET;

		



			if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iArmySide==ARMY_SIDE_PAST)
			{
				if (m_iTargetX>m_iChosenUnitX)
					m_iWalkAnimationRotatingDirection = ROTATING_DIRECTION_RIGHT;
				else
					m_iWalkAnimationRotatingDirection = ROTATING_DIRECTION_LEFT;
			}
			else
			{
				if (m_iTargetX>m_iChosenUnitX)
					m_iWalkAnimationRotatingDirection = ROTATING_DIRECTION_LEFT;
				else
					m_iWalkAnimationRotatingDirection = ROTATING_DIRECTION_RIGHT;
			}	

			return;
		}
		else
		{
			/**/ // "pop" everything back
			/*BoardsSquaresUnits[x][y].m_iIndexToPlayersArray=-1;
			BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iIndexToPlayersArray = iSaveAttackerIndex;
			BoardsSquaresUnits[(m_iChosenUnitX+x)/2][(m_iChosenUnitY+y)/2].m_iIndexToPlayersArray = iSaveDefenderIndex;*/
			/**/

			m_bInComboState = false;
		}

		
		/****/
		
			BoardsSquaresUnits[x][y].m_dwState = UNIT_TARGET;
			m_iBoardGameState = GAME_BOARD_STATE_SHOWING_FIGHT_ANIMATION;

			m_iTargetX = x;
			m_iTargetY = y;

			

			m_iFightAgainstX = (m_iTargetX+m_iChosenUnitX) / 2;
			m_iFightAgainstY = (m_iTargetY+m_iChosenUnitY) / 2;


			

			if (BoardsSquaresUnits[m_iChosenUnitX][m_iChosenUnitY].m_iArmySide==ARMY_SIDE_PAST)
			{
				if (m_iTargetX>m_iChosenUnitX)
					m_iWalkAnimationRotatingDirection = ROTATING_DIRECTION_RIGHT;
				else
					m_iWalkAnimationRotatingDirection = ROTATING_DIRECTION_LEFT;
			}
			else
			{
				if (m_iTargetX>m_iChosenUnitX)
					m_iWalkAnimationRotatingDirection = ROTATING_DIRECTION_LEFT;
				else
					m_iWalkAnimationRotatingDirection = ROTATING_DIRECTION_RIGHT;
			}	

	}

}

bool CCheckersBoard::IsFreeMove(int fromX,int fromY,int toX, int toY)
{
	// only check for a simple unit (not for a queen)

	if (BoardsSquaresAccesablity[toX][toY]==ACCESABLE_SQUARE)
		if (BoardsSquaresUnits[toX][toY].m_iIndexToPlayersArray==-1)
		{
			if (BoardsSquaresUnits[fromX][fromY].m_iUnitType==UNIT_TYPE_SOLDIER)
			{
				if ( (toX==fromX+1)||(toX==fromX-1) )
				{	
					if (m_iCurrentPlayerSide==ARMY_SIDE_PAST) // past going up
						if (toY==fromY+1)
							return true;

					if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE) // future going down
						if (toY==fromY-1)
							return true;
				}
			}
			else
			if (BoardsSquaresUnits[fromX][fromY].m_iUnitType==UNIT_TYPE_QUEEN)
			{
				int iDifferenceX,iDifferenceY;
				iDifferenceX = toX - fromX;
				iDifferenceY = toY - fromY;

				if (abs(iDifferenceX)==abs(iDifferenceY)) // we are in a diagonal line
				{
					
					//for (int m=1;m<abs(iDifferenceX);m++)
					for (int m=1;m<abs(iDifferenceX)+1;m++)
					{
						int iCurrentX=fromX + (iDifferenceX/(abs(iDifferenceX)))*m;
						int iCurrentY=fromY + (iDifferenceY/(abs(iDifferenceY)))*m;

						if (BoardsSquaresUnits[iCurrentX][iCurrentY].m_iIndexToPlayersArray>=0)
							return false;
					}

					//MessageBox(0,"1","Queen Move Free Walk",MB_OK);
				
					return true;

				}
				/*if ( (toX==fromX+1)||(toX==fromX-1) )
				{	
					if (m_iCurrentPlayerSide==ARMY_SIDE_PAST) // past going up
						if (toY==fromY+1)
							return true;

					if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE) // future going down
						if (toY==fromY-1)
							return true;
				}*/
			}


		}

				

	return false;
}

int CCheckersBoard::CanUnitKill(int x, int y)
{
	static int iUnitSide;



	iUnitSide = BoardsSquaresUnits[x][y].m_iArmySide;

	int landX,landY,fightX,fightY;
	
	if (BoardsSquaresUnits[x][y].m_iUnitType == UNIT_TYPE_SOLDIER)
	{
        if (BoardsSquaresUnits[x][y].m_iIndexToPlayersArray>=0)
		{
			if (BoardsSquaresUnits[x][y].m_iArmySide==ARMY_SIDE_PAST)
			{
				// check left up kill
				if (x>1 && y<SQUARES_NUM_Y-3)				
					if (BoardsSquaresUnits[x-1][y+1].m_iIndexToPlayersArray>=0) // left up if not empty
						if (BoardsSquaresUnits[x-1][y+1].m_iArmySide==ARMY_SIDE_FUTURE) // left up if enemy side
							if (BoardsSquaresUnits[x-2][y+2].m_iIndexToPlayersArray<0) // 2xleft 2xup is empty
								return KILL_DIRECTION_LEFT;					



				// check right up kill
				if (x<SQUARES_NUM_X-x && y<SQUARES_NUM_Y-3)				
					if (BoardsSquaresUnits[x+1][y+1].m_iIndexToPlayersArray>=0) // right up if not empty
						if (BoardsSquaresUnits[x+1][y+1].m_iArmySide==ARMY_SIDE_FUTURE) // right up if enemy side
							if (BoardsSquaresUnits[x+2][y+2].m_iIndexToPlayersArray<0) // 2xright 2xup is empty
								return KILL_DIRECTION_RIGHT;									
			}
			else // ARMY_SIDE_FUTURE
			{
				// check left down kill
				if (x>1 && y>1)				
					if (BoardsSquaresUnits[x-1][y-1].m_iIndexToPlayersArray>=0) // left downp if not empty
						if (BoardsSquaresUnits[x-1][y-1].m_iArmySide==ARMY_SIDE_PAST) // left down if enemy side
							if (BoardsSquaresUnits[x-2][y-2].m_iIndexToPlayersArray<0) // 2xleft 2xdown is empty
								return KILL_DIRECTION_LEFT;					



				// check right down kill
				if (x<SQUARES_NUM_X-x && y>1)				
					if (BoardsSquaresUnits[x+1][y-1].m_iIndexToPlayersArray>=0) // right down if not empty
						if (BoardsSquaresUnits[x+1][y-1].m_iArmySide==ARMY_SIDE_PAST) // right down if enemy side
							if (BoardsSquaresUnits[x+2][y-2].m_iIndexToPlayersArray<0) // 2xright 2xdown is empty
								return KILL_DIRECTION_RIGHT;									

			}


		}

		return KILL_DIRECTION_NONE;
	}
	else
	if (BoardsSquaresUnits[x][y].m_iUnitType == UNIT_TYPE_QUEEN)
	{
		// here i need to put code for queen

		for (int i=0;i<SQUARES_NUM_X;i++)
			for (int j=0;j<SQUARES_NUM_Y;j++)
			{
				if (IsKillMove(x,y,i,j))
					return 1;				
			}


		return 0; // nothing to eat
	}
	else
		return 0;
	
}


int CCheckersBoard::CanUnitKillInTempBoard(int x, int y)
{
	static int iUnitSide;



	iUnitSide = BoardsSquaresUnitsTEMP[x][y].m_iArmySide;

	int landX,landY,fightX,fightY;
	
	if (BoardsSquaresUnitsTEMP[x][y].m_iUnitType == UNIT_TYPE_SOLDIER)
	{
        if (BoardsSquaresUnitsTEMP[x][y].m_iIndexToPlayersArray>=0)
		{
			if (BoardsSquaresUnitsTEMP[x][y].m_iArmySide==ARMY_SIDE_PAST)
			{
				// check left up kill
				if (x>1 && y<SQUARES_NUM_Y-2)				
					if (BoardsSquaresUnitsTEMP[x-1][y+1].m_iIndexToPlayersArray>=0) // left up if not empty
						if (BoardsSquaresUnitsTEMP[x-1][y+1].m_iArmySide==ARMY_SIDE_FUTURE) // left up if enemy side
							if (BoardsSquaresUnitsTEMP[x-2][y+2].m_iIndexToPlayersArray<0) // 2xleft 2xup is empty
								return KILL_DIRECTION_LEFT;					



				// check right up kill
				if (x<SQUARES_NUM_X-2 && y<SQUARES_NUM_Y-2)				
					if (BoardsSquaresUnitsTEMP[x+1][y+1].m_iIndexToPlayersArray>=0) // right up if not empty
						if (BoardsSquaresUnitsTEMP[x+1][y+1].m_iArmySide==ARMY_SIDE_FUTURE) // right up if enemy side
							if (BoardsSquaresUnitsTEMP[x+2][y+2].m_iIndexToPlayersArray<0) // 2xright 2xup is empty
								return KILL_DIRECTION_RIGHT;									
			}
			else // ARMY_SIDE_FUTURE
			{
				// check left down kill
				if (x>1 && y>1)				
					if (BoardsSquaresUnitsTEMP[x-1][y-1].m_iIndexToPlayersArray>=0) // left downp if not empty
						if (BoardsSquaresUnitsTEMP[x-1][y-1].m_iArmySide==ARMY_SIDE_PAST) // left down if enemy side
							if (BoardsSquaresUnitsTEMP[x-2][y-2].m_iIndexToPlayersArray<0) // 2xleft 2xdown is empty
								return KILL_DIRECTION_LEFT;					



				// check right down kill
				if (x<SQUARES_NUM_X-2 && y>1)				
					if (BoardsSquaresUnitsTEMP[x+1][y-1].m_iIndexToPlayersArray>=0) // right down if not empty
						if (BoardsSquaresUnitsTEMP[x+1][y-1].m_iArmySide==ARMY_SIDE_PAST) // right down if enemy side
							if (BoardsSquaresUnitsTEMP[x+2][y-2].m_iIndexToPlayersArray<0) // 2xright 2xdown is empty
								return KILL_DIRECTION_RIGHT;									

			}


		}

		return KILL_DIRECTION_NONE;
	}
	else
	if (BoardsSquaresUnitsTEMP[x][y].m_iUnitType == UNIT_TYPE_QUEEN)
	{
				// here i need to put code for queen

		for (int i=0;i<SQUARES_NUM_X;i++)
			for (int j=0;j<SQUARES_NUM_Y;j++)
			{
				if (IsKillMoveInTempBoard(x,y,i,j))
					return 1;				
			}


		return 0; // nothing to eat
	}
	else
		return 0;
	

}

bool CCheckersBoard::IsKillMove(int fromX,int fromY,int toX, int toY)
{
	

	if (BoardsSquaresAccesablity[toX][toY]==ACCESABLE_SQUARE)
		if (BoardsSquaresUnits[toX][toY].m_iIndexToPlayersArray==-1)
		{
			if ( BoardsSquaresUnits[fromX][fromY].m_iUnitType==UNIT_TYPE_SOLDIER)
			{
				if ( (toX==fromX+2)||(toX==fromX-2) )
				{
					if (m_iCurrentPlayerSide==ARMY_SIDE_PAST)
						if (toY==fromY+2)
						{
							// now see if there's an enemy unit to pass
							if (toX==fromX+2)
								if (BoardsSquaresUnits[fromX+1][fromY+1].m_iIndexToPlayersArray>=0)														
									if (BoardsSquaresUnits[fromX+1][fromY+1].m_iArmySide==ARMY_SIDE_FUTURE)
										return true;

							// now see if there's an enemy unit to pass
							if (toX==fromX-2)
								if (BoardsSquaresUnits[fromX-1][fromY+1].m_iIndexToPlayersArray>=0)														
									if (BoardsSquaresUnits[fromX-1][fromY+1].m_iArmySide==ARMY_SIDE_FUTURE)
										return true;
						}

					if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
						if (toY==fromY-2)
						{
							// now see if there's an enemy unit to pass
							if (toX==fromX+2)
								if (BoardsSquaresUnits[fromX+1][fromY-1].m_iIndexToPlayersArray>=0)														
									if (BoardsSquaresUnits[fromX+1][fromY-1].m_iArmySide==ARMY_SIDE_PAST)
										return true;

							// now see if there's an enemy unit to pass
							if (toX==fromX-2)
								if (BoardsSquaresUnits[fromX-1][fromY-1].m_iIndexToPlayersArray>=0)														
									if (BoardsSquaresUnits[fromX-1][fromY-1].m_iArmySide==ARMY_SIDE_PAST)
										return true;		
						}
					}
			}//
			else
			if ( BoardsSquaresUnits[fromX][fromY].m_iUnitType==UNIT_TYPE_QUEEN)
			{	
				int iDifferenceX,iDifferenceY;
				iDifferenceX = toX - fromX;
				iDifferenceY = toY - fromY;

				//if (abs(iDifferenceX)==abs(iDifferenceY)) // we are in a diagonal line
				if ( (abs(iDifferenceX)>1) && (abs(iDifferenceX)==abs(iDifferenceY))) // we are in a diagonal line
				{
					
					for (int m=1;m<abs(iDifferenceX)+1;m++)
					{
						int iCurrentX=fromX + (iDifferenceX/(abs(iDifferenceX)))*m;
						int iCurrentY=fromY + (iDifferenceY/(abs(iDifferenceY)))*m;

						if (m==abs(iDifferenceX)-1)
						{
							// if there isn't a unit in the one square before last then it's not a kill move
							if (BoardsSquaresUnits[iCurrentX][iCurrentY].m_iIndexToPlayersArray<0)
								return false;

							// if from the same army then we can't eat it...
							if ( BoardsSquaresUnits[fromX][fromY].m_iArmySide==BoardsSquaresUnits[iCurrentX][iCurrentY].m_iArmySide)
								return false;

						}
						else
						if (BoardsSquaresUnits[iCurrentX][iCurrentY].m_iIndexToPlayersArray>=0)
							return false;
					}
				}
				else
					return false;

				return true;
			}
			


		}
				

	return false;
}


bool CCheckersBoard::IsKillMoveInTempBoard(int fromX,int fromY,int toX, int toY)
{
	if (BoardsSquaresAccesablity[toX][toY]==ACCESABLE_SQUARE)
		if (BoardsSquaresUnitsTEMP[toX][toY].m_iIndexToPlayersArray==-1)
		{
			if ( BoardsSquaresUnitsTEMP[fromX][fromY].m_iUnitType==UNIT_TYPE_SOLDIER)
			{
				if ( (toX==fromX+2)||(toX==fromX-2) )
				{
					if (m_iCurrentPlayerSide==ARMY_SIDE_PAST)
						if (toY==fromY+2)
						{
							// now see if there's an enemy unit to pass
							if (toX==fromX+2)
								if (BoardsSquaresUnitsTEMP[fromX+1][fromY+1].m_iIndexToPlayersArray>=0)														
									if (BoardsSquaresUnitsTEMP[fromX+1][fromY+1].m_iArmySide==ARMY_SIDE_FUTURE)
										return true;

							// now see if there's an enemy unit to pass
							if (toX==fromX-2)
								if (BoardsSquaresUnitsTEMP[fromX-1][fromY+1].m_iIndexToPlayersArray>=0)														
									if (BoardsSquaresUnitsTEMP[fromX-1][fromY+1].m_iArmySide==ARMY_SIDE_FUTURE)
										return true;
						}

					if (m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
						if (toY==fromY-2)
						{
							// now see if there's an enemy unit to pass
							if (toX==fromX+2)
								if (BoardsSquaresUnitsTEMP[fromX+1][fromY-1].m_iIndexToPlayersArray>=0)														
									if (BoardsSquaresUnitsTEMP[fromX+1][fromY-1].m_iArmySide==ARMY_SIDE_PAST)
										return true;

							// now see if there's an enemy unit to pass
							if (toX==fromX-2)
								if (BoardsSquaresUnitsTEMP[fromX-1][fromY-1].m_iIndexToPlayersArray>=0)														
									if (BoardsSquaresUnitsTEMP[fromX-1][fromY-1].m_iArmySide==ARMY_SIDE_PAST)
										return true;		
						}
					}
			}//
			else
			if ( BoardsSquaresUnitsTEMP[fromX][fromY].m_iUnitType==UNIT_TYPE_QUEEN)
			{	
				int iDifferenceX,iDifferenceY;
				iDifferenceX = toX - fromX;
				iDifferenceY = toY - fromY;

				if ( (abs(iDifferenceX)>1) && (abs(iDifferenceX)==abs(iDifferenceY))) // we are in a diagonal line
				{
					
					for (int m=1;m<abs(iDifferenceX)+1;m++)
					{
						int iCurrentX=fromX + (iDifferenceX/(abs(iDifferenceX)))*m;
						int iCurrentY=fromY + (iDifferenceY/(abs(iDifferenceY)))*m;

						if (m==abs(iDifferenceX)-1)
						{
							// if there isn't a unit in the one square before last then it's not a kill move
							if (BoardsSquaresUnitsTEMP[iCurrentX][iCurrentY].m_iIndexToPlayersArray<0)
								return false;

							// if from the same army then we can't eat it...
							if ( BoardsSquaresUnitsTEMP[fromX][fromY].m_iArmySide==BoardsSquaresUnitsTEMP[iCurrentX][iCurrentY].m_iArmySide)
								return false;

						}
						else
						if (BoardsSquaresUnitsTEMP[iCurrentX][iCurrentY].m_iIndexToPlayersArray>=0)
							return false;
					}
				}
				else
					return false;

				return true;
			}
			


		}
				

	return false;
}

void CCheckersBoard::DrawGameUnits()
{
/*	for (int i=0;i<m_iNumOfUnits;i++)
	if ( !     ((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->m_bFinishedDying )
	{
		m_ppGameUnits[i]->Draw();
	}*/

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			int iIndex = BoardsSquaresUnits[i][j].m_iIndexToPlayersArray;

			//if (iIndex>=0 && iIndex!=9999)
			if (iIndex>=0)
				if (!m_ppGameUnits[iIndex]->m_pPlayerClient->m_bFinishedDying)
				{

					// debug - just check the first
					//if (iIndex==11)
					//if (iIndex>7 && iIndex<16)
					/*if ( (iIndex>=0 && iIndex<12) ||  (iIndex>23 && iIndex<36))
					{
						g_cgslShader.UpdateDiffuseColorBUMPMAP(2.f,2.f,2.f);
						g_cgslShader.UpdateLightColorBUMPMAP(0.9,0.6,0.6);
					}
					else
					{
						g_cgslShader.UpdateDiffuseColorBUMPMAP(2.f,2.f,2.f);
						g_cgslShader.UpdateLightColorBUMPMAP(0.6,0.6,0.9);
					}*/

					if (st_RendererI->m_bPPLused)
					{
						g_cgslShader.UpdateDiffuseColorBUMPMAP(1.2f,1.2f,1.2f);
						g_cgslShader.UpdateLightColorBUMPMAP(1.f,1.f,1.f);
					}

					//if ((iIndex==0) || (iIndex==20))

					m_ppGameUnits[iIndex]->Draw(true);
					// really render to screen everything that was pushed to the renderer
					//st_Renderer::Inst()->RenderAll(m_ppGameUnits[iIndex]->m_pPlayerClient->m_pPlayerModel->get); 
				}

		}

		int iDebug;

		iDebug = 4356456;
}


void CCheckersBoard::UpdateAIBoard(void)
{
	static int iIndex;

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			iIndex = BoardsSquaresUnits[i][j].m_iIndexToPlayersArray;
			if (iIndex>=0)
			{
				m_CheckersAI.m_Board[i][j].m_bHoldAUnit = true;
				m_CheckersAI.m_Board[i][j].m_iExperiencePoints =  m_ppGameUnits[iIndex]->m_pPlayerClient->m_iExperiencePoints;
				m_CheckersAI.m_Board[i][j].m_iType = BoardsSquaresUnits[i][j].m_iUnitType;
				m_CheckersAI.m_Board[i][j].m_bParalised = BoardsSquaresUnits[i][j].m_bParalised;

				// because the difference of int / bool (... ;))

				if (BoardsSquaresUnits[i][j].m_iArmySide==ARMY_SIDE_PAST)
					m_CheckersAI.m_Board[i][j].m_bArmySide = ARMY_SIDE_PAST;
				else
					m_CheckersAI.m_Board[i][j].m_bArmySide = ARMY_SIDE_FUTURE;

			}
			else
				m_CheckersAI.m_Board[i][j].m_bHoldAUnit = false;		
			

		}

}

/*void CCheckersBoard::DrawParticleSystems(void)
{
	//m_ParticleSystem.DebugDrawParticles();
}

void CCheckersBoard::UpdateParticlesSystems(void)
{
	//m_ParticleSystem.SetParticlePos(g_vBoardRayCollision + CVector3f(0.f,30.f,0.f));

	//m_ParticleSystem.SetParticlePos(m_ppGameUnits[0]->m_pPlayerClient->m_pPlayerModel->m_vBonePosition);

	//m_ParticleSystem.UpdateParticles();
}*/

void CCheckersBoard::ActivateBurningUnitsLights(void)
{
	CLight* pLight;

	for (int i=0;i<m_iNumOfUnits;i++)
		if ((!m_ppGameUnits[i]->m_pPlayerClient->m_bFinishedDying) ||
			(m_ppGameUnits[i]->m_pPlayerClient->m_fTimeSinceDied<3000.f))
		if (m_ppGameUnits[i]->m_pPlayerClient->m_bBurning)
		{
			pLight = st_RendererI->AddLight();
			pLight->m_fRadius=3.f;
			pLight->m_vPos.Set(m_ppGameUnits[i]->m_pPlayerClient->m_pPlayerModel->m_vWorldPosition );
			pLight->m_vColor.Set(1.f,1.f,1.f);
	
		}
}

bool CCheckersBoard::EndGame(void) // is this used at all????
{
	Destroy();
	return true;
}

bool CCheckersBoard::NewGame(void)
{
	/*if(!AreAllBoardAnimationsDone())
		return false;*/
	Destroy();
	Init(true);
	m_iBoardGameState=GAME_BOARD_STATE_CHOOSE_UNIT;




	CGUIWindow* win;

	win = g_pGUIManager->m_pOptionsMenu;
	CGUI2StatesButton* p2SidedButton;
	
	
	/*p2SidedButton = (CGUI2StatesButton*)win->GetElementNum(0);	
	if (p2SidedButton->GetState()==TWO_SIDES_BUTTON_STATE_1)
		m_iPlayerTypePAST=PLAYER_TYPE_HUMAN;		
	else
		m_iPlayerTypePAST=PLAYER_TYPE_PC;		




	p2SidedButton = (CGUI2StatesButton*)win->GetElementNum(2);	
	if (p2SidedButton->GetState()==TWO_SIDES_BUTTON_STATE_1)
		m_iPlayerTypeFUTURE=PLAYER_TYPE_HUMAN;		
	else
		m_iPlayerTypeFUTURE=PLAYER_TYPE_PC;		*/

	
			/*if (g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_HUMAN)
			{
				
				((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(1))->SetState1();     
			}
			else // past army is pc
			{
				((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(0))->SetState1();     
				((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(1))->SetState2();     
			}



			if (g_pCheckersBoard->m_iPlayerTypeFUTURE==PLAYER_TYPE_HUMAN)
			{
				((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(2))->SetState2();     
				((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(3))->SetState1();     
			}
			else // past army is pc
			{
				((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(2))->SetState1();     
				((CGUI2StatesButton*)m_pOptionsMenu->GetElementNum(3))->SetState2();     
			}*/


	return true;
}

bool CCheckersBoard::SaveBoardGame(void)
{
	if(!AreAllBoardAnimationsDone())
		return false;


	// backup the client values

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			if (BoardsSquaresUnits[i][j].m_iIndexToPlayersArray>=0)
			{
				BoardsSquaresUnits[i][j].m_iHitPointsBACKUP = m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife;
				BoardsSquaresUnits[i][j].m_iExperiencePointsBACKUP = m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_iExperiencePoints;
				BoardsSquaresUnits[i][j].m_fYAngBACKUP =  m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_fAngY;
			}
		}

    //save file as binary...
		std::ofstream file;

	if (m_iCurrentSaveGame==0)
		file.open("Game00.sav",std::ios::out | std::ios::binary);
	else
	if (m_iCurrentSaveGame==1)
		file.open("Game01.sav",std::ios::out | std::ios::binary);
	else
	if (m_iCurrentSaveGame==2)
		file.open("Game02.sav",std::ios::out | std::ios::binary);
	else
	if (m_iCurrentSaveGame==3)
		file.open("Game03.sav",std::ios::out | std::ios::binary);
	else
	if (m_iCurrentSaveGame==4)
		file.open("Game04.sav",std::ios::out | std::ios::binary);
	else
	if (m_iCurrentSaveGame==5)
		file.open("Game05.sav",std::ios::out | std::ios::binary);
	else
		return false;

	if(file.fail())
		return false;								

	file.write((char*)BoardsSquaresUnits,sizeof(CUnit) * SQUARES_NUM_X * SQUARES_NUM_Y);

	file.write((char*) &m_iCurrentPlayerSide, sizeof(int));

	file.write((char*) &m_iPlayerTypePAST, sizeof(int));

	file.write((char*) &m_iPlayerTypeFUTURE, sizeof(int));

	

	file.write((char*) &m_iCharmedUnitsOwnedByPastNum, sizeof(int));
	file.write((char*) &m_CharmedUnitsOwnedByPAST[0], sizeof(tCharmedUnit));
	file.write((char*) &m_iCharmedUnitsOwnedByFutureNum, sizeof(int));
	file.write((char*) &m_CharmedUnitsOwnedByFUTURE[0], sizeof(tCharmedUnit));


	file.write((char*) &m_iParalisedUnitsOwnedByPastNum, sizeof(int));
	file.write((char*) &m_ParalisedUnitsOwnedByPAST[0], sizeof(tParalisedUnit));
	file.write((char*) &m_iParalisedUnitsOwnedByFutureNum, sizeof(int));
	file.write((char*) &m_ParalisedUnitsOwnedByFUTURE[0], sizeof(tParalisedUnit));



	

	file.close();

	return true;
}

bool CCheckersBoard::LoadBoardGame(void)
{

	if(!AreAllBoardAnimationsDone())
		return false;

	//load file as binary...
	std::ifstream file;



	if (m_iCurrentSaveGame==0)
	{
		if (_access("Game00.sav",00)== -1)
			return false;

		file.open("Game00.sav",std::ios::in | std::ios::binary);
	}
	else
	if (m_iCurrentSaveGame==1)
	{
		if (_access("Game01.sav",00)== -1)
			return false;

		file.open("Game01.sav",std::ios::in | std::ios::binary);
	}
	else
	if (m_iCurrentSaveGame==2)
	{
		if (_access("Game02.sav",00)== -1)
			return false;

		file.open("Game02.sav",std::ios::in | std::ios::binary);
	}
	else
	if (m_iCurrentSaveGame==3)
	{
		if (_access("Game03.sav",00)== -1)
			return false;

		file.open("Game03.sav",std::ios::in | std::ios::binary);
	}
	else
	if (m_iCurrentSaveGame==4)
	{
		if (_access("Game04.sav",00)== -1)
			return false;

		file.open("Game04.sav",std::ios::in | std::ios::binary);
	}
	else
	if (m_iCurrentSaveGame==5)
	{
		if (_access("Game05.sav",05)== -1)
			return false;

		file.open("Game05.sav",std::ios::in | std::ios::binary);
	}
	else
		return false;


	if(file.fail())
		return false;

	

	
					
	Destroy();
	Init(false);

	m_iBoardGameState=GAME_BOARD_STATE_CHOOSE_UNIT;
	ClearBoardSelections();

	for (int i=0;i<m_iNumOfUnits;i++)
	{
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->m_PhysicsModel.Init(20.f);
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->m_pPlayerModel->InitAnimationStates();
		((CEntityPlayer*)m_ppGameUnits[i])->m_pPlayerClient->InitStates();		
	}

	
		
    file.read((char*)BoardsSquaresUnits,sizeof(CUnit) * SQUARES_NUM_X * SQUARES_NUM_Y);

	file.read((char*)&m_iCurrentPlayerSide, sizeof(int));

	file.read((char*) &m_iPlayerTypePAST, sizeof(int));

	file.read((char*) &m_iPlayerTypeFUTURE, sizeof(int));

	file.read((char*) &m_iCharmedUnitsOwnedByPastNum, sizeof(int));
	file.read((char*) &m_CharmedUnitsOwnedByPAST[0], sizeof(tCharmedUnit));
	file.read((char*) &m_iCharmedUnitsOwnedByFutureNum, sizeof(int));
	file.read((char*) &m_CharmedUnitsOwnedByFUTURE[0], sizeof(tCharmedUnit));


	file.read((char*) &m_iParalisedUnitsOwnedByPastNum, sizeof(int));
	file.read((char*) &m_ParalisedUnitsOwnedByPAST[0], sizeof(tParalisedUnit));
	file.read((char*) &m_iParalisedUnitsOwnedByFutureNum, sizeof(int));
	file.read((char*) &m_ParalisedUnitsOwnedByFUTURE[0], sizeof(tParalisedUnit));

	file.close();


	// extract clients values

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			if (BoardsSquaresUnits[i][j].m_iIndexToPlayersArray>=0)
			{
				m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_iExperiencePoints = BoardsSquaresUnits[i][j].m_iExperiencePointsBACKUP;
				m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife = BoardsSquaresUnits[i][j].m_iHitPointsBACKUP;			
				m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_pPlayerModel->m_fAngY = BoardsSquaresUnits[i][j].m_fYAngBACKUP;
			}


		}

	ClearBoardSelections();

	return true;
}


void CCheckersBoard::TriggerNewGameParticles(void)
{
	CVector3f v1;
	CParticleSystem* pPS;
	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		if (IsOdd(i+j))
		{
			if (j<3 || j>4) 
			{
				v1 = GetMiddleOfSquare(i,j) + CVector3f(0.f,3.f,0.f);
				pPS = st_PSManagerI->PushParticleSystem(PARTICLE_SYSTEM_TYPE_MAGIC_BLUE,60,&v1,false,CVector3f(0.f,0.f,0.f),0.f);
				pPS->SetTimeToDeath(1000.f);
			}						
		}
}



void CCheckersBoard::ComputeShakeVector(void)
{
	static CWave wave1,wave2,wave3,wave4;
	CVector3f vShake1,vShake2,vShake3,vShake4;

	wave1.SetParams(WF_SIN,0.8,5.2,0,10.2);
	wave2.SetParams(WF_SIN,0.f,1.25f,0.f,10.25);

	vShake1.Set(0.f,1.f,0.f);
	vShake1*=wave1.Compute(st_Timer::Inst()->TimerGetTimeFAKE());

	vShake2.Set(0.f,0.5f,-0.5f);
	vShake2*=wave2.Compute(st_Timer::Inst()->TimerGetTimeFAKE());
		
	m_vShakeSum = vShake1+vShake2;

	/*wave1.SetParams(WF_SIN,0.3,5.2,0,11.2);
	wave2.SetParams(WF_INVERSESAWTOOTH,0.f,5.25f,0.f,11.25);
	wave3.SetParams(WF_SAWTOOTH,0.f,5.25f,0.f,11.25);

	vShake1.Set(0.f,1.f,0.f);
	vShake1*=wave1.Compute(st_Timer::Inst()->TimerGetTimeFAKE());

	vShake2.Set(0.f,0.5f,-0.5f);
	vShake2*=wave2.Compute(st_Timer::Inst()->TimerGetTimeFAKE());

	vShake3.Set(0.f,-0.5f,0.5f);
	vShake3*=wave3.Compute(st_Timer::Inst()->TimerGetTimeFAKE());
		
	m_vShakeSum = vShake1+vShake2+vShake3;*/

}

void CCheckersBoard::UpdateCharmed(void)
{

	static CUnit* pUnit;

	for (int i=0;i<m_iCharmedUnitsOwnedByPastNum;i++)
	{
		if ( m_iTurnNum - m_CharmedUnitsOwnedByPAST[i].iTurnCharmedNum > 4 )
		{
			pUnit = GetUnitByPlayerIndex(m_CharmedUnitsOwnedByPAST[i].iPlayerIndex);

			if (!pUnit)
				continue;

			// bring control back to original owner
			if (pUnit->m_iArmySide==ARMY_SIDE_PAST)
				pUnit->m_iArmySide=ARMY_SIDE_FUTURE;
			else
				pUnit->m_iArmySide=ARMY_SIDE_PAST;

			if (pUnit->m_iUnitType==UNIT_TYPE_SOLDIER)
				{
					m_ppGameUnits[m_CharmedUnitsOwnedByPAST[i].iPlayerIndex]->m_pPlayerClient->m_pPlayerModel->m_fAngY+=180.f;

					if (m_ppGameUnits[m_CharmedUnitsOwnedByPAST[i].iPlayerIndex]->m_pPlayerClient->m_pPlayerModel->m_fAngY>=360.f)
						m_ppGameUnits[m_CharmedUnitsOwnedByPAST[i].iPlayerIndex]->m_pPlayerClient->m_pPlayerModel->m_fAngY-=360.f;
				}

			// delete it from the list
			for (int j=i;j<m_iCharmedUnitsOwnedByPastNum;j++)
			{	
				m_CharmedUnitsOwnedByPAST[j].iTurnCharmedNum=m_CharmedUnitsOwnedByPAST[j+1].iTurnCharmedNum;
				m_CharmedUnitsOwnedByPAST[j].iPlayerIndex=m_CharmedUnitsOwnedByPAST[j+1].iPlayerIndex;
			}


			m_iCharmedUnitsOwnedByPastNum--;

		}
	}




	for (int i=0;i<m_iCharmedUnitsOwnedByFutureNum;i++)
	{
		if ( m_iTurnNum - m_CharmedUnitsOwnedByFUTURE[i].iTurnCharmedNum > 4 )
		{
			pUnit = GetUnitByPlayerIndex(m_CharmedUnitsOwnedByFUTURE[i].iPlayerIndex);

			if (!pUnit)
				continue;

			// bring control back to original owner
			if (pUnit->m_iArmySide==ARMY_SIDE_PAST)
				pUnit->m_iArmySide=ARMY_SIDE_FUTURE;
			else
				pUnit->m_iArmySide=ARMY_SIDE_PAST;

			if (pUnit->m_iUnitType==UNIT_TYPE_SOLDIER)
				{
					m_ppGameUnits[m_CharmedUnitsOwnedByFUTURE[i].iPlayerIndex]->m_pPlayerClient->m_pPlayerModel->m_fAngY+=180.f;

					if (m_ppGameUnits[m_CharmedUnitsOwnedByFUTURE[i].iPlayerIndex]->m_pPlayerClient->m_pPlayerModel->m_fAngY>=360.f)
						m_ppGameUnits[m_CharmedUnitsOwnedByFUTURE[i].iPlayerIndex]->m_pPlayerClient->m_pPlayerModel->m_fAngY-=360.f;
				}

			// delete it from the list
			for (int j=i;j<m_iCharmedUnitsOwnedByFutureNum;j++)
			{	
				m_CharmedUnitsOwnedByFUTURE[j].iTurnCharmedNum=m_CharmedUnitsOwnedByFUTURE[j+1].iTurnCharmedNum;
				m_CharmedUnitsOwnedByFUTURE[j].iPlayerIndex=m_CharmedUnitsOwnedByFUTURE[j+1].iPlayerIndex;
			}


			m_iCharmedUnitsOwnedByFutureNum--;

		}
	}
	
	//tCharmedUnit m_CharmedUnitsOwnedByPAST[30];

}


void CCheckersBoard::UpdateParalised(void)
{

	static CUnit* pUnit;

	for (int i=0;i<m_iParalisedUnitsOwnedByPastNum;i++)
	{
		if ( m_iTurnNum - m_ParalisedUnitsOwnedByPAST[i].iTurnParalisedNum > 4 )
		{

			// if it doesn't point to the same player as the time it was paralised continue.
			if (BoardsSquaresUnits[m_ParalisedUnitsOwnedByPAST[i].coord.x][m_ParalisedUnitsOwnedByPAST[i].coord.y].m_iIndexToPlayersArray!=m_ParalisedUnitsOwnedByPAST[i].iPlayerIndex)
				continue;

			BoardsSquaresUnits[m_ParalisedUnitsOwnedByPAST[i].coord.x][m_ParalisedUnitsOwnedByPAST[i].coord.y].m_bParalised=false;

	
			// delete it from the list
			for (int j=i;j<m_iParalisedUnitsOwnedByPastNum;j++)
			{	
				m_ParalisedUnitsOwnedByPAST[j].iTurnParalisedNum=m_ParalisedUnitsOwnedByPAST[j+1].iTurnParalisedNum;
				m_ParalisedUnitsOwnedByPAST[j].iPlayerIndex=m_ParalisedUnitsOwnedByPAST[j+1].iPlayerIndex;
			}


			m_iParalisedUnitsOwnedByPastNum--;

		}
	}




	for (int i=0;i<m_iParalisedUnitsOwnedByFutureNum;i++)
	{
		if ( m_iTurnNum - m_ParalisedUnitsOwnedByFUTURE[i].iTurnParalisedNum > 4 )
		{
			// if it doesn't point to the same player as the time it was paralised continue.
			if (BoardsSquaresUnits[m_ParalisedUnitsOwnedByFUTURE[i].coord.x][m_ParalisedUnitsOwnedByFUTURE[i].coord.y].m_iIndexToPlayersArray!=m_ParalisedUnitsOwnedByFUTURE[i].iPlayerIndex)
				continue;

			BoardsSquaresUnits[m_ParalisedUnitsOwnedByFUTURE[i].coord.x][m_ParalisedUnitsOwnedByFUTURE[i].coord.y].m_bParalised=false;

	

			// delete it from the list
			for (int j=i;j<m_iParalisedUnitsOwnedByFutureNum;j++)
			{	
				m_ParalisedUnitsOwnedByFUTURE[j].iTurnParalisedNum=m_ParalisedUnitsOwnedByFUTURE[j+1].iTurnParalisedNum;
				m_ParalisedUnitsOwnedByFUTURE[j].iPlayerIndex=m_ParalisedUnitsOwnedByFUTURE[j+1].iPlayerIndex;
			}


			m_iParalisedUnitsOwnedByFutureNum--;

		}
	}
		

}

CUnit* CCheckersBoard::GetUnitByPlayerIndex(int iPlayerIndex)
{
	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		if (IsOdd(i+j))
			if (BoardsSquaresUnits[i][j].m_iIndexToPlayersArray==iPlayerIndex)
				return &BoardsSquaresUnits[i][j];

	return NULL;
}

void CCheckersBoard::TimeHealUnits(void)
{
	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		if (IsOdd(i+j))
			if (BoardsSquaresUnits[i][j].m_iIndexToPlayersArray>=0)
			{
				m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife+=3;

				if (m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife>100)
					m_ppGameUnits[BoardsSquaresUnits[i][j].m_iIndexToPlayersArray]->m_pPlayerClient->m_iStartLife=100;

			}


}