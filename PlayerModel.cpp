#include "PlayerModel.h"


#include <string>

#include <fstream>

#include "Parser.h"

#include "Timer.h"

#include "Quaternions.h"

extern int      g_iFrameNum;

extern CMD3* g_pWeaponModel;

#include "Renderer.h"

#include <assert.h>
#include <crtdbg.h>

#include "Engine.h"

#include "LogFile.h"
extern CLogFile g_LogFile;

CPlayerModel::CPlayerModel()
{

	m_bDelayedDeath = false;
	m_fDelayedDeath = true;

	m_pHead = NULL;
	m_pUpper = NULL;
	m_pLower = NULL;
	m_pCurrentWeapon = NULL;
	m_pRocketLauncher = NULL;
	m_pPlasmaShooter = NULL;
	m_pShotgun = NULL;
	m_pGauntlet = NULL;

	m_iCurrentTorsoAnimation = 0;
	m_iCurrentLegsAnimation = 0;

	/*m_iCurrentTorsoAnimation = 11;
	m_iCurrentLegsAnimation  = 22;*/

	m_vWorldPosition.Set(672.f,23.f,-2113.f);

	m_fAngX = 0.f;
	m_fAngY = 0.f;


	m_bStrafingLeft = false;
	m_bStrafingRight = false;

	//m_iWeaponType = ATTACK_SHORT_RANGE;
	
	m_iWeaponType = ATTACK_LONG_RANGE;
	UpdateStandAndAttackType();
	//SetWeaponType(ATTACK_LONG_RANGE);


	// both
	AnimationsPriorities[0] = 0;
	AnimationsPriorities[1] = 0;
	AnimationsPriorities[2] = 0;
	AnimationsPriorities[3] = 0;
	AnimationsPriorities[4] = 0;
	AnimationsPriorities[5] = 0;
	// torso
	AnimationsPriorities[6] = 2; // gesture
	AnimationsPriorities[7] = 1; // atack
	AnimationsPriorities[8] = 1; // atack2
	AnimationsPriorities[9] = 3; // drop
	AnimationsPriorities[10] = 3; // raise
	AnimationsPriorities[11] = 3; // stand
	AnimationsPriorities[12] = 3; // stand2
	// legs
	AnimationsPriorities[13] = 3; // walkcr
	AnimationsPriorities[14] = 2; // walk
	AnimationsPriorities[15] = 3; // run
	AnimationsPriorities[16] = 3; // back
	AnimationsPriorities[17] = 3; // swim
	AnimationsPriorities[18] = 1; // jump
	AnimationsPriorities[19] = 1; // land
	AnimationsPriorities[20] = 1; // jump back
	AnimationsPriorities[21] = 1; // land back
	AnimationsPriorities[22] = 3; // idle
	AnimationsPriorities[23] = 3; // idlecr
	AnimationsPriorities[24] = 2; // legs turn

}


CPlayerModel::~CPlayerModel()
{
	SafeDelete(m_pHead);
	SafeDelete(m_pUpper);
	SafeDelete(m_pLower);
	SafeDelete(m_pRocketLauncher);
	SafeDelete(m_pPlasmaShooter);
	SafeDelete(m_pShotgun);
	SafeDelete(m_pGauntlet);
}

void CPlayerModel::UpdateStandAndAttackType(void)
{
	if (m_iWeaponType==ATTACK_SHORT_RANGE)
	{
		m_iTorsoStandAnimationID = TORSO_STAND2;
		m_iTorsoAttackAnimationID = TORSO_ATTACK2;
	}
	else // WEAPON_LARGE_RANGE
	{
		m_iTorsoStandAnimationID = TORSO_STAND;
		m_iTorsoAttackAnimationID = TORSO_ATTACK;
	}

}

// FIXME:  TERRIBLE BUG!!!!!!!
//         ONLY WORKS WHEN I INIT m_iWeaponType to long range!!!
//         IF DIFFERENT, UPPER DOESN'T GO TO CORRECT STATE 

void CPlayerModel::SetWeaponType(int iSet)
{
	m_iWeaponType = iSet;
	UpdateStandAndAttackType();	

	//BruteSetCurrentTorsoAnimationID(m_iTorsoStandAnimationID);
	//ForceTorsoAnimations();

	SetTorsoAnimation(m_iTorsoStandAnimationID);

}


void CPlayerModel::ForceTorsoAnimations(void)
{
	m_pUpper->ForceAnimByGeneralAnimationName(m_iTorsoStandAnimationID);
	m_pHead->ForceAnimByGeneralAnimationName(m_iTorsoStandAnimationID);
	m_pCurrentWeapon->ForceAnimByGeneralAnimationName(m_iTorsoStandAnimationID);
}

void CPlayerModel::InitAnimationStates(void)
{
	m_pUpper->ForceAnimByGeneralAnimationName(m_iTorsoStandAnimationID);
	m_pHead->ForceAnimByGeneralAnimationName(m_iTorsoStandAnimationID);
	m_pCurrentWeapon->ForceAnimByGeneralAnimationName(m_iTorsoStandAnimationID);
	m_pLower->ForceAnimByGeneralAnimationName(LEGS_IDLE);
}

bool CPlayerModel::Load(char *pPath)
{
	std::string strPath;
	std::string strHeadMD3,strUpperMD3,strLowerMD3;
	std::string strHeadSkin,strUpperSkin,strLowerSkin;

	//assert(_CrtCheckMemory());

	std::string strAnimationFile;

	strPath = pPath;

	strHeadMD3 = strPath + "head.md3";
	strUpperMD3 = strPath + "upper.md3";
	strLowerMD3 = strPath + "lower.md3";

	strHeadSkin = strPath + "head_default.skin";
	strUpperSkin = strPath + "upper_default.skin";
	strLowerSkin = strPath + "lower_default.skin";


	strAnimationFile = strPath + "animation.cfg";

	SafeDelete(m_pHead);
	SafeDelete(m_pUpper);
	SafeDelete(m_pLower);
	SafeDelete(m_pCurrentWeapon);
	SafeDelete(m_pRocketLauncher);
	SafeDelete(m_pPlasmaShooter);
	SafeDelete(m_pShotgun);
	SafeDelete(m_pGauntlet);

	m_pHead = new CMD3;
	m_pUpper = new CMD3;
	m_pLower = new CMD3;
	
	m_pCurrentWeapon = NULL;
	m_pRocketLauncher = new CMD3;
	m_pPlasmaShooter = new CMD3;
	m_pShotgun = new CMD3;
	m_pGauntlet = new CMD3;



	bool bAlreadyInManager = m_pUpper->ExistInManager(strUpperMD3.c_str());

	

	//assert(_CrtCheckMemory());

	if (!m_pHead->Load(strHeadMD3.c_str(),strHeadSkin.c_str(),NULL,true))
		return false;


	//assert(_CrtCheckMemory());

	if (!m_pUpper->Load(strUpperMD3.c_str(),strUpperSkin.c_str(),NULL,true))
		return false;

	//assert(_CrtCheckMemory());

	if (!m_pLower->Load(strLowerMD3.c_str(),strLowerSkin.c_str(),NULL,true))
		return false;

	//assert(_CrtCheckMemory());

	//if (!m_pCurrentWeapon->Load("models/weapons2/shotgun/shotgun.md3",NULL,NULL)) 
	//if (!m_pCurrentWeapon->Load("models/weapons2/gauntlet/gauntlet.md3",NULL,NULL)) 	
	//if (!m_pCurrentWeapon->Load("models/weapons2/machinegun/machinegun.md3",NULL,NULL)) 	
	//if (!m_pCurrentWeapon->Load("models/weapons2/plasma/plasma.md3",NULL,NULL)) 	
	
	if (!m_pRocketLauncher->Load("models/weapons2/rocketl/rocketl.md3",NULL,NULL,true)) 			
		return false;

	//assert(_CrtCheckMemory());

	if (!m_pPlasmaShooter->Load("models/weapons2/plasma/plasma.md3",NULL,NULL,true)) 	
		return false;

	//assert(_CrtCheckMemory());

	if (!m_pShotgun->Load("models/weapons2/shotgun/shotgun.md3",NULL,NULL,true)) 
		return false;

	//assert(_CrtCheckMemory());

	if (!m_pGauntlet->Load("models/weapons2/gauntlet/gauntlet.md3",NULL,NULL,true))
		return false;

	//assert(_CrtCheckMemory());



	m_pCurrentWeapon = m_pGauntlet; //default to gauntlet


	/*if (!m_pCurrentWeapon->Load("models/weapons2/Test/railgun2.md3",NULL))
		return false;*/

	


	// Link the lower body to the upper body when the tag "tag_torso" is found in our tag array
	Link(m_pLower, m_pUpper, "tag_torso");


	// Link the weapon to the model's hand that has the weapon tag
	Link(m_pUpper, m_pCurrentWeapon, "tag_weapon");

	// Link the upper body to the head when the tag "tag_head" is found in our tag array
	Link(m_pUpper, m_pHead, "tag_head");



	//assert(_CrtCheckMemory());



	if (!bAlreadyInManager)
	{
		LoadAnimationFile(strAnimationFile.c_str());		
	}


	//assert(_CrtCheckMemory());

	InitAnimationStates();
	

	////
	
	return true;
}

void CPlayerModel::SetCurrentWeapon(int iNumWeapon)
{
	if (iNumWeapon==0)
		m_pCurrentWeapon = m_pGauntlet;
	else
	if (iNumWeapon==1)
		m_pCurrentWeapon = m_pShotgun;
	else
	if (iNumWeapon==2)
		m_pCurrentWeapon = m_pRocketLauncher;
	else
	if (iNumWeapon==3)
		m_pCurrentWeapon = m_pPlasmaShooter;


	// Link the weapon to the model's hand that has the weapon tag
	Link(m_pUpper, m_pCurrentWeapon, "tag_weapon");
}

bool CPlayerModel::LoadAnimationFile(const char* pPath)
{
	std::ifstream file;

	file.open(pPath);

	if(file.fail())
		{	
			char* strMessage = new char[300];
			sprintf(strMessage, "Unable to load animation file: %s!", pPath);
			MessageBox(NULL, strMessage, "Error", MB_OK);
			SafeDeleteArray(strMessage);
			return false;								
		}
	

		//assert(_CrtCheckMemory());

	std::string t_CurrentLine;
	int iLineNum = 0;

	/*CParser parser;
	char* cpTempWord = new char[400];*/

	char s1[200],s2[200],s3[200],s4[200],s5[200],s6[200],s7[200],s8[200],s9[200],s10[200];
	int num_args = 0;

	int iAnimationSequenceNum=0;

	CAnimationSequence t_AnimationSequence;

	//assert(_CrtCheckMemory());


	m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations = 0;
	m_pUpper->m_pMD3Shared->m_iNumOfAnimations = 0;
	m_pLower->m_pMD3Shared->m_iNumOfAnimations = 0;
	m_pHead->m_pMD3Shared->m_iNumOfAnimations = 0;

	while(std::getline(file,t_CurrentLine))
		{
			iLineNum++;
			//parser.SetAll(t_CurrentLine.c_str());

			num_args=sscanf(t_CurrentLine.c_str(),"%s %s %s %s %s %s %s %s %s %s",s1,s2,s3,s4,s5,s6,s7,s8,s9,s10);

			// empty line
			if (num_args<=0)
			{
				s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;
				continue;
			}

			// a comment
			if (s1[0]=='/')
			{
				s1[0]=s2[0]=s3[0]=s4[0]=s5[0]=s6[0]=s7[0]=s8[0]=s9[0]=s10[0]=NULL;
				continue;
			}

			if (strcmp("sex",s1)==0)
			{
				// do something
				continue;
			}

			if (strcmp("headoffset",s1)==0)
			{
				// do something
				continue;
			}

			if (strcmp("footsteps",s1)==0)
			{
				// do something
				continue;
			}

			if (strcmp("fixedlegs",s1)==0)
			{
				// do something
				continue;
			}

			if (strcmp("fixedtorso",s1)==0)
			{
				// do something
				continue;
			}

			
			//assert(_CrtCheckMemory());



			// we are reading an animation sequence

			t_AnimationSequence.startFrame = atoi(s1);
			t_AnimationSequence.endFrame = t_AnimationSequence.startFrame + atoi(s2);

			if (atoi(s3)>0)
				t_AnimationSequence.bIsRepetetive = true;
			else
				t_AnimationSequence.bIsRepetetive = false;


			t_AnimationSequence.loopingFrames = atoi(s3);

			t_AnimationSequence.framesPerSecond = atoi(s4);


			//assert(_CrtCheckMemory());

			if ( iAnimationSequenceNum>=0 && iAnimationSequenceNum<=5) // both torso and legs
			{

				//assert(_CrtCheckMemory());

				m_pUpper->m_pMD3Shared->animations[m_pUpper->m_pMD3Shared->m_iNumOfAnimations].startFrame = t_AnimationSequence.startFrame;
				m_pUpper->m_pMD3Shared->animations[m_pUpper->m_pMD3Shared->m_iNumOfAnimations].endFrame = t_AnimationSequence.endFrame;
				m_pUpper->m_pMD3Shared->animations[m_pUpper->m_pMD3Shared->m_iNumOfAnimations].framesPerSecond = t_AnimationSequence.framesPerSecond;
				m_pUpper->m_pMD3Shared->animations[m_pUpper->m_pMD3Shared->m_iNumOfAnimations].bIsRepetetive = t_AnimationSequence.bIsRepetetive;
				m_pUpper->m_pMD3Shared->animations[m_pUpper->m_pMD3Shared->m_iNumOfAnimations].iAnimationID = iAnimationSequenceNum;
				m_pUpper->m_pMD3Shared->m_iNumOfAnimations++;
				

				//assert(_CrtCheckMemory());


				// somewhere here //////////////////////////////////////////////////////////

				int iDebug1 = m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations;

				m_pCurrentWeapon->m_pMD3Shared->animations[m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations].startFrame = t_AnimationSequence.startFrame;

				//assert(_CrtCheckMemory());

				m_pCurrentWeapon->m_pMD3Shared->animations[m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations].endFrame = t_AnimationSequence.endFrame;

				//assert(_CrtCheckMemory());

				m_pCurrentWeapon->m_pMD3Shared->animations[m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations].framesPerSecond = t_AnimationSequence.framesPerSecond;

				//assert(_CrtCheckMemory());

				m_pCurrentWeapon->m_pMD3Shared->animations[m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations].bIsRepetetive = t_AnimationSequence.bIsRepetetive;

				//assert(_CrtCheckMemory());

				m_pCurrentWeapon->m_pMD3Shared->animations[m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations].iAnimationID = iAnimationSequenceNum;

				//assert(_CrtCheckMemory());

				m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations++;

				////////////////////////////////////////////////////////////////////////////

				//assert(_CrtCheckMemory());

				m_pLower->m_pMD3Shared->animations[m_pLower->m_pMD3Shared->m_iNumOfAnimations].startFrame = t_AnimationSequence.startFrame;
				m_pLower->m_pMD3Shared->animations[m_pLower->m_pMD3Shared->m_iNumOfAnimations].endFrame = t_AnimationSequence.endFrame;
				m_pLower->m_pMD3Shared->animations[m_pLower->m_pMD3Shared->m_iNumOfAnimations].framesPerSecond = t_AnimationSequence.framesPerSecond;
				m_pLower->m_pMD3Shared->animations[m_pLower->m_pMD3Shared->m_iNumOfAnimations].bIsRepetetive = t_AnimationSequence.bIsRepetetive;
				m_pLower->m_pMD3Shared->animations[m_pLower->m_pMD3Shared->m_iNumOfAnimations].iAnimationID = iAnimationSequenceNum;
				m_pLower->m_pMD3Shared->m_iNumOfAnimations++;

				//assert(_CrtCheckMemory());

				m_pHead->m_pMD3Shared->animations[m_pHead->m_pMD3Shared->m_iNumOfAnimations].startFrame = t_AnimationSequence.startFrame;
				m_pHead->m_pMD3Shared->animations[m_pHead->m_pMD3Shared->m_iNumOfAnimations].endFrame = t_AnimationSequence.endFrame;
				m_pHead->m_pMD3Shared->animations[m_pHead->m_pMD3Shared->m_iNumOfAnimations].framesPerSecond = t_AnimationSequence.framesPerSecond;
				m_pHead->m_pMD3Shared->animations[m_pHead->m_pMD3Shared->m_iNumOfAnimations].bIsRepetetive = t_AnimationSequence.bIsRepetetive;
				m_pHead->m_pMD3Shared->animations[m_pHead->m_pMD3Shared->m_iNumOfAnimations].iAnimationID = iAnimationSequenceNum;
				m_pHead->m_pMD3Shared->m_iNumOfAnimations++;

				//assert(_CrtCheckMemory());
			}

			//assert(_CrtCheckMemory());

			if ( iAnimationSequenceNum>=6 && iAnimationSequenceNum<=12) // just torso
			{

				//assert(_CrtCheckMemory());

				m_pUpper->m_pMD3Shared->animations[m_pUpper->m_pMD3Shared->m_iNumOfAnimations].startFrame = t_AnimationSequence.startFrame;
				m_pUpper->m_pMD3Shared->animations[m_pUpper->m_pMD3Shared->m_iNumOfAnimations].endFrame = t_AnimationSequence.endFrame;
				m_pUpper->m_pMD3Shared->animations[m_pUpper->m_pMD3Shared->m_iNumOfAnimations].framesPerSecond = t_AnimationSequence.framesPerSecond;
				m_pUpper->m_pMD3Shared->animations[m_pUpper->m_pMD3Shared->m_iNumOfAnimations].bIsRepetetive = t_AnimationSequence.bIsRepetetive;
				m_pUpper->m_pMD3Shared->animations[m_pUpper->m_pMD3Shared->m_iNumOfAnimations].iAnimationID = iAnimationSequenceNum;
				m_pUpper->m_pMD3Shared->m_iNumOfAnimations++;

				//assert(_CrtCheckMemory());

				m_pCurrentWeapon->m_pMD3Shared->animations[m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations].startFrame = t_AnimationSequence.startFrame;
				m_pCurrentWeapon->m_pMD3Shared->animations[m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations].endFrame = t_AnimationSequence.endFrame;
				m_pCurrentWeapon->m_pMD3Shared->animations[m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations].framesPerSecond = t_AnimationSequence.framesPerSecond;
				m_pCurrentWeapon->m_pMD3Shared->animations[m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations].bIsRepetetive = t_AnimationSequence.bIsRepetetive;
				m_pCurrentWeapon->m_pMD3Shared->animations[m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations].iAnimationID = iAnimationSequenceNum;
				m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations++;


				//assert(_CrtCheckMemory());

				m_pHead->m_pMD3Shared->animations[m_pHead->m_pMD3Shared->m_iNumOfAnimations].startFrame = t_AnimationSequence.startFrame;
				m_pHead->m_pMD3Shared->animations[m_pHead->m_pMD3Shared->m_iNumOfAnimations].endFrame = t_AnimationSequence.endFrame;
				m_pHead->m_pMD3Shared->animations[m_pHead->m_pMD3Shared->m_iNumOfAnimations].framesPerSecond = t_AnimationSequence.framesPerSecond;
				m_pHead->m_pMD3Shared->animations[m_pHead->m_pMD3Shared->m_iNumOfAnimations].bIsRepetetive = t_AnimationSequence.bIsRepetetive;
				m_pHead->m_pMD3Shared->animations[m_pHead->m_pMD3Shared->m_iNumOfAnimations].iAnimationID = iAnimationSequenceNum;
				m_pHead->m_pMD3Shared->m_iNumOfAnimations++;


			}



			//assert(_CrtCheckMemory());

			if ( iAnimationSequenceNum>=13 && iAnimationSequenceNum<=24) // just legs
			{									
				m_pLower->m_pMD3Shared->animations[m_pLower->m_pMD3Shared->m_iNumOfAnimations].startFrame = t_AnimationSequence.startFrame;
				m_pLower->m_pMD3Shared->animations[m_pLower->m_pMD3Shared->m_iNumOfAnimations].endFrame = t_AnimationSequence.endFrame;
				m_pLower->m_pMD3Shared->animations[m_pLower->m_pMD3Shared->m_iNumOfAnimations].framesPerSecond = t_AnimationSequence.framesPerSecond;
				m_pLower->m_pMD3Shared->animations[m_pLower->m_pMD3Shared->m_iNumOfAnimations].bIsRepetetive = t_AnimationSequence.bIsRepetetive;
				m_pLower->m_pMD3Shared->animations[m_pLower->m_pMD3Shared->m_iNumOfAnimations].iAnimationID = iAnimationSequenceNum;
				if (iAnimationSequenceNum == LEGS_JUMP || iAnimationSequenceNum == LEGS_LAND ||
					iAnimationSequenceNum == LEGS_JUMPB || iAnimationSequenceNum == LEGS_LANDB)
					m_pLower->m_pMD3Shared->animations[m_pLower->m_pMD3Shared->m_iNumOfAnimations].bShouldFreezeAtEnd = true;
				m_pLower->m_pMD3Shared->m_iNumOfAnimations++;
			}

			
	

			iAnimationSequenceNum++;

		}

		//assert(_CrtCheckMemory());

//	SafeDeleteArray(cpTempWord);

			// Because I found that some config files have the starting frame for the
				// torso and the legs a different number, we need to account for this by finding
				// the starting frame of the first legs animation, then subtracting the starting
				// frame of the first torso animation from it.  For some reason, some exporters
				// might keep counting up, instead of going back down to the next frame after the
				// end frame of the BOTH_DEAD3 anim.  This will make your program crash if so.
			
				// If the torso offset hasn't been set, set it
				/*if(!torsoOffset)*/

		int torsoOffset = m_pLower->m_pMD3Shared->animations[m_pLower->GetAnimationByID(LEGS_WALKCR)].startFrame - m_pUpper->m_pMD3Shared->animations[m_pUpper->GetAnimationByID(TORSO_GESTURE)].startFrame;

				// Minus the offset from the legs animation start and end frame.
				/*animations[currentAnim].startFrame -= torsoOffset;
				animations[currentAnim].endFrame -= torsoOffset;*/

		for (int i=0;i<m_pLower->m_pMD3Shared->m_iNumOfAnimations;i++)
		{
			if (m_pLower->m_pMD3Shared->animations[i].iAnimationID>=LEGS_WALKCR)
			{
				m_pLower->m_pMD3Shared->animations[i].startFrame-=torsoOffset;
				m_pLower->m_pMD3Shared->animations[i].endFrame-=torsoOffset;

			}
		}



		//assert(_CrtCheckMemory());

		/**/ // debug draw the animation arrays

		char* pDebugString = new char[400];

		sprintf(pDebugString,"m_pLower:");
		g_LogFile.OutPutPlainText(pDebugString,"3","990000",true);

		for (int i=0;i<m_pLower->m_pMD3Shared->m_iNumOfAnimations;i++)
		{
			sprintf(pDebugString,"Start:%d, End:%d, FPS:%d, ID: %d",
				m_pLower->m_pMD3Shared->animations[i].startFrame,
				m_pLower->m_pMD3Shared->animations[i].endFrame,
				m_pLower->m_pMD3Shared->animations[i].framesPerSecond,
				m_pLower->m_pMD3Shared->animations[i].iAnimationID);
			g_LogFile.OutPutPlainText(pDebugString,"3","990000",true);
		}


		//assert(_CrtCheckMemory());

		sprintf(pDebugString,"m_pUpper:");
		g_LogFile.OutPutPlainText(pDebugString,"3","990000",true);

		for (int i=0;i<m_pUpper->m_pMD3Shared->m_iNumOfAnimations;i++)
		{
			sprintf(pDebugString,"Start:%d, End:%d, FPS:%d, ID: %d",
				m_pUpper->m_pMD3Shared->animations[i].startFrame,
				m_pUpper->m_pMD3Shared->animations[i].endFrame,
				m_pUpper->m_pMD3Shared->animations[i].framesPerSecond,
				m_pUpper->m_pMD3Shared->animations[i].iAnimationID);
			g_LogFile.OutPutPlainText(pDebugString,"3","990000",true);
		}

		sprintf(pDebugString,"m_pHead:");
		g_LogFile.OutPutPlainText(pDebugString,"3","990000",true);

		for (int i=0;i<m_pHead->m_pMD3Shared->m_iNumOfAnimations;i++)
		{
			sprintf(pDebugString,"Start:%d, End:%d, FPS:%d, ID: %d",
				m_pHead->m_pMD3Shared->animations[i].startFrame,
				m_pHead->m_pMD3Shared->animations[i].endFrame,
				m_pHead->m_pMD3Shared->animations[i].framesPerSecond,
				m_pHead->m_pMD3Shared->animations[i].iAnimationID);
			g_LogFile.OutPutPlainText(pDebugString,"3","990000",true);
		}

		sprintf(pDebugString,"m_pCurrentWeapon:");
		g_LogFile.OutPutPlainText(pDebugString,"3","990000",true);

		for (int i=0;i<m_pCurrentWeapon->m_pMD3Shared->m_iNumOfAnimations;i++)
		{
			sprintf(pDebugString,"Start:%d, End:%d, FPS:%d, ID: %d",
				m_pCurrentWeapon->m_pMD3Shared->animations[i].startFrame,
				m_pCurrentWeapon->m_pMD3Shared->animations[i].endFrame,
				m_pCurrentWeapon->m_pMD3Shared->animations[i].framesPerSecond,
				m_pCurrentWeapon->m_pMD3Shared->animations[i].iAnimationID);
			g_LogFile.OutPutPlainText(pDebugString,"3","990000",true);
		}

		
		//assert(_CrtCheckMemory());

		SafeDeleteArray(pDebugString);

		//assert(_CrtCheckMemory());

	return true;
}

void CPlayerModel::SetTorsoAnimation(int iNum)
{
	m_iCurrentTorsoAnimation = iNum;

	m_pUpper->ForceAnimByGeneralAnimationName(m_iCurrentTorsoAnimation);
	m_pHead->ForceAnimByGeneralAnimationName(m_iCurrentTorsoAnimation);
	m_pCurrentWeapon->ForceAnimByGeneralAnimationName(m_iCurrentTorsoAnimation);	
}

void CPlayerModel::SetLegsAnimation(int iNum)
{
	m_iCurrentLegsAnimation = iNum;

	m_pLower->ForceAnimByGeneralAnimationName(m_iCurrentLegsAnimation);
}




void CPlayerModel::SetTorsoAnimationIfNotSame(int iNum)
{
	//if ( AnimationsPriorities[iNum] <= AnimationsPriorities[m_iCurrentTorsoAnimation] )
	if (m_iCurrentTorsoAnimation!=iNum)
	{

		/**/
		for (int i=0;i<m_pUpper->m_pMD3Shared->m_iMeshesNum;i++)
		{
			for (int j=0;j<m_pUpper->m_pMeshes[i].m_pSharedMesh->m_iVerticesNum;j++)
			{
				// optimize to memcpy!!!!
				m_pUpper->m_pMeshes[i].m_pBackupVerts[j] = m_pUpper->m_pMeshes[i].m_pCurrentInterpolatedVerts[j];
			}
		}

		m_pUpper->m_bChangingAni = true;
		m_pUpper->m_fChangeAniT = 0.f;

		/**/


		m_iCurrentTorsoAnimation = iNum;

		m_pUpper->ForceAnimByGeneralAnimationName(m_iCurrentTorsoAnimation);
		m_pHead->ForceAnimByGeneralAnimationName(m_iCurrentTorsoAnimation);
		m_pCurrentWeapon->ForceAnimByGeneralAnimationName(m_iCurrentTorsoAnimation);	
	}
	
}

void CPlayerModel::SetLegsAnimationIfNotSame(int iNum)
{

	//
	// here i should add some priority thingy
	//

	//if ( AnimationsPriorities[iNum] <= AnimationsPriorities[m_iCurrentLegsAnimation] )
	if (m_iCurrentLegsAnimation!=iNum)
	{

		/**/
		for (int i=0;i<m_pLower->m_pMD3Shared->m_iMeshesNum;i++)
		{
			for (int j=0;j<m_pLower->m_pMeshes[i].m_pSharedMesh->m_iVerticesNum;j++)
				m_pLower->m_pMeshes[i].m_pBackupVerts[j] = m_pLower->m_pMeshes[i].m_pCurrentInterpolatedVerts[j];
		}
		

		
		/**/

		m_iCurrentLegsAnimation = iNum;

		m_pLower->ForceAnimByGeneralAnimationName(m_iCurrentLegsAnimation);

	}
}




void CPlayerModel::SetTorsoAnimationIfNotSameNoPriority(int iNum)
{
	if (m_iCurrentTorsoAnimation!=iNum)
	{

		/**/
		for (int i=0;i<m_pUpper->m_pMD3Shared->m_iMeshesNum;i++)
		{
			for (int j=0;j<m_pUpper->m_pMeshes[i].m_pSharedMesh->m_iVerticesNum;j++)
			{
				// optimize to memcpy!!!!
				m_pUpper->m_pMeshes[i].m_pBackupVerts[j] = m_pUpper->m_pMeshes[i].m_pCurrentInterpolatedVerts[j];
			}
		}

		m_pUpper->m_bChangingAni = true;
		m_pUpper->m_fChangeAniT = 0.f;

		/**/


		m_iCurrentTorsoAnimation = iNum;

		m_pUpper->ForceAnimByGeneralAnimationName(m_iCurrentTorsoAnimation);
		m_pHead->ForceAnimByGeneralAnimationName(m_iCurrentTorsoAnimation);
		m_pCurrentWeapon->ForceAnimByGeneralAnimationName(m_iCurrentTorsoAnimation);	
	}
	
}

void CPlayerModel::SetLegsAnimationIfNotSameNoPriority(int iNum)
{

	//
	// here i should add some priority thingy
	//

	if (m_iCurrentLegsAnimation!=iNum)
	{

		/**/
		for (int i=0;i<m_pLower->m_pMD3Shared->m_iMeshesNum;i++)
		{
			for (int j=0;j<m_pLower->m_pMeshes[i].m_pSharedMesh->m_iVerticesNum;j++)
				m_pLower->m_pMeshes[i].m_pBackupVerts[j] = m_pLower->m_pMeshes[i].m_pCurrentInterpolatedVerts[j];
		}
		

		
		/**/

		m_iCurrentLegsAnimation = iNum;

		m_pLower->ForceAnimByGeneralAnimationName(m_iCurrentLegsAnimation);

	}
}


void CPlayerModel::SetTorsoAnimationIfNotSameAndCurrentEnded(int iNum)
{

	if (!m_pUpper->m_pMD3Shared->animations[m_pUpper->GetAnimationByID(m_iCurrentTorsoAnimation)].bIsRepetetive)
	if (!m_pUpper->GetAnimationEndedNow())
		return;

	if (m_iCurrentTorsoAnimation!=iNum)
	{

		/**/
		for (int i=0;i<m_pUpper->m_pMD3Shared->m_iMeshesNum;i++)
		{
			for (int j=0;j<m_pUpper->m_pMeshes[i].m_pSharedMesh->m_iVerticesNum;j++)
			{
				// optimize to memcpy!!!!
				m_pUpper->m_pMeshes[i].m_pBackupVerts[j] = m_pUpper->m_pMeshes[i].m_pCurrentInterpolatedVerts[j];
			}
		}

		m_pUpper->m_bChangingAni = true;
		m_pUpper->m_fChangeAniT = 0.f;

		/**/


		m_iCurrentTorsoAnimation = iNum;

		m_pUpper->ForceAnimByGeneralAnimationName(m_iCurrentTorsoAnimation);
		m_pHead->ForceAnimByGeneralAnimationName(m_iCurrentTorsoAnimation);
		m_pCurrentWeapon->ForceAnimByGeneralAnimationName(m_iCurrentTorsoAnimation);	
	}
	
}

void CPlayerModel::SetLegsAnimationIfNotSameAndCurrentEnded(int iNum)
{
	
	if (!m_pLower->m_pMD3Shared->animations[m_pLower->GetAnimationByID(m_iCurrentLegsAnimation)].bIsRepetetive)
	if (!m_pLower->GetAnimationEndedNow())
		return;

	if (m_iCurrentLegsAnimation!=iNum)
	{

		/**/
		for (int i=0;i<m_pLower->m_pMD3Shared->m_iMeshesNum;i++)
		{
			for (int j=0;j<m_pLower->m_pMeshes[i].m_pSharedMesh->m_iVerticesNum;j++)
				m_pLower->m_pMeshes[i].m_pBackupVerts[j] = m_pLower->m_pMeshes[i].m_pCurrentInterpolatedVerts[j];
		}
		

		
		/**/

		m_iCurrentLegsAnimation = iNum;

		m_pLower->ForceAnimByGeneralAnimationName(m_iCurrentLegsAnimation);

	}
}




void CPlayerModel::AdvanceTorsoAnimation(void)
{
	m_iCurrentTorsoAnimation++;
	if (m_iCurrentTorsoAnimation > m_pUpper->m_pMD3Shared->m_iNumOfAnimations-1)
		m_iCurrentTorsoAnimation=0;

	m_pUpper->ForceAnimByLocalArrayIndex(m_iCurrentTorsoAnimation);
	m_pHead->ForceAnimByLocalArrayIndex(m_iCurrentTorsoAnimation);
	m_pCurrentWeapon->ForceAnimByLocalArrayIndex(m_iCurrentTorsoAnimation);	

}

void CPlayerModel::AdvanceLegsAnimation(void)
{
	m_iCurrentLegsAnimation++;
	if (m_iCurrentLegsAnimation > m_pLower->m_pMD3Shared->m_iNumOfAnimations-1)
		m_iCurrentLegsAnimation=0;

	m_pLower->ForceAnimByLocalArrayIndex(m_iCurrentLegsAnimation);

}




void  CPlayerModel::Link(CMD3 *pModel, CMD3 *pLink, LPSTR strTagName)
{

	// Go through all of our tags and find which tag contains the strTagName, then link'em
	for(int i = 0; i < pModel->m_pMD3Shared->m_iNumOfTags; i++)
	{
	
		// If this current tag index has the tag name we are looking for
		if( !_stricmp(pModel->m_pMD3Shared->m_pTags[i].strName, strTagName) )
		{
			// Link the model's link index to the link (or model/mesh) and return
			pModel->m_pLinks[i] = pLink;
			return;
		}
	}
}




void CPlayerModel::DrawPlayer(bool bPPLused)
{
	st_EngineI->m_fDrawPart11S = st_TimerI->TimerGetTimeF();


	m_pLower->SetMeshesCurrentFramePointers(m_pLower->m_iCurrentFrame);
	m_pLower->SetMeshesNextFramePointers(m_pLower->m_iNextFrame);
	m_pLower->ComputeMeshesInterpolatedVertices(m_pLower->m_fT);
	

	m_pUpper->SetMeshesCurrentFramePointers(m_pUpper->m_iCurrentFrame);
	m_pUpper->SetMeshesNextFramePointers(m_pUpper->m_iNextFrame);
	m_pUpper->ComputeMeshesInterpolatedVertices(m_pUpper->m_fT);

	m_pHead->SetMeshesCurrentFramePointers(0); // the head (vertices) are NEVER animated
	//m_pUpper->SetMeshesNextFramePointers(m_pHead->m_iNextFrame);
	m_pHead->ComputeMeshesInterpolatedVertices(0.f);
	
	m_pCurrentWeapon->SetMeshesCurrentFramePointers(0); // the weapon (vertices) are NEVER animated
	//m_pCurrentWeapon->SetMeshesNextFramePointers(m_pHead->m_iNextFrame);
	m_pCurrentWeapon->ComputeMeshesInterpolatedVertices(0.f);

	m_vTempTranslation.Set(0.f,0.f,0.f);
	
	m_vTempRotation.Set(0.f,0.f,0.f);

	m_vTempMatrix.MatMakeUnit();

	CMatrix m1,m2;


	if (m_bStrafingLeft) // strafe left
		m1.MatMakeRot(DEG2RAD(-90.f),DEG2RAD(180.f),DEG2RAD(m_fAngY));
	else
	if (m_bStrafingRight) // strafe right
		m1.MatMakeRot(DEG2RAD(-90.f),DEG2RAD(0.f),DEG2RAD(m_fAngY));
	else // no strafing
	m1.MatMakeRot(DEG2RAD(-90.f),DEG2RAD(90.f+m_fAngY),0.f);

	m2.MatMakeMove(m_vWorldPosition.v[0],m_vWorldPosition.v[1],m_vWorldPosition.v[2]);
	//m2.MatMakeMove(672.f,30.f,-2113.f);

	m1 = m2 * m1;

	m_vTempMatrix = m1;

	m1.ConvertToOpenglMatrix(&m_fMatrix[0]);
	
	st_EngineI->m_fDrawPart11D += st_TimerI->TimerGetTimeF() - st_EngineI->m_fDrawPart11S;
	DrawLink(m_pLower,bPPLused);
		
}




void CPlayerModel::DrawLink(CMD3* pModel,bool bPPLused)
{

	st_EngineI->m_fDrawPart11S = st_TimerI->TimerGetTimeF();
	

	CMatrix m1;

	CMatrix mBackupMatrix;


	if (pModel==m_pUpper)// change the upper body to point where the player looks at
	{

		// if we rotated the legs to some direction because of strafing,
		// the rest of the body shouldn't be effected
		if (m_bStrafingLeft) // strafe left // -90
			m1.MatMakeRot(DEG2RAD(-m_fAngX),DEG2RAD(0.f),DEG2RAD(-90.f));
		else
		if (m_bStrafingRight) // strafe right //+90
			m1.MatMakeRot(DEG2RAD(m_fAngX),DEG2RAD(0.f),DEG2RAD(90.f));
		else // no strafing
			m1.MatMakeRot(0.f,DEG2RAD(-m_fAngX),0.f);
		
		m_vTempMatrix= m_vTempMatrix * m1;
		m_vTempMatrix.ConvertToOpenglMatrix(&m_fMatrix[0]);		
		
	}


	// Create some local variables to store all this crazy interpolation data
	CQuaternion qQuat, qNextQuat, qInterpolatedQuat;

	// the 3x3 rotation matrix pointers
	float *pMatrix, *pNextMatrix;



	// RESTORE RESTORE  RESTORE RESTORE
	/*pModel->Update(true,false,&m_fMatrix[0]);
	pModel->PushToRenderer();*/

	pModel->Update(true,false,&m_fMatrix[0]);

	
	st_EngineI->m_fDrawPart11D += st_TimerI->TimerGetTimeF() - st_EngineI->m_fDrawPart11S;


	st_EngineI->m_fDrawPart12S = st_TimerI->TimerGetTimeF();

	pModel->PushToRenderer(bPPLused);

	if (bPPLused)
		st_Renderer::Inst()->RenderAll(m_fMatrix,false);

	st_EngineI->m_fDrawPart12D += st_TimerI->TimerGetTimeF() - st_EngineI->m_fDrawPart12S;

		
	
	st_EngineI->m_fDrawPart11S = st_TimerI->TimerGetTimeF();
	

	// Now we need to go through all of this models tags and draw them.
	for(int i = 0; i < pModel->m_pMD3Shared->m_iNumOfTags; i++)
	{
		// Get the current link from the models array of links (Pointers to models)
		CMD3 *pLink = pModel->m_pLinks[i];

		// If this link has a valid address, let's draw it!
		if(pLink)
		{	
			//////////////////////////////////////////////////////////////
			// "push" the World matrix (save it into the backup matrix)
			//////////////////////////////////////////////////////////////

			mBackupMatrix = m_vTempMatrix;
	
			//int iTagNum = pModel->m_iCurrentDrawnFrame * pModel->m_iNumOfTags + i;
			int iTagNum = pModel->m_iCurrentFrame * pModel->m_pMD3Shared->m_iNumOfTags + i;

			
			// original
			//m1.MatImportFrom3x3(&pModel->m_pTags[iTagNum].rotation[0]);

			// Get a pointer to the start of the 3x3 rotation matrix for the current frame
			pMatrix = pModel->m_pMD3Shared->m_pTags[pModel->m_iCurrentFrame * pModel->m_pMD3Shared->m_iNumOfTags + i].rotation;

			// Get a pointer to the start of the 3x3 rotation matrix for the next frame
			pNextMatrix = pModel->m_pMD3Shared->m_pTags[pModel->m_iNextFrame * pModel->m_pMD3Shared->m_iNumOfTags + i].rotation;
			
			// Now that we have 2 1D arrays that store the matrices, let's interpolate them

			// Convert the current and next key frame 3x3 matrix into a quaternion
			qQuat.CreateFromMatrix( pMatrix, 3);
			qNextQuat.CreateFromMatrix( pNextMatrix, 3 );

			// Using spherical linear interpolation, we find the interpolated quaternion
			qInterpolatedQuat = qQuat.Slerp(qQuat, qNextQuat, pModel->m_fT);

			// Here we convert the interpolated quaternion into a 4x4 matrix
			qInterpolatedQuat.CreateMatrix( m1 );



			
			CVector3f vPosition = pModel->m_pMD3Shared->m_pTags[iTagNum].vPosition;


			if (pModel->m_pMD3Shared->m_iNumOfAnimations>0)
			{
				// Grab the next key frame translation position
				CVector3f vNextPosition = pModel->m_pMD3Shared->m_pTags[pModel->m_iNextFrame * pModel->m_pMD3Shared->m_iNumOfTags + i].vPosition;
		
				// By using the equation: p(t) = p0 + t(p1 - p0), with a time t,
				// we create a new translation position that is closer to the next key frame.
				vPosition.v[0] = vPosition.v[0] + pModel->m_fT * (vNextPosition.v[0] - vPosition.v[0]),
				vPosition.v[1]	= vPosition.v[1] + pModel->m_fT * (vNextPosition.v[1] - vPosition.v[1]),
				vPosition.v[2]	= vPosition.v[2] + pModel->m_fT * (vNextPosition.v[2] - vPosition.v[2]);	
			}


			m1.m[3][0] = vPosition.v[0];
			m1.m[3][1] = vPosition.v[1];
			m1.m[3][2] = vPosition.v[2];
			m1.m[3][3] = 1.f;


			/***/
			// get weapon center position
			if (pLink==m_pCurrentWeapon)
			{
				// the weapon projectiles are shot from here
				//m_vWeaponCenter = m_vTempMatrix * (vPosition+CVector3f(0.f,8.f,-10.f));
				m_vWeaponCenter = m_vTempMatrix * vPosition;				
			}
			else
			if (pLink==m_pUpper)
			{
				// for particle system

				m_vBonePosition = m_vTempMatrix * vPosition;
			}

			/**/

			m_vTempMatrix = m_vTempMatrix * m1;


			

			



			/***/
			
			m_vTempMatrix.ConvertToOpenglMatrix(&m_fMatrix[0]);

			// Recursively draw the next model that is linked to the current one.
			// This could either be a body part or a gun that is attached to
			// the hand of the upper body model.

			
			st_EngineI->m_fDrawPart11D += st_TimerI->TimerGetTimeF() - st_EngineI->m_fDrawPart11S;

			DrawLink(pLink,bPPLused);


			///////////////////////////////////////////////////////
			// "pop" the saved world matrix
			///////////////////////////////////////////////////////

			m_vTempMatrix = mBackupMatrix;

		}
	}
}

float CPlayerModel::GetAnimationTotalTime(int iIndex)
{
	return ( (1.f / animations[iIndex].framesPerSecond) * (animations[iIndex].endFrame-animations[iIndex].startFrame) )*1000.f;
	//return ( (animations[iIndex].framesPerSecond/1000.f) * (animations[iIndex].endFrame-animations[iIndex].startFrame) );
}