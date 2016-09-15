#ifndef YOEL_CHECKERS_BOARD_H
#define YOEL_CHECKERS_BOARD_H

#pragma once

#include <windows.h>
#include "Entities.h"

//#include "defines.h"

#include "BoardSpells.h"

#include "CheckersAI.h"

#include "ParticleSystem.h"

#include "SoundManager.h"

#define CB_SOUND_SAMPLES_SELECT_UNIT        1
#define CB_SOUND_SAMPLES_SELECT_DESTINATION 2
#define CB_SOUND_SAMPLES_BURN               3
#define CB_SOUND_SAMPLES_COMBO              4
#define CB_SOUND_SAMPLES_PREPARE_TO_FIGHT   5
#define CB_SOUND_SAMPLES_FIGHT              6
#define CB_SOUND_SAMPLES_MAX                7


void PlayIntroSong(void);
	void PlayBoardGameSong(void);	
	void PlayPreBattleSong(void);
	void PlayBattleSong(void);	
	void PlayWinSong(void);

#define CB_SOUND_STREAM_INTRO            1
#define CB_SOUND_STREAM_BOARD            2
#define CB_SOUND_STREAM_PRE_BATTLE       3
#define CB_SOUND_STREAM_BATTLE           4
#define CB_SOUND_STREAM_WIN              5
#define CB_SOUND_STREAM_MAX              6


#define MAX_UNITS_NUM 64
#define BOARD_SQUARE_SIZE 50.f
#define HALF_BOARD_SQUARE_SIZE 25.f

/*#define SQUARES_NUM_X 8
#define SQUARES_NUM_Y 8*/

#define UNIT_NOT_SELECTED 0
#define UNIT_SELECTED     1
#define UNIT_TARGET       2


#define ADVANTAGE_WEAPON    0
#define ADVANTAGE_EXTRALIFE 1
#define ADVANTAGE_AMMO      2







#define ACCESABLE_SQUARE     0
#define UNACCESABLE_SQUARE   1

#define KILL_DIRECTION_NONE      0
#define KILL_DIRECTION_LEFT      1
#define KILL_DIRECTION_RIGHT     2

#define GAME_BOARD_STATE_MENU                             0
#define GAME_BOARD_STATE_CHOOSE_UNIT                      1
#define GAME_BOARD_STATE_CHOOSE_TARGET                    2
#define GAME_BOARD_STATE_CHOOSE_TARGET_COMBO              3
#define GAME_BOARD_STATE_CHOOSE_TARGET_SPELL              4
#define GAME_BOARD_STATE_SHOWING_WALK_ANIMATION           5
#define GAME_BOARD_STATE_SHOWING_FIGHT_ANIMATION          6
#define GAME_BOARD_STATE_SHOWING_COMBO_ANIMATION          7
#define GAME_BOARD_STATE_SHOWING_SPELL_ANIMATION          8

#define ROTATING_DIRECTION_LEFT    0
#define ROTATING_DIRECTION_RIGHT   1


#define COMBO_ANIMATION_FIRST_TURN          0
#define COMBO_ANIMATION_WALK                1
#define COMBO_ANIMATION_SECOND_TURN         2


#define WALK_ANIMATION_FIRST_TURN           0
#define WALK_ANIMATION_WALK                 1
#define WALK_ANIMATION_SECOND_TURN          2



#define FIGHT_ANIMATION_FIRST_TURN           0
#define FIGHT_ANIMATION_WALK                 1
#define FIGHT_ANIMATION_POST_FIGHT_WALK      2
#define FIGHT_ANIMATION_SECOND_TURN          3

#define SPELL_ANIMATION_FIRST_TURN           0
#define SPELL_ANIMATION_PRE_SPELL            1
#define SPELL_ANIMATION_SPELL                2
#define SPELL_ANIMATION_SECOND_TURN          3




#define COMBO_MOVES_MAXIMUM_NUM  100

#define COMBO_MOVE_TIME 1000.f;
//#define COMBO_MOVE_TIME 10000.f;


#define PLAYER_TYPE_HUMAN    0
#define PLAYER_TYPE_PC       1
#define PLAYER_TYPE_NETWORK  2


class CUnit
{
public:
	//int m_iIndexToPlayersArrayBACKUP;
	float m_fYAngBACKUP;
	int m_iIndexToPlayersArray;
	int m_dwState; // state - UNIT_SELECTED / UNIT_NOT_SELECTED / etc...
	int m_iArmySide; // army side
	int m_iUnitType;
	int m_iExperiencePointsBACKUP;
	int m_iHitPointsBACKUP;
	bool m_bParalised;
	//bool m_bEnergyField;
};
// int = char*2
// float = char*4
// 12

/*struct tCoord
{
	int x,y;
};*/


//class CCheckersAI;
class CCheckersBoard
{
public:
	CCheckersBoard();
	~CCheckersBoard();

	void InitSounds(void);
	void StopBackgroundMusic(void);

	void PreInit(void);
	void Init(bool bTriggerParticleSystems);
	void Destroy(void);

	void UpdateGameUnits(void);
	void DrawGameUnits(void);

	

	bool SaveBoardGame(void);
	bool LoadBoardGame(void);
	bool NewGame(void);
	bool EndGame(void);

	int m_iCurrentSaveGame;

	CUnit* GetUnitByPlayerIndex(int iPlayerIndex);

	void TriggerNewGameParticles(void);

	int BoardsSquaresAccesablity[SQUARES_NUM_X][SQUARES_NUM_Y];

	CUnit BoardsSquaresUnits[SQUARES_NUM_X][SQUARES_NUM_Y];


	bool AreAllBoardAnimationsDone(void);

	
	CUnit BoardsSquaresUnitsTEMP[SQUARES_NUM_X][SQUARES_NUM_Y];

	CVector3f GetMiddleOfSquare(int x,int y);


	// spells
	void ChooseSpellTarget(int x,int y);

	bool IsSpellPossible(int iSpellType);

	// walk/fight/combo

	void ChooseUnit(int x, int y);
	void ChooseTarget(int x, int y);

	void TryAllSquaresAsTarget(void);

	void ClearBoardSelections(void);

	bool CheckIfPlayerWon(int& iSide);

	void CopyBoardToTemp(void);

	void RestoreBoardFromTemp(void);

	bool IsFreeMove(int fromX,int fromY,int toX, int toY);
	
	bool IsKillMove(int fromX,int fromY,int toX, int toY);

	bool IsKillMoveInTempBoard(int fromX,int fromY,int toX, int toY);

	int CanUnitKill(int x, int y);

	int CanUnitKillInTempBoard(int x, int y);

	void GoBackToSelectUnit(void);

	void ActivateBurningUnitsLights(void);

	CEntity** m_ppGameUnits;

	CBoardSpell* GetSpells(void) { return &m_Spells[0];};

	int m_iNumOfUnits;

	float fRotateXAng;
	float fRotateYAng;


	int m_iCurrentPlayerSide;

	int m_iBoardGameState;

	int m_iChosenUnitX;
	int m_iChosenUnitY;

   	int m_iTargetX;
	int m_iTargetY;


	int m_iSubComboFromX;
	int m_iSubComboFromY;
	int m_iSubComboToX;
	int m_iSubComboToY;


	int m_iFightAgainstX;
	int m_iFightAgainstY;

	float m_fAnimationT;	
	int   m_iAnimationState;
	int   m_iWalkAnimationRotatingDirection;

	bool  m_bInComboState;

	int     m_iComboMovesNum;
	tCoord* m_ComboMovesVector;

	int     m_iComboMoveDirection;

	float   m_fComboMoveTimeLeft;

	int     m_iPlaceInComboMoveAnimation;


	void UpdateAIBoard(void);

	CCheckersAI m_CheckersAI;

	// current chosen unit

	
	void TimeHealUnits(void);

	bool m_bCanSelectedUnitKill;
	int  m_iSelectedUnitExperiencePoints;
	int  m_iSelectedUnitHitPoints;

	float m_fGamePlaySpeed;

	bool m_bPlayersChanged;
	int m_iPlayerTypePAST;
	int m_iPlayerTypeFUTURE;

	int m_iTempIndex;

	int m_iCurrentSpellCast;

	int m_iChosenSpellTargetNum;

	int m_iSpellCastUponTarget1X;
	int m_iSpellCastUponTarget1Y;

	int m_iSpellCastUponTarget2X;
	int m_iSpellCastUponTarget2Y;

	float m_fTimeForThisAnimationT;

	int m_iMagicPSNum;
	CParticleSystem* m_pMagicPS1;
	CParticleSystem* m_pMagicPS2;
	CParticleSystem* m_pMagicPS3;
	CParticleSystem* m_pMagicPS4;
	

	bool m_bSpellParticlesStarted;

	bool m_bSpellAlreadyEffected;


	char caPlayerInfoLine[400];
	char caPlayerInfoLineBACKUP[400];

	int m_iTurnNum;

	int m_iAdvantagePAST;
	int m_iAdvantageFUTURE;

	///////////////////////////////////////////////////////////////////////
	// spells
	///////////////////////////////////////////////////////////////////////


	// shockwave

	bool m_bShakeBoard;
	CVector3f m_vShakeSum;

	void ComputeShakeVector(void);

	// possesion/charm

	int    m_iCharmedUnitsOwnedByPastNum;
	tCharmedUnit m_CharmedUnitsOwnedByPAST[30];

	int    m_iCharmedUnitsOwnedByFutureNum;
	tCharmedUnit m_CharmedUnitsOwnedByFUTURE[30];

	void UpdateCharmed(void);

	// paralise

	int    m_iParalisedUnitsOwnedByPastNum;
	tParalisedUnit m_ParalisedUnitsOwnedByPAST[30];

	int    m_iParalisedUnitsOwnedByFutureNum;
	tParalisedUnit m_ParalisedUnitsOwnedByFUTURE[30];

	void UpdateParalised(void);
	
	FSOUND_SAMPLE** m_ppBoardSoundSamples;
	FSOUND_STREAM** m_ppBackgroundStreams;

	// sound
	bool m_bPlayingWinSong;


	bool m_bAlreadyClicked;

	private:

	CBoardSpell m_Spells[SPELL_TYPE_MAX];
	

	


	

	///////////////////////////////////////////////////////////////////////

	//CParticleSystem m_ParticleSystem;
};



#endif // YOEL_CHESSBOARD_H