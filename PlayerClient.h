#ifndef YOEL_PLAYER_CLIENT
#define YOEL_PLAYER_CLIENT

#include "PhysicsModel.h"
#include "PlayerModel.h"
#include "Entities.h"
#include "SoundManager.h"

#include <vector>



#define PLAYER_CLIENT_NOT_ATTACKING  (1<<0)
#define PLAYER_CLIENT_ATTACKING (1<<1)


#define PLAYER_HOLDING_NO_WEAPON                    0
#define PLAYER_HOLDING_WEAPON_SHOTGUN               1
#define PLAYER_HOLDING_WEAPON_ROCKET_LAUNCHER       2
#define PLAYER_HOLDING_WEAPON_PLASMA                3



#define TIME_BETWEEN_SHOTS_ROCKET_LAUNCHER 750.f
//#define TIME_BETWEEN_SHOTS_PLASMA          250.f
#define TIME_BETWEEN_SHOTS_PLASMA          180.f
#define TIME_BETWEEN_SHOTS_BULLET          550.f



#define CLIENT_SOUND_SAMPLE_DEATH      0 
#define CLIENT_SOUND_SAMPLE_FALL       1 
#define CLIENT_SOUND_SAMPLE_JUMP       2 
#define CLIENT_SOUND_SAMPLE_PAIN_25    3 
#define CLIENT_SOUND_SAMPLE_PAIN_50    4 
#define CLIENT_SOUND_SAMPLE_PAIN_75    5 
#define CLIENT_SOUND_SAMPLE_PAIN_100   6
#define CLIENT_SOUND_TAUNT             7
#define CLIENT_SOUND_MAX               8



class CPlayerClient
{
public:

	CPlayerClient();
	~CPlayerClient();

	void InitStates(void);


	void LoadSounds(char* pSoundsPath);

	void PlaySound(int iSound);

	void PlayPainSound(int iDamage);
	

	CPhysicsModel* GetPhysicsModel(void) { return &m_PhysicsModel;};
	void PutPlayerInStartPos(void);

	void PutPlayerInPos(CVector3f& vPos);

	void AdvanceStartPosition(void);

	void UpdateCharacterAnimation(void);

	
	void Shoot(bool bDontCheckForTime,bool bNetwork);

	void Die(void);


	void ShootProjectile(DWORD dwProjectileType);

	void Update(void);


	void SetPlayerWeapon(int iWeaponNum);
	DWORD GetCurrentlyHeldWeapon(void) { return m_dwCurrentlyHeldWeapon;};


	void UpdatePlayerLeafsList(void);

	void SetDesiredMove(CVector3f& vSet) { m_vDesiredMove = vSet;};


	int m_iAbsoluteEntityIndex;

	float m_fTimeSinceDied;

	bool m_bBurning;


	void SetSpellType(int iSet) { m_iSpellType=iSet;};
	int  GetSpellType(void) { return m_iSpellType;};

	int m_iSpellType;

	int m_iLife;
	int m_iStartLife; // when battle starts use this
	int m_iMana;
	float m_fTimeOnLava;
	bool m_bLavaHurtStarted;
	float m_fTimeOnSlime;
	bool  m_bSlimeHurtStarted;

	bool m_bStartAttack;
	bool m_bStartDie;
	bool m_bStartedDieSoundEffect;
	bool m_bDying;
	bool m_bFinishedDying;

	// collectable

	int m_iAmmoBullets;
	int m_iAmmoRockets;
	int m_iAmmoShells;

	bool m_bYellowArmor;
	float m_fTimeHoldingYellowArmor;


	bool m_bRedArmor;
	float m_fTimeHoldingRedArmor;


	float m_fReduceDamageFactor;


	CPlayerModel* m_pPlayerModel;


	// Debug

	//LEAFS_INDICES_VECTOR playerInLeafs;

	CVector3f m_vDesiredMove;

	CPhysicsModel m_PhysicsModel;


	DWORD m_dwPlayerAttackFlags;

	DWORD m_dwCurrentlyHeldWeapon;

	bool m_bReviveUponDeath;


	// board game related

	int m_iExperiencePoints;

	float m_fShotRateFactor;

	bool m_bInfiniteAmmo;
	

	// sounds

	FSOUND_SAMPLE** m_ppSoundSamples;

	// networking

	CVector3f vOldPosition;
	float     fOldAngX;
	float     fOldAngY;
	float     fOldTime;



	CVector3f vLastRecievedPosition;
	CVector3f vLastRecievedVelocity;
	CVector3f vLastRecievedAcceleration;
	CVector3f vLastRecievedDesiredPosition;
	float     fLastRecievedAngX;
	float     fLastRecievedAngY;
	float     fLastRecievedTime;

	CVector3f vLastSentPosition;
	float     fLastSentAngX;
	float     fLastSentAngY;


	//float fLastTimeTriedToShoot;
	bool    bShot;

private:
	
	int m_iStartPosition;
	
	

	float m_fCountDownTillNextShoot;

	

	

};







#endif //YOEL_PLAYER_CLIENT