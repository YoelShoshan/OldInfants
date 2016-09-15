#include "Engine.h"
#include <Windows.h>
#include <gl/GL.h>
#include <gl/glu.h>
#include "stdio.h"
#include "glext.h"


#include "LogFile.h"
extern CLogFile g_LogFile;

#include "Singleton.h"

#include "NeHeGL.h"
extern GL_Window* g_pWindow; // Main Window

#include "Timer.h"

#include "Input.h"

#include "GLFonts.h"
extern CFonts      g_Fonts; // fonts global variable (draw 2d text to screen)

#include "Bezier.h"

extern int g_iUserDefinedBezierPatchTessLevel;

#include "EntitiesParser.h"

#include "ShaderParser.h"

#include <assert.h>
#include <crtdbg.h>

extern char defaultIP[16];

extern int g_iPlayerPosNum;
//#include "CollisionDetection.h"

extern CTexture* g_pPleaseWaitTex;
extern CTexture* g_pCrossHairTex;
extern CTexture* g_pPortalTex;
extern CTexture* g_pCharacterFrameTex;

extern CTexture* g_pUnAccecableSquareTex;
extern CTexture* g_pAccecableSquareTex;
extern CTexture* g_pChosenSquareTex;

extern CTexture* g_pDiffuseTexDEBUG;
extern CTexture* g_pNormalTexDEBUG;
extern CTexture* g_pSpecularTexDEBUG;


extern bool     g_bLookOnOtherPlayer;

#include "File.h"
extern CFileManager g_FileManager;

#include "Renderer.h"

#include "Texture.h"
#include "MD3MeshManager.h"
#include "MD3Manager.h"


extern int g_iPlayerScore;

//////////////////////////////////////////
// TODO: put this in a special class

extern PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
extern int g_iCardTexUnits; // num of tex units on current card
/////////////////////


#include "cgslShader.h"
extern CcgslShader g_cgslShader;

/*#include "cgslShader.h"
extern CcgslShader g_cgslShader;*/


#include "Renderer.h"


#include "Sky.h"
extern CSky*     g_pSky;

#include "LogFile.h"
extern CLogFile g_LogFile;

///////////////
//#include "glstatecapturer.h"
//extern CGLStateCapturer g_glStateCapturer;
///////////////

#include "Math3d.h"

#include "Camera.h"
extern CCamera g_Camera1;


/*#include "Doom3Map.h"
extern CDoom3Map g_Doom3Map;*/

#include "Q3AMap.h"
extern CQuake3BSP* g_pQuake3Map;

extern bool g_bTestCollisionDetection;



extern CVector3f g_vGravity;
//extern bool      g_bPlayerInAir;
/*extern bool      g_bJumping;
extern bool      g_bStartJumping;*/
//extern float     g_fTimePlayerInAir;
extern float     g_fPlayerMoveStrength;
extern CVector3f g_vVelocity;

//extern bool      g_bPlayerFlying;

#include "PhysicsModel.h"
//extern CPhysicsModel g_Player;
#include "PlayerClient.h"
extern CPlayerClient *g_pCurrentlyProcessedPlayerClient,*g_pPlayerClient1,*g_pPlayerClient2;

extern bool      g_bCollidedWithStaticEntity;
extern bool      g_bFinalDirectionIsOppositeThenDesired;

#include "md3.h"
extern CMD3* g_pWeaponModel;
extern CMD3 *g_pAimingAid, *g_pAimingAid2;
extern float g_fAimingAidAng;
//extern CMD3* g_pModel1;

#include "PlayerModel.h"
//extern CPlayerModel* g_pCurrentlyProcessedPlayerClient->m_pPlayerModel;


extern CVector3f g_vWeaponPos;



extern int      g_iFrameNum;

extern int g_iNumberOfDynamicEntitiesDrawn;


#include "ParticleSystem.h"

/******************/
// board game 
#include "CheckersBoard.h"
extern CCheckersBoard* g_pCheckersBoard;

extern CVertexBuffer* g_pvbPPLtest;

extern CVertexBuffer* g_pvbBoardSquaresAccesable;
extern CVertexBuffer* g_pvbBoardSquaresNotAccesable;
extern CVertexBuffer* g_pvbBoardSquaresSelected;

extern CVertexBuffer* g_pvbCharacterFrame;
extern CVertexBuffer* g_pvbCharacterFrame2;
extern CVertexBuffer* g_pvbCursor;




#include "TextMessages.h"
extern CTextMessages g_TextMessages;

/*******************/


// sound manager
#include "SoundManager.h"
extern CSoundManager g_SoundManager;


#include "WeaponsSounds.h"
extern CWeaponsSound g_WeaponsSounds;

///////////////////////////
#include "ZedTracker.h"
///////////////////////////


#include "Communication.h"
extern CCommunication g_Communication;

extern char chat[500];

CEngine::~CEngine()
{

	SafeDelete(g_pWeaponModel);
	SafeDelete(g_pAimingAid);
	SafeDelete(g_pAimingAid2);
	//SafeDelete(g_pCurrentlyProcessedPlayerClient->m_pPlayerModel);

	SafeDelete(g_pQuake3Map);

	SafeDelete(g_pPleaseWaitTex);

	/*SafeDelete(g_pUnAccecableSquareTex);
	SafeDelete(g_pAccecableSquareTex);
	SafeDelete(g_pChosenSquareTex);*/


	SafeDelete(g_pSky);

	st_Renderer::Destroy();


	st_MD3MeshBaseManager::Destroy();

	st_MD3BaseManager::Destroy();

	st_ShaderManager::Destroy();

	st_TextureManager::Destroy();
	
	st_Timer::Destroy();

	st_EntitiesParser::Destroy();

	// remember i need to delete the checkersboard when shutting down
	
}


//////////////////////////////////////////////
// InitGL()
// TODO: this should be in the new CRenderer
//////////////////////////////////////////////
void CEngine::InitGL(void)
{

	
	/************************************************************************/

	// load several general images used by the engine
	g_pPleaseWaitTex = new CTexture;
	g_pPleaseWaitTex->LoadTexture("ceImages\\PleaseWait.jpg",false,true,true);
	
	/********************************************************************/
	st_Renderer::Inst()->InitGL();


	glDisable(GL_CULL_FACE);

}

////////////////
// Init()
////////////////
bool CEngine::Init(void)
{
	chat[0] = NULL;

    m_dwEngineMessages = 0;

	m_bSwitchingGameState = false;

	g_iPlayerScore = 0;

	g_bLookOnOtherPlayer = false;

	g_iNumberOfDynamicEntitiesDrawn = 0;

	g_fAimingAidAng = 0.f;

	g_iFrameNum = 0;

	//g_vWeaponPos.Set(13.f,-15.f,-30.f);
	g_vWeaponPos.Set(0.f,0.f,0.f);

	g_bCollidedWithStaticEntity = false;
	//g_bPlayerFlying = true;
	//g_bPlayerFlying = false;

	g_bFinalDirectionIsOppositeThenDesired = false;

	g_fPlayerMoveStrength=1.f;

	g_vVelocity.Set(0.f,0.f,0.f);

//	g_bStartJumping = false;

	g_vGravity.Set(0.f,-9.8f,0.f);

	//
	g_vGravity*=60.f;
	//g_vGravity.Set(0.f,0.f,0.f);


	//g_bPlayerInAir = false;
	/*g_bJumping = false;*/
	//g_fTimePlayerInAir = 0.f;

	g_bTestCollisionDetection = true;

	m_iFrameCounter=0;

	

	st_TextureManager::CreateInstance();  // init singleton texture manager


	st_MD3MeshBaseManager::CreateInstance();  // init singleton texture manager

	st_MD3BaseManager::CreateInstance();  // init singleton texture manager


	/// Timer singleton init
	st_Timer::CreateInstance();
	st_Timer::Inst()->TimerInit();
	


	//st_CD::CreateInstance();


	/// fonts
	g_Fonts.BuildFont();


	//float fInitTime = st_Timer::Inst()->TimerGetTimeSeconds();

	// Direct Input wrapper singleton init
	st_Input::CreateInstance();
	st_Input::Inst()->Init((unsigned long)g_pWindow->width,(unsigned long)g_pWindow->height,g_pWindow->hWnd);
	st_Input::Inst()->SetCursorPosition((unsigned long)g_pWindow->width / 2,(unsigned long)g_pWindow->height / 2);


	///////////////////////////////////////////////////////////////////////////////////////////


	// sound manager
	g_SoundManager.Init(); // RESTORE RESTORE RESTORE RESTORE


	g_iPlayerPosNum=1;



	st_EntitiesParser::CreateInstance();
	

	st_ShaderManager::CreateInstance();



	int seed = 10000;  /* seed value for the srand function */ 
	srand(seed);  /*initialize random number generator*/ 








	g_LogFile.OutPutPlainText("Generating SkyBox:","3","000099",false);
	g_pSky = new CSky;
	g_pSky->GenerateSkyBox(); // generates verts and 2 texcoords
	g_LogFile.OutPutPlainText("Successful.","3","009900",true);


	st_ShaderParser::CreateInstance();


	/***********************/
	// Loading Shaders Cache
	g_LogFile.OutPutPlainText("Building Shaders Cache Map:","4","000099",true);
	st_ShaderParser::Inst()->BuildFileList("scripts/","*.shader");
	st_ShaderParser::Inst()->BuildCacheMap();
	/**********************/

	/// init the renderer

	st_Renderer::CreateInstance();

	InitGL();
	

	st_PSManager::CreateInstance();

	/*g_fSlowMotionRate=1.f;

	g_Timer.SetFakeSlowMotion(g_fSlowMotionRate);*/

	
	//st_Timer::Inst()->SetFakeSlowMotion(4.f);
	st_Timer::Inst()->SetFakeSlowMotion(1.f);
	//st_Timer::Inst()->SetFakeSlowMotion(2.f);
	//st_Timer::Inst()->SetFakeSlowMotion(8.f);



	/////// Init the camera

	g_Camera1.Init();

	

	

	

	char* pTempFileName = new char[512];

	pTempFileName[0]=NULL;


	//g_FileManager.UserInputFileDialog(".proc","Doom 3 Proc files\0*.proc\0",pTempFileName);
	
	
	
	//g_FileManager.UserInputFileDialog(".bsp","Quake 3 Files\0*.bsp\0",pTempFileName);

	/**/

	/**/

	//
	//strcpy(pTempFileName,"maps/q3dm11.bsp");
	//strcpy(pTempFileName,"maps/q3dm17.bsp");
	//strcpy(pTempFileName,"maps/q3dm8.bsp");
	//strcpy(pTempFileName,"maps/q3dm7.bsp");
	strcpy(pTempFileName,"maps/q3dm1.bsp");

	if (pTempFileName[0] == NULL)
	{
		MessageBox(g_pWindow->hWnd,"No map chosen, shutting down.", "Error", MB_OK);
		return false;
	}


	/*SafeDelete(pTempFileName);*/
	


	/////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	// I NEED TO ADD | I NEED TO ADD | I NEED TO ADD | I NEED TO ADD | 
	st_Renderer::Inst()->DrawFullScreenPic(g_pPleaseWaitTex);
	///////////////////////////////////////////////////////////////////



	///////// actually draw the picture to some back buffer	
	glFlush ();											// Flush The GL Rendering Pipeline

	///////// wait for vsinc and swap buffers
	SwapBuffers (g_pWindow->hDC);					// Swap Buffers (Double Buffering)



	//g_Doom3Map.LoadProc("maps/fred/e3bathroom5.proc");
	


	// loading models

	g_pWeaponModel = new CMD3;
    //g_pModel1      = new CMD3;

	//g_pWeaponModel->Load("models/weapons2/rocketl/rocketl.md3");
	//g_pWeaponModel->Load("models/weapons2/bfg/bfg.md3");
	//g_pWeaponModel->Load("models/weapons2/gauntlet/gauntlet.md3");
	//g_pWeaponModel->Load("models/weapons2/lightning/lightning.md3");
	//g_pWeaponModel->Load("models/weapons2/bfg/bfg.md3");
	//g_pWeaponModel->Load("models/weapons2/shotgun/shotgun.md3",NULL);
	//g_pWeaponModel->Load("models/ammo/rocket/rocket.md3");


	
	// RESTORE RESTORE RESTORE
	g_pWeaponModel->Load("models/weapons2/railgun/railgun.md3",NULL,NULL,true);
	g_pWeaponModel->SetAlwaysDraw(TRUE);
	//g_pWeaponModel->SetIgnoreCameraMatrix(TRUE);


	g_pAimingAid = new CMD3;
	
	//g_pAimingAid->Load("models/weaphits/bullet.md3",NULL,"aimingaid");
	//g_pAimingAid->Load("models/weaphits/bullet.md3",NULL,"textures/sfx/xflame2");
	
	//g_pAimingAid->Load("models/weaphits/bullet.md3",NULL,"textures/sfx/launchpad_diamond");

	//g_pAimingAid->Load("models/powerups/health/large_sphere.md3",NULL,"textures/sfx/flame1side");
	
	
	
	// original
	g_pAimingAid->Load("models/powerups/health/large_sphere.md3",NULL,"AimingAid",false);

	//g_pAimingAid->Load("models/powerups/health/medium_cross.md3",NULL,"textures/sfx/launchpad_diamond");

	
	//g_pAimingAid->Load("models/powerups/health/large_sphere.md3",NULL,"models/players/slash/yurikoskate");

	
	
	g_pAimingAid2 = new CMD3;
	
	g_pAimingAid2->Load("models/powerups/health/large_sphere.md3",NULL,"textures/sfx/launchpad_diamond",false);
	
	





	/*
	g_pQuake3Map = new CQuake3BSP;
	
	g_pQuake3Map->LoadBSP(pTempFileName);
	*/

	
	// DEBUG DEBUG DEBUG
	//g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->SetPosition(CVector3f(0.f,0.f,0.f	));

	g_iUserDefinedBezierPatchTessLevel = DEFAULT_BEZIER_PATCH_TESSELATION_LEVEL;

	



	




	//g_Doom3Map.LoadCM("maps/fred/e3bathroom5.cm");

	/*

	if (strstr(pTempFileName,"q3dm1.")!=NULL)
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->SetPosition(CVector3f(723.f,58.f,-1178.f));
	else
	if (strstr(pTempFileName,"q3dm17.")!=NULL)
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->SetPosition(CVector3f(300.f,459.f,581.f));
	else
	{
		CVector3f vVec;
		int iNum = 14;
		st_EntitiesParser::Inst()->GetPlayerPosition(iNum,vVec);
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->SetPosition(vVec);
	}

	*/

	



	

	g_Camera1.SetAngles(1.8f,6.2f,0.f);


	st_Renderer::Inst()->InitSkyVBs();

	//g_Camera1.LookAt(CVector3f(0,0,-1));



	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// load some gui textures
	//g_pCrossHairTex = st_TextureManager::Inst()->LoadTexAndPush("ceImages\\CrossHair.jpg",false,true);
	g_pCrossHairTex = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\Cursors\\States.jpg",false,true,true);
	g_pPortalTex    = st_TextureManager::Inst()->LoadTexAndPush("ceImages\\Portal.jpg",false,true,true);

	g_pUnAccecableSquareTex = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\Black.jpg",false,true,true);
	//g_pAccecableSquareTex = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\Cursors\\Blue.jpg",false,true);
	g_pAccecableSquareTex = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\Brown.jpg",false,true,true);
	//g_pChosenSquareTex = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\Cursors\\White.jpg",false,true);
	//g_pChosenSquareTex = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\Cursors\\Brown.jpg",false,true);
	g_pChosenSquareTex = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\Blue.jpg",false,true,true);

	//g_pCharacterFrameTex = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CharacterFrame.jpg",false,true);
	
	/*g_pDiffuseTexDEBUG = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\hellknight.tga",false,true);
	g_pNormalTexDEBUG = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\hellknight_local.tga",false,true);*/
	g_pDiffuseTexDEBUG = st_TextureManager::Inst()->LoadTexAndPush("models\\players\\major\\major_h.tga",false,true,true);
	g_pNormalTexDEBUG = st_TextureManager::Inst()->LoadTexAndPush("models\\players\\major\\major_h_local.tga",false,true,false);
	g_pSpecularTexDEBUG = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\black.tga",false,true,true);


	/*g_pDiffuseTexDEBUG = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\Board_Blue.tga",false,true);
	g_pNormalTexDEBUG = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\Board_Blue_local.tga",false,true);*/



	// character frame gui vb
	
	g_pvbCharacterFrame = new CVertexBuffer;
	char Name[40];
	strcpy(Name,"menuCharaterFrame");
	CShader* pShader = st_ShaderManager::Inst()->Load(&Name[0]);
	g_pvbCharacterFrame->SetShaderSP(pShader);
	
	// cursor vb 

	g_pvbCursor = new CVertexBuffer;
	//strcpy(Name,"menuCharaterFrame");
	//strcpy(Name,"textures/sfx/flame2");
	strcpy(Name,"zedusCursor");
	
	pShader = st_ShaderManager::Inst()->Load(&Name[0]);
	g_pvbCursor->SetShaderSP(pShader);

	// character 2 frame

	g_pvbCharacterFrame2 = new CVertexBuffer;

	strcpy(Name,"menuCharaterFrame2");

	pShader = st_ShaderManager::Inst()->Load(&Name[0]);

	g_pvbCharacterFrame2->SetShaderSP(pShader);

	

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	

	


	////////////////////////////// ppl test /////////////////////////////


	// Squares Seleceted 


	//g_ppvbBoardSquaresSelected

	// Squares Not Accesable

	/*g_ppvbBoardSquaresSelected = new CVertexBuffer*[SQUARES_NUM_X*SQUARES_NUM_Y];

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
	{
		int iIndex = i + j*SQUARES_NUM_X;*/

		g_pvbBoardSquaresSelected = new CVertexBuffer;
		g_pvbBoardSquaresSelected->Alloc(VB_HAS_VERTS | VB_HAS_NORMALS  | VB_HAS_TEXCOORDS | VB_HAS_INDICES |
							VB_HAS_TANGENTS | VB_HAS_BINORMALS,4);
	
		g_pvbBoardSquaresSelected->AllocIndexBuffer(6);

		// set indices

		UINT* uipIndices = g_pvbBoardSquaresSelected->GetIndices();
		
		uipIndices[0] = 0;
		uipIndices[1] = 1;
		uipIndices[2] = 2;
		uipIndices[3] = 2;
		uipIndices[4] = 3;
		uipIndices[5] = 0;


		// set vertices
		CVector3f* pVerts = (CVector3f*) g_pvbBoardSquaresSelected->Get(VB_HAS_VERTS);


		CVector3f vMiddle = CVector3f(0.f,0.f,0.f);

		pVerts[0] = vMiddle+CVector3f(-HALF_BOARD_SQUARE_SIZE,0.f,-HALF_BOARD_SQUARE_SIZE);
		pVerts[1] = vMiddle+CVector3f(HALF_BOARD_SQUARE_SIZE,0.f,-HALF_BOARD_SQUARE_SIZE);
		pVerts[2] = vMiddle+CVector3f(HALF_BOARD_SQUARE_SIZE,0.f,HALF_BOARD_SQUARE_SIZE);
		pVerts[3] = vMiddle+CVector3f(-HALF_BOARD_SQUARE_SIZE,0.f,HALF_BOARD_SQUARE_SIZE);


		// set normals
		CVector3f* pNormals = (CVector3f*) g_pvbBoardSquaresSelected->Get(VB_HAS_NORMALS);

	
		pNormals[0].Set(0.f,1.f,0.f);
		pNormals[1].Set(0.f,1.f,0.f);
		pNormals[2].Set(0.f,1.f,0.f);
		pNormals[3].Set(0.f,1.f,0.f);


		// set texcoords

		CVector2f* pTexCoords = (CVector2f*) g_pvbBoardSquaresSelected->Get(VB_HAS_TEXCOORDS);

		pTexCoords[0].Set(0.f,1.f);
		pTexCoords[1].Set(1.f,1.f);
		pTexCoords[2].Set(1.f,0.f);
		pTexCoords[3].Set(0.f,0.f);

	


		// calculate tangents and binormals

		g_pvbBoardSquaresSelected->CalculateTangentAndBinormalArrays();

		// set shader

		strcpy(Name,"ceGui/CheckersBoard/Blue.jpg");
		pShader = st_ShaderManager::Inst()->Load(&Name[0]);
		g_pvbBoardSquaresSelected->SetShaderSP(pShader);

		// set textures for bumpmap

		if (st_RendererI->m_bPPLused)
		{
			g_pvbBoardSquaresSelected->SetBaseTexture(st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\gotcgate2_d2.tga",true,true,true));
			g_pvbBoardSquaresSelected->SetNormalTexture(st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\gotcgate2_local.tga",true,true,false));
			g_pvbBoardSquaresSelected->SetSpecularTexture(st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\gotcgate2_s2.tga",true,true,true));
		}

	/*}*/
	


	// SquaresAccesable

	/*g_pvbBoardSquaresAccesable = new CVertexBuffer;

	for (int i=0;i<SQUARES_NUM_X;i++)
		for (int j=0;j<SQUARES_NUM_Y;j++)
	{
		int iIndex = i + j*SQUARES_NUM_X;*/

		int iIndex = 0;

		g_pvbBoardSquaresAccesable = new CVertexBuffer;
		g_pvbBoardSquaresAccesable->Alloc(VB_HAS_VERTS | VB_HAS_NORMALS  | VB_HAS_TEXCOORDS | VB_HAS_INDICES |
							VB_HAS_TANGENTS | VB_HAS_BINORMALS,4);
	
		g_pvbBoardSquaresAccesable->AllocIndexBuffer(6);

		// set indices

		uipIndices = g_pvbBoardSquaresAccesable->GetIndices();
		
		uipIndices[0] = 0;
		uipIndices[1] = 1;
		uipIndices[2] = 2;
		uipIndices[3] = 2;
		uipIndices[4] = 3;
		uipIndices[5] = 0;


		// set vertices
		pVerts = (CVector3f*) g_pvbBoardSquaresAccesable->Get(VB_HAS_VERTS);


		//CVector3f vMiddle = g_pCheckersBoard->GetMiddleOfSquare(i,j);
		vMiddle = CVector3f(0.f,0.f,0.f);

		pVerts[0] = vMiddle+CVector3f(-HALF_BOARD_SQUARE_SIZE,0.f,-HALF_BOARD_SQUARE_SIZE);
		pVerts[1] = vMiddle+CVector3f(HALF_BOARD_SQUARE_SIZE,0.f,-HALF_BOARD_SQUARE_SIZE);
		pVerts[2] = vMiddle+CVector3f(HALF_BOARD_SQUARE_SIZE,0.f,HALF_BOARD_SQUARE_SIZE);
		pVerts[3] = vMiddle+CVector3f(-HALF_BOARD_SQUARE_SIZE,0.f,HALF_BOARD_SQUARE_SIZE);


		// set normals
		pNormals = (CVector3f*) g_pvbBoardSquaresAccesable->Get(VB_HAS_NORMALS);

	
		pNormals[0].Set(0.f,1.f,0.f);
		pNormals[1].Set(0.f,1.f,0.f);
		pNormals[2].Set(0.f,1.f,0.f);
		pNormals[3].Set(0.f,1.f,0.f);


		// set texcoords

		pTexCoords = (CVector2f*) g_pvbBoardSquaresAccesable->Get(VB_HAS_TEXCOORDS);

		pTexCoords[0].Set(0.f,1.f);
		pTexCoords[1].Set(1.f,1.f);
		pTexCoords[2].Set(1.f,0.f);
		pTexCoords[3].Set(0.f,0.f);

	


		// calculate tangents and binormals

		g_pvbBoardSquaresAccesable->CalculateTangentAndBinormalArrays();

		// set shader

		strcpy(Name,"ceGui/CheckersBoard/Brown.jpg");
		pShader = st_ShaderManager::Inst()->Load(&Name[0]);
		g_pvbBoardSquaresAccesable->SetShaderSP(pShader);

		// set textures for bumpmap


		if (st_RendererI->m_bPPLused)
		{
			g_pvbBoardSquaresAccesable->SetBaseTexture(st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\gotcgate2_d.tga",true,true,true));
			g_pvbBoardSquaresAccesable->SetNormalTexture(st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\gotcgate2_local.tga",true,true,false));
			g_pvbBoardSquaresAccesable->SetSpecularTexture(st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\gotcgate2_s2.tga",true,true,true));
		}

	/*}*/


	// Squares Not Accesable


		g_pvbBoardSquaresNotAccesable = new CVertexBuffer;
		g_pvbBoardSquaresNotAccesable->Alloc(VB_HAS_VERTS | VB_HAS_NORMALS  | VB_HAS_TEXCOORDS | VB_HAS_INDICES |
							VB_HAS_TANGENTS | VB_HAS_BINORMALS,4);
	
		g_pvbBoardSquaresNotAccesable->AllocIndexBuffer(6);

		// set indices

		uipIndices = g_pvbBoardSquaresNotAccesable->GetIndices();
		
		uipIndices[0] = 0;
		uipIndices[1] = 1;
		uipIndices[2] = 2;
		uipIndices[3] = 2;
		uipIndices[4] = 3;
		uipIndices[5] = 0;


		// set vertices
		pVerts = (CVector3f*) g_pvbBoardSquaresNotAccesable->Get(VB_HAS_VERTS);


		//vMiddle = g_pCheckersBoard->GetMiddleOfSquare(i,j);
		vMiddle = CVector3f(0.f,0.f,0.f);

		pVerts[0] = vMiddle+CVector3f(-HALF_BOARD_SQUARE_SIZE,0.f,-HALF_BOARD_SQUARE_SIZE);
		pVerts[1] = vMiddle+CVector3f(HALF_BOARD_SQUARE_SIZE,0.f,-HALF_BOARD_SQUARE_SIZE);
		pVerts[2] = vMiddle+CVector3f(HALF_BOARD_SQUARE_SIZE,0.f,HALF_BOARD_SQUARE_SIZE);
		pVerts[3] = vMiddle+CVector3f(-HALF_BOARD_SQUARE_SIZE,0.f,HALF_BOARD_SQUARE_SIZE);


		// set normals
		pNormals = (CVector3f*) g_pvbBoardSquaresNotAccesable->Get(VB_HAS_NORMALS);

	
		pNormals[0].Set(0.f,1.f,0.f);
		pNormals[1].Set(0.f,1.f,0.f);
		pNormals[2].Set(0.f,1.f,0.f);
		pNormals[3].Set(0.f,1.f,0.f);


		// set texcoords

		pTexCoords = (CVector2f*) g_pvbBoardSquaresNotAccesable->Get(VB_HAS_TEXCOORDS);

		pTexCoords[0].Set(0.f,1.f);
		pTexCoords[1].Set(1.f,1.f);
		pTexCoords[2].Set(1.f,0.f);
		pTexCoords[3].Set(0.f,0.f);

	


		// calculate tangents and binormals

		g_pvbBoardSquaresNotAccesable->CalculateTangentAndBinormalArrays();

		// set shader

		strcpy(Name,"ceGui/CheckersBoard/Black.jpg");
		pShader = st_ShaderManager::Inst()->Load(&Name[0]);
		g_pvbBoardSquaresNotAccesable->SetShaderSP(pShader);

		// set textures for bumpmap

		
		if (st_RendererI->m_bPPLused)
		{
			g_pvbBoardSquaresNotAccesable->SetBaseTexture(st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\gotendo1_d.tga",false,true,true));
			g_pvbBoardSquaresNotAccesable->SetNormalTexture(st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\gotendo1_local.tga",false,true,false));
			g_pvbBoardSquaresNotAccesable->SetSpecularTexture(st_TextureManager::Inst()->LoadTexAndPush("ceGui\\CheckersBoard\\gotendo1_s.tga",false,true,true));
		}



		
	/////////////////////////////////////////////////////////////////////

	




	////////////////////////////// ppl test /////////////////////////////







	// checkersboard stuff

	g_pCheckersBoard = new CCheckersBoard;


	//m_iGameState = 1; // start in the FPS stage
	
	st_Engine::Inst()->m_iGameState=0; // make it a board game




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

	
	/// init and load vertex/fragment programs
	if (st_RendererI->m_bPPLused)
		g_cgslShader.LoadAllCgPrograms();




	//////////////////////// GUI ////////////////////////

	g_pGUIManager = new CGUIManager;

	/////////////////////////////////////////////////////


	/**/
	char name[400];
	CShader* pShaderTEMP;

	strcpy(Name,"menuTopBoardNewGameUP");		
	pShaderTEMP = st_ShaderManager::Inst()->Load(&Name[0]);

	strcpy(Name,"menuTopBoardNewGameDOWN");		
	pShaderTEMP = st_ShaderManager::Inst()->Load(&Name[0]);

	strcpy(Name,"menuTopBoardNewGameHOVER");		
	pShaderTEMP = st_ShaderManager::Inst()->Load(&Name[0]);


	
	strcpy(Name,"menuTopBoardEndGameUP");		
	pShaderTEMP = st_ShaderManager::Inst()->Load(&Name[0]);

	strcpy(Name,"menuTopBoardEndGameDOWN");		
	pShaderTEMP = st_ShaderManager::Inst()->Load(&Name[0]);

	strcpy(Name,"menuTopBoardEndGameHOVER");		
	pShaderTEMP = st_ShaderManager::Inst()->Load(&Name[0]);

	/**/


	g_pCheckersBoard->InitSounds();

	
	// intro music

	g_pCheckersBoard->StopBackgroundMusic();
	g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_INTRO]);




	g_WeaponsSounds.Init();

	return true;
}



void CEngine::HandleMessages(void)
{

	if (m_dwEngineMessages & ENGINE_MESSAGE_SAVE_GAME)
	{
		g_pCheckersBoard->SaveBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_LOAD_GAME)
	{
		g_pCheckersBoard->LoadBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_NEW_GAME)
	{
		// if checkers board is in menu

		if (g_pCheckersBoard->m_iBoardGameState==GAME_BOARD_STATE_MENU)
		{
			/*if (g_pCheckersBoard->m_bPlayingWinSong)
				{
					g_pCheckersBoard->StopBackgroundMusic();
					g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_INTRO]);
				}*/

			g_pCheckersBoard->StopBackgroundMusic();
			g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_INTRO]);

			g_pCheckersBoard->NewGame();
			g_pGUIManager->ResetBoardGameMenus();
		}
		else // end game
		{
			//g_pCheckersBoard->StopBackgroundMusic();
			//g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_INTRO]);

			g_pCheckersBoard->PreInit();
			g_pGUIManager->ResetBoardGameMenus();
			g_pCheckersBoard->m_iBoardGameState=GAME_BOARD_STATE_MENU;


			// start a new game if inside a connection
			if ( g_Communication.GetAlreadyInited())
			{
			   /* g_Communication.StopConnectionGame();

				 //// logging game for debug ////
				static char buffer[1024];
				sprintf(buffer, "Player Disconnected!");
				g_LogFile.OutPutPlainText(buffer,"3","009900",true);			
				/////////////////////////////////
				//MessageBox(HWND_DESKTOP,"Player Aborted Connection", "Error", MB_OK);
				g_TextMessages.AddMessage("You Have Disconnected From Network.",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);*/

				g_pCheckersBoard->StopBackgroundMusic();
				g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_BOARD]);

				g_pCheckersBoard->NewGame();
				g_pGUIManager->ResetBoardGameMenus();

				// send the actual "new game" message

				// if you are the server then send a "handshake" saying which one is which side
				if (g_Communication.GetCommunicationType()==COMMUNICATION_SERVER)
				{
					// if you are the server then send a "handshake" saying which one is which side
					CGUIWindow* pMenu = g_pGUIManager->m_pOptionsMenu;
					if (((CGUI2StatesButton*)pMenu->GetElementNum(0))->GetState()!=TWO_SIDES_BUTTON_STATE_1)
					{
						g_pCheckersBoard->m_iPlayerTypePAST=PLAYER_TYPE_HUMAN;
						g_pCheckersBoard->m_iPlayerTypeFUTURE=PLAYER_TYPE_NETWORK;
						g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_PAST,0,0,NULL);
					}
					else
					{
						g_pCheckersBoard->m_iPlayerTypePAST=PLAYER_TYPE_NETWORK;
						g_pCheckersBoard->m_iPlayerTypeFUTURE=PLAYER_TYPE_HUMAN;
						g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_FUTURE,0,0,NULL);
					}

				}
				else
				// the past/futue value doesn't matter if we are clients... (only server decides that...)
					g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_PAST,0,0,NULL);

				
				
				
				
			}

			
		}

	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_QUIT)
	{
		g_pWindow->TerminateApplication();	// Terminate The Program
	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_SAVED_GAME_00)
	{
		g_pCheckersBoard->m_iCurrentSaveGame = 0;
		g_pCheckersBoard->SaveBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_SAVED_GAME_01)
	{
		g_pCheckersBoard->m_iCurrentSaveGame = 1;
		g_pCheckersBoard->SaveBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_SAVED_GAME_02)
	{
		g_pCheckersBoard->m_iCurrentSaveGame = 2;
		g_pCheckersBoard->SaveBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_SAVED_GAME_03)
	{
		g_pCheckersBoard->m_iCurrentSaveGame = 3;
		g_pCheckersBoard->SaveBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_SAVED_GAME_04)
	{
		g_pCheckersBoard->m_iCurrentSaveGame = 4;
		g_pCheckersBoard->SaveBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_SAVED_GAME_05)
	{
		g_pCheckersBoard->m_iCurrentSaveGame = 5;
		g_pCheckersBoard->SaveBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}



	if (m_dwEngineMessages & ENGINE_MESSAGE_LOAD_GAME_00)
	{
		g_pCheckersBoard->m_iCurrentSaveGame = 0;
		g_pCheckersBoard->LoadBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_LOAD_GAME_01)
	{
		g_pCheckersBoard->m_iCurrentSaveGame = 1;
		g_pCheckersBoard->LoadBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_LOAD_GAME_02)
	{
		g_pCheckersBoard->m_iCurrentSaveGame = 2;
		g_pCheckersBoard->LoadBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_LOAD_GAME_03)
	{
		g_pCheckersBoard->m_iCurrentSaveGame = 3;
		g_pCheckersBoard->LoadBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_LOAD_GAME_04)
	{
		g_pCheckersBoard->m_iCurrentSaveGame = 4;
		g_pCheckersBoard->LoadBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}

	if (m_dwEngineMessages & ENGINE_MESSAGE_LOAD_GAME_05)
	{
		g_pCheckersBoard->m_iCurrentSaveGame = 5;
		g_pCheckersBoard->LoadBoardGame();
		g_pGUIManager->ResetBoardGameMenus();
	}


	if (m_dwEngineMessages & ENGINE_MESSAGE_SEND_CHAT_MESSAGE)
	{
		if (g_Communication.GetAlreadyInited())
		{
			g_TextMessages.AddMessage(&chat[0],1.f,0.f,1.f,TEXT_MESSAGE_TYPE_CHAT_YOU);		
			g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_NEXT_IS_CHAT,0, 0,&chat[0]);
			g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_CHAT,0,0,&chat[0]);	
		}
		else
			g_TextMessages.AddMessage("You must be connected in order to chat!",1.f,1.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);		
		
		
	}

	
	if (m_dwEngineMessages & ENGINE_MESSAGE_START_NETWORK_SERVER)
	{
		if (!g_Communication.GetAlreadyInited())
		{
			// this should open a dialog that asks which side you want to take
			if (g_Communication.InitServer())
			{
				
				//if (g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_HUMAN)
				CGUIWindow* pMenu = g_pGUIManager->m_pOptionsMenu;
				if (((CGUI2StatesButton*)pMenu->GetElementNum(0))->GetState()!=TWO_SIDES_BUTTON_STATE_1)
				{
					g_pCheckersBoard->m_iPlayerTypePAST=PLAYER_TYPE_HUMAN;
					g_pCheckersBoard->m_iPlayerTypeFUTURE=PLAYER_TYPE_NETWORK;
					g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_PAST,0,0,NULL);
				}
				else
				{
					g_pCheckersBoard->m_iPlayerTypePAST=PLAYER_TYPE_NETWORK;
					g_pCheckersBoard->m_iPlayerTypeFUTURE=PLAYER_TYPE_HUMAN;
					g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_FUTURE,0,0,NULL);
				}
							

				g_pGUIManager->m_bChoseNetwork=true;
				g_pGUIManager->m_dwGUIMessages|= GUI_MESSAGE_GET_TO_DEFAULT_MENUS;
				g_pGUIManager->HandleGUIMessages();

				// make a new game
				g_pCheckersBoard->Destroy();
				g_pCheckersBoard->Init(true);
				g_pCheckersBoard->m_iBoardGameState=GAME_BOARD_STATE_CHOOSE_UNIT;
				if (g_pCheckersBoard->m_bPlayingWinSong)
				{
					g_pCheckersBoard->StopBackgroundMusic();
					g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_INTRO]);
				}				
			}
			g_Communication.UpdateOptionsMenu();
		}
		else
		{
			g_TextMessages.AddMessage("Already In Connection!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			g_TextMessages.AddMessage("Press [Close] To Stop Current Connection First!",0.f,0.f,1.f,TEXT_MESSAGE_TYPE_SYSTEM);
		}
	}
	else
	if (m_dwEngineMessages & ENGINE_MESSAGE_START_NETWORK_CLIENT)
	{
		if (!g_Communication.GetAlreadyInited())
		{
			// this should open a dialog that asks which ip adress you want to connect to
			if (g_Communication.InitClient(defaultIP))
			{								
				/*g_pCheckersBoard->m_iPlayerTypePAST=PLAYER_TYPE_NETWORK;
				g_pCheckersBoard->m_iPlayerTypeFUTURE=PLAYER_TYPE_HUMAN;*/

				g_pGUIManager->m_bChoseNetwork=true;
				g_pGUIManager->m_dwGUIMessages|= GUI_MESSAGE_GET_TO_DEFAULT_MENUS;
				g_pGUIManager->HandleGUIMessages();

				// make a new game
				g_pCheckersBoard->Destroy();
				g_pCheckersBoard->Init(true);
				g_pCheckersBoard->m_iBoardGameState=GAME_BOARD_STATE_CHOOSE_UNIT;
				if (g_pCheckersBoard->m_bPlayingWinSong)
				{
					g_pCheckersBoard->StopBackgroundMusic();
					g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_INTRO]);
				}				
			}
			g_Communication.UpdateOptionsMenu();
		}
		else
		{
			g_TextMessages.AddMessage("Already In Connection!",1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			g_TextMessages.AddMessage("Press ""Stop!"" To Stop Current Connection First!",0.f,1.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
		}
	}

	
	if (m_dwEngineMessages & ENGINE_MESSAGE_STOP_CONNECTION)
	{
		if (g_Communication.GetCommunicationType()==COMMUNICATION_SERVER)
		{
			g_Communication.SERVERStopWaitingForClients();
			g_Communication.UpdateOptionsMenu();
		}
		else
		if (g_Communication.GetCommunicationType()==COMMUNICATION_CLIENT)
		{
			g_Communication.CLIENTStopConnection();
			g_Communication.UpdateOptionsMenu();
		}
		
	}

	////////

	// keep trying if there is attemt to host

	if (g_Communication.Update())
	{
		if (g_Communication.GetCommunicationType()==COMMUNICATION_SERVER)
		{
			//if (g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_HUMAN)
			CGUIWindow* pMenu = g_pGUIManager->m_pOptionsMenu;
			if (((CGUI2StatesButton*)pMenu->GetElementNum(0))->GetState()!=TWO_SIDES_BUTTON_STATE_1)
				{
					g_pCheckersBoard->m_iPlayerTypePAST=PLAYER_TYPE_HUMAN;
					g_pCheckersBoard->m_iPlayerTypeFUTURE=PLAYER_TYPE_NETWORK;
					g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_PAST,0,0,NULL);
				}
				else
				{
					g_pCheckersBoard->m_iPlayerTypePAST=PLAYER_TYPE_NETWORK;
					g_pCheckersBoard->m_iPlayerTypeFUTURE=PLAYER_TYPE_HUMAN;
					g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_NEW_GAME_SERVER_IS_FUTURE,0,0,NULL);
				}

			g_pGUIManager->m_bChoseNetwork=true;
			g_pGUIManager->m_dwGUIMessages|= GUI_MESSAGE_GET_TO_DEFAULT_MENUS;
			g_pGUIManager->HandleGUIMessages();

			// make a new game
			g_pCheckersBoard->Destroy();
			g_pCheckersBoard->Init(true);
			g_pCheckersBoard->m_iBoardGameState=GAME_BOARD_STATE_CHOOSE_UNIT;
			if (g_pCheckersBoard->m_bPlayingWinSong)
			{
				g_pCheckersBoard->StopBackgroundMusic();
				g_SoundManager.PlayStream(g_pCheckersBoard->m_ppBackgroundStreams[CB_SOUND_STREAM_INTRO]);
			}
		}
		
	}
	



	////////


	if (m_dwEngineMessages & ENGINE_MESSAGE_ADVANTAGE_WEAPON)
	{
		if (g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_PAST)
            g_pCheckersBoard->m_iAdvantagePAST = ADVANTAGE_WEAPON;
		else
			g_pCheckersBoard->m_iAdvantageFUTURE = ADVANTAGE_WEAPON;
	}


	if (m_dwEngineMessages & ENGINE_MESSAGE_ADVANTAGE_EXTRALIFE)
	{
		if (g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_PAST)
            g_pCheckersBoard->m_iAdvantagePAST = ADVANTAGE_EXTRALIFE;
		else
			g_pCheckersBoard->m_iAdvantageFUTURE = ADVANTAGE_EXTRALIFE;
	
	}


	if (m_dwEngineMessages & ENGINE_MESSAGE_ADVANTAGE_AMMO)
	{
		if (g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_PAST)
            g_pCheckersBoard->m_iAdvantagePAST = ADVANTAGE_AMMO;
		else
			g_pCheckersBoard->m_iAdvantageFUTURE = ADVANTAGE_AMMO;
	}


	if (m_dwEngineMessages & ENGINE_MESSAGE_CAST_A_SPELL)
	{
		if (g_pCheckersBoard->m_iBoardGameState==GAME_BOARD_STATE_CHOOSE_TARGET)
			if (g_pCheckersBoard->m_iSelectedUnitExperiencePoints>2)
			{
			if (g_pCheckersBoard->m_iChosenUnitX>=0 && g_pCheckersBoard->m_iChosenUnitY>=0)
				// if same player side
				if (g_pCheckersBoard->BoardsSquaresUnits[g_pCheckersBoard->m_iChosenUnitX][g_pCheckersBoard->m_iChosenUnitY].m_iArmySide==g_pCheckersBoard->m_iCurrentPlayerSide)
				{

					int iTempIndex = g_pCheckersBoard->BoardsSquaresUnits[g_pCheckersBoard->m_iChosenUnitX][g_pCheckersBoard->m_iChosenUnitY].m_iIndexToPlayersArray;

					if (iTempIndex>=0)
					{

						if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType>=0)
							// check if the spell is possible						
							if (g_pCheckersBoard->IsSpellPossible(g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType)) 
							{
								// network
								// networking
								if ( (g_pCheckersBoard->m_iPlayerTypeFUTURE==PLAYER_TYPE_NETWORK && g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_PAST) || 					
									(g_pCheckersBoard->m_iPlayerTypePAST==PLAYER_TYPE_NETWORK && g_pCheckersBoard->m_iCurrentPlayerSide==ARMY_SIDE_FUTURE) )
									g_Communication.SendBoardMessage(NETWORK_BOARD_MESSAGE_CAST_MAGIC,0,0,NULL);
								

								g_pCheckersBoard->m_iBoardGameState=GAME_BOARD_STATE_CHOOSE_TARGET_SPELL;

								/*if (g_pCheckersBoard->GetSpells()[g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType].m_iEffects==SPELL_EFFECTS_AUTO)
									g_pCheckersBoard->ChooseSpellTarget(g_pCheckersBoard->m_iChosenUnitX,g_pCheckersBoard->m_iChosenUnitY);				*/


							}
					}
				}
			}

	}
	


	m_dwEngineMessages = 0 ;
}

