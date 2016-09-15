//#include "stdafx.h"


#ifndef CDS_FULLSCREEN								// CDS_FULLSCREEN Is Not Defined By Some Compilers
#define CDS_FULLSCREEN 4			    			
#endif												



#include <Windows.h>
#include <gl\glu.h>			
#include "glaux.h"
#include "glext.h"
#include <assert.h>
#include <crtdbg.h>
//#include "ZedTracker.h"

#include "Cg/cg.h"
#include "Cg/cgGL.h"


int g_iPlayerScore;

char* g_strWindowTitle;


#include "LogFile.h"
CLogFile g_LogFile;

#include "File.h"
CFileManager g_FileManager;

#include "cgslShader.h"
CcgslShader g_cgslShader;



void handleCgError()
{ 
	// restore if i wanna use cg...
	g_cgslShader.CgError();
}

//////////////////////////////////////////
// TODO: put this in a special class
// I'm using those to make life a bit easier...
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
int g_iCardTexUnits; // num of tex units on current card

//char defaultIP[16];
char defaultIP[17];
char chat[500];

int g_iPlayerPosNum;


#include "glExtensionsHandling.h"


bool     g_bLookOnOtherPlayer;


// ai stuff //////////////////////

bool g_bDirectLineOfSight;
bool g_bBotDistanceToFloorTooBig;

bool g_bBotJumpsWithJumpPad;
int  g_iBotSteeringMode;

//////////////////////////////////

//#include "glstatecapturer.h"


//CGLStateCapturer g_glStateCapturer;

/////////

#include "NeHeGL.h"
GL_Window* g_pWindow; // Main Window

#include "Timer.h"
#include "Input.h"

#include "GLFonts.h"
CFonts      g_Fonts; // fonts global variable (draw 2d text to screen)

#include "Frustum.h"
CFrustum g_OpenglCameraFrustum;

#include "Texture.h"
#include "MD3MeshManager.h"
#include "MD3Manager.h"

// general textures used by the engine
CTexture* g_pPleaseWaitTex;
CTexture* g_pCrossHairTex;
CTexture* g_pPortalTex;
CTexture* g_pCharacterFrameTex;





#include "Renderer.h"

//bool g_bDrawText = true;
bool g_bDrawText = false;

//#include "CollisionDetection.h"

#include "TextMessages.h"
CTextMessages g_TextMessages;


#include "Engine.h"


CVector3f g_vCameraFrom;
CVector3f g_vCameraTo;


sKeys*		g_pKeys;   // Keys

/////////////////////////////////////
bool g_bShowPortals=false;

float g_fFastCameraSpeed=30.f;
float g_fNormalCameraSpeed=0.3f;



#include "ParticleSystem.h"

////////////////////////////////////





#include "Camera.h"
CCamera g_Camera1;  // the global camera

#include "VertexBuffer.h"


#include "Q3AMap.h"
CQuake3BSP* g_pQuake3Map;

// bezier patches globals

int g_iUserDefinedBezierPatchTessLevel;

#include "Sky.h"
CSky*     g_pSky;




#include "PhysicsModel.h"
//CPhysicsModel g_Player;
#include "PlayerClient.h"

CPlayerClient *g_pCurrentlyProcessedPlayerClient,*g_pPlayerClient1,*g_pPlayerClient2; // g_pCurrentlyProcessedPlayerClient only hold the CURRENTLY tested clientplayer
CPlayerClient *g_pHumanPlayerClient;
CBot          *g_pCurrentlyProcessedBot,*g_pCurrentlyProcessedBot1,*g_pCurrentlyProcessedBot2;



int g_iNumberOfDynamicEntitiesDrawn;

#include "ZedTracker.h"

/////////////////////////
// debug stuff


float g_fDistanceBetweenPlayer;

float g_fDebugRotate;

CVertexBuffer* g_pVB1;
CVertexBuffer *g_pBoxLeft,*g_pBoxRight,*g_pBoxUp,*g_pBoxDown,*g_pBoxFront,*g_pBoxBack;

bool g_bDebugPauseClipping=false;

// collision detection

CVector3f* d_pTri;


/*CVector3f d_pCameraRayStart;
CVector3f d_pCameraRayEnd;*/

CTraceRequest g_RayTraceInput;
CTraceResult g_RayTraceOutput;

CTraceRequest g_CameraLookTraceInput;
CTraceResult g_CameraLookTraceOutput;


CTraceRequest g_CameraMoveTraceInput;
CTraceResult g_CameraMoveTraceOutput;

bool g_bTestCollisionDetection;

bool g_bShouldLinkDebug;


// physics

CVector3f g_vGravity;
float     g_fPlayerMoveStrength;


bool      g_bCollidedWithStaticEntity;
bool      g_bFinalDirectionIsOppositeThenDesired;

/*int       g_iShootTestFlags;
int       g_iShootTestContents;*/

CVector3f g_vVelocity;

/////////////////////////////

#include "md3.h"
CMD3* g_pWeaponModel;


CMD3 *g_pAimingAid, *g_pAimingAid2;
float g_fAimingAidAng;
//CMD3* g_pModel1;

#include "PlayerModel.h"

/*float g_fWeaponPosX;
float g_fWeaponPosY;*/
CVector3f g_vWeaponPos;


int      g_iFrameNum;


// sound manager
#include "SoundManager.h"
CSoundManager g_SoundManager;



#include "WeaponsSounds.h"
CWeaponsSound g_WeaponsSounds;

/******************/
// board game 
#include "CheckersBoard.h"
CCheckersBoard* g_pCheckersBoard;

//#include "CheckersAI.h"
//CCheckersBoardAI g_CheckersBoardAI;

CVector3f g_vBoardRayStart;
CVector3f g_vBoardRayEnd;


CVector3f g_vBoardRayCollision;

float g_fMouseX;
float g_fMouseY;

int  g_PointedSquareIndexX = -1;
int  g_PointedSquareIndexY = -1;

CTexture* g_pUnAccecableSquareTex;
CTexture* g_pAccecableSquareTex;
CTexture* g_pChosenSquareTex;


CTexture* g_pDiffuseTexDEBUG;
CTexture* g_pNormalTexDEBUG;
CTexture* g_pSpecularTexDEBUG;


CVertexBuffer* g_pvbPPLtest;

CVertexBuffer* g_pvbBoardSquaresAccesable;
CVertexBuffer* g_pvbBoardSquaresNotAccesable;
CVertexBuffer* g_pvbBoardSquaresSelected;







CVertexBuffer* g_pvbCharacterFrame;
CVertexBuffer* g_pvbCharacterFrame2;
CVertexBuffer* g_pvbCursor;


#include "GUI.h"
CGUIManager* g_pGUIManager=NULL;

/*******************/

// networking

#include "Communication.h"
CCommunication g_Communication;

/////////////////////////////
// Initialize()
// Init Our Window
/////////////////////////////
BOOL GL_Window::Initialize(bool bFullScreen)				
{	
	g_pWindow	= this;
	g_pKeys		= this->keys;
	//g_Camera1.Init();



	//ShowCursor(false);  
	
	
	

	InitCardTexUnitsNum();

	if (!InitMultiTextureFunctionPointers())
		return false;
	
	//Make Leak
	//float* pFloat = new float[200];
	
	//g_glStateCapturer.SetMainWindow(g_pWindow->hWnd);


	//// Init Crimson Engine!!!				
	st_Engine::CreateInstance();

	
	if (!st_Engine::Inst()->Init())
		return FALSE;
	////////////////////////////

	

	////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	// DEBUG stuff zone
	g_fDebugRotate=0.f;

	////////////////////////

	d_pTri = new CVector3f[3];

	//g_Camera1.SetPosition(-9397.1768,101.83075,  15426.614); // fred/e3bathroom5

	d_pTri[0].Set(0.f,0.f,0.f);
	d_pTri[0]+=CVector3f(-9397.1768,101.83075+5.f,  15426.614);
	d_pTri[1].Set(8.f,2.f,-4.f);
	d_pTri[1]+=CVector3f(-9397.1768,101.83075+5.f,  15426.614);
	d_pTri[2].Set(-5.f,3.f,5.f);
	d_pTri[2]+=CVector3f(-9397.1768,101.83075+5.f,  15426.614);


	//st_Renderer::Inst()->m_pSphere = gluNewQuadric();			// Get a Quadric off the stack
	

	return TRUE;												// Return TRUE (Initialization Successful)
}




/////////////////////////////
// Deinitialize()
/////////////////////////////
void GL_Window::Deinitialize (void)										// Any User DeInitialization Goes Here
{

	SafeDeleteArray(d_pTri);

	SafeDeleteArray(g_strWindowTitle);

	SafeDelete(g_pVB1);
	SafeDelete(g_pBoxFront);

	SafeDelete(g_pvbPPLtest);

	st_Engine::Destroy();

	//ShowCursor(true);								// Show Mouse Pointer

	
}


////////////////////////////////////////////////////
// Update()
// Update the world (get input, move objects, etc.)
////////////////////////////////////////////////////
void GL_Window::Update (bool bFullScreen)								// Perform Motion Updates Here
{
	
	//st_EngineI->ResetEngineMessages();


st_EngineI->m_fEntireUpdateTimerS = st_TimerI->TimerGetTimeF();



//assert(_CrtCheckMemory());
///////////////////////////////////////////////////////////////////////
// Handling Camera related keys
g_Camera1.InitMoveRotFlags(); // init the translation and rotation flags
	

	/*g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_Z_BACKWARDS;		
	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_Z_FORWARD;		
	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_X_RIGHT;		
	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_X_LEFT;		
	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_Y_DOWN;		
	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_Y_UP;*/


/*if (g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags& PLAYER_MOVE_Z_BACKWARDS)
	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_Z_BACKWARDS;*/


/*if (g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags& PLAYER_MOVE_Z_BACKWARDS)
	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_Z_BACKWARDS;

if (g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags& PLAYER_MOVE_Z_FORWARD)
	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_Z_FORWARD;*/


// test networking
	

//if ( (st_Engine::Inst()->m_iFrameCounter<3) ||
if	(st_TimerI->GetTimeSinceLastGameLogicHappened() >= GAME_LOGIC_MILISECONDS_INTERVAL)
{
	st_Input::Inst()->Update();
}
//assert(_CrtCheckMemory());

bool bOnMenu;

//if (st_Engine::Inst()->m_iGameState==0) // board mode
	bOnMenu = g_pGUIManager->UpdateMouseAndKeyboard();
	

	/////////////////////////////////
	//    KeyBoard Handling
	////////////////////////////////
	

	if (st_Input::Inst()->KeyPressed(DIK_LCONTROL) && st_Input::Inst()->KeyPressed(DIK_LSHIFT))
	{
		st_Input::Inst()->UnAcquireMouse();
		st_Input::Inst()->UnAcquireKeboard();
		//ShowCursor(true);
	}
	

	if(st_Input::Inst()->KeyPressed(DIK_F7))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_F7]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_F7]=true;
			
			float fSlowMotion = st_Timer::Inst()->GetFakeSlowMotion();
			fSlowMotion+=1.f;
			st_Timer::Inst()->SetFakeSlowMotion(fSlowMotion);
		}
	}
	else
	{
		st_Input::Inst()->m_WPKeyBuffer[DIK_F7]=false;
	}



	if(st_Input::Inst()->KeyPressed(DIK_F8))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_F8]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_F8]=true;
			
			float fSlowMotion = st_Timer::Inst()->GetFakeSlowMotion();
			fSlowMotion-=1.f;

			if (fSlowMotion<1.f)
				fSlowMotion=1.f;
			st_Timer::Inst()->SetFakeSlowMotion(fSlowMotion);
		}
	}
	else
	{
		st_Input::Inst()->m_WPKeyBuffer[DIK_F8]=false;
	}





if (st_Engine::Inst()->m_iGameState==0) // board mode
{

	if	(st_TimerI->GetTimeSinceLastGameLogicHappened() >= GAME_LOGIC_MILISECONDS_INTERVAL)
	{
		st_Input::Inst()->Update();

		/*if (
				(g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_PAST && g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_NETWORK) ||
				(g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_FUTURE && g_pCheckersBoard->m_iPlayerTypeFUTURE==PLAYER_TYPE_NETWORK)
				)*/
		{
			
			bool bMessageWaitingToBeProcessed=false;

			// i should ONLY allow recieve after the game completely finished initing!!
			// only AFTER all 
			if (g_Communication.GetAlreadyInited())
			if (g_pCheckersBoard->AreAllBoardAnimationsDone() || g_Communication.m_bStartedHandshake)
			{
				if (g_Communication.GetMessageFromQueue(g_Communication.m_BoardGameData))
					bMessageWaitingToBeProcessed=true;
				else
				if (g_Communication.RecieveBoardMessage())
					bMessageWaitingToBeProcessed=true;
			}
			


			if (bMessageWaitingToBeProcessed)
			{
				g_Communication.m_bStartedHandshake=false;

				
				if (g_Communication.m_BoardGameData.iType==NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_PAST) // server is future
				{
					g_pCheckersBoard->StopBackgroundMusic();
					g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_BOARD]);
					g_pCheckersBoard->NewGame();
					g_pGUIManager->ResetBoardGameMenus();

					// since only server can change player sides,only client accepts that
					if (g_Communication.GetCommunicationType()==COMMUNICATION_CLIENT)
					{
						g_pCheckersBoard->m_iPlayerTypePAST=PLAYER_TYPE_NETWORK;
						g_pCheckersBoard->m_iPlayerTypeFUTURE=PLAYER_TYPE_HUMAN;
					}
				}
				else
				if (g_Communication.m_BoardGameData.iType==NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_FUTURE) // server is future
				{
					g_pCheckersBoard->StopBackgroundMusic();
					g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_BOARD]);
					g_pCheckersBoard->NewGame();
					g_pGUIManager->ResetBoardGameMenus();

					// since only server can change player sides,only client accepts that
					if (g_Communication.GetCommunicationType()==COMMUNICATION_CLIENT)
					{
						g_pCheckersBoard->m_iPlayerTypePAST=PLAYER_TYPE_HUMAN;
						g_pCheckersBoard->m_iPlayerTypeFUTURE=PLAYER_TYPE_NETWORK;
					}
				}
				else
				/*if (g_Communication.m_BoardGameData.iType==NETWORK_BOARD_MESSAGE_NEW_GAME) // new game
				{
					g_pCheckersBoard->StopBackgroundMusic();
					g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_BOARD]);
					g_pCheckersBoard->NewGame();
					g_pGUIManager->ResetBoardGameMenus();
				}
				else*/
				if (g_Communication.m_BoardGameData.iType==0) // select/move/attack/combo
				{
					if (g_Communication.m_BoardGameData.iCoordX>=0 &&g_Communication.m_BoardGameData.iCoordY>=0)
					{
						// waiting for player to choose unit
							if (g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_UNIT)
							{
								g_pCheckersBoard->ChooseUnit(g_Communication.m_BoardGameData.iCoordX,g_Communication.m_BoardGameData.iCoordY);
							}
							else
							// waiting for player to choose target
							if ( (g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET) ||
								(g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET_COMBO))
							{
								g_pCheckersBoard->ChooseTarget(g_Communication.m_BoardGameData.iCoordX,g_Communication.m_BoardGameData.iCoordY);
							}
							else
							if (g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET_SPELL)
							{
								g_pCheckersBoard->ChooseSpellTarget(g_Communication.m_BoardGameData.iCoordX,g_Communication.m_BoardGameData.iCoordY);
							}
					}
				}
				else
				if (g_Communication.m_BoardGameData.iType==NETWORK_BOARD_MESSAGE_CAST_MAGIC)
				{
					g_pCheckersBoard->m_iBoardGameState=GAME_BOARD_STATE_CHOOSE_TARGET_SPELL;

					// handle the special case of shockwave 
					// when there is no need to choose destination of spell
					/*
					int iTempIndex = g_pCheckersBoard->BoardsSquaresUnits[g_pCheckersBoard->m_iChosenUnitX][g_pCheckersBoard->m_iChosenUnitY].m_iIndexToPlayersArray;
					if (g_pCheckersBoard->GetSpells()[g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType].m_iEffects==SPELL_EFFECTS_AUTO)
						g_pCheckersBoard->ChooseSpellTarget(g_pCheckersBoard->m_iChosenUnitX,g_pCheckersBoard->m_iChosenUnitY);				
					*/
				}

			
			}
			
		} //todo: maybe add keeplive
		//else  // handle chat messages 
		if (g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_NETWORK || g_pCheckersBoard->m_iPlayerTypeFUTURE==PLAYER_TYPE_NETWORK)
		{
			if (g_Communication.RecieveBoardMessage())
			{
				if (g_Communication.m_BoardGameData.iType!=NETWORK_BOARD_MESSAGE_NEXT_IS_CHAT && 
					g_Communication.m_BoardGameData.iType!=NETWORK_BOARD_MESSAGE_CHAT &&
					g_Communication.m_BoardGameData.iType!=NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_PAST   &&
					g_Communication.m_BoardGameData.iType!=NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_FUTURE)
				
					g_Communication.AddMessageToQueue(g_Communication.m_BoardGameData);
			}
		}
	}



	if(st_Input::Inst()->KeyPressed(DIK_ESCAPE))
	{
		//g_Renderer.DebugOutPutSortToFile(); // output some debug stuff
		
		//g_pWindow->TerminateApplication();						// Terminate The Program

		st_EngineI->AddGUIMessage(GUI_MESSAGE_ESCAPE);
	}

	// just a hacky fly around temp implementation


	float fSpeed = (st_Timer::Inst()->GetDeltaTimeFAKE()*0.3f);

	
	
	if(st_Input::Inst()->KeyPressed(DIK_S))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_S]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_S]=true;			
			//g_Communication.SendTest();
			//g_SoundManager.Disable();
			//g_TextMessages.AddMessage("Shwoooooong!");

			/*g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_NEXT_IS_CHAT,0, 0,"You are going down!!!");
			g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_CHAT,0,0,"You are going down!!!");							*/

		}		

	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_S]=false;


	if(st_Input::Inst()->KeyPressed(DIK_R))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_R]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_R]=true;			
			//g_Communication.RecieveBoardMessage();

			//g_TextMessages.AddMessage("Bazzzaaaam.");

			//g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_NEXT_IS_CHAT,0, 0,"That MUST have hirt!",true);
			//g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_CHAT,0,0,"That MUST have hirt!",true);	
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_R]=false;

	// CAMERA - ROTATE AROUND (0,10,0)

	/*if(st_Input::Inst()->KeyPressed(DIK_1))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_1]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_1]=true;

			if (g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_PAST)
				g_pCheckersBoard->m_iCurrentPlayerSide=ARMY_SIDE_FUTURE;
			else
				g_pCheckersBoard->m_iCurrentPlayerSide=ARMY_SIDE_PAST;
			
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_1]=false;*/

	
	if(st_Input::Inst()->KeyPressed(DIK_F2))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_F2]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_F2]=true;

			g_pCheckersBoard->SaveBoardGame();
			
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_F2]=false;


	if(st_Input::Inst()->KeyPressed(DIK_F3))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_F3]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_F3]=true;

			g_pCheckersBoard->LoadBoardGame();
			
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_F3]=false;

	
	
	if(st_Input::Inst()->KeyPressed(DIK_F4))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_F4]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_F4]=true;

			g_pCheckersBoard->m_iCurrentSaveGame++;

			if (g_pCheckersBoard->m_iCurrentSaveGame>5)
				g_pCheckersBoard->m_iCurrentSaveGame=0;		
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_F4]=false;




	/*if(st_Input::Inst()->KeyPressed(DIK_5))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_5]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_5]=true;

			//g_pCheckersBoard->m_iCurrentSaveGame++;

			if (g_pCheckersBoard->m_iChosenUnitX>=0 && g_pCheckersBoard->m_iChosenUnitY>=0)
			{
				int iTempIndex = g_pCheckersBoard->BoardsSquaresUnits[g_pCheckersBoard->m_iChosenUnitX][g_pCheckersBoard->m_iChosenUnitY].m_iIndexToPlayersArray;
				if (iTempIndex>=0)
				{
					
					g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iExperiencePoints++;
					g_pCheckersBoard->BoardsSquaresUnits[g_pCheckersBoard->m_iChosenUnitX][g_pCheckersBoard->m_iChosenUnitY].m_iExperiencePointsBACKUP=g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iExperiencePoints;
					g_pCheckersBoard->m_iSelectedUnitExperiencePoints=g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iExperiencePoints;
				}

			}
				
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_5]=false;*/


if (g_pCheckersBoard->m_iBoardGameState!=GAME_BOARD_STATE_MENU)
{

	

/*
	if(st_Input::Inst()->KeyPressed(DIK_W))
	{
		if (DISTANCE(CVector3f(0.f,10.f,0.f),g_Camera1.m_Pos)>140.f)
			g_Camera1.m_Pos= g_Camera1.m_Pos + (g_Camera1.RotateWithCameraRotationMatrix(CVector3f(0.f,0.f,-1.f)) * fSpeed);
	}

	if(st_Input::Inst()->KeyPressed(DIK_S))
	{
		g_Camera1.m_Pos= g_Camera1.m_Pos + (g_Camera1.RotateWithCameraRotationMatrix(CVector3f(0.f,0.f,1.f)) * fSpeed);	
	}

	if(st_Input::Inst()->KeyPressed(DIK_A))
	{
		g_Camera1.m_Pos = g_Camera1.m_Pos + (g_Camera1.RotateWithCameraRotationMatrix(CVector3f(-1.f,0.f,0.f)) * fSpeed);	 
	}

	if(st_Input::Inst()->KeyPressed(DIK_D))
	{
		g_Camera1.m_Pos = g_Camera1.m_Pos + (g_Camera1.RotateWithCameraRotationMatrix(CVector3f(1.f,0.f,0.f)) * fSpeed);	 
	}

	if(st_Input::Inst()->KeyPressed(DIK_ADD))
	{
		if (g_Camera1.m_Pos.v[1]<400.f &&
			(DISTANCE(CVector3f(g_Camera1.m_Pos.v[0],10.f,g_Camera1.m_Pos.v[2]),CVector3f(0.f,10.f,0.f))>50.f))
				g_Camera1.m_Pos = g_Camera1.m_Pos + (g_Camera1.RotateWithCameraRotationMatrix(CVector3f(0.f,1.f,0.f)) * fSpeed);	 
	}

	if(st_Input::Inst()->KeyPressed(DIK_SUBTRACT))
	{
		g_Camera1.m_Pos = g_Camera1.m_Pos + (g_Camera1.RotateWithCameraRotationMatrix(CVector3f(0.f,-1.f,0.f)) * fSpeed);	 
	}


	if (g_Camera1.m_Pos.v[1]<25.f)
		g_Camera1.m_Pos.v[1]=25.f;*/

	float fCameraMove = st_Timer::Inst()->GetDeltaTimeFAKE()*0.05f;

	if (!g_Camera1.m_bIgnoreThisFrame)
	{

		//if (g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_PAST)
		if (g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_HUMAN)
		{
			g_Camera1.m_Pos += (CVector3f(0.f,300.f,-306.f)-g_Camera1.m_Pos)* st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;
		}
		else
		{
			g_Camera1.m_Pos += (CVector3f(0.f,300.f,306.f)-g_Camera1.m_Pos)* st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;		
		}

		g_Camera1.m_bIgnoreThisFrame=false;		
	}
	else
		g_Camera1.m_bIgnoreThisFrame=false;		
}
else
{
	//g_Camera1.m_Pos = g_Camera1.m_Pos + (g_Camera1.RotateWithCameraRotationMatrix(CVector3f(1.f,0.f,0.f)) * st_Timer::Inst()->GetDeltaTimeFAKE()*0.08f);
	g_Camera1.m_Pos = g_Camera1.m_Pos + (g_Camera1.RotateWithCameraRotationMatrix(CVector3f(1.f,0.f,0.f)) * st_Timer::Inst()->GetDeltaTimeFAKE()*0.01f);


}


	


	g_Camera1.LookAt(CVector3f(0.f,10.f,0.f));



	
	if(st_Input::Inst()->KeyPressed(DIK_F5))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_F5]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_F5]=true;
			g_bDrawText = !g_bDrawText;
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_F5]=false;

	/*
	// rotating the checkers board

	if(st_Input::Inst()->KeyPressed(DIK_G))
	{
		g_pCheckersBoard->fRotateXAng+= 0.1f * st_Timer::Inst()->GetDeltaTimeFAKE();
	}

	if(st_Input::Inst()->KeyPressed(DIK_H))
	{
		g_pCheckersBoard->fRotateXAng-= 0.1f * st_Timer::Inst()->GetDeltaTimeFAKE();
	}

	if(st_Input::Inst()->KeyPressed(DIK_T))
	{
		g_pCheckersBoard->fRotateYAng+= 0.1f * st_Timer::Inst()->GetDeltaTimeFAKE();
	}

	if(st_Input::Inst()->KeyPressed(DIK_Y))
	{
		g_pCheckersBoard->fRotateYAng-= 0.1f * st_Timer::Inst()->GetDeltaTimeFAKE();
	}
	*/

	/*if(st_Input::Inst()->KeyPressed(DIK_Y))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_Y]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_Y]=true;
			
			CEntity** pPlayersEntities = g_pCheckersBoard->m_ppGameUnits;

	
			((CEntityPlayer*)pPlayersEntities[3])->m_pPlayerClient->Die();
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_Y]=false;*/

	
	
	
} 
else if (st_Engine::Inst()->m_iGameState==1) // fps mode
{


	
	if(st_Input::Inst()->KeyPressed(DIK_ESCAPE))
	{
		//g_pWindow->TerminateApplication();						// Terminate The Program

		g_pQuake3Map->KillSelf();

		if (g_Communication.GetAlreadyInited())
			g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_OPPONENT_KILLED_SELF,0,0,NULL);
			
	}

	g_bCollidedWithStaticEntity=false;
	
	// this isn't good - it inits everything
//if(st_Input::Inst()->KeyPressed(DIK_F1))
//	{
//		g_pWindow->ToggleFullscreen();							// Toggle Fullscreen Mode
//	}




	/////////////////////////////////////////////////////
	// Debug Keys
CGUIWindow * pMenu    = g_pGUIManager->m_pChatMenu;
CGUIEditBox* pEditBox = (CGUIEditBox*) pMenu->GetElementNum(0);

// if there is at least one human and player isn't chatting
if ( (g_pCheckersBoard->m_iPlayerTypePAST != PLAYER_TYPE_PC || g_pCheckersBoard->m_iPlayerTypeFUTURE != PLAYER_TYPE_PC) 
	&& (!pEditBox->m_bWriteMode))
{

/*	if(st_Input::Inst()->KeyPressed(DIK_1))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_1]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_1]=true;
			//g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->SetWeaponType(ATTACK_SHORT_RANGE);
			g_pCurrentlyProcessedPlayerClient->SetPlayerWeapon(0);
			//g_pCurrentlyProcessedPlayerClient->m_dwCurrentlyHeldWeapon = PLAYER_HOLDING_NO_WEAPON;
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_1]=false;*/



	if(st_Input::Inst()->KeyPressed(DIK_2))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_2]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_2]=true;
			//g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->SetWeaponType(ATTACK_LONG_RANGE);

			g_pCurrentlyProcessedPlayerClient->SetPlayerWeapon(1);
			//g_pCurrentlyProcessedPlayerClient->m_dwCurrentlyHeldWeapon = PLAYER_HOLDING_WEAPON_SHOTGUN;
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_2]=false;


	if(st_Input::Inst()->KeyPressed(DIK_3))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_3]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_3]=true;
			//g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->SetWeaponType(ATTACK_LONG_RANGE);
			g_pCurrentlyProcessedPlayerClient->SetPlayerWeapon(2);
			//g_pCurrentlyProcessedPlayerClient->m_dwCurrentlyHeldWeapon = PLAYER_HOLDING_WEAPON_ROCKET_LAUNCHER;
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_3]=false;


	
	if(st_Input::Inst()->KeyPressed(DIK_4))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_4]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_4]=true;
			//g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->SetWeaponType(ATTACK_LONG_RANGE);
			g_pCurrentlyProcessedPlayerClient->SetPlayerWeapon(3);
			//g_pCurrentlyProcessedPlayerClient->m_dwCurrentlyHeldWeapon = PLAYER_HOLDING_WEAPON_PLASMA;
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_4]=false;





	if(st_Input::Inst()->KeyPressed(DIK_T))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_T]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_T]=true;
			g_bDrawText = !g_bDrawText;
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_T]=false;

	if(st_Input::Inst()->KeyPressed(DIK_C))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_C]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_C]=true;
			g_bTestCollisionDetection = !g_bTestCollisionDetection;
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_C]=false;


	// advance player start position

	if(st_Input::Inst()->KeyPressed(DIK_V))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_V]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_V]=true;
			{
				g_pCurrentlyProcessedPlayerClient->AdvanceStartPosition();
				g_pCurrentlyProcessedPlayerClient->PutPlayerInStartPos();
			}			
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_V]=false;

	// advance enemy start position

	if(st_Input::Inst()->KeyPressed(DIK_H))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_H]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_H]=true;
			{
				if (g_pCurrentlyProcessedPlayerClient==g_pPlayerClient1)
				{
					g_pPlayerClient2->AdvanceStartPosition();
					g_pPlayerClient2->PutPlayerInStartPos();
				}
				else
				{
					g_pPlayerClient1->AdvanceStartPosition();
					g_pPlayerClient1->PutPlayerInStartPos();

				}
			}			
		}		
	}
	else
		st_Input::Inst()->m_WPKeyBuffer[DIK_V]=false;

	
	

	if(st_Input::Inst()->KeyPressed(DIK_Z))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_Z]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_Z]=true;
			
			if (g_pCurrentlyProcessedPlayerClient == g_pPlayerClient1)
			{
				g_pCurrentlyProcessedPlayerClient = g_pPlayerClient2;
				g_pHumanPlayerClient = g_pPlayerClient2;
				g_pCurrentlyProcessedBot          = g_pCurrentlyProcessedBot2;
			}
			else
			{
				g_pCurrentlyProcessedPlayerClient = g_pPlayerClient1;
				g_pHumanPlayerClient = g_pPlayerClient1;
				g_pCurrentlyProcessedBot          = g_pCurrentlyProcessedBot1;
			}
		}
	}
	else
	{
		st_Input::Inst()->m_WPKeyBuffer[DIK_Z]=false;
	}




	/*// only _see_ what bot is doing
	if(st_Input::Inst()->KeyPressed(DIK_G))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_G]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_G]=true;
			
			g_bLookOnOtherPlayer = !g_bLookOnOtherPlayer;
		}
	}
	else
	{
		st_Input::Inst()->m_WPKeyBuffer[DIK_G]=false;
	}*/



	/*// add life to the current player
	if(st_Input::Inst()->KeyPressed(DIK_J))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_J]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_J]=true;
			
			g_pHumanPlayerClient->m_iLife = 100.f;
		}
	}
	else
	{
		st_Input::Inst()->m_WPKeyBuffer[DIK_J]=false;
	}*/



	/*if(st_Input::Inst()->KeyPressed(DIK_X))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_X]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_X]=true;
			
			st_Timer::Inst()->SetFakeSlowMotion(3.f);
		}
	}
	else
	{
		st_Input::Inst()->m_WPKeyBuffer[DIK_X]=false;
	}*/



	

	

	/*if (st_Input::Inst()->KeyPressed(DIK_L))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_L]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_L]=true;

			if (g_fPlayerMoveStrength==1.f)
				g_fPlayerMoveStrength=3.f;
			else
				g_fPlayerMoveStrength=1.f;

		}
	}
	else
	{
		st_Input::Inst()->m_WPKeyBuffer[DIK_L]=false;
	}*/

	/*// Load a different map

	if(st_Input::Inst()->KeyPressed(DIK_M))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_M]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_M]=true;

			char* pTempFileName;
			pTempFileName = NULL;

			ShowWindow (g_pWindow->hWnd, SW_SHOWMINIMIZED);			
			ShowCursor(true);  
,
			
			if (!g_FileManager.UserInputFileDialog(".bsp","Quake 3 Arena BSP files\0*.bsp\0",pTempFileName))
			{
				//ShowCursor(false);  
				ShowCursor(true);
				ShowWindow (g_pWindow->hWnd, SW_NORMAL);								// Make The Window Visible				
				
				SafeDelete(pTempFileName);
			}
			else
			{

				//ShowCursor(false);  
				ShowCursor(true);  
				ShowWindow (g_pWindow->hWnd, SW_NORMAL);								// Make The Window Visible				
				
				SafeDelete(g_pQuake3Map);
				g_pSky->FreeSideTextures();
				g_pQuake3Map = new CQuake3BSP;

				g_pQuake3Map->LoadBSP(pTempFileName);
				g_pQuake3Map->SetMapFileName(pTempFileName);
				st_Renderer::Inst()->InitSkyVBs();		
				SafeDelete(pTempFileName);

				g_pQuake3Map->FillDynamicEntities(NULL,NULL);

				g_pPlayerClient1->InitStates();
				g_pPlayerClient2->InitStates();

				g_pPlayerClient1->PutPlayerInStartPos();
				
				g_pPlayerClient2->AdvanceStartPosition();
				g_pPlayerClient2->PutPlayerInStartPos();


			}

										// Make The Window Visible
			
		}
	}
	else
	{
		st_Input::Inst()->m_WPKeyBuffer[DIK_M]=false;
	}
*/

	// change general tesselation level

	if(st_Input::Inst()->KeyPressed(DIK_Y))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_Y]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_Y]=true;
	
			if (g_iUserDefinedBezierPatchTessLevel<14)
				g_iUserDefinedBezierPatchTessLevel++;
		}

	
	}
	else
	{
		st_Input::Inst()->m_WPKeyBuffer[DIK_Y]=false;
	}


	// change general tesselation level

	if(st_Input::Inst()->KeyPressed(DIK_U))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_U]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_U]=true;

			if (g_iUserDefinedBezierPatchTessLevel>3)
				g_iUserDefinedBezierPatchTessLevel--;
		}

	}
	else
	{
		st_Input::Inst()->m_WPKeyBuffer[DIK_U]=false;
	}

	/*if(st_Input::Inst()->KeyPressed(DIK_P))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_P]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_P]=true;
			g_bShowPortals=!g_bShowPortals;
		}
	}
	else
	{
		st_Input::Inst()->m_WPKeyBuffer[DIK_P]=false;
	}

	if(st_Input::Inst()->KeyPressed(DIK_F))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_F]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_F]=true;
			g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_bPlayerFlying=!g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_bPlayerFlying;
		}
	}
	else
	{
		st_Input::Inst()->m_WPKeyBuffer[DIK_F]=false;
	}*/


	


	
	if(st_Input::Inst()->KeyPressed(DIK_SPACE))
	{
		if (st_Input::Inst()->m_WPKeyBuffer[DIK_SPACE]==false)
		{
			st_Input::Inst()->m_WPKeyBuffer[DIK_SPACE]=true;

			g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->SetStartJumping(false);
			
			if (!g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->getPlayerInAir() || g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->getDistanceToFloor()<2.f)
			{

				g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->SetJumping(true);
				g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->SetStartJumping(true);
			}
		}

	}
	else
	{
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->SetStartJumping(false);
		st_Input::Inst()->m_WPKeyBuffer[DIK_SPACE]=false;

	}




	

	if(st_Input::Inst()->KeyPressed(DIK_W))
	{
		if (g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags& PLAYER_MOVE_Z_BACKWARDS)
			g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_Z_BACKWARDS;
		else
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags|= PLAYER_MOVE_Z_FORWARD;	
	}

	

	if(st_Input::Inst()->KeyPressed(DIK_S))
	{
		if (g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags& PLAYER_MOVE_Z_FORWARD)
			g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_Z_FORWARD;
		else
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags|= PLAYER_MOVE_Z_BACKWARDS;
	}


	//} // end of "if (st_TimerI->GetTimeSinceLastGameLogicHappened() > GAME_LOGIC_MILISECONDS_INTERVAL)"

	if(st_Input::Inst()->KeyPressed(DIK_A))
	{	
		if (g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags& PLAYER_MOVE_X_RIGHT)
			g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_X_RIGHT;
		else
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags|= PLAYER_MOVE_X_LEFT;
	}


	if(st_Input::Inst()->KeyPressed(DIK_D))
	{
		if (g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags& PLAYER_MOVE_X_LEFT)
			g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_X_LEFT;
		else
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags|= PLAYER_MOVE_X_RIGHT;
	}

	/*if(st_Input::Inst()->KeyPressed(DIK_ADD))
	{
		if (g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags& PLAYER_MOVE_Y_DOWN)
			g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_Y_DOWN;
		else
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags|= PLAYER_MOVE_Y_UP;
	}

	if(st_Input::Inst()->KeyPressed(DIK_SUBTRACT))
	{
		if (g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags& PLAYER_MOVE_Y_UP)
			g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags&= ~PLAYER_MOVE_Y_UP;
		else
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags|= PLAYER_MOVE_Y_DOWN;
	}*/


	


	if(st_Input::Inst()->KeyPressed(DIK_UP))
	{
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags|= PLAYER_MOVE_Z_FORWARD;//PLAYER_ROT_X_UP; 
	}

	if(st_Input::Inst()->KeyPressed(DIK_DOWN))
	{
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags|= PLAYER_MOVE_Z_BACKWARDS;//PLAYER_ROT_X_DOWN;
	}


	if(st_Input::Inst()->KeyPressed(DIK_LEFT))
	{
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags|= PLAYER_ROT_Y_LEFT;
	}


	if(st_Input::Inst()->KeyPressed(DIK_RIGHT))
	{
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags|= PLAYER_ROT_Y_RIGHT;
	}
}


}	// END OF "if (st_Engine::Inst()->m_iGameState==1) // fps mode"

	//////////////////////////
	// Find mouse movement
	float fTempMouseX,fTempMouseY,fMax=43.f;

	float fTempMouseXNR,fTempMouseYNR; // (NR == Non Restricted)

	fTempMouseX = st_Input::Inst()->GetRelativeX(false);
	fTempMouseY = st_Input::Inst()->GetRelativeY(false);

	fTempMouseXNR = st_Input::Inst()->GetAbsoluteX();
	fTempMouseYNR = st_Input::Inst()->GetAbsoluteY();


	if (fTempMouseX>fMax)
		fTempMouseX=fMax;
	if (fTempMouseX<-fMax)
		fTempMouseX=-fMax;

	if (fTempMouseY>fMax)
		fTempMouseY=fMax;
	if (fTempMouseY<-fMax)
		fTempMouseY=-fMax;
	///////////////////////////	


	CMatrix m1;
	CMatrix m2;


if (st_Engine::Inst()->m_iGameState==1) // fps mode
{

// if there is at least one human
if (g_pCheckersBoard->m_iPlayerTypePAST != PLAYER_TYPE_PC || g_pCheckersBoard->m_iPlayerTypeFUTURE != PLAYER_TYPE_PC)
{

    
	// activate mouse movement on current player
	//g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngX -= fTempMouseY/8.f;
	//g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngY -= fTempMouseX/8.f;

	// original original original original
	/*g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngX -= fTempMouseY/16.f;
	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngY -= fTempMouseX/16.f;*/

	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngX -= fTempMouseY*TIMER_DELTA_TIME*0.01f;
	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngY -= fTempMouseX*TIMER_DELTA_TIME*0.01f;
	

	//m_fAngXBACKUP,m_fAngYBACKUP

	CVector3f PlayerMove(0.f,0.f,0.f);

if (g_pCurrentlyProcessedPlayerClient->m_iLife>0)
{

	/***********/
	//assert(_CrtCheckMemory());


	// ading movement force


	CVector3f vDesiredPosition;

	if (g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_bPlayerFlying)
		vDesiredPosition = g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetDesiredPosition(PLAYER_MODE_FLY);
	else
		vDesiredPosition = g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetDesiredPosition(PLAYER_MODE_WALK);

	CVector3f vDesiredMoveDirection = (vDesiredPosition-g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetPosition()).Normalize();
	

	if (!g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_bPlayerFlying)
		vDesiredMoveDirection.v[1] = 0.f;
	

	if (!g_pCurrentlyProcessedPlayerClient->m_bDying && !g_pCurrentlyProcessedPlayerClient->m_bStartDie)
	if ((g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags & PLAYER_MOVE_X_LEFT) ||
		(g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags & PLAYER_MOVE_X_RIGHT) ||
		(g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags & PLAYER_MOVE_Y_UP) ||
		(g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags & PLAYER_MOVE_Y_DOWN) ||
		(g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags & PLAYER_MOVE_Z_FORWARD) ||
		(g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags & PLAYER_MOVE_Z_BACKWARDS))
	{		
		PlayerMove = vDesiredMoveDirection;
	}
	
	//MoveForce+= vJumpForce; // adding the jump force		


} // end of "if (g_pCurrentlyProcessedPlayerClient->m_iLife>0)"
/*else
	PlayerMove.Set(0.f,0.f,0.f);*/




	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// perhaps HERE i should loop on all the dynamic entities
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!




	g_pCurrentlyProcessedPlayerClient->SetDesiredMove(PlayerMove);

}

	//assert(_CrtCheckMemory());


	if (!g_pCurrentlyProcessedPlayerClient->m_bDying && !g_pCurrentlyProcessedPlayerClient->m_bStartDie)
	{
		g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->m_fAngY = g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngY;
		g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->m_fAngX = g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngX;
	}


	CPlayerClient *pPlayerClientTEMP;




	g_vCameraFrom.Set(0.f,50.f,0.f);
	g_vCameraTo.Set(-100.f,90.f,0); // original		
	//CVector3f vCameraTo(-300.f,280.f,0);
		
	

	/**/ // handle "look on other player"
	pPlayerClientTEMP = g_pCurrentlyProcessedPlayerClient;

	if (g_bLookOnOtherPlayer)
	{
		if (g_pCurrentlyProcessedPlayerClient==g_pPlayerClient1)
			g_pCurrentlyProcessedPlayerClient = g_pPlayerClient2;
		else
			g_pCurrentlyProcessedPlayerClient = g_pPlayerClient1;
	}
	/**/

	m1.MatMakeRot(0.f,DEG2RAD(g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngY+90.f),0.f);

		

	g_vCameraFrom = m1* g_vCameraFrom;
	g_vCameraTo = m1* g_vCameraTo;


	


	g_CameraLookTraceInput.dwTraceType = TRACE_TYPE_SPHERE;
	g_CameraLookTraceInput.dwTraceCausedBy = TRACE_CAUSED_BY_TEST;
	g_CameraLookTraceInput.fRadius = 10.f;

	g_CameraLookTraceInput.vRayStart.Set(g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->m_vWorldPosition);
	g_CameraLookTraceInput.vRayEnd.Set(g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->m_vWorldPosition);
		

	g_CameraLookTraceInput.vRayStart+= g_vCameraFrom;
	g_CameraLookTraceInput.vRayEnd+= g_vCameraTo;

	g_pQuake3Map->Trace(&g_CameraLookTraceInput,&g_CameraLookTraceOutput,false,true,NULL);


		{

			// RESTORE RESTORE RESTORE 17 06 04

			/*float fTX = 0.0040f*st_Timer::Inst()->GetDeltaTimeFAKE();
			float fTY = 0.0180f*st_Timer::Inst()->GetDeltaTimeFAKE();
			float fTZ = 0.0040f*st_Timer::Inst()->GetDeltaTimeFAKE();*/

			float fTX = 0.0080f*st_Timer::Inst()->GetDeltaTimeFAKE();
			float fTY = 0.0100f*st_Timer::Inst()->GetDeltaTimeFAKE();
			float fTZ = 0.0080f*st_Timer::Inst()->GetDeltaTimeFAKE();

			if (fTX>1.f)
				fTX=1.f;

			if (fTY>1.f)
				fTY=1.f;

			if (fTZ>1.f)
				fTZ=1.f;

			if (DISTANCE(g_CameraLookTraceOutput.EndPoint,g_Camera1.m_Pos)>500.f) // if distance too big
			{
				// distance too big, ignore the "cinematic" way
				g_Camera1.m_Pos = g_CameraLookTraceOutput.EndPoint;
			}
			else
			{
				// do it in a "cinematic" way
				g_Camera1.m_Pos.v[0] = g_Camera1.m_Pos.v[0] + (g_CameraLookTraceOutput.EndPoint.v[0]- g_Camera1.m_Pos.v[0])*fTX; // original
				g_Camera1.m_Pos.v[1] = g_Camera1.m_Pos.v[1] + (g_CameraLookTraceOutput.EndPoint.v[1]- g_Camera1.m_Pos.v[1])*fTY; // original
				g_Camera1.m_Pos.v[2] = g_Camera1.m_Pos.v[2] + (g_CameraLookTraceOutput.EndPoint.v[2]- g_Camera1.m_Pos.v[2])*fTZ; // original
			}

			
		}			



	/*CVector3f vNewLookAt = GetVectorFromAngles(g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngX,0.f);

	vNewLookAt*=20.f;*/


	// restore restore restore
	//g_Camera1.LookAt(g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->m_vWorldPosition + CVector3f(0.f,50.f,0.f));


	/**/ // "pop" back the playerclient
	if (g_bLookOnOtherPlayer)
		g_pCurrentlyProcessedPlayerClient = pPlayerClientTEMP;
	/**/

	

	// i need to check that some X time has passed since the last time this was done
	/*if (st_Input::Inst()->MouseButtonDown(DIMOUSE_LEFTBUTTON))
		g_pCurrentlyProcessedPlayerClient->m_bStartAttack = true;*/

	// shoot


/*	if (st_TimerI->GetTimeSinceLastGameLogicHappened() >= GAME_LOGIC_MILISECONDS_INTERVAL)
	{*/

	if	(st_TimerI->GetTimeSinceLastGameLogicHappened() >= GAME_LOGIC_MILISECONDS_INTERVAL)
					g_pCurrentlyProcessedPlayerClient->bShot=false;

		if (!g_pCurrentlyProcessedPlayerClient->m_bDying && !g_pCurrentlyProcessedPlayerClient->m_bStartDie)
		{
	
			if (st_Input::Inst()->MouseButtonDown(DIMOUSE_LEFTBUTTON))
			{				
				g_pCurrentlyProcessedPlayerClient->Shoot(false,true);

				
			}

			/*if (st_Input::Inst()->MouseButtonDown(DIMOUSE_RIGHTBUTTON))
				g_pCurrentlyProcessedPlayerClient->m_bStartDie = true;*/
		}




	/**/ // handle "look on other player"
		pPlayerClientTEMP = g_pCurrentlyProcessedPlayerClient;
		if (g_bLookOnOtherPlayer)
		{
			if (g_pCurrentlyProcessedPlayerClient==g_pPlayerClient1)
				g_pCurrentlyProcessedPlayerClient = g_pPlayerClient2;
			else
				g_pCurrentlyProcessedPlayerClient = g_pPlayerClient1;
		}
		/**/


	/**/ // camera 	
	{
		g_RayTraceInput.dwTraceType = TRACE_TYPE_LINE;
		//g_RayTraceInput.dwTraceCausedBy = TRACE_CAUSED_BY_TEST;
		g_RayTraceInput.dwTraceCausedBy = TRACE_CAUSED_BY_SHOOT;
		g_RayTraceInput.fRadius = 0.f;
		
		g_RayTraceInput.vRayStart.Set(g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->m_vWeaponCenter);
		g_RayTraceInput.vRayEnd.Set(g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->m_vWeaponCenter);

		g_RayTraceInput.vRayEnd+= g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetLookDirection() * 3000.f;

		g_RayTraceInput.iAbsoluteEntityIndexCause = pPlayerClientTEMP->m_iAbsoluteEntityIndex;

		g_pQuake3Map->Trace(&g_RayTraceInput,&g_RayTraceOutput,false,true,NULL);
	}


	// set the camera look at point

	
	// restore 04 07 04
	//g_Camera1.m_vLookAt = g_Camera1.m_vLookAt + (g_RayTraceOutput.EndPoint-g_Camera1.m_vLookAt)*0.0040f*st_Timer::Inst()->GetDeltaTimeFAKE(); // original

	// restore 05 07 04

	/*if (DISTANCE(g_RayTraceOutput.EndPoint,g_Camera1.m_vLookAt)>600.f)
	{
		// simply rotate there instantly
		g_Camera1.m_vLookAt = g_RayTraceOutput.EndPoint;
	}
	else
	{
		// change angles in a "cinematic" way
		g_Camera1.m_vLookAt = g_Camera1.m_vLookAt + (g_RayTraceOutput.EndPoint-g_Camera1.m_vLookAt)*0.0080f*st_Timer::Inst()->GetDeltaTimeFAKE(); // original
	}*/

	//g_Camera1.m_vLookAt = g_Camera1.m_vLookAt + (g_RayTraceOutput.EndPoint-g_Camera1.m_vLookAt)*0.0080f*st_Timer::Inst()->GetDeltaTimeFAKE(); // original


	
	float fTX = 0.0080f*st_Timer::Inst()->GetDeltaTimeFAKE();
	//float fTY = 0.0120f*st_Timer::Inst()->GetDeltaTimeFAKE();
	float fTY = 0.0080f*st_Timer::Inst()->GetDeltaTimeFAKE();
	float fTZ = 0.0080f*st_Timer::Inst()->GetDeltaTimeFAKE();

	if (fTX>1.f)
		fTX=1.f;

	if (fTY>1.f)
		fTY=1.f;

	if (fTZ>1.f)
		fTZ=1.f;
	

	/*if (DISTANCE(g_Camera1.m_vLookAt,g_RayTraceOutput.EndPoint)>1000.f) // if distance too big
	{
		g_Camera1.m_vLookAt = g_RayTraceOutput.EndPoint;
	}
	else*/
	{
		g_Camera1.m_vLookAt.v[0] = g_Camera1.m_vLookAt.v[0] + (g_RayTraceOutput.EndPoint.v[0]-g_Camera1.m_vLookAt.v[0])*fTX;
		g_Camera1.m_vLookAt.v[1] = g_Camera1.m_vLookAt.v[1] + (g_RayTraceOutput.EndPoint.v[1]-g_Camera1.m_vLookAt.v[1])*fTY;
		g_Camera1.m_vLookAt.v[2] = g_Camera1.m_vLookAt.v[2] + (g_RayTraceOutput.EndPoint.v[2]-g_Camera1.m_vLookAt.v[2])*fTZ;
	}
	

	g_Camera1.LookAt(g_Camera1.m_vLookAt);

	
	
	
	/**/ // "pop" back the playerclient
	if (g_bLookOnOtherPlayer)
		g_pCurrentlyProcessedPlayerClient = pPlayerClientTEMP;
	/**/


	//assert(_CrtCheckMemory());
		


	
	//g_Camera1.LookAt(g_RayTraceOutput.EndPoint);

	// updating the aiming aid position


		

		m1.MatMakeMove(g_RayTraceOutput.EndPoint.v[0],g_RayTraceOutput.EndPoint.v[1],g_RayTraceOutput.EndPoint.v[2]);

		// i wanted to rotate it here, but i need a sphere that its center is in (0,0,0) ...
/*		g_fAimingAidAng+=0.06f*st_Timer::Inst()->GetDeltaTimeFAKE();
		m2.MatMakeRot(DEG2RAD(0.f),DEG2RAD(g_fAimingAidAng),0.f);
		m1 = m1 * m2;*/

		float pfOpenglMatrix[16];

		m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);

		g_pAimingAid->Update(true,false,&pfOpenglMatrix[0]);


		/**/ // aiming aid 2 - for checking the memory manager


		m1.MatMakeMove(g_RayTraceOutput.EndPoint.v[0],g_RayTraceOutput.EndPoint.v[1]+15.f,g_RayTraceOutput.EndPoint.v[2]);
		float pfOpenglMatrix2[16];
		m1.ConvertToOpenglMatrix(&pfOpenglMatrix2[0]);
		g_pAimingAid2->Update(true,false,&pfOpenglMatrix2[0]);

		/**/



	/// is this the place to handle this?

	/*g_pPlayerClient1->Update();
	g_pPlayerClient2->Update();*/

	// restore restore restore
	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->UpdateInterfaceWeaponPos();
	
	/*g_pPlayerClient1->UpdateCharacterAnimation();
	g_pPlayerClient2->UpdateCharacterAnimation();*/

	//assert(_CrtCheckMemory());


	if (st_Engine::Inst()->m_iGameState==1) // if we are STILL in the FPS (and aren`t in the process of going back to board)
	{
		g_pQuake3Map->UpdateAllStaticEntities();
		g_pQuake3Map->UpdateLinkDynamicEntities();


		//assert(_CrtCheckMemory());

		if (!st_Engine::Inst()->m_bSwitchingGameState)
			g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags=0;
		else
			st_Engine::Inst()->m_bSwitchingGameState=false;


		st_EngineI->HandleMessages();

		if (g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_NETWORK || g_pCheckersBoard->m_iPlayerTypeFUTURE==PLAYER_TYPE_NETWORK)
		{
			if (g_Communication.RecieveBoardMessage())
			{
				if (g_Communication.m_BoardGameData.iType!=NETWORK_BOARD_MESSAGE_NEXT_IS_CHAT && 
					g_Communication.m_BoardGameData.iType!=NETWORK_BOARD_MESSAGE_CHAT &&
					g_Communication.m_BoardGameData.iType!=NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_PAST   &&
					g_Communication.m_BoardGameData.iType!=NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_FUTURE &&
					g_Communication.m_BoardGameData.iType!=NETWORK_BOARD_MESSAGE_OPPONENT_KILLED_SELF &&
					g_Communication.m_BoardGameData.iType!=NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_WEAPON &&
					g_Communication.m_BoardGameData.iType!=NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_EXTRALIFE &&
					g_Communication.m_BoardGameData.iType!=NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_AMMO)
						g_Communication.AddMessageToQueue(g_Communication.m_BoardGameData);
				else
				{
					if (g_Communication.m_BoardGameData.iType==NETWORK_BOARD_MESSAGE_OPPONENT_KILLED_SELF)
					{
						g_pQuake3Map->KillOpponent();
					}
					else
					if (g_Communication.m_BoardGameData.iType==NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_WEAPON)
					{
						// nothing to do...
					}
					else
					if (g_Communication.m_BoardGameData.iType==NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_EXTRALIFE)
					{
                        g_pPlayerClient2->m_iLife+=50;
					}
					else
					if (g_Communication.m_BoardGameData.iType==NETWORK_BOARD_MESSAGE_OPPONENT_ADVANTAGE_AMMO)
					{
						// nothing to do...
					}
				}
			}
		}
	}
	


	

	//assert(_CrtCheckMemory());
	
	//st_Input::Inst()->SetCursorPosition((float)g_pWindow->width / 2,(float) g_pWindow->height / 2);	

}	// END OF "if (st_Engine::Inst()->m_iGameState==1) // fps mode"
else 
	if (st_Engine::Inst()->m_iGameState==0) // board mode
{

	///// mouse related

	/*g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngX -= fTempMouseY/8.f;
	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngY -= fTempMouseX/8.f;*/

	/**/

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glRotatef(-g_Camera1.xang,1.0f,0,0);
	glRotatef(-g_Camera1.yang,0,1.0f,0);
	glRotatef(-g_Camera1.zang,0,0,1.0f);
	glTranslatef(-g_Camera1.m_Pos.v[0],-g_Camera1.m_Pos.v[1],-g_Camera1.m_Pos.v[2]);

	/**/

	double WorldPosX, WorldPosY, WorldPosZ, MousePosX, MousePosY, MousePosZ;
	double ModelMatrix[16]; 
	glGetDoublev(GL_MODELVIEW_MATRIX, ModelMatrix);
	double ProjMatrix[16];
	glGetDoublev(GL_PROJECTION_MATRIX, ProjMatrix);

	// Get the current viewport
    int Viewport[4];
    glGetIntegerv(GL_VIEWPORT, Viewport);



	 // Set the end point of ray in windows coordinates
     MousePosX = st_Input::Inst()->GetAbsoluteX();
     MousePosY = Viewport[3] - st_Input::Inst()->GetAbsoluteY(); // invert mouse Y coordinate
     MousePosZ = 0.5; // Near clip plane depth value


	 g_fMouseX = MousePosX;
	 g_fMouseY = MousePosY;

     // Get unprojected end point
     gluUnProject
      (
            MousePosX, 
            MousePosY, 
            MousePosZ, 
            ModelMatrix, 
            ProjMatrix,
            Viewport,
            &WorldPosX,
            &WorldPosY,
            &WorldPosZ
        );


	CVector3f vWorldPos(WorldPosX,WorldPosY,WorldPosZ);

	

	/*g_vBoardRayStart = vWorldPos;
	g_vBoardRayEnd   = vWorldPos + ((vWorldPos-g_Camera1.m_Pos).Normalize()*1000.f);*/


	g_vBoardRayStart = g_Camera1.m_Pos;
	g_vBoardRayEnd = vWorldPos;




	CPlane BoardPlane;
	BoardPlane.d = 10.f;
	BoardPlane.n.Set(0.f,1.f,0.f);


	//RayPlaneIntersection(CVector3f& RayStart,CVector3f& RayEnd,CPlane &Plane)

	g_vBoardRayCollision = RayPlaneIntersection(g_vBoardRayStart,g_vBoardRayEnd,BoardPlane);

	if (g_vBoardRayCollision.v[0]>(-4.f*BOARD_SQUARE_SIZE) && g_vBoardRayCollision.v[0]<(4.f*BOARD_SQUARE_SIZE) &&
		g_vBoardRayCollision.v[2]>(-4.f*BOARD_SQUARE_SIZE) && g_vBoardRayCollision.v[2]<(4.f*BOARD_SQUARE_SIZE))
	{
		float fTempX = (g_vBoardRayCollision.v[0]+4*BOARD_SQUARE_SIZE) / BOARD_SQUARE_SIZE;
		float fTempY = (g_vBoardRayCollision.v[2]+4*BOARD_SQUARE_SIZE) / BOARD_SQUARE_SIZE;



		g_PointedSquareIndexX = (int) fTempX;
		g_PointedSquareIndexY = (int) fTempY;

		g_PointedSquareIndexX = (SQUARES_NUM_X-1)-g_PointedSquareIndexX;				

	}
	else
	{
		g_PointedSquareIndexX = -1;
		g_PointedSquareIndexY = -1;
	}

	//assert(_CrtCheckMemory());


	
	if (g_pCheckersBoard->m_iPlayerTypePAST!=PLAYER_TYPE_NETWORK && g_pCheckersBoard->m_iPlayerTypeFUTURE!=PLAYER_TYPE_NETWORK)
	if (g_pCheckersBoard->m_iPlayerTypeFUTURE==PLAYER_TYPE_PC || g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_PC)
	{
		DWORD dwAIside;
		
		// if computer ai turn
		if (g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_FUTURE &&
			g_pCheckersBoard->m_iPlayerTypeFUTURE==PLAYER_TYPE_PC)
				dwAIside = ARMY_SIDE_FUTURE;
		else
		if (g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_PAST &&
			g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_PC)
				dwAIside = ARMY_SIDE_PAST;
		else 
			dwAIside = 1000;

		if (dwAIside!=1000)
		{
			sMoveAI computerAIresult = g_pCheckersBoard->m_CheckersAI.FakeChooseBestMove(dwAIside);

		// choose unit
		if (g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_UNIT)
			{
			g_pCheckersBoard->ChooseUnit(computerAIresult.vFrom.x,computerAIresult.vFrom.y);
			}
		//else
		// choose target
		/*if ( (g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET) ||
			(g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET_COMBO))
			{
				g_pCheckersBoard->ChooseTarget(computerAIresult.vTo.x,computerAIresult.vTo.y);				
			}		*/

			if (g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET)
				g_pCheckersBoard->ChooseTarget(computerAIresult.vTo.x,computerAIresult.vTo.y);				
			

			if (g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET_COMBO)
			{
				int iTemp;
				if (!g_pCheckersBoard->CheckIfPlayerWon(iTemp))
					g_pCheckersBoard->TryAllSquaresAsTarget();
			}

		}

	}
		
	
	
		

	
		
	

	/*else
	{
		sMoveAI computerAIresult = g_pCheckersBoard->m_CheckersAI.FakeChooseBestMove(ARMY_SIDE_PAST);

		// waiting for player to choose unit
		if (g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_UNIT)
		{
			g_pCheckersBoard->ChooseUnit(computerAIresult.vFrom.x,computerAIresult.vFrom.y);
		}
		else
		// waiting for player to choose target
		if ( (g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET) ||
			(g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET_COMBO))
		{
			//MessageBox(0,"1","GAME_BOARD_STATE_CHOOSE_TARGET",MB_OK);
			g_pCheckersBoard->ChooseTarget(computerAIresult.vTo.x,computerAIresult.vTo.y);				
		}

	}*/


	

		
	g_pGUIManager->HandleGUIMessages();
	// engine messages must be last!! because the gui messages might invoke engine messages.
	st_EngineI->HandleMessages();
	
	
if (!bOnMenu)
{
	/*// debug right click
	if (g_PointedSquareIndexX>=0)
	if (st_Input::Inst()->MouseButtonDown(DIMOUSE_RIGHTBUTTON))
	{
		g_pCheckersBoard->BoardsSquaresUnits[g_PointedSquareIndexX][g_PointedSquareIndexY].m_iIndexToPlayersArray=-1;


	}*/
}

if	(st_TimerI->GetTimeSinceLastGameLogicHappened() >= GAME_LOGIC_MILISECONDS_INTERVAL)
{
	
// just a fake one...
//	st_Input::Inst()->MouseButtonDown(DIMOUSE_LEFTBUTTON);

	if (g_PointedSquareIndexX>=0)
	//if (st_Input::Inst()->MouseButtonDown(DIMOUSE_LEFTBUTTON))
	if (st_Input::Inst()->MouseButtonDownFirstFrame(DIMOUSE_LEFTBUTTON))
	{
		if (g_pCheckersBoard->m_iBoardGameState!=GAME_BOARD_STATE_MENU)
		{
			if (!bOnMenu &&g_pGUIManager->CanPlay())
				{

					int iIndex = g_pCheckersBoard->BoardsSquaresUnits[g_PointedSquareIndexX][g_PointedSquareIndexY].m_iIndexToPlayersArray;

					// if there is Human Player
					if (g_pCheckersBoard->m_iPlayerTypeFUTURE==PLAYER_TYPE_HUMAN || g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_HUMAN)
					if (
						(g_pCheckersBoard->m_iPlayerTypeFUTURE==PLAYER_TYPE_HUMAN && g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_FUTURE)
						||
						(g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_HUMAN && g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_PAST))

					{
						// waiting for player to choose unit
						if (g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_UNIT)
						{
							if (iIndex>=0)
								g_pCheckersBoard->ChooseUnit(g_PointedSquareIndexX,g_PointedSquareIndexY);
						}
						else
						// waiting for player to choose target
						if ( (g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET) ||
							(g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET_COMBO))
						{
							//MessageBox(0,"1","GAME_BOARD_STATE_CHOOSE_TARGET",MB_OK);
							g_pCheckersBoard->ChooseTarget(g_PointedSquareIndexX,g_PointedSquareIndexY);				
						}
						else
						if (g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET_SPELL)
						{
							// temp - here i should use the unit spell
							g_pCheckersBoard->ChooseSpellTarget(g_PointedSquareIndexX,g_PointedSquareIndexY);				
						}


						//assert(_CrtCheckMemory());
					}
				}
		}
		else
		{
			if (g_pGUIManager->IsDefaultMenusAndNotQuitMenu())
			if (!bOnMenu)
			if (!(g_pGUIManager->m_dwGUIMessages &GUI_MESSAGE_GET_TO_DEFAULT_MENUS))
			{				
				/*g_pCheckersBoard->Destroy();
				g_pCheckersBoard->Init(true);
				g_pCheckersBoard->m_iBoardGameState=GAME_BOARD_STATE_CHOOSE_UNIT;
				if (g_pCheckersBoard->m_bPlayingWinSong)
				{
					g_pCheckersBoard->StopBackgroundMusic();
					g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_INTRO]);
				}*/
				//g_TextMessages.AddMessage("Please click on New Game ",1.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);		

			}

		}			
	}

} // end of "if	(st_TimerI->GetTimeSinceLastGameLogicHappened() >= GAME_LOGIC_MILISECONDS_INTERVAL)"



	g_pGUIManager->ResetGUIMessages();

	
	/////

	// just a test model

	//m1.MatMakeMove(0.f,0.f,-100.f);
	//m1.MatMakeMove(g_vBoardRayStart.v[0],g_vBoardRayStart.v[1],g_vBoardRayStart.v[2]);
	m1.MatMakeMove(g_vBoardRayCollision.v[0],g_vBoardRayCollision.v[1],g_vBoardRayCollision.v[2]);
	
	float pfOpenglMatrix[16];
	m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);
	g_pAimingAid->Update(true,false,&pfOpenglMatrix[0]);
  
	// activate mouse movement on the camera angles
	/*g_Camera1.xang -= fTempMouseY/8.f;
	g_Camera1.yang -= fTempMouseX/8.f;*/



	//assert(_CrtCheckMemory());



	g_pCheckersBoard->UpdateGameUnits();

	//assert(_CrtCheckMemory());



	//g_pCheckersBoard->UpdateParticlesSystems();
	st_PSManagerI->Update();
	
	
}

//assert(_CrtCheckMemory());


	// RESTORE RESTORE
	/*st_Input::Inst()->CenterMouse();


	if (bFullScreen)
	{
		int middleX = (int) st_Input::Inst()->GetScreenWidth()  >> 1;
		int middleY = (int) st_Input::Inst()->GetScreenHeight()  >> 1;
		SetCursorPos(middleX, middleY);							
	}*/

	g_Camera1.UpdateMatrices();

	g_SoundManager.UpdateListenerPosition();

	g_SoundManager.Update();


st_EngineI->m_fEntireUpdateTimerD = st_TimerI->TimerGetTimeF() - st_EngineI->m_fEntireUpdateTimerS;
	
}


////////////////////////////////////////////////////
// Draw()
// draw the window
////////////////////////////////////////////////////
void GL_Window::Draw(void)   
{

	st_EngineI->m_fEntireDrawTimerS = st_TimerI->TimerGetTimeF();
	st_EngineI->m_fDrawPart1S = st_TimerI->TimerGetTimeF();

	//assert(_CrtCheckMemory());

	st_Renderer::Inst()->m_iTotalFrameCounter++;
	st_Engine::Inst()->m_iFrameCounter++;
	

	g_iNumberOfDynamicEntitiesDrawn = 0;

	st_Renderer::Inst()->Reset();


	

	// add lights //////////////////////////////////////////////////////////////////////

	CLight* pLight1 = st_RendererI->AddLight();

	pLight1->m_fRadius=100.f;
	//pLight1->m_fRadius=3.f;
	pLight1->m_vPos.Set(g_vBoardRayCollision+CVector3f(0.f,20.f,0.f));
	pLight1->m_vColor.Set(1.f,1.f,1.f);
	//pLight1->m_bFadeDie=false;	

	

	//////////////////
	
	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			if (g_pCheckersBoard->BoardsSquaresAccesablity[i][j] == ACCESABLE_SQUARE)
				if ( (g_pCheckersBoard->BoardsSquaresUnits[i][j].m_dwState == UNIT_SELECTED) ||
					(g_pCheckersBoard->BoardsSquaresUnits[i][j].m_dwState == UNIT_TARGET))
				{
					CLight* pLight2 = st_RendererI->AddLight();

					pLight2->m_fRadius=3.f;
					pLight2->m_vPos.Set(g_pCheckersBoard->GetMiddleOfSquare(i,j)+CVector3f(0.f,20.f,0.f));
					if (g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_PAST)
						pLight2->m_vColor.Set(0.f,1.f,1.f);
					else
						pLight2->m_vColor.Set(0.f,0.f,1.f);
					

				}
	
		}
		

		g_pCheckersBoard->ActivateBurningUnitsLights();

	/*CVector3f v1 = g_vBoardRayCollision+CVector3f(0.f,12.f,0.f);*/

	////////////////////////////////////////////////////////////////////////////////////
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	//glClear(GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer

	st_Engine::Inst()->ResetDebugCounters();

		
	glDepthFunc (GL_LEQUAL);									// The Type Of Depth Testing (Less Or Equal)	
	glEnable (GL_DEPTH_TEST);									// Enable Depth Testing


	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glRotatef(-g_Camera1.xang,1.0f,0,0);
	glRotatef(-g_Camera1.yang,0,1.0f,0);
	glRotatef(-g_Camera1.zang,0,0,1.0f);
	glTranslatef(-g_Camera1.m_Pos.v[0],-g_Camera1.m_Pos.v[1],-g_Camera1.m_Pos.v[2]);

	//g_OpenglCameraFrustum.FillFromOpenGLFrustum();

	g_OpenglCameraFrustum.CalculateOpenGLFrustum();


//assert(_CrtCheckMemory());

if (st_Engine::Inst()->m_iGameState==1) // we are inside the FPS game
{

	/*if (!g_Communication.RecievedFirstUDPMessage())
	{
		return;
	}*/

	//	assert(_CrtCheckMemory());


	//////////////////////////////
	// CGSHADERS
	//g_cgslShader.DisableProfiles();
	//////////////////////////////

	
	
	//assert(_CrtCheckMemory());
	
	// RESTORE RESTORE RESTORE
	g_pQuake3Map->RenderLevel(g_Camera1.m_Pos);
	

	//assert(_CrtCheckMemory());

	//g_pQuake3Map->RenderMapModels();


	//////////////////////////////////////////////////////////////////////////////
	// Draw all the VertexBuffers on the renderer queue pending for being drawn

	/*//////////////////////////////
	// CGSHADERS
	//g_cgslShader.PerFrameSetupBUMPMAP();	
	//////////////////////////////////////////////////////////////////////////////*/
	
	
	//g_pWeaponModel->PushToRenderer();


	if (!g_pCurrentlyProcessedPlayerClient->m_bDying && !g_pCurrentlyProcessedPlayerClient->m_bStartDie)
	{
		g_pAimingAid->PushToRenderer(false);
		//g_pAimingAid2->PushToRenderer();
	}


	//assert(_CrtCheckMemory());

	// restore restore restore
	

	/*
	g_pPlayerClient1->m_pPlayerModel->DrawPlayer();
	g_pPlayerClient2->m_pPlayerModel->DrawPlayer();
	*/
	

	
	/*/////////////////////////////////
	// CGSHADERS
	//g_cgslShader.DisableProfiles();
	////////////////////////////////*/
	

	//MessageBox(g_pWindow->hWnd,"Got to here 1", "Error", MB_OK);
	
	
	
	/*// really render to screen everything that was pushed to the renderer*/
	st_Renderer::Inst()->RenderAll(NULL,true); 

	//MessageBox(g_pWindow->hWnd,"Got to here 2", "Error", MB_OK);
} // END OF "if (st_Engine::Inst()->m_iGameState==1) // we are inside the FPS game"
if (st_Engine::Inst()->m_iGameState==0) // we are inside the Board game
{

	//////////////////////////////
	// CGSHADERS
	if (st_RendererI->m_bPPLused)
		g_cgslShader.EnableProfiles();
	//////////////////////////////


	//////////////////////////////
	// CGSHADERS
	if (st_RendererI->m_bPPLused)
		g_cgslShader.PerFrameSetupBUMPMAP();	
	//g_cgslShader.PerFrameSetupBASIC();	
	//////////////////////////////////////////////////////////////////////////////

	// draw test model
	//g_pAimingAid->PushToRenderer();

	/*glMatrixMode(GL_MODELVIEW);

	glRotatef(-g_pCheckersBoard->fRotateXAng,1.0f,0,0);
	glRotatef(-g_pCheckersBoard->fRotateYAng,0,1.0f,0);

	g_pCheckersBoard->DrawGameUnits();*/

	
	
}




/*************/



/*************/















/***********/

	

/*****************************************/
///////// draw stuff like lines and text


	// eliminate any "left overs"
	for (int i=0; i<g_iCardTexUnits;i++)
	{
		glClientActiveTextureARB(GL_TEXTURE0_ARB+i); 
		glActiveTextureARB(GL_TEXTURE0_ARB+i);
		glDisable(GL_TEXTURE_2D);
		//			glDisable(GL_TEXTURE_CUBE_MAP);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	

	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();


	





	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE,GL_ZERO);
		//glBlendFunc(GL_ONE,GL_ONE);

//		if (g_bShowPortals)
/*		{
			glDepthRange(1.f,1.f);
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			//g_Doom3Map.DebugDrawPortals(g_pPortalTex);
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
		}*/
		
	}


	// DEBUG DEBUG DEBUG DEBUG
	//glDepthFunc(GL_ALWAYS);
	//glDisable(GL_DEPTH_TEST);
	

	///////////////////////////////
	// 


	glColor4f(0.f,0.f,1.f,1.f);


	// draw debug polygon
	//st_Renderer::Inst()->DebugDrawLineLoop(d_pTri,3);
	

	glEnable(GL_DEPTH_TEST);
	

	// draw debug camera line
	glColor4f(0.f,1.f,0.f,1.f);
	//st_Renderer::Inst()->DebugDrawLine(d_pCameraRayStart,d_pCameraRayCollision);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-g_Camera1.xang,1.0f,0,0);
	glRotatef(-g_Camera1.yang,0,1.0f,0);
	glRotatef(-g_Camera1.zang,0,0,1.0f);
	glTranslatef(-g_Camera1.m_Pos.v[0],-g_Camera1.m_Pos.v[1],-g_Camera1.m_Pos.v[2]);

//assert(_CrtCheckMemory());

if (st_Engine::Inst()->m_iGameState==1) // we are inside the FPS game
{


	////////////////////////////////////////////////
	// render the character life in the FPS mode
	////////////////////////////////////////////////

	 // RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE 
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();




	

	st_Renderer::Inst()->RenderVB(g_pvbCharacterFrame2,false,true); 

	glColor4f(1.f,1.f,1.f,1.f);

	st_Renderer::Inst()->DrawAnchored2DQuad(NULL,0,256,128,10,10);

	

	
	
	

	////////////////////////////////////////////////
	////////////////////////////////////////////////
	////////////////////////////////////////////////


	//assert(_CrtCheckMemory());

	// RESTORE RESTORE RESTORE
	if (!g_pCurrentlyProcessedPlayerClient->m_bDying && !g_pCurrentlyProcessedPlayerClient->m_bStartDie)
	{
		//st_Renderer::Inst()->DebugDrawLine(g_RayTraceInput.vRayStart,g_RayTraceOutput.EndPoint);
	}

	//assert(_CrtCheckMemory());

	glColor4f(0.f,0.f,1.f,1.f);
	// RESTORE RESTORE RESTORE
	//st_Renderer::Inst()->DebugDrawLine(g_RayTraceOutput.EndPoint,g_RayTraceOutput.EndPoint + (g_RayTraceOutput.CollisionPlane.n * 40.f));
	
	glEnable(GL_DEPTH_TEST);
	
	//g_pQuake3Map->DrawMapModelsBBoxes();
	//g_pQuake3Map->DrawMapLeafsBBoxes();

	/*

	////////////////////////////////////////////////////////////////////////
	// draw debug collision point
	CPlane pPolygonPlane;
	pPolygonPlane.FindPlane(d_pTri[0],d_pTri[1],d_pTri[2]);

	d_pCollisionPoint = RayPlaneIntersection(d_pCameraRayStart,d_pCameraRayEnd,pPolygonPlane);

	if (PointInPolygon(d_pCollisionPoint,d_pTri,3))
		glColor4f(0.f,1.f,0.f,1.f);
	else
		glColor4f(1.f,0.f,0.f,1.f);


	st_Renderer::Inst()->DebugDrawLine(d_pTri[0],d_pCollisionPoint);
	st_Renderer::Inst()->DebugDrawLine(d_pTri[1],d_pCollisionPoint);
	st_Renderer::Inst()->DebugDrawLine(d_pTri[2],d_pCollisionPoint);


	*/

	/*glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(-9397.1768,101.83075, 15426.614);
	gluSphere(st_Renderer::Inst()->m_pSphere, 8.f, 16, 32);				// Draw the sphere with a radius of 0.2
	glPopMatrix();*/



    for (int i=0; i<g_iCardTexUnits;i++)
	{
		glClientActiveTextureARB(GL_TEXTURE0_ARB+i); 
		glActiveTextureARB(GL_TEXTURE0_ARB+i);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}

	g_pGUIManager->Render();

	glDisable(GL_DEPTH_TEST);

	// eliminate any "left overs"
	for (int i=0; i<g_iCardTexUnits;i++)
	{
		glClientActiveTextureARB(GL_TEXTURE0_ARB+i); 
		glActiveTextureARB(GL_TEXTURE0_ARB+i);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}

	st_Renderer::Inst()->DrawText();

} // END OF	if (st_Engine::Inst()->m_iGameState==1) // we are inside the Board game
else if (st_Engine::Inst()->m_iGameState==0) // we are inside the Board game
{

	if (g_pCheckersBoard->m_bShakeBoard)
		g_pCheckersBoard->ComputeShakeVector();
	else
		g_pCheckersBoard->m_vShakeSum.Set(0.f,0.f,0.f);


	glMatrixMode(GL_MODELVIEW);

	glRotatef(-g_pCheckersBoard->fRotateXAng,1.0f,0,0);
	glRotatef(-g_pCheckersBoard->fRotateYAng,0,1.0f,0);

	if (st_RendererI->m_bPPLused)
		g_cgslShader.BindVPBUMPMAP();

	
	st_EngineI->m_fDrawPart11D = 0;
	st_EngineI->m_fDrawPart12D = 0;

	
	g_pCheckersBoard->DrawGameUnits();




	st_EngineI->m_fDrawPart1D = st_TimerI->TimerGetTimeF() - st_EngineI->m_fDrawPart1S;

	st_EngineI->m_fDrawPart2S = st_TimerI->TimerGetTimeF();

	float fSquareSize = BOARD_SQUARE_SIZE;


	/*for (int i=0;i<9;i++)
	{
		glColor4f(0.f,1.f,0.f,1.f);
		st_Renderer::Inst()->DebugDrawLine(CVector3f( (-4.f*fSquareSize)+((float)i)*fSquareSize,10.f,-(4.f*fSquareSize) ),CVector3f( (-4.f*fSquareSize) +((float)i)*fSquareSize,10.f,(4.f*fSquareSize)));


		glColor4f(0.f,1.f,0.f,1.f);
		st_Renderer::Inst()->DebugDrawLine(CVector3f( (-4.f*fSquareSize),10.f, (-4.f*fSquareSize)+((float)i)*fSquareSize),CVector3f(4.f*fSquareSize,10.f, (-4.f*fSquareSize) +((float)i)*fSquareSize));
	}*/



	/* debug test the getsquaremiddle func*/

	CVector3f vVec1;
	CVector3f vVec2;
	CVector3f vVec3;
	CVector3f vVec4;

	glClientActiveTextureARB(GL_TEXTURE0_ARB); 
	glActiveTextureARB(GL_TEXTURE0_ARB);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();



// player turn
/*if ( (g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_UNIT) || 
	(g_pCheckersBoard->m_iBoardGameState == GAME_BOARD_STATE_CHOOSE_TARGET)||)*/
{


	
	/****************************************************************/

	/****************************************************************/

	static CMatrix m1;
	static CMatrix m2;

	CVector3f v1;

	static float pfMatrix[16];




	// render all not accesable
	
	/*if (st_RendererI->m_bPPLused)
	{
		g_cgslShader.UpdateDiffuseColorBUMPMAP(1.f,1.f,1.f);
		g_cgslShader.UpdateLightColorBUMPMAP(0.9f,0.9f,0.9f);
	}*/

	

	/*if (g_pCheckersBoard->m_bShakeBoard)
		g_pCheckersBoard->ComputeShakeVector();
	else
		g_pCheckersBoard->m_vShakeSum.Set(0.f,0.f,0.f);*/
	

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
		{
			if (g_pCheckersBoard->BoardsSquaresAccesablity[i][j] != ACCESABLE_SQUARE)
			{
				v1 = g_pCheckersBoard->GetMiddleOfSquare(i,j)+g_pCheckersBoard->m_vShakeSum;
				m1.MatMakeMove(v1.v[0],v1.v[1],v1.v[2]);
				m1.ConvertToOpenglMatrix(&pfMatrix[0]);

				g_pvbBoardSquaresNotAccesable->FillSpecialWorldMatrix(pfMatrix);
				g_pvbBoardSquaresNotAccesable->m_bSpecialWorldMatrix = true;		



				st_Renderer::Inst()->Push(g_pvbBoardSquaresNotAccesable);	
				st_Renderer::Inst()->RenderAll(pfMatrix,false); 
			}
			else
			if (g_pCheckersBoard->BoardsSquaresAccesablity[i][j] == ACCESABLE_SQUARE)
				if ( (g_pCheckersBoard->BoardsSquaresUnits[i][j].m_dwState == UNIT_SELECTED) ||
					(g_pCheckersBoard->BoardsSquaresUnits[i][j].m_dwState == UNIT_TARGET))
						//if (g_pCheckersBoard->BoardsSquaresUnits[i][j].m_iIndexToPlayersArray<0)
				{		
					v1 = g_pCheckersBoard->GetMiddleOfSquare(i,j)+g_pCheckersBoard->m_vShakeSum;
					m1.MatMakeMove(v1.v[0],v1.v[1],v1.v[2]);
					m1.ConvertToOpenglMatrix(&pfMatrix[0]);

					g_pvbBoardSquaresSelected->FillSpecialWorldMatrix(pfMatrix);
					g_pvbBoardSquaresSelected->m_bSpecialWorldMatrix = true;		

					st_Renderer::Inst()->Push(g_pvbBoardSquaresSelected);	
					st_Renderer::Inst()->RenderAll(pfMatrix,false); 
				}
			else
				if (IsOdd(i+j))
				if (g_pCheckersBoard->BoardsSquaresUnits[i][j].m_dwState != UNIT_SELECTED)
					if (g_pCheckersBoard->BoardsSquaresUnits[i][j].m_dwState != UNIT_TARGET)
					{
							v1 = g_pCheckersBoard->GetMiddleOfSquare(i,j)+g_pCheckersBoard->m_vShakeSum;
							m1.MatMakeMove(v1.v[0],v1.v[1],v1.v[2]);
							m1.ConvertToOpenglMatrix(&pfMatrix[0]);

							g_pvbBoardSquaresAccesable->FillSpecialWorldMatrix(pfMatrix);
							g_pvbBoardSquaresAccesable->m_bSpecialWorldMatrix = true;		
							st_Renderer::Inst()->Push(g_pvbBoardSquaresAccesable);	
							st_Renderer::Inst()->RenderAll(pfMatrix,false); 
					}


		
		}







	st_EngineI->m_fDrawPart2D = st_TimerI->TimerGetTimeF() - st_EngineI->m_fDrawPart2S;

	 st_EngineI->m_fDrawPart3S = st_TimerI->TimerGetTimeF();





}



	//////////////////////////////
	// CGSHADERS
	if (st_RendererI->m_bPPLused)
		g_cgslShader.DisableProfiles();
	//////////////////////////////



//// render particle systems

		// eliminate any "left overs"
	for (int i=0; i<g_iCardTexUnits;i++)
	{
		glClientActiveTextureARB(GL_TEXTURE0_ARB+i); 
		glActiveTextureARB(GL_TEXTURE0_ARB+i);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}

	glClientActiveTextureARB(GL_TEXTURE0_ARB); 
	glActiveTextureARB(GL_TEXTURE0_ARB);

	glDepthMask(GL_FALSE);
	
	//g_pCheckersBoard->DrawParticleSystems();
	st_PSManagerI->Draw();

	glDepthMask(GL_TRUE);







/////////////////////////////

	
	


	

	// DEBUG DRAW THE MOUSE CLICK RAY

	/*glColor4f(1.f,0.f,0.f,1.f);
	st_Renderer::Inst()->DebugDrawLine(g_vBoardRayStart,g_vBoardRayEnd);*/

	/********************/ //// draw board character frame

 // RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE 

	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	


	/*st_Renderer::Inst()->RenderVB(g_pvbCharacterFrame,false,true); 
	glColor4f(1.f,1.f,1.f,1.f);
	st_Renderer::Inst()->DrawAnchored2DQuad(NULL,(ANCHOR_X_MAX|ANCHOR_Y_MAX),300,150,0,0);*/

	//g_pWindowMainMenuGUI->Render();
	
	for (int i=0; i<g_iCardTexUnits;i++)
	{
		glClientActiveTextureARB(GL_TEXTURE0_ARB+i); 
		glActiveTextureARB(GL_TEXTURE0_ARB+i);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}

	g_pGUIManager->Render();


	

	/********************/ 


	float fTempMouseXNR = st_Input::Inst()->GetAbsoluteX();
	float fTempMouseYNR = st_Input::Inst()->GetAbsoluteY();
	


	/*CVector2f vec1(0.f+fTempMouseXNR,0.f+fTempMouseYNR);
	CVector2f vec2(40.f+fTempMouseXNR,0.f+fTempMouseYNR);
	CVector2f vec3(40.f+fTempMouseXNR,40.f+fTempMouseYNR);
	CVector2f vec4(0.f+fTempMouseXNR,40.f+fTempMouseYNR);*/

	//float fCursorSize = ((float) st_Input::Inst()->GetScreenWidth())/10.f;

	//float fCursorSize = ((float) st_Input::Inst()->GetScreenWidth())/10.f;
	float fCursorSize = 64;

	CVector2f vec1(0.f+fTempMouseXNR,0.f+fTempMouseYNR);
	CVector2f vec2(fCursorSize+fTempMouseXNR,0.f+fTempMouseYNR);
	CVector2f vec3(fCursorSize+fTempMouseXNR,fCursorSize+fTempMouseYNR);
	CVector2f vec4(0.f+fTempMouseXNR,fCursorSize+fTempMouseYNR);

	
	glDisable(GL_DEPTH_TEST);

	// eliminate any "left overs"
	for (int i=0; i<g_iCardTexUnits;i++)
	{
		glClientActiveTextureARB(GL_TEXTURE0_ARB+i); 
		glActiveTextureARB(GL_TEXTURE0_ARB+i);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}

	st_Renderer::Inst()->DrawText();

	
	
	glClientActiveTextureARB(GL_TEXTURE0_ARB); 
	glActiveTextureARB(GL_TEXTURE0_ARB);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();





 // RESTORE RESTORE RESTORE RESTORE RESTORE RESTORE 


	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();

	// draw the cursor (LAST!!!!!)

	// ORIGINAL ORIGINAL ORIGINAL ORIGINAL ORIGINAL 
	//st_Renderer::Inst()->DebugDraw2DQuad(g_pCrossHairTex,vec1,vec2,vec3,vec4,true);

	st_Renderer::Inst()->RenderVB(g_pvbCursor,false,true); 

	//glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	//glDisableClientState(GL_COLOR_ARRAY);

	glColor4f(1.f,1.f,1.f,1.f);

	//st_Renderer::Inst()->DebugDraw2DQuad(NULL,vec1,vec2,vec3,vec4,false);
	st_Renderer::Inst()->DebugDraw2DQuad(NULL,fTempMouseXNR,fCursorSize+fTempMouseXNR,fTempMouseYNR,fCursorSize+fTempMouseYNR,false);

	

	/**/
	//

	
} // end of "if (st_Engine::Inst()->m_iGameState==0) // we are inside the Board game"



	//assert(_CrtCheckMemory());


	//gluDeleteQuadric(st_Renderer::Inst()->m_pSphere);							// Free the quadric object

	////////////////////////////////////////////////////////////////////////

	///////////////////////////////

	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();


	///////// DRAW TEXT TO SCREEN
	///////// LAST!!!!

	//st_Renderer::Inst()->DrawText();
	///////////////////////////////////////////////////////////////
	


	st_EngineI->m_fDrawPart3D = st_TimerI->TimerGetTimeF() - st_EngineI->m_fDrawPart3S;

	st_EngineI->m_fDrawPart4S = st_TimerI->TimerGetTimeF();


	///////// actually draw the picture to some back buffer	
	//glFlush ();											// Flush The GL Rendering Pipeline

	///////// wait for vsinc and swap buffers
	SwapBuffers (g_pWindow->hDC);					// Swap Buffers (Double Buffering)


	if (st_TimerI->GetTimeSinceLastGameLogicHappened() >= GAME_LOGIC_MILISECONDS_INTERVAL)
	{
		st_TimerI->ZeroTimeSinceLastGameLogicHappend();
	}

	if (st_TimerI->GetTimeSinceLastGameLogicHappened_UDP_SEND() >= GAME_LOGIC_MILISECONDS_INTERVAL_UDP_SEND)
	{
		st_TimerI->ZeroTimeSinceLastGameLogicHappend_UDP_SEND();
	}

	st_Timer::Inst()->Update(); // update the timer
	
	/*sprintf(g_strWindowTitle,"Crimson Engine - FPS:%f.2, ms:%f.2. Pos:%f.3,%f.3,%f.3",
		st_Timer::Inst()->m_fFPS,st_Timer::Inst()->m_fTimeForFrame,
		g_Camera1.m_Pos.v[0],g_Camera1.m_Pos.v[1],g_Camera1.m_Pos.v[2]);
	g_pWindow->title = g_strWindowTitle;

	SetWindowText(g_pWindow->hWnd,g_strWindowTitle);*/

	/*
	g_Fonts.glPrint("FPS:%f ",g_Timer.m_fFPS);
	g_Fonts.glPrint("ms:%f",g_Timer.m_fTimeForFrame);
	*/


	st_EngineI->m_fDrawPart4D = st_TimerI->TimerGetTimeF() - st_EngineI->m_fDrawPart4S;
	st_EngineI->m_fEntireDrawTimerD = st_TimerI->TimerGetTimeF() - st_EngineI->m_fEntireDrawTimerS;
}
