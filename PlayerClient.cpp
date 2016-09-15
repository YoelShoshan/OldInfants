#include "PlayerClient.h"

#include "Q3AMap.h"
extern CQuake3BSP* g_pQuake3Map;

#include "Timer.h"

extern int g_iPlayerScore;

#include "PlayerModel.h"

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

extern CPlayerClient *g_pHumanPlayerClient;

extern CPlayerClient *g_pCurrentlyProcessedPlayerClient,*g_pPlayerClient1,*g_pPlayerClient2; // g_pCurrentlyProcessedPlayerClient only hold the CURRENTLY tested clientplayer

extern CVector3f g_vCameraFrom;
extern CVector3f g_vCameraTo;

#include "Engine.h"

#include "Camera.h"
extern CCamera g_Camera1;  // the global camera

#include "CheckersBoard.h"
extern CCheckersBoard* g_pCheckersBoard;

#include <assert.h>
#include <crtdbg.h>

#include "SoundFlags.h"

#include "SoundManager.h"
extern CSoundManager g_SoundManager;

#include "WeaponsSounds.h"
extern CWeaponsSound g_WeaponsSounds;

#include "Communication.h"
extern CCommunication g_Communication;

void CPlayerClient::LoadSounds(char* pSoundsPath)
{
	std::string path;

	

	path = pSoundsPath;
	path+="death1.wav";
	g_SoundManager.LoadSample(path.c_str(),m_ppSoundSamples[CLIENT_SOUND_SAMPLE_DEATH]);

	path = pSoundsPath;
	path+="fall1.wav";
	g_SoundManager.LoadSample(path.c_str(),m_ppSoundSamples[CLIENT_SOUND_SAMPLE_FALL]);

	path = pSoundsPath;
	path+="jump1.wav";
	g_SoundManager.LoadSample(path.c_str(),m_ppSoundSamples[CLIENT_SOUND_SAMPLE_JUMP]);

	path = pSoundsPath;
	path+="pain25_1.wav";
	g_SoundManager.LoadSample(path.c_str(),m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_25]);

	path = pSoundsPath;
	path+="pain50_1.wav";
	g_SoundManager.LoadSample(path.c_str(),m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_50]);

	path = pSoundsPath;
	path+="pain75_1.wav";
	g_SoundManager.LoadSample(path.c_str(),m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_75]);

	path = pSoundsPath;
	path+="pain100_1.wav";
	g_SoundManager.LoadSample(path.c_str(),m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_100]);

	path = pSoundsPath;
	path+="taunt.wav";
	g_SoundManager.LoadSample(path.c_str(),m_ppSoundSamples[CLIENT_SOUND_TAUNT]);

	/*g_SoundManager.LoadSample("sound/player/biker/death1.wav",m_ppSoundSamples[CLIENT_SOUND_SAMPLE_DEATH]);
	g_SoundManager.LoadSample("sound/player/biker/fall1.wav",m_ppSoundSamples[CLIENT_SOUND_SAMPLE_FALL]);
	g_SoundManager.LoadSample("sound/player/biker/jump1.wav",m_ppSoundSamples[CLIENT_SOUND_SAMPLE_JUMP]);
	g_SoundManager.LoadSample("sound/player/biker/pain25_1.wav",m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_25]);
	g_SoundManager.LoadSample("sound/player/biker/pain50_1.wav",m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_50]);
	g_SoundManager.LoadSample("sound/player/biker/pain75_1.wav",m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_75]);
	g_SoundManager.LoadSample("sound/player/biker/pain100_1.wav",m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_100]);
	g_SoundManager.LoadSample("sound/player/biker/taunt.wav",m_ppSoundSamples[CLIENT_SOUND_TAUNT]);*/

}

void CPlayerClient::PlaySound(int iSound)
{
	//g_SoundManager.PlaySample(m_ppSoundSamples[iSound],100);

	//g_SoundManager.PlaySample3D(m_ppSoundSamples[iSound],m_PhysicsModel.GetPosition(),200);
	g_SoundManager.PlaySample3D(m_ppSoundSamples[iSound],m_pPlayerModel->m_vWorldPosition,200);
}

void CPlayerClient::PlayPainSound(int iDamage)
{
	if (iDamage<26)
		//g_SoundManager.PlaySample(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_25],70);
		g_SoundManager.PlaySample3D(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_25],m_PhysicsModel.GetPosition(),200);
	else 
	if (iDamage<51) 
		//g_SoundManager.PlaySample(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_50],70);
		g_SoundManager.PlaySample3D(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_50],m_PhysicsModel.GetPosition(),200);
	else 
	if (iDamage<76) 
		//g_SoundManager.PlaySample(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_75],70);
		g_SoundManager.PlaySample3D(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_75],m_PhysicsModel.GetPosition(),200);
	else 
	//if (iDamage<101) 
		//g_SoundManager.PlaySample(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_100],70);
		g_SoundManager.PlaySample3D(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_PAIN_100],m_PhysicsModel.GetPosition(),200);
}

/*
void CPlayerClient::ActiveSoundsIfNeeded(DWORD dwSounds)
{
	if (dwSounds&SOUND_FLAG_JUMP)
		g_SoundManager.PlaySample(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_JUMP],100);


}*/


CPlayerClient::CPlayerClient()
{

	m_ppSoundSamples = new FSOUND_SAMPLE*[CLIENT_SOUND_MAX];
	for (int i=0;i<CLIENT_SOUND_MAX;i++)
		m_ppSoundSamples[i] = NULL;


	m_bStartedDieSoundEffect=false;

	m_bInfiniteAmmo=false;
	m_fShotRateFactor=1.f;

	m_iSpellType=-1;

	m_fTimeSinceDied =0.f;

	m_bBurning=false;

	//m_iExperiencePoints=0;
	m_iExperiencePoints=3;

	m_bReviveUponDeath = true;
	
	//m_PhysicsModel.Init(30.f);
	m_PhysicsModel.Init(20.f);
	
	m_pPlayerModel = new CPlayerModel;

	InitStates();

	m_dwPlayerAttackFlags = 0;

	m_fCountDownTillNextShoot = 0.f;

	m_iStartLife=100;



	vLastSentPosition.Set(0.f,0.f,0.f);
	fLastSentAngX = 0;
	fLastSentAngY = 0;

	vLastRecievedPosition.Set(0.f,0.f,0.f);
	fLastRecievedAngX = 0;
	fLastRecievedAngY = 0;
	
}

CPlayerClient::~CPlayerClient()
{
	SafeDelete(m_pPlayerModel);
}

void CPlayerClient::InitStates()
{
	m_bStartedDieSoundEffect=false;
	m_bInfiniteAmmo=false;
	m_fShotRateFactor=1.f;
	m_fTimeSinceDied =0.f;

	//m_PhysicsModel.Init(20.f);
	m_PhysicsModel.Init(25.f);

	//m_dwCurrentlyHeldWeapon = PLAYER_HOLDING_WEAPON_ROCKET_LAUNCHER;
	

	m_fReduceDamageFactor = 1.f;

	m_bYellowArmor = false;
	m_fTimeHoldingYellowArmor=0.f;

	m_bRedArmor = false;
	m_fTimeHoldingRedArmor=0.f;

	m_vDesiredMove.Set(0.f,0.f,0.f);


	m_dwPlayerAttackFlags = 0;

	m_iStartPosition=1;	
	m_iLife = m_iStartLife;
	m_iMana = 100;

	//m_iAmmoBullets = 0;
	m_iAmmoBullets = 400;

	m_iAmmoRockets = 40;

	m_iAmmoShells = 100;

	m_fTimeOnLava = 0.f;
	m_bLavaHurtStarted = false;
	m_fTimeOnSlime = 0.f;
	m_bSlimeHurtStarted = false;

	m_bStartAttack = false;
	m_bStartDie    = false;
	m_bDying       = false;
	m_bFinishedDying = false;
}

void CPlayerClient::Die(void)
{
	if (m_pPlayerModel->GetDelayedDeathB() && m_pPlayerModel->GetDelayedDeathF()>=0.f)
	{
		float fTemp = m_pPlayerModel->GetDelayedDeathF();
		fTemp-=st_TimerI->GetDeltaTimeFAKE();
		m_pPlayerModel->SetDelayedDeath(fTemp);
	}
	else
	{
		if (!m_bStartDie)
		m_bStartDie = true;
	}

}

void CPlayerClient::Update(void)
{
	// update weapon next shoot countdown

	m_fCountDownTillNextShoot-=st_Timer::Inst()->GetDeltaTimeFAKE();

	if (m_fCountDownTillNextShoot<0.f)
		m_fCountDownTillNextShoot=0.f;


	// cause damage if standing on slime
		
	if (m_PhysicsModel.m_iContentsOfTouchedGround & CONTENTS_SLIME)
	{
		m_fTimeOnSlime+=st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;

		if (!m_bSlimeHurtStarted)
		{
			m_iLife-=10;
			m_bSlimeHurtStarted=true;
		}
		
		if (m_fTimeOnSlime>0.5f)
		{
			m_iLife-=10;
			m_fTimeOnSlime-=0.5f;
		}
		
	}
	else
	{
		m_fTimeOnSlime = 0.f;
		m_bSlimeHurtStarted = false;
	}


	// cause damage if standing on lava

	if (m_PhysicsModel.m_iContentsOfTouchedGround & CONTENTS_LAVA)
	{
		m_fTimeOnLava+=st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;

		if (!m_bLavaHurtStarted)
		{
			m_iLife-=10;
			m_bLavaHurtStarted=true;
		}
		
		if (m_fTimeOnLava>0.5f)
		{
			m_iLife-=10;
			m_fTimeOnLava-=0.5f;
		}
		
	}
	else
	{
		m_fTimeOnLava = 0.f;
		m_bLavaHurtStarted = false;
	}


	// die if life below zero

	if (m_iLife<=0)
	{	
		// original
		// g_pQuake3Map->m_bFightOver=true;

		if (!m_bStartedDieSoundEffect)
		{
			//g_SoundManager.PlaySample(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_DEATH],100);
			g_SoundManager.PlaySample3D(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_DEATH],m_PhysicsModel.GetPosition(),255);
			m_bStartedDieSoundEffect=true;
		}

		if (!m_bStartDie)
		{			
			m_bStartDie=true;
		}

		if (m_bReviveUponDeath)
		if (!m_bDying)
		{

			//if (g_iPlayerScore==0)
			{

				if (this == g_pPlayerClient1)
				{				
					if (g_pHumanPlayerClient==g_pPlayerClient1)
						g_iPlayerScore--;
					else
						g_iPlayerScore++;
				}
				else
				//if (this == g_pPlayerClient2)
				{
					if (g_pHumanPlayerClient==g_pPlayerClient2)
						g_iPlayerScore--;
					else
						g_iPlayerScore++;
				}

				char temp[200];
				sprintf(temp,"Win Score: %d",g_iPlayerScore);

				g_TextMessages.AddMessage(temp,0.f,0.f,1.f,TEXT_MESSAGE_TYPE_GAME);

				//if (g_iPlayerScore>0)
				if (g_iPlayerScore>1)
					{
						g_pQuake3Map->m_bFightOver=true;
						CVector3f v;
						v = g_Camera1.m_Pos;// + g_Camera1.ReturnLookDirectionNormalized();
						//g_SoundManager.PlaySample(g_pQuake3Map->m_pSampleYOUWIN,160);
						g_SoundManager.PlaySample3D(g_pQuake3Map->m_pSampleYOUWIN,v,255);
					}
				else
					if (g_iPlayerScore<-1)
					{
						g_pQuake3Map->m_bFightOver=true;
						CVector3f v;
						v = g_Camera1.m_Pos;// + g_Camera1.ReturnLookDirectionNormalized();
						//g_SoundManager.PlaySample(g_pQuake3Map->m_pSampleYOULOST,160);
						g_SoundManager.PlaySample3D(g_pQuake3Map->m_pSampleYOULOST,v,255);
					}
			}
		}

		
	}


	// Update yellow armor

	if (m_bYellowArmor)
	{
		m_fTimeHoldingYellowArmor+=st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;

		if (m_fTimeHoldingYellowArmor>20.f)
		{
			m_bYellowArmor=false;
			m_fTimeHoldingYellowArmor=0.f;

		}
	}



	// Update red armor

	if (m_bRedArmor)
	{
		m_fTimeHoldingRedArmor+=st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;

		if (m_fTimeHoldingRedArmor>20.f)
		{
			m_bRedArmor=false;
			m_fTimeHoldingRedArmor=0.f;

		}
	}


	m_fReduceDamageFactor = 1.f;

	// if that player is wearing a yellow shield then cause half the damage
	if (m_bYellowArmor)
		m_fReduceDamageFactor*=0.5f;

	if (m_bRedArmor)
		m_fReduceDamageFactor*=0.75f;

	UpdatePlayerLeafsList();
}


void CPlayerClient::AdvanceStartPosition(void)
{
	m_iStartPosition++;
	if (m_iStartPosition>=g_pQuake3Map->GetStartPositionsNum())
		m_iStartPosition = 0;
}

void CPlayerClient::PutPlayerInStartPos(void)
{

	if (g_pQuake3Map->GetStartPositionsNum()<=0)
		return;


	if (m_iStartPosition>=0 && m_iStartPosition < g_pQuake3Map->GetStartPositionsNum())
		m_PhysicsModel.SetPosition(g_pQuake3Map->GetStartPositions()[m_iStartPosition] + CVector3f(0.f,20.f,0.f));
	else
	{
		m_iStartPosition=0;
		m_PhysicsModel.SetPosition(g_pQuake3Map->GetStartPositions()[0] + CVector3f(0.f,20.f,0.f));
	}


}

void CPlayerClient::PutPlayerInPos(CVector3f& vPos)
{
	m_PhysicsModel.SetPosition(vPos);
}

void CPlayerClient::UpdatePlayerLeafsList(void)
{

	// init leaf list

	/*playerInLeafs.clear();
	g_pQuake3Map->FindLeafs(playerInLeafs,m_PhysicsModel.GetPosition(),m_PhysicsModel.getRadius(),0);*/

}

void CPlayerClient::UpdateCharacterAnimation(void)
{

	if (m_pPlayerModel->GetDelayedDeathB())
		Die();

	if (m_bDying)
		m_fTimeSinceDied+=st_TimerI->GetDeltaTimeFAKE();


	m_dwPlayerAttackFlags = 0;


	// update what the legs are doing

	if (!m_bDying) // only if we are alive :)
	{

		m_pPlayerModel->m_bStrafingLeft = m_pPlayerModel->m_bStrafingRight = false;


		///
		///


/*		if (m_PhysicsModel.m_dwPlayerState & kStanding)
		{
			//m_pPlayerModel->SetLegsAnimationIfNotSame(LEGS_IDLE);
			// i need here to freeze the legs state of after the last frame
		}
		else*/
		if (m_PhysicsModel.m_dwPlayerState & kJumping)
		{
			m_pPlayerModel->SetLegsAnimationIfNotSame(LEGS_JUMP);
		}
		else
		if (m_PhysicsModel.m_dwPlayerState & kJumpingB)
		{
			m_pPlayerModel->SetLegsAnimationIfNotSame(LEGS_JUMPB);
		}
		else
		if (m_PhysicsModel.m_dwPlayerState & kLanding)
		{
			m_pPlayerModel->SetLegsAnimationIfNotSame(LEGS_LAND);
		}
		else
		if (m_PhysicsModel.m_dwPlayerState & kLandingB)
		{
			m_pPlayerModel->SetLegsAnimationIfNotSame(LEGS_LANDB);
		}
		else
		if (m_PhysicsModel.m_dwPlayerState & (kIdle | kGesturing))
			m_pPlayerModel->SetLegsAnimationIfNotSameAndCurrentEnded(LEGS_IDLE);
		else
		if (m_PhysicsModel.m_dwPlayerState & kStrafeLeft)
		{
			m_pPlayerModel->m_bStrafingLeft = true;
			m_pPlayerModel->SetLegsAnimationIfNotSame(LEGS_RUN);
		}
		else
		if (m_PhysicsModel.m_dwPlayerState & kStrafeRight)
		{
			m_pPlayerModel->m_bStrafingRight = true;
			m_pPlayerModel->SetLegsAnimationIfNotSame(LEGS_RUN);
		}
		else
		if (m_PhysicsModel.m_dwPlayerState & kRunning)
			m_pPlayerModel->SetLegsAnimationIfNotSame(LEGS_RUN);
		else
		if (m_PhysicsModel.m_dwPlayerState & kWalking)
			m_pPlayerModel->SetLegsAnimationIfNotSame(LEGS_WALK);
		else
		if (m_PhysicsModel.m_dwPlayerState & kTurning)
			m_pPlayerModel->SetLegsAnimationIfNotSame(LEGS_TURN);
		else
		if (m_PhysicsModel.m_dwPlayerState & kMovingBackwards)
			m_pPlayerModel->SetLegsAnimationIfNotSame(LEGS_BACK);
		else	
		if (m_PhysicsModel.m_dwPlayerState & kFlying)
			m_pPlayerModel->SetLegsAnimationIfNotSame(LEGS_SWIM);
	}


	// update what the torso is doing


	// if upper body animation ended, go back to idle state


	if (m_bDying)
	{
		if (m_pPlayerModel->m_pUpper->GetAnimationEndedNow())
		{
			m_pPlayerModel->SetTorsoAnimation(BOTH_DEAD1);
			m_pPlayerModel->SetLegsAnimation(BOTH_DEAD1);

			/*g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->SetTorsoAnimation(BOTH_DEAD2);
			g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->SetLegsAnimation(BOTH_DEAD2);*/

			/*g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->SetTorsoAnimation(BOTH_DEAD3);
			g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->SetLegsAnimation(BOTH_DEAD3);*/

			m_bFinishedDying = true;

			////
			if (m_bReviveUponDeath) // only in FPS
			{				
				
				//if ( (g_iPlayerScore>0) || (g_iPlayerScore<-0))
				if ( (g_iPlayerScore>1) || (g_iPlayerScore<-1))
				{
/*					if (g_pQuake3Map->m_fDelayGoingBackToBoard<1500.f) // delay the return to board game so the announcer will finish talking...
					{
						if (g_pQuake3Map->m_fDelayGoingBackToBoard==0.f)
						{
						}

						g_pQuake3Map->m_fDelayGoingBackToBoard+=TIMER_DELTA_TIME_FAKE;
					}
					else*/
					{
						//g_pQuake3Map->m_fDelayGoingBackToBoard=0.f;

						

						//assert(_CrtCheckMemory());


						//((CEntityPlayer*)g_pQuake3Map->m_ppAllDynamicEntities[0])->m_pPlayerClient
						DWORD dwSavedWeapon = ((CEntityPlayer*)g_pQuake3Map->m_ppAllDynamicEntities[0])->m_pPlayerClient->GetCurrentlyHeldWeapon();						

						((CEntityPlayer*)g_pQuake3Map->m_ppAllDynamicEntities[0])->m_pPlayerClient->InitStates();
						((CEntityPlayer*)g_pQuake3Map->m_ppAllDynamicEntities[0])->m_pPlayerClient->m_pPlayerModel->InitAnimationStates();
						((CEntityPlayer*)g_pQuake3Map->m_ppAllDynamicEntities[0])->m_pPlayerClient->SetPlayerWeapon(dwSavedWeapon);



						dwSavedWeapon = ((CEntityPlayer*)g_pQuake3Map->m_ppAllDynamicEntities[1])->m_pPlayerClient->GetCurrentlyHeldWeapon();						

						((CEntityPlayer*)g_pQuake3Map->m_ppAllDynamicEntities[1])->m_pPlayerClient->InitStates();
						((CEntityPlayer*)g_pQuake3Map->m_ppAllDynamicEntities[1])->m_pPlayerClient->m_pPlayerModel->InitAnimationStates();
						((CEntityPlayer*)g_pQuake3Map->m_ppAllDynamicEntities[1])->m_pPlayerClient->SetPlayerWeapon(dwSavedWeapon);



						SafeDelete(g_pQuake3Map);

						//assert(_CrtCheckMemory());

						st_Engine::Inst()->m_iGameState=0; // make it a board game
						st_Engine::Inst()->m_bSwitchingGameState=true;

						// start at default board camera position
						if (g_pCheckersBoard->m_iPlayerTypeFUTURE==PLAYER_TYPE_HUMAN && g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_PC)
						{
							g_Camera1.m_Pos.Set(0.f,300.f,306.f);
							// start at default board camera angles
							g_Camera1.xang = -45.f;
							g_Camera1.yang = 0.f;
							g_Camera1.zang = 0.f;					
						}
						else
						// start at default board camera position
						if (g_pCheckersBoard->m_iPlayerTypeFUTURE==PLAYER_TYPE_PC && g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_HUMAN)
						{
							g_Camera1.m_Pos.Set(0.f,300.f,-306.f);
							// start at default board camera angles
							g_Camera1.xang = -45.f;
							g_Camera1.yang = 0.f;
							g_Camera1.zang = 0.f;					
						}
						else
						{		 
							g_Camera1.m_Pos.Set(0.f,300.f,-306.f);
							// start at default board camera angles
							g_Camera1.xang = -45.f;
							g_Camera1.yang = 0.f;
							g_Camera1.zang = 0.f;
						}

						g_pGUIManager->ResetBoardGameMenus();

						return;
					}

				}
				else
				{
					DWORD dwSavedWeapon = m_dwCurrentlyHeldWeapon;

					AdvanceStartPosition();
					PutPlayerInStartPos();
					InitStates();
					m_pPlayerModel->InitAnimationStates();
					SetPlayerWeapon(dwSavedWeapon);
				}
			}
		}

	}
	else
	if (m_bStartDie)
	{
		m_pPlayerModel->SetTorsoAnimationIfNotSame(BOTH_DEATH2);
		m_pPlayerModel->SetLegsAnimationIfNotSame(BOTH_DEATH2);

		/*g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->SetTorsoAnimationIfNotSame(BOTH_DEATH2);
		g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->SetLegsAnimationIfNotSame(BOTH_DEATH2);*/

		/*g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->SetTorsoAnimationIfNotSame(BOTH_DEATH3);
		g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->SetLegsAnimationIfNotSame(BOTH_DEATH3);*/

		m_bDying = true;
	}
	else
	{

		if (m_PhysicsModel.m_dwPlayerState & kGesturing)
		{
			m_pPlayerModel->SetTorsoAnimationIfNotSame(TORSO_GESTURE);
			//m_pPlayerModel->SetTorsoAnimationIfNotSame(TORSO_ATTACK2);

		}
		else
		if (m_PhysicsModel.m_dwPlayerState & kCastingSpell)
		{
			m_pPlayerModel->SetTorsoAnimationIfNotSame(TORSO_ATTACK2);

		}			
		else
		{	
			if (m_bStartAttack)
			{
				m_pPlayerModel->SetTorsoAnimationIfNotSame(m_pPlayerModel->m_iTorsoAttackAnimationID);
				m_bStartAttack;
			}

			if (m_pPlayerModel->m_pUpper->GetAnimationEndedNow() && !m_bStartAttack)
			{
	
				m_pPlayerModel->SetTorsoAnimation(m_pPlayerModel->m_iTorsoStandAnimationID);
			}
		}
	}



	if (m_pPlayerModel->m_iCurrentTorsoAnimation == m_pPlayerModel->m_iTorsoAttackAnimationID)
		m_dwPlayerAttackFlags |= PLAYER_CLIENT_ATTACKING;


	/// init all the boolean variables to fals

	m_bStartAttack = false;
	m_bStartDie    = false;

}

void CPlayerClient::SetPlayerWeapon(int iWeaponNum)
{
	m_pPlayerModel->SetCurrentWeapon(iWeaponNum);

	if (iWeaponNum==0)
	{
		m_pPlayerModel->SetWeaponType(ATTACK_SHORT_RANGE);
		m_dwCurrentlyHeldWeapon = PLAYER_HOLDING_NO_WEAPON;
	}
	else
	if (iWeaponNum==1)
	{
		m_pPlayerModel->SetWeaponType(ATTACK_LONG_RANGE);
		m_dwCurrentlyHeldWeapon = PLAYER_HOLDING_WEAPON_SHOTGUN;
	}
	else
	if (iWeaponNum==2)
	{
		m_pPlayerModel->SetWeaponType(ATTACK_LONG_RANGE);
		m_dwCurrentlyHeldWeapon = PLAYER_HOLDING_WEAPON_ROCKET_LAUNCHER;
	}
	else
	if (iWeaponNum==3)
	{
		m_pPlayerModel->SetWeaponType(ATTACK_LONG_RANGE);
		m_dwCurrentlyHeldWeapon = PLAYER_HOLDING_WEAPON_PLASMA;
	}

}

void CPlayerClient::Shoot(bool bDontCheckForTime,bool bNetwork)
{

	static CTraceRequest g_BulletTraceInput;
	static CTraceResult g_BulletTraceOutput;
			
	bShot=false;
	
			if (m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_NO_WEAPON)
			{
				if ( !(m_dwPlayerAttackFlags &PLAYER_CLIENT_ATTACKING))
				{
					m_bStartAttack = true;
					bShot=true;
				}
			}
			else		
			if (m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_WEAPON_SHOTGUN)
			{
				if (m_iAmmoBullets>0)
				if (m_fCountDownTillNextShoot==0.f || bDontCheckForTime)
				if ( !(m_dwPlayerAttackFlags &PLAYER_CLIENT_ATTACKING))
				{
					g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_SHOTGUN_SHOOT,m_PhysicsModel.GetPosition());

					m_fCountDownTillNextShoot = TIME_BETWEEN_SHOTS_BULLET/m_fShotRateFactor;

					m_bStartAttack = true;
					// RESTORE RESTORE RESTORE
					if (!m_bInfiniteAmmo)
						m_iAmmoBullets-=7;
					if (m_iAmmoBullets<0)
						m_iAmmoBullets=0;


					//g_WeaponsSounds.PlaySound(WEAPON_SOUND_SHOTGUN_SHOOT);

					/**/

					g_BulletTraceInput.dwTraceType = TRACE_TYPE_LINE;
					g_BulletTraceInput.dwTraceCausedBy = TRACE_CAUSED_BY_SHOOT;
					g_BulletTraceInput.fRadius = 0.f;
		
					g_BulletTraceInput.vRayStart.Set(g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->m_vWeaponCenter);
					g_BulletTraceInput.iAbsoluteEntityIndexCause = m_iAbsoluteEntityIndex;
		

					float fRandMissX;
					float fRandMissY;
					float fRandMissZ;

					for (int i=0;i<7;i++)
					{

						float fRandMissX = RANDOM_NUM;
						float fRandMissY = RANDOM_NUM;
						float fRandMissZ = RANDOM_NUM;

						fRandMissX-=0.5f;
						fRandMissY-=0.5f;
						fRandMissZ-=0.5f;

						fRandMissX*=0.15f;
						fRandMissY*=0.15f;
						fRandMissZ*=0.15f;
			

						g_BulletTraceInput.vRayEnd.Set(g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->m_vWeaponCenter);
						g_BulletTraceInput.vRayEnd+= (g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetLookDirection()+(CVector3f(fRandMissX,fRandMissY,fRandMissZ))) * 3000.f;
		
						g_pQuake3Map->Trace(&g_BulletTraceInput,&g_BulletTraceOutput,false,true,NULL);		
					}
					

					bShot=true;
					/**/
				}
			}
			else
			if (m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_WEAPON_ROCKET_LAUNCHER)
			{
				if (m_iAmmoRockets>0)
				if (m_fCountDownTillNextShoot==0.f  || bDontCheckForTime)
				if ( !(m_dwPlayerAttackFlags &PLAYER_CLIENT_ATTACKING))
				{
					if (!m_bStartAttack)
					{						
						m_bStartAttack = true;
						g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_ROCKET_SHOOT,m_PhysicsModel.GetPosition());
						ShootProjectile(PROJECTILE_TYPE_ROCKET);
						m_fCountDownTillNextShoot = TIME_BETWEEN_SHOTS_ROCKET_LAUNCHER/m_fShotRateFactor;
						if (!m_bInfiniteAmmo)
							m_iAmmoRockets--;
						bShot=true;
					}
				}
			}
			else
			if (m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_WEAPON_PLASMA)
			{
				if (m_iAmmoShells>0)
				if (m_fCountDownTillNextShoot==0.f  || bDontCheckForTime)
				//if ( !(m_dwPlayerAttackFlags &PLAYER_CLIENT_ATTACKING))
				{
					//if (!m_bStartAttack)
					{
						m_bStartAttack = true;
						g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_PLASMA_SHOOT,m_PhysicsModel.GetPosition());
						ShootProjectile(PROJECTILE_TYPE_PLASMA);
						m_fCountDownTillNextShoot = TIME_BETWEEN_SHOTS_PLASMA/m_fShotRateFactor;
						if (!m_bInfiniteAmmo)
							m_iAmmoShells--;
						bShot=true;

					}
				}
			}


	if (bShot && bNetwork)
	{
		DWORD dwPlayerActions = 0;

		dwPlayerActions|=FPS_BATTLE_MESSAGE_SHOOT;

		g_Communication.SendFPSBattleMessage(GetPhysicsModel()->GetPosition(),
			GetPhysicsModel()->GetVelocity(),
			GetPhysicsModel()->GetAcceleration(),
			GetPhysicsModel()->GetLastDesiredPosition(),

		GetPhysicsModel()->m_fAngX,
		GetPhysicsModel()->m_fAngY,
		GetPhysicsModel()->m_dwPlayerState,
		dwPlayerActions);

	}

}

void CPlayerClient::ShootProjectile(DWORD dwProjectileType)
{
	CEntity* pEntity;

	g_pQuake3Map->GetFreeProjectileEntity(pEntity,dwProjectileType); // recycle/allocate a projectile
	

	CVector3f vLookDirection = m_PhysicsModel.GetLookDirection();

	pEntity->m_bActive=true;
	pEntity->m_pPhysicsModel->SetPosition(g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->m_vWeaponCenter + vLookDirection*50.f);

	pEntity->m_pPhysicsModel->ChangePositionNow(true);

	if (dwProjectileType == PROJECTILE_TYPE_ROCKET)
		((CEntityProjectile*)pEntity)->m_vTryMoveDirection = vLookDirection*120.f;
	else
	if (dwProjectileType == PROJECTILE_TYPE_PLASMA)
		((CEntityProjectile*)pEntity)->m_vTryMoveDirection = vLookDirection*150.f;
		//((CEntityProjectile*)pEntity)->m_vTryMoveDirection = vLookDirection*300.f;
	
}
