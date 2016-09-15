#ifndef YOEL_CHARACTER_H
#define YOEL_CHARACTER_H

#pragma once

#include <Windows.h>

#include "Math3d.h"

#include "SoundManager.h"
//#include "Entities.h"


#define PLAYER_ROT_X_UP         (1<<1)
#define PLAYER_ROT_X_DOWN       (1<<2)
#define PLAYER_ROT_Y_LEFT       (1<<3)
#define PLAYER_ROT_Y_RIGHT      (1<<4)
#define PLAYER_ROT_Z_LEFT       (1<<5)
#define PLAYER_ROT_Z_RIGHT      (1<<6)

#define PLAYER_MOVE_X_LEFT      (1<<7)
#define PLAYER_MOVE_X_RIGHT     (1<<8)
#define PLAYER_MOVE_Y_UP        (1<<9)
#define PLAYER_MOVE_Y_DOWN      (1<<10)
#define PLAYER_MOVE_Z_FORWARD   (1<<11)
#define PLAYER_MOVE_Z_BACKWARDS (1<<12)


#define PLAYER_MODE_FLY  0
#define PLAYER_MODE_WALK 1

// This is our lumps enumeration

#define	kNotSet  (1<<0)  // didn't set it yet
#define	kIdle    (1<<1)  // player is standing idle
#define	kTurning (1<<2)  // player is standing idle
#define	kWalking (1<<3)  // player is standing idle
#define	kRunning (1<<4)	// player walking
#define	kMovingBackwards (1<<5)	// player walking backwards
#define	kStrafeLeft (1<<6)	// player starting left
#define	kStrafeRight (1<<7)	// player starting right
#define	kJumping (1<<8)	// player starting to jump
#define	kJumpingB (1<<9)	// player starting to jump
#define	kFlying	 (1<<10)  // player is flying
#define	kLanding (1<<11)  // player landing
#define	kLandingB (1<<12)  // player landing
#define	kStanding (1<<13)  // player landing
#define kAttacking (1<<14) // player attacking
#define kGesturing (1<<15) // player attacking
#define kCastingSpell (1<<16) // player attacking


class CEntity;
class CPhysicsModel
{
public:

	CPhysicsModel();
	CPhysicsModel(float fSphereSize);

	~CPhysicsModel();

	
	void Init(float fSphereSize);


	/******************************************************/
	// General functions
	
	void      SetPosition(const CVector3f& vSet) { m_vPos=vSet;};
	CVector3f GetPosition(void) { return m_vPos;};

	void      SetVelocity(const CVector3f& vSet) { m_vVelocity=vSet;};
	CVector3f GetVelocity(void) { return m_vVelocity;};

	void      SetAcceleration(const CVector3f& vSet) { m_vAcceleration=vSet;};
	CVector3f GetAcceleration(void) { return m_vAcceleration;};

	

	float getRadius(void){return m_fRadius;};
	void setRadius(float fSet) { m_fRadius = fSet;};

	CVector3f GetLookDirection(void);

	/******************************************************/


	



	/******************************************************/
	// Player related

	float updateDistanceToFloor( CEntity* pCausedByEntity); 

	float computeDistanceToFloor(CVector3f& vPos, CEntity* pCausedByEntity);

	void tryPlayerMove(CVector3f& vMoveVelocity, CEntity* pCausedByEntity,bool bMove);

	float wouldMoveLeadToPit(CVector3f& vPos, CVector3f& vTarget, CVector3f& vFinal);

	bool lineOfSight(CVector3f& vTarget);

	bool getPlayerInAir(void){return m_bPlayerInAir;};

	float getDistanceToFloor(void){return m_fDistanceToFloor;};

	void ChangePositionNow(bool bChange) { m_bChangePositionNow = bChange;};
	bool GetChangePositionNow(void) { return m_bChangePositionNow;};

	void SetJumping(bool bSet) { m_bJumping=bSet;};
	bool GetJumping(void) { return m_bJumping;};

	void SetStartJumping(bool bSet) { m_bStartJumping=bSet;};
	bool GetStartJumping(void) { return m_bStartJumping;};

	void UpdateInterfaceWeaponPos(void); // not really used since i moved to 3rd person camera

	CVector3f GetDesiredPosition(int iPlayerMode);

	/******************************************************/
	

	CVector3f tryProjectileMove(CVector3f& vMoveVelocity,  CEntity* pCausedByEntity); // returns the normal of collision
	
	CVector3f GetLastDesiredPosition(void) { return m_vLastDesiredPosition;};

	/******************************************************/
	// Projectile related
	
	


	/******************************************************/


	



/******************************************************/
// Player related
bool      m_bClimbingStair;
float     m_fClimbingPhase; // [0.f,1.f]
float     m_fClimbingStepNum;

CVector3f m_vStartClimbPos;
CVector3f m_vClimbMove;

bool      m_bPlayerFlying;
float     m_fTimePlayerInAir;

int       m_iContentsOfTouchedGround;
int       m_iFlagsOfTouchedGround;

DWORD m_dwPlayerState;

//DWORD m_dwSounds; // sounds

void SetSoundSamples(FSOUND_SAMPLE** pSet);

FSOUND_SAMPLE** m_ppSoundSamples;

/******************************************************/



/******************************************************/
// General Variables
float     RotateYSpeed,RotateXSpeed,RotateZSpeed;
CVector3f TempPoint;
CVector3f m_vLookDirection;
float     m_fAngX,m_fAngY;
DWORD     m_dwMoveRotFlags;
CVector3f m_vOutsideForce;
CVector3f m_vLastDesiredPosition;
/******************************************************/


private:

/******************************************************/
// General Variables
bool      m_bChangePositionNow;
float     m_fRadius;
CVector3f m_vVelocity;
CVector3f m_vAcceleration;
CVector3f m_vPos; // the position of the character
/******************************************************/



/******************************************************/
// Player related
bool      m_bPlayerInAir;
float     m_fDistanceToFloor;
bool      m_bJumping;
bool	  m_bStartJumping;
bool      m_bLanding;

/******************************************************/











};










#endif // YOEL_CHARACTER_H
