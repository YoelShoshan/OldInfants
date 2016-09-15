#ifndef YOEL_PLAYER_MODEL_H
#define YOEL_PLAYER_MODEL_H

#pragma once

#include "MD3.h"

#include "Math3d.h"

enum eAnimations
{
	// If one model is set to one of the BOTH_* animations, the other one should be too,
	// otherwise it looks really bad and confusing.

	BOTH_DEATH1 = 0,		// The first twirling death animation
	BOTH_DEAD1,				// The end of the first twirling death animation
	BOTH_DEATH2,			// The second twirling death animation
	BOTH_DEAD2,				// The end of the second twirling death animation
	BOTH_DEATH3,			// The back flip death animation
	BOTH_DEAD3,				// The end of the back flip death animation

	// The next block is the animations that the upper body performs

	TORSO_GESTURE,			// The torso's gesturing animation
	
	TORSO_ATTACK,			// The torso's attack1 animation
	TORSO_ATTACK2,			// The torso's attack2 animation

	TORSO_DROP,				// The torso's weapon drop animation
	TORSO_RAISE,			// The torso's weapon pickup animation

	TORSO_STAND,			// The torso's idle stand animation
	TORSO_STAND2,			// The torso's idle stand2 animation

	// The final block is the animations that the legs perform

	LEGS_WALKCR,			// The legs's crouching walk animation
	LEGS_WALK,				// The legs's walk animation
	LEGS_RUN,				// The legs's run animation
	LEGS_BACK,				// The legs's running backwards animation
	LEGS_SWIM,				// The legs's swimming animation
	
	LEGS_JUMP,				// The legs's jumping animation
	LEGS_LAND,				// The legs's landing animation

	LEGS_JUMPB,				// The legs's jumping back animation
	LEGS_LANDB,				// The legs's landing back animation

	LEGS_IDLE,				// The legs's idle stand animation
	LEGS_IDLECR,			// The legs's idle crouching animation

	LEGS_TURN,				// The legs's turn animation

	MAX_ANIMATIONS			// The define for the maximum amount of animations
};


   
											

#define ATTACK_SHORT_RANGE 0
#define ATTACK_LONG_RANGE 1
// 0=close range, 1=long range


class CPlayerModel
{
public:

	CPlayerModel();
	~CPlayerModel();

	void InitAnimationStates(void);

	bool Load(char* pPath);

	bool LoadAnimationFile(const char* pPath);

	void Link(CMD3 *pModel, CMD3 *pLink, LPSTR strTagName);
	
	void DrawLink(CMD3* pModel,bool bPPLused);

	void SetTorsoAnimation(int iNum);
	void SetLegsAnimation(int iNum);

	void SetTorsoAnimationIfNotSame(int iNum);
	void SetLegsAnimationIfNotSame(int iNum);


	void SetTorsoAnimationIfNotSameAndCurrentEnded(int iNum);
	void SetLegsAnimationIfNotSameAndCurrentEnded(int iNum);

	void SetTorsoAnimationIfNotSameNoPriority(int iNum);
	void SetLegsAnimationIfNotSameNoPriority(int iNum);


	void AdvanceTorsoAnimation(void);
	void AdvanceLegsAnimation(void);

	void SetCurrentWeapon(int iNumWeapon);

	
	void DrawPlayer(bool bPPLused);

	CMD3* GetHead(void) { return m_pHead;};
	CMD3* GetUpper(void) { return m_pUpper;};
	CMD3* GetLower(void) { return m_pLower;};
	CMD3* GetWeapon(void) { return m_pCurrentWeapon;};



	void SetWeaponType(int iSet);
	void UpdateStandAndAttackType(void);

	void ForceTorsoAnimations(void);


	
	CVector3f m_vBonePosition;



	CVector3f m_vWorldPosition;

	float     m_fAngX,m_fAngY;

	CVector3f m_vWeaponCenter;

	bool      m_bStrafingLeft;
	bool      m_bStrafingRight;


	CMD3* m_pHead;
	CMD3* m_pUpper;
	CMD3* m_pLower;
	
	CMD3* m_pRocketLauncher;
	CMD3* m_pPlasmaShooter;
	CMD3* m_pShotgun;
	CMD3* m_pGauntlet;

	CMD3* m_pCurrentWeapon;


	void BruteSetCurrentTorsoAnimationID(bool bSet) { m_iCurrentTorsoAnimation=bSet;};
	void BruteSetCurrentLegsAnimationID(bool bSet) { m_iCurrentLegsAnimation=bSet;};


	int   m_iWeaponType;  // 0=short range, 1=long range


	int   m_iTorsoStandAnimationID;
	int   m_iTorsoAttackAnimationID;

	int m_iCurrentTorsoAnimation;
	int m_iCurrentLegsAnimation;

	void SetDelayedDeath(float fSet) {m_bDelayedDeath=true;m_fDelayedDeath=fSet;}; 
	void SetDisableDelayedDeath(void) {m_bDelayedDeath=false;m_fDelayedDeath=0.f;}; 
	bool GetDelayedDeathB(void) { return m_bDelayedDeath;};
	float GetDelayedDeathF(void) { return m_fDelayedDeath;};

	float GetAnimationTotalTime(int iIndex);


private:
	
	bool  m_bDelayedDeath;
	float m_fDelayedDeath;
	
	int m_iNumOfAnimations;				// The number of animations in this model 
	CAnimationSequence animations[MAX_MD3_MODEL_ANIMATION_SEQUENCES]; // The list of animations


	


	int AnimationsPriorities[25];


	CVector3f m_vTempTranslation;
	CVector3f m_vTempRotation;

	CMatrix   m_vTempMatrix;
	float     m_fMatrix[16];
};




#endif
