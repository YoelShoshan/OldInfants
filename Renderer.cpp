#include "Renderer.h"


#include <GL/gl.h>
#include <GL/glu.h>

#include "Sky.h"
extern CSky*     g_pSky;


#include "Singleton.h"
#include "Timer.h"

#include "Camera.h"
extern CCamera g_Camera1;  // the global camera

#include "NeHeGL.h"
extern GL_Window* g_pWindow; // Main Window

#include "GLFonts.h"
extern CFonts      g_Fonts; // fonts global variable (draw 2d text to screen)

#include "Engine.h"

#include "PlayerModel.h"
//extern CPlayerModel* g_pCurrentlyProcessedPlayerClient->m_pPlayerModel;

#include "glExtensionsHandling.h"

#include "MD3MeshManager.h"
#include "MD3Manager.h"

#include "cgslShader.h"
extern CcgslShader g_cgslShader;

#include "Input.h"

#include "CheckersBoard.h"
extern CCheckersBoard* g_pCheckersBoard;

extern float g_fDistanceBetweenPlayer;

extern int g_iUserDefinedBezierPatchTessLevel;

//extern bool g_bPlayerInAir;
//extern float g_fTimePlayerInAir;

extern int g_iPlayerScore;

#include "PhysicsModel.h"
//extern CPhysicsModel g_Player;
#include "PlayerClient.h"
extern CPlayerClient *g_pCurrentlyProcessedPlayerClient,*g_pPlayerClient1,*g_pPlayerClient2,*g_pCurrentlyProcessedPlayerClient;

extern CVector3f g_vVelocity;

#include "Entities.h"

extern bool g_bShouldLinkDebug;

extern bool g_bCollidedWithStaticEntity;

extern bool g_bFinalDirectionIsOppositeThenDesired;


extern int g_iNumberOfDynamicEntitiesDrawn;

#include "SurfaceFlags.h"

/*extern int g_iShootTestFlags;
extern int g_iShootTestContents;*/

#include <assert.h>
#include <crtdbg.h>

extern bool g_bDrawText;

extern CVector3f g_vBoardRayCollision;



#include "Q3AMap.h"
extern CQuake3BSP* g_pQuake3Map;

extern bool g_bBotDistanceToFloorTooBig;
extern bool g_bBotJumpsWithJumpPad;
extern int  g_iBotSteeringMode;
extern bool g_bDirectLineOfSight;

extern float g_fMouseX;
extern float g_fMouseY;

extern int  g_PointedSquareIndexX;
extern int  g_PointedSquareIndexY;


//extern CVertexBuffer** g_ppvbBoardSquaresAccesable;

extern CTexture* g_pDiffuseTexDEBUG;
extern CTexture* g_pNormalTexDEBUG;
extern CTexture* g_pSpecularTexDEBUG;


#include "TextMessages.h"
extern CTextMessages g_TextMessages;

#include "Communication.h"
extern CCommunication g_Communication;

///////////////////////////
#include "ZedTracker.h"
///////////////////////////

CRenderer::CRenderer()
{
	m_iUsedLights = 0;

	m_bPPLused = false;
	//m_bPPLused = true;

	m_uiFrameCounter = 0;

	m_pVBPTRQueue = new CVertexBufferPTR[VB_MAX];

	m_pSkyVBs = new CVertexBuffer[6];

	
	m_uiVBNum=0;

	m_fTimeSinceLastBezierTessLODCheck = 0;
	m_pbTempColorArray    = new byte[4*VB_VERTICES_MAX];
	m_pTempTexCoordsArray = new CVector2f[VB_VERTICES_MAX];
	m_pTempLMCoordsArray = new CVector2f[VB_VERTICES_MAX];
	m_pTempVertexArray    = new CVector3f[VB_VERTICES_MAX];

	m_puiTempIndicesArray = new UINT[VB_VERTICES_MAX*6];

	iDebugCounter=0;

	m_dwOpenGLState=0;
	m_dwOpenGLState|=RENDERER_STATE_DEPTHFUNC_EQUAL;
	m_dwOpenGLState|=RENDERER_STATE_ALPHAFUNC_NONE;
	m_dwOpenGLState|=RENDERER_STATE_CULLBACK;

	m_iTotalFrameCounter = 0;

	m_ppLights = new CLight*[RENDERER_MAX_LIGHTS];

	for (int i=0;i<RENDERER_MAX_LIGHTS;i++)
		m_ppLights[i] = new CLight;


}

CRenderer::~CRenderer()
{
  SafeDeleteArray(m_pbTempColorArray);
  SafeDeleteArray(m_pTempTexCoordsArray);
  SafeDeleteArray(m_pTempLMCoordsArray);
  SafeDeleteArray(m_pTempVertexArray);
  SafeDeleteArray(m_puiTempIndicesArray);

  SafeDeleteArray(m_ppLights);

  /*for (int i=0;i<RENDERER_MAX_LIGHTS;i++)
		SafeDelete(m_ppLights[i]);*/

  /////////
  //
  // since those are "Fake" VertexBuffer that actualy "steals" it's
  // vertices/texcoords/indices from gSky.PutNameHere
  // this prevents the VertexBuffer from trying to kill them itself.
  for (int i=0;i<6;i++)
  {
	  m_pSkyVBs[i].Set(VB_HAS_VERTS,NULL);
	  m_pSkyVBs[i].SetSize(0);
	  m_pSkyVBs[i].Set(VB_HAS_TEXCOORDS,NULL);
	  m_pSkyVBs[i].SetIndexCount(0);
	  m_pSkyVBs[i].SetIndices(NULL);
  }

  SafeDeleteArray(m_pSkyVBs);
  SafeDeleteArray(m_pVBPTRQueue);

}

void CRenderer::InitGL(void)
{
	// OpenGL Init

	glMatrixMode(GL_PROJECTION);							//select projection matrix


	// since I haven't implemented stencil shadows yet, I don't need the frustum to be infinite
	// so using glu here is ok
	/*gluPerspective (45.f, (GLfloat)(g_pWindow->width)/(GLfloat)(g_pWindow->height),
		7.f, 18000.f);			// Calculate The Aspect Ratio Of The Window*/

	gluPerspective (105.f/2.f, (GLfloat)(g_pWindow->width)/(GLfloat)(g_pWindow->height),
		7.f, 18000.f);			// Calculate The Aspect Ratio Of The Window

	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);						// Black Background
	//glClearColor (0.0f, 0.0f, 0.0f, 1.0f);						// Black Background

	glClearDepth (1.0f);										// Depth Buffer Setup

	glDepthFunc (GL_LEQUAL);									// The Type Of Depth Testing (Less Or Equal)	
	glEnable (GL_DEPTH_TEST);									// Enable Depth Testing

	glShadeModel (GL_SMOOTH);									// Select Smooth Shading
	//glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate

	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);			// Set Perspective Calculations To Fastest

	// Enable front face culling
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);


	////TODO: check if the extension is supported
	/*glPointParameterfEXT(GL_POINT_SIZE_MIN_EXT, 4.f);*/
	//glPointParameterfEXT(GL_POINT_SIZE_MAX_EXT, 4.f);
	

	//glPointParameterfvEXT()
}


void CRenderer::Push(CVertexBuffer* PushMe)
{
	if (m_uiVBNum>=VB_MAX) return;

	m_pVBPTRQueue[m_uiVBNum++].SetVB(PushMe);
}

void CRenderer::RenderVBppl(CVertexBuffer* pVB)
{
	if (pVB==NULL)
	return;
	m_dwCurrentVBType=0;
	DWORD dwVBFaceType=0;


	m_iCurShaderPass=0;
	m_iCurTexUnit=0;

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
        		
		glDepthMask(GL_TRUE);

			// vertex array


			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3,GL_FLOAT,sizeof(CVector3f),pVB->Get(VB_HAS_VERTS));


			// four tex coord unit used to:

			// texcoord
			glClientActiveTextureARB(GL_TEXTURE0_ARB+0); 
			glActiveTextureARB(GL_TEXTURE0_ARB+0);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT,sizeof(CVector2f),pVB->Get(VB_HAS_TEXCOORDS));  	


			

	

			// RESTORE RESTORE RESTORE
			
			


			// tangents

			glClientActiveTextureARB(GL_TEXTURE0_ARB+1); 
			glActiveTextureARB(GL_TEXTURE0_ARB+1);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(3, GL_FLOAT,sizeof(CVector3f),pVB->Get(VB_HAS_TANGENTS));  

			//binormals


			glClientActiveTextureARB(GL_TEXTURE0_ARB+2); 
			glActiveTextureARB(GL_TEXTURE0_ARB+2);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(3, GL_FLOAT,sizeof(CVector3f),pVB->Get(VB_HAS_BINORMALS));  


			glClientActiveTextureARB(GL_TEXTURE0_ARB+3); 
			glActiveTextureARB(GL_TEXTURE0_ARB+3);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(3, GL_FLOAT,sizeof(CVector3f),pVB->Get(VB_HAS_NORMALS));  

		

			glEnable(GL_TEXTURE_2D);



			/*****/ // render ambient pass

			
			if (!pVB->GetBaseTexture())
			{				
				g_cgslShader.BindCGTexture(g_cgslShader.m_cgRegularTexture,pVB->GetBaseTexture()->GetID());					
			}
			else
			{
				g_cgslShader.BindCGTexture(g_cgslShader.m_cgRegularTexture, pVB->GetBaseTexture()->GetID());	
			}

			g_cgslShader.PerFrameSetupBASIC();

			// render 1st pass - only writing z values and no color values
			glDepthMask(GL_TRUE); // enable writing z values
			// hmm - how to disable color writing???
			glDisable(GL_BLEND);


			g_cgslShader.BindFPBASIC();
			g_cgslShader.BindVPBASIC();

			glColor4f(1.f,1.f,1.f,1.f);	
			// draw ambient pass
			glDrawElements(GL_TRIANGLES,pVB->GetIndexNum(),
				GL_UNSIGNED_INT,pVB->GetIndices()); 

			m_uiDrawElementsCallsNum++;

			

			/// light passes /////

			g_cgslShader.BindFPBUMPMAP();
			g_cgslShader.BindVPBUMPMAP();


			if (!pVB->GetBaseTexture())
			{				
				//g_cgslShader.BindCGTexture(g_cgslShader.m_cgRegularTexture,pVB->GetBaseTexture()->GetID());					
				g_cgslShader.BindCGTexture(g_cgslShader.m_cgNormalTexture,pVB->GetNormalTexture()->GetID());
				g_cgslShader.BindCGTexture(g_cgslShader.m_cgSpecularTexture, g_pSpecularTexDEBUG->GetID());
			}
			else
			{
				//g_cgslShader.BindCGTexture(g_cgslShader.m_cgRegularTexture, pVB->GetBaseTexture()->GetID());	
				g_cgslShader.BindCGTexture(g_cgslShader.m_cgNormalTexture, pVB->GetNormalTexture()->GetID());
				g_cgslShader.BindCGTexture(g_cgslShader.m_cgSpecularTexture, pVB->GetBaseTexture()->GetID());
			}
			


		for (int i=0;i<m_iUsedLights;i++)
		{

			CLight* pLight= GetLight(i);
			if (!pLight->m_bAlive)
				continue;



			/*if (i==0) // first pass
			{
				// render 1st pass - only writing z values and no color values
				glDepthMask(GL_TRUE); // enable writing z values
				// hmm - how to disable color writing???
				glDisable(GL_BLEND);
				g_cgslShader.UpdateDiffuseColorBUMPMAP(1.f,1.f,1.f);
				
			}
			else
			{
				glDepthMask(GL_FALSE); // disabling writing z values
				// hmm - how to enable color writing???
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE,GL_ONE);

				g_cgslShader.UpdateDiffuseColorBUMPMAP(1.f,1.f,1.f);
			
			}*/

			glDepthMask(GL_FALSE); // enable writing z values
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);

			
			g_cgslShader.UpdateLightPositionBUMPMAP(pLight->m_vPos.v[0],pLight->m_vPos.v[1],pLight->m_vPos.v[2]);
			g_cgslShader.UpdateLightColorBUMPMAP(pLight->m_vColor.v[0],pLight->m_vColor.v[1],pLight->m_vColor.v[2]);
			g_cgslShader.UpdateLightRadiusBUMPMAP(pLight->m_fRadius);


			

			glColor4f(1.f,1.f,1.f,1.f);

			

			// draw lights passes
			glDrawElements(GL_TRIANGLES,pVB->GetIndexNum(),
				GL_UNSIGNED_INT,pVB->GetIndices()); 

			m_uiDrawElementsCallsNum++;

			m_iCurTexUnit=0;	
			//bDepthMask=false;
		}
		
		
}

void CRenderer::RenderVB(CVertexBuffer* pVB,bool bIsSky,bool bIgnoreVB)
{
	if (pVB==NULL)
		return;
	m_dwCurrentVBType=0;
	DWORD dwVBFaceType=0;
	//UINT  uiShaderIndex=SHADER_MAN_MAX_SHADERS;
	//CShader** ppTempShader;

	CShaderPass* t_pShaderPass;
	CShader*     t_pShader;


	bool bDepthMask=false;

	dwVBFaceType = pVB->GetFaceType();	
	
	t_pShader = pVB->GetShader();

if (!bIgnoreVB)
{		

	m_dwCurrentVBType = pVB->GetVBHasType();

	if (pVB->GetFaceType()==VB_BEZIER_PATCH)
	{
		if (pVB->GetCurrentlyTesselatedLOD()!=g_iUserDefinedBezierPatchTessLevel)
		{
			pVB->ReMakeBezierPatch(g_iUserDefinedBezierPatchTessLevel);
		}			
	}

	glEnableClientState(GL_VERTEX_ARRAY);


	

	if (t_pShader->GetSurfaceParm() & SURFACEPARM_NODRAW)
	{
		return;
	}

	// since I've implemented a method of my own to skyboxes, I don't really care
	// about the geometry that uses shaders marked with SURFACEPARM_SKY.
	// My method is: (based on lot's of help from skyNet, he also implemented the q3 approach)
	// 6 sides of a skybox are formed (with a tesselation factor)
	// for each side I create a new vertexbuffer
	// each side is completely culled if facing away from the camera (here we lose complete 3 skybox sides always)
	// and the floor is never drawn (just like in q3)
	// so eventually 2-3 sides are always drawn.
	// The original q3 method is:
	// "cut" the polygon into sub-polygons the when you shoot a ray from your eye through them,
	// their rays will only hit one side.
	// you go through each sub-polygon and find out polygon it "carves" using those rays.
	// then, you draw those artificially created faces normaly (so you'll get the cloud passes etc.)

	if ((t_pShader->GetSurfaceParm() & SURFACEPARM_SKY) && bIsSky==false )
	{
		return;
	}
}
else //bIgnoreVB == true
{
	glDisableClientState(GL_VERTEX_ARRAY);

}

	SetupShader(t_pShader,pVB,bIgnoreVB);

	m_iCurShaderPass=0;
	m_iCurTexUnit=0;


	////////////////////////////////////////////////////////////////////////////////////
	// all those are used later in order to determine if we need HAVE to force a render
	// (because this/the next pass needs that)
	// the situations that require this are:
	// * RGBGen, ALPHAGen is different then identity ( because GL_COLOR_ARRAY and	glColor4f() both 
	//   operate ONLY on the FIRST tex unit).
	// * If there's any use of alpha test (glAlphaFunc()...)
	// * If it ISN'T an opaque shader (an opaque shader is a shader that its first pass is opaque)
	//
	// this case:
	// 
	// pass 0: GL_ADD  <-- this isn't opaque
	// pass 1: GL_ADD
	// pass 2: GL_ADD
	// pass 3: GL_MODULATE
	// pass 4: GL_MODULATE
	// pass 5: GL_MODULATE
	// pass 6: GL_DECAL
	// pass 7: GL_DECAL
	// pass 8: GL_DECAL
	//
	// would require 5 passes in a card with infinite tex units.
	// all GL_ADD, and GL_MODULATE that are in a row can be rendered using 1 glDrawElements,
	// (they are commutative - after all, they represent the mathematical operations add and multiply)
	// but because GL_DECAL ISN'T commutative, EACH pass should be rendered seperately.
	
	bool bForceRender=false;

	bool bNextIsRGBGen    = false;
	bool bCurrentIsRGBGen = false;
	UINT uiNextRGBGen     = RGBGEN_IDENTITY;
	UINT uiCurrentRGBGen  = RGBGEN_IDENTITY;

	bool bNextIsALPHAGen    = false;
	bool bCurrentIsALPHAGen = false;
	UINT uiNextALPHAGen     = ALPHAGEN_IDENTITY;
	UINT uiCurrentALPHAGen  = ALPHAGEN_IDENTITY;

	bool bNextIsALPHAFunc = false;
	bool bCurrentIsALPHAFunc = false;
	DWORD dwNextALPHAFunc = ALPHAFUNC_NONE;
	DWORD dwCurrentALPHAFunc = ALPHAFUNC_NONE;
	///////////////////////////////////////////////////////////////////////////

//	BREAKPOINT(strcmp(t_pShader->GetName(),"textures/sfx/fanfx")==0);

	// as long as we haven't finished rendering all of this shader passes...
	while (m_iCurShaderPass<t_pShader->GetPassCount())  
	{
		glClientActiveTextureARB(GL_TEXTURE0_ARB + m_iCurTexUnit); //related tex coords arrays
		glActiveTextureARB(GL_TEXTURE0_ARB + m_iCurTexUnit);  //related to the actual texture IMAGE

		//if (!bIgnoreVB)
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);  
		
		glEnable(GL_TEXTURE_2D);


		t_pShaderPass = t_pShader->GetPass(m_iCurShaderPass);

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		SetupShaderPass(t_pShaderPass,t_pShader,m_iCurShaderPass,pVB,bIgnoreVB);

		///////////////////////////////////////////////////////////////////////////
		// determine if we should force render
		///////////////////////////////////////////////////////////////////////////

		
		bNextIsALPHAFunc = false;
		bCurrentIsALPHAFunc = false;
		dwNextALPHAFunc = ALPHAFUNC_NONE;
		dwCurrentALPHAFunc = ALPHAFUNC_NONE;

		if (m_iCurShaderPass<t_pShader->GetPassCount()-1)
		{
			dwNextALPHAFunc = t_pShader->GetPass(m_iCurShaderPass+1)->GetALPHAFunc();
			if (dwNextALPHAFunc!=ALPHAFUNC_NONE)
			{
				bNextIsALPHAFunc=true;
			}
		}


		dwCurrentALPHAFunc = t_pShader->GetPass(m_iCurShaderPass)->GetALPHAFunc();
		if (dwCurrentALPHAFunc!=ALPHAFUNC_NONE)
			bCurrentIsALPHAFunc=true;

		bNextIsRGBGen    = false;
		bCurrentIsRGBGen = false;
		uiNextRGBGen     = RGBGEN_IDENTITY;
		uiCurrentRGBGen  = RGBGEN_IDENTITY;

		if (m_iCurShaderPass<t_pShader->GetPassCount()-1)
		{
			uiNextRGBGen = t_pShader->GetPass(m_iCurShaderPass+1)->GetRGBGen();
			if (uiNextRGBGen!=RGBGEN_IDENTITY)
			{
				bNextIsRGBGen=true;
			}
		}

		uiCurrentRGBGen = t_pShader->GetPass(m_iCurShaderPass)->GetRGBGen();
		if (uiCurrentRGBGen!=RGBGEN_IDENTITY)
			bCurrentIsRGBGen=true;


		bNextIsALPHAGen    = false;
		bCurrentIsALPHAGen = false;
		uiNextALPHAGen     = ALPHAGEN_IDENTITY;
		uiCurrentALPHAGen  = ALPHAGEN_IDENTITY;

		if (m_iCurShaderPass<t_pShader->GetPassCount()-1)
		{
			uiNextALPHAGen = t_pShader->GetPass(m_iCurShaderPass+1)->GetALPHAGen();
			if (uiNextALPHAGen!=ALPHAGEN_IDENTITY)
			{
				bNextIsALPHAGen=true;
			}
		}

		uiCurrentALPHAGen = t_pShader->GetPass(m_iCurShaderPass)->GetALPHAGen();
		if (uiCurrentALPHAGen!=ALPHAGEN_IDENTITY)
			bCurrentIsALPHAGen=true;


		bForceRender=false;

		if (bNextIsRGBGen==true ||	bCurrentIsRGBGen==true ||
			bNextIsALPHAGen==true || bCurrentIsALPHAGen==true ||
			bNextIsALPHAFunc==true || bCurrentIsALPHAFunc==true)

			bForceRender=true;

	
		if ((m_iCurTexUnit==iFakeCardTexUnits-1) || (m_iCurShaderPass==t_pShader->GetPassCount()-1)
			|| bForceRender==true)
		//if ((m_iCurTexUnit==iFakeCardTexUnits-1) || (m_iCurShaderPass==t_pShader->GetPassCount()-1))
		{
			//// "clean" the next tex units available on card so there won't be "leftovers"
			if (m_iCurTexUnit<iFakeCardTexUnits-1)
			{
				for (int i=m_iCurTexUnit+1;i<iFakeCardTexUnits;i++)
				{
					glClientActiveTextureARB(GL_TEXTURE0_ARB+i); 
					glActiveTextureARB(GL_TEXTURE0_ARB+i);
					glDisable(GL_TEXTURE_2D);
					//glDisable(GL_TEXTURE_CUBE_MAP);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);								
				}
			}
				
			if (t_pShader->GetShaderFlags() & FLAG_DEPTHWRITE)
				glDepthMask(GL_TRUE);
			else
				glDepthMask(GL_FALSE);

				
			// RESTORE RESTORE RESTORE

			if (!bIgnoreVB)
			{
				glDrawElements(GL_TRIANGLES,pVB->GetIndexNum(),
					GL_UNSIGNED_INT,pVB->GetIndices());
			}
			

			m_uiDrawElementsCallsNum++;

			m_iCurTexUnit=0;	
			bDepthMask=false;
		}
		else
			m_iCurTexUnit++;

		m_iCurShaderPass++;
		}
	
}

void CRenderer::SetupShader(CShader* pShader, CVertexBuffer* pVB,bool bIgnoreVB)
{
	//TODO:checking if this is the same as before would save some time...

	DWORD dwShaderFlags = pShader->GetShaderFlags();

	if (dwShaderFlags & FLAG_NOCULL)
	{
		if (!(m_dwOpenGLState&RENDERER_STATE_NOCULL)) // only if a change is needed
			{
				glDisable(GL_CULL_FACE);				
				m_dwOpenGLState|=RENDERER_STATE_NOCULL;
				m_dwOpenGLState&=~RENDERER_STATE_CULLFRONT;
				m_dwOpenGLState&=~RENDERER_STATE_CULLBACK;
			}
	}
	else
	if (dwShaderFlags & FLAG_CULLFRONT)
		{
			if (!(m_dwOpenGLState&RENDERER_STATE_CULLFRONT)) // only if a change is needed
				{
					glEnable(GL_CULL_FACE);
					glCullFace(GL_FRONT); 
					m_dwOpenGLState|=RENDERER_STATE_CULLFRONT;
					m_dwOpenGLState&=~RENDERER_STATE_NOCULL;					
					m_dwOpenGLState&=~RENDERER_STATE_CULLBACK;
				}		
		}
	else
	if (dwShaderFlags & FLAG_CULLBACK)
		{
			if (!(m_dwOpenGLState&RENDERER_STATE_CULLBACK)) // only if a change is needed
				{
					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK); 
					m_dwOpenGLState|=RENDERER_STATE_CULLBACK;
					m_dwOpenGLState&=~RENDERER_STATE_NOCULL;					
					m_dwOpenGLState&=~RENDERER_STATE_CULLFRONT;
				}		
		}



	SDeformVertexesObject* t_DVObject;


if (!bIgnoreVB)
{

	if (pShader->GetIsDVUsed()==false) // no deformVertex was used here - use original vertices
		{
			glVertexPointer(3,GL_FLOAT,sizeof(CVector3f),pVB->Get(VB_HAS_VERTS));
		}
	else  // deformvertexes was in use in this shader  
		{
			t_DVObject = pShader->GetDVObject();
			CVector3f*  t_pVertices = (CVector3f*) pVB->Get(VB_HAS_VERTS);
			// only used by autosprite
			UINT*  t_pIndices = pVB->GetIndices();

			assert(pVB->GetSize()<VB_VERTICES_MAX);

//			if (pVB->GetFaceType()!=VB_BEZIER_PATCH)
			{
				// init the temp vertices as the original ones
				for (int s=0; s<pVB->GetSize();s++)
					{
						m_pTempVertexArray[s].v[0] = t_pVertices[s].v[0];
						m_pTempVertexArray[s].v[1] = t_pVertices[s].v[1];
						m_pTempVertexArray[s].v[2] = t_pVertices[s].v[2];
					}
			}

			/////////////////////////////////////////////////////////////////////////
				// I'll have to add more matrix stuff when implementing things more then "static world"
				// (objects that need to be moved from LOCAL->WORLD...)
				// - LOT'S of help on this from DIESEL ENGINE
				if (t_DVObject->m_bIsAutoSprite==true)
				{
					
					CVector3f Center;
					CMatrix M;
										
					M.MatMakeRot(DEG2RAD(g_Camera1.xang),DEG2RAD(g_Camera1.yang),DEG2RAD(g_Camera1.zang));	
					
					//M.Inverse();

					for (int v=0; v<pVB->GetIndexNum();v+=6)
					{
						//Center = (t_pVertices[t_pIndices[v+1]]+t_pVertices[t_pIndices[v+2]])*0.5;
						Center = (m_pTempVertexArray[t_pIndices[v+1]]+m_pTempVertexArray[t_pIndices[v+2]])*0.5;
						// move the middle of the quad to the origin, rotate it so it faces the camera and move it back 
						m_pTempVertexArray[t_pIndices[v]]=Center+(M*(t_pVertices[t_pIndices[v]]-Center));
						m_pTempVertexArray[t_pIndices[v+1]]=Center+(M*(t_pVertices[t_pIndices[v+1]]-Center));
						m_pTempVertexArray[t_pIndices[v+2]]=Center+(M*(t_pVertices[t_pIndices[v+2]]-Center));
						m_pTempVertexArray[t_pIndices[v+5]]=Center+(M*(t_pVertices[t_pIndices[v+5]]-Center));
					}        

				}
			/////////////////////////////////////////////////////////////////////////

			float t_fWaveResult = 0.f;

			// Handle all MOVE commands
			if (t_DVObject->m_iMoveNum>0)
			{
				CVector3f t_VecWaveResult(0.f,0.f,0.f),t_VecWaveTotalResult(0.f,0.f,0.f);

				// sum up all deformVertex MOVE commands into one Vector in order to save some time
				for (int v=0; v<t_DVObject->m_iMoveNum;v++)
				{
					t_fWaveResult = t_DVObject->m_ppMove[v]->Compute(st_Timer::Inst()->TimerGetTimeFAKE());
					t_VecWaveResult.v[0] = t_DVObject->m_MoveVert[v]->v[0];
					t_VecWaveResult.v[1] = t_DVObject->m_MoveVert[v]->v[1];
					t_VecWaveResult.v[2] = t_DVObject->m_MoveVert[v]->v[2];
						t_VecWaveResult= t_VecWaveResult*t_fWaveResult;

					t_VecWaveTotalResult+=t_VecWaveResult;
				}

				// add that Vector into all temp vertices
				for (int s=0; s<pVB->GetSize();s++)
					{
						m_pTempVertexArray[s]+=t_VecWaveTotalResult;
					}        
			}


			
			// Handle all WAVE commands
			CVector3f*  t_pNormals = (CVector3f*) pVB->Get(VB_HAS_NORMALS);

				// I need to fine tune this some more!
				if (t_DVObject->m_iWaveNum>0)
				{
					float t_fW,t_fT;
					CVector3f t_Def(0.f,0.f,0.f);

					for (int v=0; v<t_DVObject->m_iWaveNum;v++)
					{			
						t_fT = 1.f/t_DVObject->m_pWaveDivs[v]*2000.f;

						for (int s=0; s<pVB->GetSize();s++)
						{
							t_fW = t_DVObject->m_ppWave[v]->Compute(
								st_Timer::Inst()->TimerGetTimeFAKE()+t_fT*(t_pVertices[s].v[0]+t_pVertices[s].v[1]+t_pVertices[s].v[2])						);
							t_Def = t_pNormals[s] * t_fW;

							m_pTempVertexArray[s] += t_Def;

						}        
					}
				}

				// Handle all bulge commands

				if (t_DVObject->m_iBulgeNum>0 && pVB->GetFaceType()==VB_BEZIER_PATCH)
				{

					float width,height,speed;

					for (int v=0; v<t_DVObject->m_iBulgeNum;v++)
					{			
						width = t_DVObject->m_ppfBulgeParms[v][0];
						height = t_DVObject->m_ppfBulgeParms[v][1];
						speed = t_DVObject->m_ppfBulgeParms[v][2];

						speed = st_Timer::Inst()->TimerGetTimeSecondsFAKE()/(speed*width);
						speed-= floorf(speed);

						int FinalDimX = (pVB->GetBezierPatchDimX()/2) * (pVB->GetCurrentlyTesselatedLOD()+1);
						int FinalDimY = (pVB->GetBezierPatchDimY()/2) * (pVB->GetCurrentlyTesselatedLOD()+1);


						width/=(float) FinalDimY;

						int	elem=0;

						CVector3f def;
						float w;

						
						for (int y=0;y<FinalDimY;++y)
						{
							w=sinf(((float)y*width+speed)*TWOPI)*height;
							//w=sinf(((float)y*width+speed)*TWOPI);
							for (v=0;v<FinalDimX;++v)
							{	
								def=t_pNormals[elem]*w;
								m_pTempVertexArray[elem]=t_pVertices[elem]+def;
								elem++;
							}
						}				
					}
				}


			glVertexPointer(3,GL_FLOAT,sizeof(CVector3f),m_pTempVertexArray);        
		}

	}

}

void CRenderer::SetupShaderPass(CShaderPass* pShaderPass,CShader* pShader,int iCurTexUnit,CVertexBuffer* pVB,bool bIgnoreVB)
{

	static float fTimeSeconds;
	static float fTimeMS;

	

	fTimeSeconds = st_Timer::Inst()->TimerGetTimeSecondsFAKE();
	fTimeMS = st_Timer::Inst()->TimerGetTimeFAKE();


	float fTimeStartedDEBUG = pVB->GetTimeStarted();

	if (pVB->GetSpecialCounter())
	{
		fTimeSeconds -= fTimeStartedDEBUG;
		fTimeMS -= fTimeStartedDEBUG*1000.f;				
	}

	pShaderPass->CalcActiveFrame(fTimeMS);

	//////////////////
	// TCGen handling
	//////////////////

	DWORD t_TCGen = pShaderPass->GetTCGen();

	
	

		switch(t_TCGen) 
		{
		case TCGEN_BASE:
			if (!bIgnoreVB)
				glTexCoordPointer(2, GL_FLOAT,sizeof(CVector2f),pVB->Get(VB_HAS_TEXCOORDS));  
			glBindTexture(GL_TEXTURE_2D,pShaderPass->GetActiveFrameTexID());
			break;
		case TCGEN_LIGHTMAP:
			if (!bIgnoreVB)
				glTexCoordPointer(2, GL_FLOAT,sizeof(CVector2f),pVB->Get(VB_HAS_LMCOORDS));  
			glBindTexture(GL_TEXTURE_2D,pVB->GetLMTexID());
			break;
		case TCGEN_VECTOR:
			{
				//TODO: implement this - (haven't seen a real map that use that - only test.shader)
			}
			break;
		case TCGEN_ENVIRONMENT:		
			{
								

				// TODO: after adding models that aren't part of the map, i HAVE to take into considiration 
				//       their world matrix

				CVector3f t_Dir;
				CMatrix   M;

			
				M.MatMakeUnit(); // this should be the World Matrix of the model
				M.Inverse();

				CVector3f  t_pPos(g_Camera1.m_Pos.v[0],g_Camera1.m_Pos.v[1],g_Camera1.m_Pos.v[2]);
			
				CVector2f* t_pTexCoords = (CVector2f*) pVB->Get(VB_HAS_TEXCOORDS);			
				CVector3f*  t_pVertices = (CVector3f*) pVB->Get(VB_HAS_VERTS);
				CVector3f*  t_pNormals = (CVector3f*) pVB->Get(VB_HAS_NORMALS);

				if (!t_pNormals)
					break;

				for (int s=0; s<pVB->GetSize();s++)
				{
					t_Dir = t_pPos - t_pVertices[s]; 
					t_Dir.Normalize();
					m_pTempTexCoordsArray[s].v[0] = t_Dir.v[2] - t_pNormals[s].v[2];
					m_pTempTexCoordsArray[s].v[1] = t_Dir.v[1] - t_pNormals[s].v[1];
				}

				glTexCoordPointer(2, GL_FLOAT,sizeof(CVector2f),m_pTempTexCoordsArray);  
				glBindTexture(GL_TEXTURE_2D,pShaderPass->GetActiveFrameTexID());

			}

			break;
	case TCGEN_SKY_CUBEMAP: // this should be TCGEN_SKY_CUBEMAP
		{
			glTexCoordPointer(2, GL_FLOAT,sizeof(CVector2f),g_pSky->m_pSkyBoxCUBETexCoords);  
			//glBindTexture(GL_TEXTURE_2D,g_Sky.m_puiTexturesIndex[m_iCurSkySideDrawn]);		
			glBindTexture(GL_TEXTURE_2D,g_pSky->m_spTexturesIndex[m_iCurSkySideDrawn]->GetID());		

		}	
		break;
	default: // if not a recognized TCGEN then do the default
		if (  (pShaderPass->GetShaderPassFlags() & SPF_ISLIGHTMAP) && (m_dwCurrentVBType & VB_HAS_LMCOORDS))
		{
			if (!bIgnoreVB)
				glTexCoordPointer(2, GL_FLOAT,sizeof(CVector2f),pVB->Get(VB_HAS_LMCOORDS));  
			glBindTexture(GL_TEXTURE_2D,pVB->GetLMTexID());
		}
		else
		{
			if (!bIgnoreVB)
				glTexCoordPointer(2, GL_FLOAT,sizeof(CVector2f),pVB->Get(VB_HAS_TEXCOORDS));  
			glBindTexture(GL_TEXTURE_2D,pShaderPass->GetActiveFrameTexID());
		}
		break;
	}

	

// not required now
/*
	if (t_TCGen!=TCGEN_SKY_CUBEMAP)
	{
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP);
	}*/


	//////////////////////
	// DEPTHFunc handling
	//////////////////////

	DWORD t_dwDEPTHFunc = pShaderPass->GetDEPTHFunc();
	

	// DEBUG DEBUG DEBUG
	if (pVB->m_bAlwaysDraw)
	{
		if (!(m_dwOpenGLState&RENDERER_STATE_DEPTHFUNC_ALWAYS)) // only if a change is needed
		{
			glDepthFunc(GL_ALWAYS);
			m_dwOpenGLState|=RENDERER_STATE_DEPTHFUNC_ALWAYS;
			m_dwOpenGLState&=~RENDERER_STATE_DEPTHFUNC_LEQUAL;
			m_dwOpenGLState&=~RENDERER_STATE_DEPTHFUNC_EQUAL;
		}

	}
	else
	if (t_dwDEPTHFunc==DEPTHFUNC_EQUAL)
	{
		if (!(m_dwOpenGLState&RENDERER_STATE_DEPTHFUNC_EQUAL)) // only if a change is needed
		{
			glDepthFunc(GL_EQUAL);
			m_dwOpenGLState|=RENDERER_STATE_DEPTHFUNC_EQUAL;
			m_dwOpenGLState&=~RENDERER_STATE_DEPTHFUNC_LEQUAL;
			m_dwOpenGLState&=~RENDERER_STATE_DEPTHFUNC_ALWAYS;
		}
	}
	else
	if (t_dwDEPTHFunc==DEPTHFUNC_LEQUAL)
	{
		if (!(m_dwOpenGLState&RENDERER_STATE_DEPTHFUNC_LEQUAL)) // only if a change is needed
		{
			glDepthFunc(GL_LEQUAL);
			m_dwOpenGLState|=RENDERER_STATE_DEPTHFUNC_LEQUAL;
			m_dwOpenGLState&=~RENDERER_STATE_DEPTHFUNC_EQUAL;
			m_dwOpenGLState&=~RENDERER_STATE_DEPTHFUNC_ALWAYS;
		}
	}



	//////////////////
	// TCMod handling
	/////////////////

	STCModObject* t_TCModObject = NULL;

	float Par1,Par2;

	glLoadIdentity();


	// RESTORE RESTORE - CHECKING FPS MODE PROBLEM
    

	if (pShaderPass->GetIsTCModUsed()==true)
	{
		t_TCModObject = pShaderPass->GetTCModObject();

		for (int iCounter=(*t_TCModObject).m_iNum-1;iCounter>=0;iCounter--)
			//for (int iCounter=0;iCounter<(*t_TCModObject).m_iNum;iCounter++)
		{
			switch((*t_TCModObject).m_dwOrder[iCounter])
			{
			case TCMOD_SCALE:
				glScalef((*t_TCModObject).m_pfScale[0],(*t_TCModObject).m_pfScale[1],1.0f);
				break;
			case TCMOD_SCROLL:
				Par1= (*t_TCModObject).m_pfScroll[0] * fTimeSeconds;
				Par2= (*t_TCModObject).m_pfScroll[1] * fTimeSeconds;
				glTranslatef(Par1,-Par2,0.0f);
				break;
			case TCMOD_ROTATE:
				Par1=fmodf((*t_TCModObject).m_pfRotate[0] * fTimeSeconds,360.0);				
				glTranslatef(0.5f,0.5f,0.f);						
				glRotatef(-Par1,0,0,1.0f);
				glTranslatef(-0.5f,-0.5f,0);
				break;
			case TCMOD_STRETCH:
				{
					Par1 = 1.f / (*t_TCModObject).m_pStretch[0].Compute(fTimeMS);
					glTranslatef(0.5f,0.5f,0);
					glScalef(Par1,Par1,1.0f);
					glTranslatef(-0.5f,-0.5f,0);
				}
				break;
			case TCMOD_TRANSFORM:
				{
					float m[16]={
						(*t_TCModObject).m_pfTransform[0],(*t_TCModObject).m_pfTransform[1],0,0,
							(*t_TCModObject).m_pfTransform[2],(*t_TCModObject).m_pfTransform[3],0,0,
							0,0,1,0,
							(*t_TCModObject).m_pfTransform[4],(*t_TCModObject).m_pfTransform[5],0,1};
						glMultMatrixf(m);
				}
				break;
			case TCMOD_TURB: // TODO: THIS IS BUGGY!!!!!!!!!!!
				{
					glTexCoordPointer(2, GL_FLOAT,sizeof(CVector2f),m_pTempTexCoordsArray);  

					CVector2f* pOriginalTexCoords=(CVector2f*)pVB->Get(VB_HAS_TEXCOORDS);


					Par1=t_TCModObject->m_pfTurb[3]*fTimeSeconds+t_TCModObject->m_pfTurb[2];
					Par1-=floorf(Par1);
					Par1*=TWOPI;

					float Par2 = t_TCModObject->m_pfTurb[1];
					for (int s=0; s<pVB->GetSize();s++)
					{					
						m_pTempTexCoordsArray[s].v[0]=pOriginalTexCoords[s].v[0]+sinf(Par1+pOriginalTexCoords[s].v[0])*Par2;
						m_pTempTexCoordsArray[s].v[1]=pOriginalTexCoords[s].v[1]+sinf(Par1+pOriginalTexCoords[s].v[1])*Par2;
					}

				}

				break;
				//default:
			}
		}
	}

	//////////////////////
	// ALPHAFunc handling
	//////////////////////

	DWORD t_dwALPHAFunc = pShaderPass->GetALPHAFunc();


	switch(t_dwALPHAFunc) 
	{
	case ALPHAFUNC_GT0:		
		if (!(m_dwOpenGLState&RENDERER_STATE_ALPHAFUNC_GT0)) // only if a change is needed
		{
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER,0);

			m_dwOpenGLState|=RENDERER_STATE_ALPHAFUNC_GT0;
			m_dwOpenGLState&=~RENDERER_STATE_ALPHAFUNC_LT128;
			m_dwOpenGLState&=~RENDERER_STATE_ALPHAFUNC_GE128;
			m_dwOpenGLState&=~RENDERER_STATE_ALPHAFUNC_NONE;

		}

		break;
	case ALPHAFUNC_LT128:		
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_LESS,0.5);
		break;

	case ALPHAFUNC_GE128:
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GEQUAL,0.5);
		break;
	default:
		glDisable(GL_ALPHA_TEST);
	};


	///////////////////////////////////////////////////
	// RGBGen and Alphagen handling
	// TODO: fine tune the brightening of color array
	///////////////////////////////////////////////////

	UINT t_uiRGBGen=pShaderPass->GetRGBGen();
	UINT t_uiALPHAGen=pShaderPass->GetALPHAGen();
	CColor4* pTempColor;
	float t_fRGBColorR=255,t_fRGBColorG=255,t_fRGBColorB=255;
	float t_fALPHAColor=255;

	bool bMustUseColorArray;

	if ((pVB->VBHas(VB_HAS_COLOR)) == true
		&&
		(pVB->VBHas(VB_HAS_LMCOORDS)==false))
		bMustUseColorArray=true;
	else
		bMustUseColorArray=false;


	// RESTORE RESTORE - CHECKING FPS MODE PROBLEM
    
	/*
	if ( ((!bIgnoreVB) && 
		t_uiRGBGen==RGBGEN_VERTEX || t_uiRGBGen==RGBGEN_ONEMINUSVERTEX || 
		t_uiALPHAGen==ALPHAGEN_VERTEX || t_uiALPHAGen==ALPHAGEN_ONEMINUSVERTEX
		|| (bMustUseColorArray==true  && t_uiRGBGen!=RGBGEN_WAVE && t_uiALPHAGen!=ALPHAGEN_WAVE
		&& pShader->GetAllBlended()==false && t_uiRGBGen!=RGBGEN_CONST)))
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(CColor4),m_pbTempColorArray);

		//assert(_CrtCheckMemory());

		switch(t_uiRGBGen)
		{
		case RGBGEN_VERTEX:

			pTempColor = (CColor4*) pVB->Get(VB_HAS_COLOR);
			for (int j=0; j<pVB->GetSize();j++)
			{
				m_pbTempColorArray[j*4  ] = pTempColor[j].c[0];
				m_pbTempColorArray[j*4+1] = pTempColor[j].c[1];
				m_pbTempColorArray[j*4+2] = pTempColor[j].c[2];
			}

			break;
		case RGBGEN_EXACTVERTEX:// ?????????????????????????????????
			pTempColor = (CColor4*) pVB->Get(VB_HAS_COLOR);
			for (int j=0; j<pVB->GetSize();j++)
			{
				m_pbTempColorArray[j*4  ] = pTempColor[j].c[0];
				m_pbTempColorArray[j*4+1] = pTempColor[j].c[1];
				m_pbTempColorArray[j*4+2] = pTempColor[j].c[2];
			}

			break;
			
		case RGBGEN_ONEMINUSVERTEX:

			pTempColor = (CColor4*) pVB->Get(VB_HAS_COLOR);
			for (int j=0; j<pVB->GetSize();j++)
			{
				m_pbTempColorArray[j*4  ] = 255-pTempColor[j].c[0];
				m_pbTempColorArray[j*4+1] = 255-pTempColor[j].c[1];
				m_pbTempColorArray[j*4+2] = 255-pTempColor[j].c[2];
			}

			break;
		default:
			if (bMustUseColorArray==false) // a regular identity RGBGen (or a case that I'm not handling)
			{
				for (int j=0; j<pVB->GetSize();j++)
				{
					m_pbTempColorArray[j*4] = 255;
					m_pbTempColorArray[j*4+1] = 255;
					m_pbTempColorArray[j*4+2] = 255;
				}
			}
			else  // color array must be forced (even though no keyword exists in the shaderpass)
			{
				if (t_uiRGBGen!=RGBGEN_WAVE)
				{
					pTempColor = (CColor4*) pVB->Get(VB_HAS_COLOR);
					for (int j=0; j<pVB->GetSize();j++)
					{
						m_pbTempColorArray[j*4  ] = pTempColor[j].c[0];
						m_pbTempColorArray[j*4+1] = pTempColor[j].c[1];
						m_pbTempColorArray[j*4+2] = pTempColor[j].c[2];
					}
				}
			}
			break;
		}

		if (!bIgnoreVB)
		switch(t_uiALPHAGen)
		{
		case ALPHAGEN_VERTEX:
			pTempColor = (CColor4*) pVB->Get(VB_HAS_COLOR);
			for (int j=0; j<pVB->GetSize();j++)
			{
				m_pbTempColorArray[j*4+3] = pTempColor[j].c[3];
			}
			break;

		case ALPHAGEN_ONEMINUSVERTEX:
			pTempColor = (CColor4*) pVB->Get(VB_HAS_COLOR);
			for (int j=0; j<pVB->GetSize();j++)
			{
				m_pbTempColorArray[j*4+3] = 255-pTempColor[j].c[3];
			}
			break;
		default:
			if (bMustUseColorArray==false) // a regular identity RGBGen (or a case that I'm not handling)
			{
				for (int j=0; j<pVB->GetSize();j++)
				{
					m_pbTempColorArray[j*4+3] = 255;
				}
			}
			else
			{					
				if (t_uiRGBGen!=RGBGEN_WAVE)
				{
					pTempColor = (CColor4*) pVB->Get(VB_HAS_COLOR);
					for (int j=0; j<pVB->GetSize();j++)
					{
						m_pbTempColorArray[j*4+3] = pTempColor[j].c[3];
					}
				}
			}				
			break;
		}

		// somehow they always get too dark, so this brights them a bit

		byte t_Bright=100;

		for (int j=0; j<pVB->GetSize();j++)
		{
			if (m_pbTempColorArray[j*4]+t_Bright<=255)
				m_pbTempColorArray[j*4]+= t_Bright;
			else
				m_pbTempColorArray[j*4]= 255;

			if (m_pbTempColorArray[j*4+1]+t_Bright<=255)
				m_pbTempColorArray[j*4+1]+= t_Bright;
			else
				m_pbTempColorArray[j*4+1]= 255;

			if (m_pbTempColorArray[j*4+2]+t_Bright<=255)
				m_pbTempColorArray[j*4+2]+= t_Bright;
			else
				m_pbTempColorArray[j*4+2]= 255;

			if (m_pbTempColorArray[j*4+3]+t_Bright<=255)
				m_pbTempColorArray[j*4+3]+= t_Bright;
			else
				m_pbTempColorArray[j*4+3]= 255;
		}


	}
	else // if we WON'T need Vertex Colors*/
	{
		glDisableClientState(GL_COLOR_ARRAY);

		switch(t_uiRGBGen)
		{
		case RGBGEN_WAVE:
			// RESTORE RESTORE RESTORE
			t_fRGBColorR = pShaderPass->ComputeRGBGenWave(fTimeMS);
			// DEBUG DEBUG DEBUG
			//t_fRGBColorR = pShaderPass->ComputeRGBGenWave(fTimeSeconds);
			t_fRGBColorG = t_fRGBColorB = t_fRGBColorR;
			break;
		case RGBGEN_IDENTITY:
			t_fRGBColorR=1.f;
			t_fRGBColorG = t_fRGBColorB = t_fRGBColorR;
			break;
		case RGBGEN_CONST:
			{
				CWave* t_pWave = pShaderPass->GetRGBGenWave();
				t_fRGBColorR = t_pWave->m_fBase;
				t_fRGBColorG = t_pWave->m_fAmplitude;
				t_fRGBColorB = t_pWave->m_fPhase;

			}
			break;		
		case RGBGEN_IDENTITYLIGHTING:  // I'm not using GAMMA so I'm ignoring this...
			t_fRGBColorR=1.f;
			t_fRGBColorG = t_fRGBColorB = t_fRGBColorR;
			break;

		default:
			t_fRGBColorR=1.f;
			t_fRGBColorG = t_fRGBColorB = t_fRGBColorR;
			break;
		}

		switch(t_uiALPHAGen)
		{
		case ALPHAGEN_WAVE:
			t_fALPHAColor = pShaderPass->ComputeALPHAGenWave(fTimeMS);
			break;
		case ALPHAGEN_IDENTITY:
			t_fALPHAColor=1.f;
			break;

		case ALPHAGEN_IDENTITYLIGHTING:  // I'm not using GAMMA so I'm ignoring this...
			t_fALPHAColor=1.f;
			break;

		default:
			t_fALPHAColor=1.f;
			break;
		}
		glColor4f(t_fRGBColorR,t_fRGBColorG,t_fRGBColorB,t_fALPHAColor);
	}


	///////////////////////
	// blendFunc handling
	///////////////////////



	if (m_iCurTexUnit==0) // if the current tex unit of the card things are quite simple...
	{
		glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		if (pShaderPass->GetSrcBlend() == GL_ONE && pShaderPass->GetDstBlend() == GL_ZERO)
		{
			glDisable(GL_BLEND);							
		}	
		else
		{						

			glEnable(GL_BLEND);		
			glBlendFunc(pShaderPass->GetSrcBlend(),pShaderPass->GetDstBlend());
			
		}

	}
	else // if the current tex unit is not the first, we should use glTexEnvi to emulate things
	{							

		if (pShaderPass->GetSrcBlend() == GL_ONE && pShaderPass->GetDstBlend() == GL_ONE)
			glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		else
		if (pShaderPass->GetSrcBlend() == GL_ONE && pShaderPass->GetDstBlend() == GL_ZERO)
			{
				glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			}
		else
		if ( (pShaderPass->GetSrcBlend() == GL_ZERO && pShaderPass->GetDstBlend()== GL_SRC_COLOR)
			|| (pShaderPass->GetSrcBlend() == GL_DST_COLOR && pShaderPass->GetDstBlend()== GL_ZERO)	)
				glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		else
		if (pShaderPass->GetSrcBlend() == GL_SRC_ALPHA && pShaderPass->GetDstBlend() == GL_ONE_MINUS_SRC_ALPHA)
			{
				glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT); 
				glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_INTERPOLATE_EXT); 
				glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_ADD); 

				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE); 
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR); 
				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_TEXTURE); 
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, GL_SRC_ALPHA); 

				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT); 
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR); 
				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_PREVIOUS_EXT); 
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_SRC_ALPHA); 

				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT, GL_TEXTURE); 
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, GL_SRC_ALPHA); 
			}					
		else // default
			glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
}

void CRenderer::RenderAll(float * pSpecialTransform,bool bSort)
{
	// assumes a correct opengl modelview was set

	Sort();

	iFakeCardTexUnits = g_iCardTexUnits;                 // assign the actual num of card tex units     

	m_iCurTexUnit=0;
	m_iCurShaderPass=0;

	// RESTORE THIS RESTORE THIS RESTORE THIS

	// rendering the skybox artificially created vertex buffers
	// (only considering the rotational part of the camera matrix)

	

	// I NEED TO CHANGE THIS TO WORK LIKE IN QUAKE 3!!!

	/*
	 // RESTORE RESTORE RESTORE
	
	if (g_pSky->m_strSkyBoxShaderName!=NULL)
	{
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-g_Camera1.xang,1.0f,0,0);
	glRotatef(-g_Camera1.yang,0,1.0f,0);
	glRotatef(-g_Camera1.zang,0,0,1.0f);
	glColor4f(1.f,1.f,1.f,1.f);

	glDepthRange(1,1); // so it will be always drawn behind


	//// ADD THIS ADD THIS ADD THIS
	
	
	RenderSky();
	
	
	}

	*/

	

	/*glMatrixMode(GL_MODELVIEW);

	// rendering all of the vertexbuffers inside the renderer queue

	glLoadIdentity();
	glRotatef(-g_Camera1.xang,1.0f,0,0); // RESTORE RESTORE RESTORE
	glRotatef(-g_Camera1.yang,0,1.0f,0);
	glRotatef(-g_Camera1.zang,0,0,1.0f);
	glTranslatef(-g_Camera1.m_Pos.v[0],-g_Camera1.m_Pos.v[1],-g_Camera1.m_Pos.v[2]);*/

	// DEBUG DEBUG DEBUG

	
	/*glTranslatef(0.f,0.f,-30.f);*/
	/*glRotatef(-90.f, 1, 0, 0);
	glRotatef(90.f, 0, 1, 0);*/
	

	glDepthRange(0,1); // a regular depth range

	glEnable(GL_CULL_FACE); // RESTORE RESTORE RESTORE 
	//glDisable(GL_CULL_FACE);

	
	
	assert(m_uiVBNum<VB_MAX);

	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-g_Camera1.xang,1.0f,0,0); // RESTORE RESTORE RESTORE
	glRotatef(-g_Camera1.yang,0,1.0f,0);
	glRotatef(-g_Camera1.zang,0,0,1.0f);
	glTranslatef(-g_Camera1.m_Pos.v[0],-g_Camera1.m_Pos.v[1],-g_Camera1.m_Pos.v[2]);



	// make this static or global in some way
	
	CMatrix mIdentity;
	mIdentity.MatMakeUnit();
	static float pfOpenglMatrixIdentity[16];
	mIdentity.ConvertToOpenglMatrix(&pfOpenglMatrixIdentity[0]);
	


	for (int i=0;i<m_uiVBNum;i++)
	{
		if (i>=VB_MAX)
			return;

		

		if (m_pVBPTRQueue[i].GetVB()->m_bAlwaysDraw)
			glDepthRange(0,0.01f); // so it will be always drawn infront
		else
			glDepthRange(0,1); // normal depth range


		

		if (m_pVBPTRQueue[i].GetVB()->m_bSpecialWorldMatrix)
			{
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				if (m_pVBPTRQueue[i].GetVB()->m_bIgnoreCameraMatrix)
				{
					glMatrixMode(GL_MODELVIEW); //
					glLoadIdentity();
					glLoadMatrixf(m_pVBPTRQueue[i].GetVB()->m_pSpecialWorldMatrix);
				}
				else
				{
					glMultMatrixf(m_pVBPTRQueue[i].GetVB()->m_pSpecialWorldMatrix);
				}
			}
		
			if (m_bPPLused)
			{
				g_cgslShader.UpdateModelViewProjectBUMPMAP();

				if (m_pVBPTRQueue[i].GetVB()->m_bSpecialWorldMatrix)
					g_cgslShader.UpdateSpecialTransformBUMPMAP(m_pVBPTRQueue[i].GetVB()->m_pSpecialWorldMatrix);	
				else
					g_cgslShader.UpdateSpecialTransformBUMPMAP(&pfOpenglMatrixIdentity[0]);
			}


		if (m_bPPLused && m_pVBPTRQueue[i].GetVB()->m_bUsingPPL) // if ppl used
			RenderVBppl(m_pVBPTRQueue[i].GetVB());			
		else // quake3 style shaders
			RenderVB(m_pVBPTRQueue[i].GetVB(),false,false);
	
		
		if (m_pVBPTRQueue[i].GetVB()->m_bSpecialWorldMatrix)
		{
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}

	}	
	//g_StateCapturer.Capture();

	m_uiFrameCounter++;


	// DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG 
	m_uiVBNum=0;
}

void CRenderer::Reset(void)
{
	m_iUsedLights = 0;
	m_uiVBNum=0;
	m_uiDrawElementsCallsNum=0;
	glDepthMask(GL_TRUE);
	glDepthFunc (GL_LEQUAL);	
}


void CRenderer::Sort(void)
{
	UINT uiSort;
	CShader* pTempShader;


	for (int i=0;i<m_uiVBNum;i++)
	{
		pTempShader = m_pVBPTRQueue[i].GetVB()->GetShader();
		uiSort = pTempShader->GetShaderSort();
		m_pVBPTRQueue[i].SetSort(uiSort);
	}

	QuickSort<256,CVertexBufferPTR>(m_pVBPTRQueue,m_uiVBNum,50);

	iDebugCounter++;

	/*if (iDebugCounter==2000)
	DebugOutPutSortToFile();*/
}

void CRenderer::InitSkyVBs(void)
{	

	if (g_pSky->m_strSkyBoxShaderName!=NULL)
	{	
		// DEBUG DEBUG
		//m_pSkyVBs = new CVertexBuffer[6];

		CShader* pTempShader;
	
		for (int i=0;i<6;i++)
		{
			m_pSkyVBs[i].SetSize(6*g_pSky->m_iVertsPerSide);
			m_pSkyVBs[i].Set(VB_HAS_VERTS,g_pSky->m_pSkyBoxVerts);			
			m_pSkyVBs[i].Set(VB_HAS_TEXCOORDS,g_pSky->m_pSkyBoxSPHERETexCoords);
			m_pSkyVBs[i].SetIndexCount(g_pSky->m_iIndicesPerSide);
			m_pSkyVBs[i].SetIndices(&g_pSky->m_ipSkyBoxIndices[i*(g_pSky->m_iIndicesPerSide)]);
			m_pSkyVBs[i].SetFaceType(VB_REGULAR);

			pTempShader = CSingleton<CShaderManager>::Inst()->Load(g_pSky->m_strSkyBoxShaderName);
			m_pSkyVBs[i].SetShaderSP(pTempShader);
		}
	}
}

void CRenderer::RenderSky(void)
{
	CVector3f t_CameraLookDirection = g_Camera1.ReturnLookDirectionNormalized();

	for (int i=0;i<6;i++)
	{
		float fDebug = t_CameraLookDirection*g_pSky->m_pSkySidesNormals[i];
		if (t_CameraLookDirection*g_pSky->m_pSkySidesNormals[i] > 0)
			g_pSky->m_bDrawSide[i]=false;
		else
			g_pSky->m_bDrawSide[i]=true;
	}

	

	g_pSky->m_bDrawSide[4] = false; // buttom plane

	glEnable(GL_CULL_FACE); // enable opengls culling, because I'm doing it myself on a wider range 
	                        // (complete sides instead of single faces)


	/////// DEBUG TESTS
	//char TempDebug[200];

	//sprintf(TempDebug,);

	
	//MessageBox (HWND_DESKTOP, TempDebug, "Error", MB_OK | MB_ICONEXCLAMATION);	
	

	//assert(_CrtCheckMemory());

	//MessageBox(g_pWindow->hWnd,"Got to here 1", "Error", MB_OK);
	
	if (g_pSky->m_strSkyBoxShaderName!=NULL)
	{
		for (int i=0;i<6;i++)
		{
			if (i!=3)
			if (g_pSky->m_bDrawSide[i]==true)
			{			
				m_iCurSkySideDrawn = i;
				RenderVB(&m_pSkyVBs[i],true,false);
			}
		}
	}


	//MessageBox(g_pWindow->hWnd,"Got to here 2", "Error", MB_OK);

	//assert(_CrtCheckMemory());

	

}

/*void CRenderer::DebugOutPutSortToFile(void)
{
	std::ofstream t_File;
	t_File.open("DebugSort.txt");

	UINT t_uiShaderIndex = 0;
	UINT t_uiSort = 0;
	int  t_iSort = 0;
	
	for (int i=0;i<m_uiVBNum;i++)	
	{

		t_uiShaderIndex = m_pVBPTRQueue[i].GetVB()->GetShaderIndex();
		t_uiSort = CSingleton<CShaderManager>::Instance()->GetShader(t_uiShaderIndex)->GetShaderSort();
		t_iSort = (int) t_uiSort;
	

		t_File << t_uiShaderIndex;
		t_File << ":";
		
		t_File << """";		
		t_File << CSingleton<CShaderManager>::Instance()->GetShader(t_uiShaderIndex)->GetName();
		t_File << """";		

		t_File << ":";

		t_File << t_iSort;
		t_File << ".";

		//
		//if (t_File << CSingleton<CShaderManager>::Instance()->GetShader(t_uiShaderIndex)->GetShaderFlags() & SORT_OPAQUE)

		t_File << "\n";
		
	}
	t_File.close();	
}*/



UINT CRenderer::GetVBNum(void)
{
	return m_uiVBNum;
}

UINT CRenderer::GetDrawElementsCallsNum(void)
{
	return m_uiDrawElementsCallsNum;
}


//////////////////////
// GetAnchoredValues()
//////////////////////
void CRenderer::GetAnchoredValues(DWORD dwAnchor,int iSizeX, int iSizeY, int iOffsetX, int iOffsetY, int &iLeftX, int &iRightX,int &iUpY,int &iDownY)
{
	if (dwAnchor & ANCHOR_X_MIDDLE)
	{
		iLeftX = (((int)(g_pWindow->width))/2)-iSizeX+iOffsetX;
		iRightX = (((int)(g_pWindow->width))/2)+iSizeX+iOffsetX;
	}
	else if (dwAnchor & ANCHOR_X_MAX)
	{
		iRightX = (((int)(g_pWindow->width)))-iOffsetX;
		iLeftX = (((int)(g_pWindow->width)))-iSizeX-iOffsetX;
	}
	else
	{
		iLeftX = iOffsetX;
		iRightX = iSizeX+iOffsetX;	
	}

	if (dwAnchor & ANCHOR_Y_MIDDLE)
	{
		iDownY = (((int)(g_pWindow->height))/2)-iSizeY+iOffsetY;
		iUpY = (((int)(g_pWindow->height))/2)+iSizeY+iOffsetY;
	}
	else if (dwAnchor & ANCHOR_Y_MAX)
	{
		iUpY = (((int)(g_pWindow->height)))-iOffsetY;
		iDownY = (((int)(g_pWindow->height)))-iSizeY-iOffsetY;
	}
	else
	{
		iDownY = +iOffsetY;
		iUpY = iSizeY+iOffsetY;	
	}
	

}

//////////////////////
// DrawAnchored2DQuad()
//////////////////////
void CRenderer::DrawAnchored2DQuad(CTexture* pTex,DWORD dwAnchor, int iSizeX, int iSizeY,int iOffsetX, int iOffsetY)
{
	int iLeftX,iRightX,iUpY,iDownY;

	if (dwAnchor & ANCHOR_X_MIDDLE)
	{
		iLeftX = (((int)(g_pWindow->width))/2)-iSizeX+iOffsetX;
		iRightX = (((int)(g_pWindow->width))/2)+iSizeX+iOffsetX;
	}
	else if (dwAnchor & ANCHOR_X_MAX)
	{
		iRightX = (((int)(g_pWindow->width)))-iOffsetX;
		iLeftX = (((int)(g_pWindow->width)))-iSizeX-iOffsetX;
	}
	else
	{
		iLeftX = iOffsetX;
		iRightX = iSizeX+iOffsetX;	
	}

	if (dwAnchor & ANCHOR_Y_MIDDLE)
	{
		iDownY = (((int)(g_pWindow->height))/2)-iSizeY+iOffsetY;
		iUpY = (((int)(g_pWindow->height))/2)+iSizeY+iOffsetY;
	}
	else if (dwAnchor & ANCHOR_Y_MAX)
	{
		iUpY = (((int)(g_pWindow->height)))-iOffsetY;
		iDownY = (((int)(g_pWindow->height)))-iSizeY-iOffsetY;
	}
	else
	{
		iDownY = +iOffsetY;
		iUpY = iSizeY+iOffsetY;	
	}
	DebugDraw2DQuad(pTex,iLeftX,iRightX,iDownY,iUpY,false);
}



//////////////////////
// DebugDraw2DQuad()
//////////////////////

void CRenderer::DebugDraw2DQuad(CTexture* pTex,int iLeftX,int iRightX,int iUpY,int iDownY,bool bBlending)
{

	/// if (pTex==NULL) then it means that all tex units etc. was prepared for us by the renderer
	/// so there's only need to set projection/modelview matrices, and render the primitive(s)


	if (pTex)
	{
		glDepthRange(1.f,1.f);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		glEnable(GL_TEXTURE_2D);

		glDisable(GL_BLEND);
	}
		

	glMatrixMode(GL_PROJECTION); 
	glPushMatrix(); 
	glLoadIdentity(); 



	glOrtho( 0, st_Input::Inst()->GetScreenWidth(), st_Input::Inst()->GetScreenHeight(), 0, 0, 1 );
	

	if (pTex)
	{
		glDisable(GL_DEPTH_TEST); 

		if (bBlending)
		{
			glBlendFunc(GL_ONE,GL_ONE);
			glEnable(GL_BLEND);
		}

		glColor4f(1.f,1.f,1.f, 0.5f); 
	}
	
	static float fLeftX;
	static float fRightX;
	static float fUpY;
	static float fDownY;


	fLeftX = ((float)iLeftX);
	fRightX = ((float)iRightX);
	fUpY = ((float)iUpY);
	fDownY = ((float)iDownY);


	glBegin(GL_QUADS); 
	/*glTexCoord2f(0.0f, 0.0f); glVertex2f(fLeftX,fDownY); 
	glTexCoord2f(1.0f, 0.0f); glVertex2f(fRightX,fDownY); 
	glTexCoord2f(1.0f, 1.0f); glVertex2f(fRightX,fUpY); 
	glTexCoord2f(0.0f, 1.0f); glVertex2f(fLeftX,fUpY); */
	glTexCoord2f(0.0f, 0.0f); glVertex2f(fLeftX,fUpY); 
	glTexCoord2f(1.0f, 0.0f); glVertex2f(fRightX,fUpY); 
	glTexCoord2f(1.0f, 1.0f); glVertex2f(fRightX,fDownY); 
	glTexCoord2f(0.0f, 1.0f); glVertex2f(fLeftX,fDownY); 
	glEnd(); 


	/*glBegin(GL_QUADS); 
	glTexCoord2f(0.0f, 0.0f); glVertex2f(a.v[0],a.v[1]); 
	glTexCoord2f(1.0f, 0.0f); glVertex2f(b.v[0],b.v[1]); 
	glTexCoord2f(1.0f, 1.0f); glVertex2f(c.v[0],c.v[1]); 
	glTexCoord2f(0.0f, 1.0f); glVertex2f(d.v[0],d.v[1]); 
	glEnd(); */

	glPopMatrix(); 

	glMatrixMode(GL_MODELVIEW); 

}


////////////////////////////
// DebugDraw2DQuadPercent
////////////////////////////
void CRenderer::DebugDraw2DQuadPrecent(CTexture* pTex,int iLeftX,int iRightX,int iUpY,int iDownY,bool bBlending)
{
/// if (pTex==NULL) then it means that all tex units etc. was prepared for us by the renderer
/// so there's only need to set projection/modelview matrices, and render the primitive(s)


	if (pTex)
	{
		glDepthRange(1.f,1.f);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		glEnable(GL_TEXTURE_2D);

		glDisable(GL_BLEND);
	}
		

	glMatrixMode(GL_PROJECTION); 
	glPushMatrix(); 
	glLoadIdentity(); 


	glOrtho( 0, st_Input::Inst()->GetScreenWidth(), st_Input::Inst()->GetScreenHeight(), 0, 0, 1 );
	

	if (pTex)
	{
		glDisable(GL_DEPTH_TEST); 

		if (bBlending)
		{
			glBlendFunc(GL_ONE,GL_ONE);
			glEnable(GL_BLEND);
		}

		glColor4f(1.f,1.f,1.f, 0.5f); 
	}
	
	static float fLeftX;
	static float fRightX;
	static float fUpY;
	static float fDownY;


	fLeftX = ((float)g_pWindow->width)* ((float)iLeftX) * 0.01f;
	fRightX = ((float)g_pWindow->width)* ((float)iRightX) * 0.01f;
	fUpY = ((float)g_pWindow->height)* ((float)iUpY) * 0.01f;
	fDownY = ((float)g_pWindow->height)* ((float)iDownY) * 0.01f;

	glBegin(GL_QUADS); 
	glTexCoord2f(0.0f, 0.0f); glVertex2f(fLeftX,fDownY); 
	glTexCoord2f(1.0f, 0.0f); glVertex2f(fRightX,fDownY); 
	glTexCoord2f(1.0f, 1.0f); glVertex2f(fRightX,fUpY); 
	glTexCoord2f(0.0f, 1.0f); glVertex2f(fLeftX,fUpY); 
	glEnd(); 

	glPopMatrix(); 

	glMatrixMode(GL_MODELVIEW); 

}




//////////////////////
// DebugDraw2DQuad()
//////////////////////
void CRenderer::DebugDraw2DQuadInitAllOpenGLStuff(CTexture* pTex,CVector2f& a,CVector2f& b,CVector2f& c,CVector2f& d,bool bBlending)
{

	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();

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

	/*************/

	if (!pTex)
		return;

	glDepthRange(1.f,1.f);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);


	glEnable(GL_TEXTURE_2D);

	pTex->Bind();

	glDisable(GL_BLEND);

		


	glPushMatrix(); 

	glLoadIdentity(); 
	glMatrixMode(GL_PROJECTION); 
	glPushMatrix(); 
	glLoadIdentity(); 

	//glOrtho(-1,1,-1,1,-1,1); 

     //glOrtho( 0, g_GlobalPrefs.ScreenSize.iX, g_GlobalPrefs.ScreenSize.iY, 0, 0, 1 );      

	//glOrtho(0.f,1,-1,1,-1,1);

	/*glOrtho( 0, 800, 600, 0, 0, 1 );      */

	// remember this is only good for mouse cursor, not for menus
	glOrtho( 0, st_Input::Inst()->GetScreenWidth(), st_Input::Inst()->GetScreenHeight(), 0, 0, 1 );
	

	glDisable(GL_DEPTH_TEST); 


	if (bBlending)
	{
		glBlendFunc(GL_ONE,GL_ONE);
		glEnable(GL_BLEND);
	}
	

	glColor4f(1.f,1.f,1.f, 0.5f); 

	glBegin(GL_QUADS); 
	glTexCoord2f(0.0f, 0.0f); glVertex2f(a.v[0],a.v[1]); 
	glTexCoord2f(1.0f, 0.0f); glVertex2f(b.v[0],b.v[1]); 
	glTexCoord2f(1.0f, 1.0f); glVertex2f(c.v[0],c.v[1]); 
	glTexCoord2f(0.0f, 1.0f); glVertex2f(d.v[0],d.v[1]); 

	/*glTexCoord2f(0.0f, 1.0f); glVertex2f(200,200); 
	glTexCoord2f(1.0f, 1.0f); glVertex2f(200,400); 
	glTexCoord2f(1.0f, 0.0f); glVertex2f(400,400); 
	glTexCoord2f(0.0f, 0.0f); glVertex2f(400,200); */

	glEnd(); 

	glPopMatrix(); 

	glMatrixMode(GL_MODELVIEW); 

	glPopMatrix();
}


///////////////////////////
// DrawFullScreenPic()
///////////////////////////
void CRenderer::DrawFullScreenPic(CTexture* pTex)
{

	/*

	if (!pTex)
	return;


	glEnable(GL_TEXTURE_2D);	
	g_cgslShader.BindCGTexture(g_cgslShader.m_cgRegularTexture, pTex->GetID());

	glBegin(GL_QUADS); 

	glTexCoord2f(0.0f, 1.0f); glVertex3f(-5.25f,-5.25f,-10.f); 
	glTexCoord2f(1.0f, 1.0f); glVertex3f(5.25f,-5.25f,-10.f); 
	glTexCoord2f(1.0f, 0.0f); glVertex3f(5.25f,5.25f,-10.f); 
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.25f,5.25f,-10.f);

	glEnd();
	*/


	if (!pTex)
		return;

	glEnable(GL_TEXTURE_2D);	
	pTex->Bind();


	glPushMatrix(); 

	glLoadIdentity(); 
	glMatrixMode(GL_PROJECTION); 
	glPushMatrix(); 
	glLoadIdentity(); 

	glOrtho(-1,1,-1,1,-1,1); 

	glDisable(GL_DEPTH_TEST); 
	glColor4f(1.f,1.f,1.f, 0.5f); 

	glBegin(GL_QUADS); 
	/*glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.f,-1.f); 
	glTexCoord2f(1.0f, 0.0f); glVertex2f(1.f,-1.f); 
	glTexCoord2f(1.0f, 1.0f); glVertex2f(1.f,1.f); 
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.f,1.f); */

	glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.f,-1.f); 
	glTexCoord2f(1.0f, 1.0f); glVertex2f(1.f,-1.f); 
	glTexCoord2f(1.0f, 0.0f); glVertex2f(1.f,1.f); 
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.f,1.f);


	glEnd(); 

	glPopMatrix(); 

	glMatrixMode(GL_MODELVIEW); 

	glPopMatrix();

}



void CRenderer::DrawText(void)
{
	

		glMatrixMode(GL_PROJECTION); 
		glPushMatrix(); 
		glMatrixMode(GL_MODELVIEW); 
		glPushMatrix(); 




		glDepthFunc (GL_ALWAYS);
		glDisable(GL_CULL_FACE);


		glMatrixMode(GL_PROJECTION);
		glLoadIdentity ();	
		//glOrtho(-100.0, 100.0, -100.0, 100.0, -1.0, 1.0);
		glOrtho( 0, st_Input::Inst()->GetScreenWidth(), st_Input::Inst()->GetScreenHeight(), 0, 0, 1 );

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();


		// eliminate any "left overs"
		for (int i=0; i<g_iCardTexUnits;i++)
		{
			glClientActiveTextureARB(GL_TEXTURE0_ARB+i); 
			glActiveTextureARB(GL_TEXTURE0_ARB+i);
			glDisable(GL_TEXTURE_2D);
			//			glDisable(GL_TEXTURE_CUBE_MAP);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}



		glDisable(GL_BLEND);

		glDepthMask(GL_FALSE);
		glDepthFunc (GL_ALWAYS);	


		glColor4f(0.f,1.f,0.f,1.f);


		// draw the incoming text messages
		


		/*glRasterPos2f(60.f,70.f);
		g_Fonts.glPrint("BLAH BLAH BLAH");	// Print GL Text To The Screen*/

		g_TextMessages.UpdateMessages();
		g_TextMessages.DrawMessages();




	if (g_bDrawText)
		{
        
#ifdef _DEBUG

		glRasterPos2f(st_Input::Inst()->GetScreenWidth()-80.f,90.f);
		glRasterPos2f(80.f, 140.f);
		g_Fonts.glPrint("DEBUG");	// Print GL Text To The Screen
#else
		glRasterPos2f(st_Input::Inst()->GetScreenWidth()-100.f,140.f);
		g_Fonts.glPrint("RELEASE");	// Print GL Text To The Screen
#endif


		glRasterPos2f(10.f, 90.f);
		g_Fonts.glPrint("FPS:%.1f",st_Timer::Inst()->m_fFPS);

		char temp[400];

		if (st_Engine::Inst()->m_iGameState==1) // FPS mode
		if (!st_Engine::Inst()->m_bSwitchingGameState)
		{
			glRasterPos2f(18.f, 200.f);
			sprintf(temp,"Player1 Pos:%.1f,%.1f,%.1f",g_pPlayerClient1->GetPhysicsModel()->GetPosition().v[0],g_pPlayerClient1->GetPhysicsModel()->GetPosition().v[1],g_pPlayerClient1->GetPhysicsModel()->GetPosition().v[2]);
			g_Fonts.glPrint(temp);
			//g_TextMessages.AddMessage(temp,1.f,0.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);

			glRasterPos2f(18.f, 220.f);
			sprintf(temp,"Player2 Pos:%.1f,%.1f,%.1f",g_pPlayerClient2->GetPhysicsModel()->GetPosition().v[0],g_pPlayerClient2->GetPhysicsModel()->GetPosition().v[1],g_pPlayerClient2->GetPhysicsModel()->GetPosition().v[2]);
			//g_TextMessages.AddMessage(temp,0.f,1.f,0.f,TEXT_MESSAGE_TYPE_SYSTEM);
			g_Fonts.glPrint(temp);
		}

		glColor4f(0.f,1.f,0.f,1.f);
		glRasterPos2f(st_Input::Inst()->GetScreenWidth()/2,150.f);
		if (g_Communication.GetCommunicationType()==COMMUNICATION_NOT_SET)
			g_Fonts.glPrint("Communication Not Set.");	// Print GL Text To The Screen
		else
		if (g_Communication.GetCommunicationType()==COMMUNICATION_SERVER)
			g_Fonts.glPrint("Server Server Server");	// Print GL Text To The Screen
		else
		if (g_Communication.GetCommunicationType()==COMMUNICATION_CLIENT)
			g_Fonts.glPrint("Client Client Client");	// Print GL Text To The Screen


		glColor4f(0.f,1.f,0.f,1.f);
		glRasterPos2f(st_Input::Inst()->GetScreenWidth()/2,170.f);
		g_Fonts.glPrint(&g_Communication.RecieveBoardMessageString[0]);	// Print GL Text To The Screen


		glRasterPos2f(70.f, 80.f);

		if (st_Engine::Inst()->m_iGameState==1) // if we are STILL in the FPS (and aren`t in the process of going back to board)
		{

		if (g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->getPlayerInAir())
			g_Fonts.glPrint("In Air");	// Print GL Text To The Screen
		else
			g_Fonts.glPrint("On Ground");	// Print GL Text To The Screen

		glRasterPos2f(70.f, 70.f);
		g_Fonts.glPrint("%.1f",g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fTimePlayerInAir);	// Print GL Text To The Screen
		

		glRasterPos2f(-98.f, 70.f);
		g_Fonts.glPrint("Player Ang:%.1f,%.1f",g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngX,g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_fAngY);

		glRasterPos2f(-98.f, 30.f);
		g_Fonts.glPrint("Velocity:%.4f,%.4f,%.4f",g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetVelocity().v[0],g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetVelocity().v[1],g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetVelocity().v[2]);
		

			glRasterPos2f(-98.f, 10.f);
		if (g_bBotDistanceToFloorTooBig)
			g_Fonts.glPrint("Move Would Lead To Fall!");
		else
			g_Fonts.glPrint("Move Safe.");



		glRasterPos2f(-98.f, 0.f);
		if (g_bBotJumpsWithJumpPad)
			g_Fonts.glPrint("Bot Jumping On JumpPad!");
		else
			g_Fonts.glPrint("Not.");

        

		glRasterPos2f(-98.f, -10.f);
		if (g_iBotSteeringMode == 0)
			g_Fonts.glPrint("No Steering");
		else
		if (g_iBotSteeringMode == 1)
			g_Fonts.glPrint("Steering LEFT");
		if (g_iBotSteeringMode == 2)
			g_Fonts.glPrint("Steering RIGHT");


		glRasterPos2f(-98.f, -20.f);
		if (g_bDirectLineOfSight)
			g_Fonts.glPrint("Direct Line Of Sight!");
		else
			g_Fonts.glPrint("Can't see player.");

		}


		/*glRasterPos2f(-60.f, 90.f);
		g_Fonts.glPrint("Tex Units:%d ",g_iCardTexUnits);*/

		//glRasterPos2f(10.f, 90.f);
		//g_Fonts.glPrint("FPS:%.1f",st_Timer::Inst()->m_fFPS);


		glRasterPos2f(-28.f, 80.f);
		g_Fonts.glPrint("Draw:%.3f",st_EngineI->m_fEntireDrawTimerD);

		glRasterPos2f(-28.f, 70.f);
		g_Fonts.glPrint("Draw1:%.3f",st_EngineI->m_fDrawPart1D);

		glRasterPos2f(-28.f, 60.f);
		g_Fonts.glPrint("Draw1-1:%.3f",st_EngineI->m_fDrawPart11D);

		glRasterPos2f(-28.f, 50.f);
		g_Fonts.glPrint("Draw1-2:%.3f",st_EngineI->m_fDrawPart12D);

		/*glRasterPos2f(-48.f, 60.f);
		g_Fonts.glPrint("Draw2:%.3f",st_EngineI->m_fDrawPart2D);

		glRasterPos2f(-48.f, 50.f);
		g_Fonts.glPrint("Draw3:%.3f",st_EngineI->m_fDrawPart3D);

		glRasterPos2f(-48.f, 40.f);
		g_Fonts.glPrint("Draw4:%.3f",st_EngineI->m_fDrawPart4D);

		glRasterPos2f(-48.f, 30.f);
		g_Fonts.glPrint("Update:%.3f",st_EngineI->m_fEntireUpdateTimerD);*/


		
		

		



		glRasterPos2f(-98.f, 60.f);
		g_Fonts.glPrint("Camera Leaf:%d",st_Engine::Inst()->m_iCameraLeafNum);

		glRasterPos2f(-98.f, 50.f);
		g_Fonts.glPrint("Camera Cluster:%d",st_Engine::Inst()->m_iCameraClusterNum);

		glRasterPos2f(-98.f, 40.f);
		g_Fonts.glPrint("glDrawElements:%d",m_uiDrawElementsCallsNum);



		



		/*glRasterPos2f(-98.f, 30.f);
		g_Fonts.glPrint("MD3 Objects:%d",st_MD3BaseManagerI->GetModelsNum());

		glRasterPos2f(-98.f, 20.f);
		g_Fonts.glPrint("MD3 Sub-Meshes:%d",st_MD3MeshBaseManagerI->GetModelsNum());*/


		/*glRasterPos2f(-98.f, 30.f);
		g_Fonts.glPrint("Distance between players:%f",g_fDistanceBetweenPlayer);*/


		/*glRasterPos2f(-98.f, 30.f);
		g_Fonts.glPrint("Hit Surface Flags:%d",g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_iFlagsOfTouchedGround);

		glRasterPos2f(-98.f, 20.f);
		g_Fonts.glPrint("Hit Surface Contents:%d",g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_iContentsOfTouchedGround);*/

		glRasterPos2f(-98.f, 20.f);
		g_Fonts.glPrint("Dynamic Entities Drawn:%d",g_iNumberOfDynamicEntitiesDrawn);


	

		

		
				
		

		

/*		glRasterPos2f(-50.f, 10.f);
		g_Fonts.glPrint("Entity 0: angX:%.3f, angY:%.3f",g_pQuake3Map->m_ppAllDynamicEntities[0]->m_pPlayerClient->m_pPlayerModel->m_fAngX,g_pQuake3Map->m_ppAllDynamicEntities[0]->m_pPlayerClient->m_pPlayerModel->m_fAngY);
		glRasterPos2f(-50.f, 0.f);
		g_Fonts.glPrint("Entity 0: angX:%.3f, angY:%.3f",g_pQuake3Map->m_ppAllDynamicEntities[0]->m_pPlayerClient->m_PhysicsModel.m_fAngX,g_pQuake3Map->m_ppAllDynamicEntities[0]->m_pPlayerClient->m_PhysicsModel.m_fAngY);


		glRasterPos2f(-50.f, -10.f);
		g_Fonts.glPrint("Entity 1: angX:%.3f, angY:%.3f",g_pQuake3Map->m_ppAllDynamicEntities[1]->m_pPlayerClient->m_pPlayerModel->m_fAngX,g_pQuake3Map->m_ppAllDynamicEntities[1]->m_pPlayerClient->m_pPlayerModel->m_fAngY);
		glRasterPos2f(-50.f, -20.f);
		g_Fonts.glPrint("Entity 1: angX:%.3f, angY:%.3f",g_pQuake3Map->m_ppAllDynamicEntities[1]->m_pPlayerClient->m_PhysicsModel.m_fAngX,g_pQuake3Map->m_ppAllDynamicEntities[1]->m_pPlayerClient->m_PhysicsModel.m_fAngY);*/
	

		glRasterPos2f(30.f,70.f);
		g_Fonts.glPrint("Camera Position: %.3f, %.3f, %.3f",g_Camera1.m_Pos.v[0],g_Camera1.m_Pos.v[1],g_Camera1.m_Pos.v[2]);
		glRasterPos2f(30.f,50.f);
		g_Fonts.glPrint("Camera Angles: %.3f, %.3f, %.3f",g_Camera1.xang,g_Camera1.yang,g_Camera1.zang);


		glRasterPos2f(0.f, 10.f);
		//g_Fonts.glPrint("Mouse: %.3f, %.3f",st_Input::Inst()->GetRelativeX(false),st_Input::Inst()->GetRelativeY(false));
		//g_Fonts.glPrint("Mouse: %.3f, %.3f",g_fMouseX,g_fMouseY);
		g_Fonts.glPrint("Square: %d, %d",g_PointedSquareIndexX,g_PointedSquareIndexY);
		

		

		


		/*float fT = 20.f;

		for (int i=0;i<g_pQuake3Map->m_pLeafs[st_Engine::Inst()->m_iCameraLeafNum].DynamicEntities.size();i++)
		{			
			glRasterPos2f(-98.f, fT);

			g_Fonts.glPrint("Entity:%d",(int) (&g_pQuake3Map->m_pLeafs[st_Engine::Inst()->m_iCameraLeafNum].DynamicEntities[i]));

			fT-=10.f;
		}*/


		/*
		float fT = 20.f;

		for (int i=0;i<g_pQuake3Map->m_ppAllDynamicEntities[0]->inLeafs.size();i++)
		{			
			glRasterPos2f(-98.f, fT);
			g_Fonts.glPrint("Leaf:%d",g_pQuake3Map->m_ppAllDynamicEntities[0]->inLeafs[i]);

			fT-=10.f;
		}



		fT = 20.f;

		for (int i=0;i<g_pQuake3Map->m_ppAllDynamicEntities[0]->inLeafsTEMP.size();i++)
		{			
			glRasterPos2f(-68.f, fT);
			g_Fonts.glPrint("Leaf:%d",g_pQuake3Map->m_ppAllDynamicEntities[0]->inLeafsTEMP[i]);

			fT-=10.f;
		}


		*/
		
	



		
		

/*		glRasterPos2f(-98.f, 30.f);
		g_Fonts.glPrint("LOWER Frame:%d, Next Frame: %d",g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->GetLower()->m_iCurrentFrame,g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->GetLower()->m_iNextFrame);

		glRasterPos2f(-98.f, 20.f);
		g_Fonts.glPrint("UPPER Frame:%d, Next Frame: %d",g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->GetUpper()->m_iCurrentFrame,g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->GetUpper()->m_iNextFrame);

		glRasterPos2f(-98.f, 10.f);
		g_Fonts.glPrint("Head Frame:%d, Next Frame: %d",g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->GetHead()->m_iCurrentFrame,g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->GetHead()->m_iNextFrame);

		glRasterPos2f(-98.f, 0.f);
		g_Fonts.glPrint("WEAPON Frame:%d, Next Frame: %d",g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->GetWeapon()->m_iCurrentFrame,g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->GetWeapon()->m_iNextFrame);*/

				


		
		/*glRasterPos2f(0.f, 40.f);
		if (g_iShootTestContents & CONTENTS_TELEPORTER)
			g_Fonts.glPrint("CONTENTS_TELEPORTER");

		glRasterPos2f(0.f, 30.f);
		if (g_iShootTestContents & CONTENTS_JUMPPAD)
			g_Fonts.glPrint("CONTENTS_JUMPPAD");

		glRasterPos2f(0.f, 20.f);
		if (g_iShootTestContents & CONTENTS_NODROP)
			g_Fonts.glPrint("CONTENTS_NODROP");

		glRasterPos2f(0.f, 10.f);
		if (g_iShootTestContents & CONTENTS_DONOTENTER)
			g_Fonts.glPrint("CONTENTS_DONOTENTER");

		glRasterPos2f(0.f, 0.f);
		if (g_iShootTestContents & CONTENTS_CORPSE)
			g_Fonts.glPrint("CONTENTS_CORPSE");

		

		
*/
		
		

		
		

/*

		glRasterPos2f(-98.f, 30.f);
		if (g_bCollidedWithStaticEntity)
			g_Fonts.glPrint("Collided With Entity!");
		else
			g_Fonts.glPrint("Didn't Collide With Entity");


		glRasterPos2f(-98.f, 20.f);		
		g_Fonts.glPrint("HeightAboveGround:%f",st_Engine::Inst()->m_fHeightAboveGround);

		bool bTemp = g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetJumping();
		glRasterPos2f(-98.f, 10.f);				
		if (bTemp)
			g_Fonts.glPrint("Jumping:TRUE");
		else
			g_Fonts.glPrint("Jumping:FALSE");


		bTemp = g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetStartJumping();
		glRasterPos2f(-98.f, 0.f);				
		if (bTemp)
			g_Fonts.glPrint("StartJumping:TRUE");
		else
			g_Fonts.glPrint("StartJumping:FALSE");


		
		glRasterPos2f(-98.f, -10.f);				
		if (g_bFinalDirectionIsOppositeThenDesired)
			g_Fonts.glPrint("FinalOppositeThenDesired:TRUE");
		else
			g_Fonts.glPrint("FinalOppositeThenDesired:FALSE");

		

		*/



		/*int m_iCameraClusterNum;
	int m_iCameraLeafNum;*/

		


	}


	

	if (st_Engine::Inst()->m_iGameState==1) // FPS mode
	if (!st_Engine::Inst()->m_bSwitchingGameState)
	{

		/*glColor4f(1.f,0.f,0.f,1.f);
		glRasterPos2f(20.f, 90.f);		
		g_Fonts.glPrint("Score: %d",g_iPlayerScore);


		glColor4f(0.f,0.f,1.f,1.f);
		glRasterPos2f(-98.f, -88.f);
		g_Fonts.glPrint("Weapon:");

		glColor4f(1.f,0.f,0.f,1.f);
		glRasterPos2f(-80.f, -88.f);
		if (g_pCurrentlyProcessedPlayerClient->m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_WEAPON_SHOTGUN)
			g_Fonts.glPrint("ShotGun");
		else
		if (g_pCurrentlyProcessedPlayerClient->m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_WEAPON_ROCKET_LAUNCHER)
			g_Fonts.glPrint("Rocket Launcher");
		else
		if (g_pCurrentlyProcessedPlayerClient->m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_WEAPON_PLASMA)
			g_Fonts.glPrint("Plasma");
		else
		if (g_pCurrentlyProcessedPlayerClient->m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_NO_WEAPON)
			g_Fonts.glPrint("No Weapon");*/


		glColor4f(0.f,0.f,1.f,1.f);
		glRasterPos2f(0, st_Input::Inst()->GetScreenHeight()-20.f);
		g_Fonts.glPrint("Life:");

		if (g_pCurrentlyProcessedPlayerClient->m_iLife<=30)
			glColor4f(1.f,0.f,0.f,1.f);
		else
			glColor4f(0.f,1.f,0.f,1.f);
		glRasterPos2f(40.f, st_Input::Inst()->GetScreenHeight()-20.f);

		if (g_pCurrentlyProcessedPlayerClient->m_iLife<0.f || g_pCurrentlyProcessedPlayerClient->m_iLife>300.f)
		{
			glColor4f(1.f,0.f,0.f,1.f);
			g_Fonts.glPrint("0");
		}
		else
			g_Fonts.glPrint("%d",g_pCurrentlyProcessedPlayerClient->m_iLife);



		/*glColor4f(0.f,0.f,1.f,1.f);
		glRasterPos2f(-78.f, -98.f);
		g_Fonts.glPrint("Mana:");

		if (g_pCurrentlyProcessedPlayerClient->m_iMana<=30)
			glColor4f(1.f,0.f,0.f,1.f);
		else
			glColor4f(0.f,1.f,0.f,1.f);
		glRasterPos2f(-64.f, -98.f);
		g_Fonts.glPrint("%d",g_pCurrentlyProcessedPlayerClient->m_iMana);*/




		glColor4f(0.f,0.f,1.f,1.f);
		glRasterPos2f(80.f, st_Input::Inst()->GetScreenHeight()-20.f);
		g_Fonts.glPrint("Ammo:");

	if (g_pCurrentlyProcessedPlayerClient->m_bInfiniteAmmo)
	{
			glColor4f(0.f,1.f,0.f,1.f);
			glRasterPos2f(135.f, st_Input::Inst()->GetScreenHeight()-20.f);
			g_Fonts.glPrint("Infinite");

	}
	else
	{

		if (g_pCurrentlyProcessedPlayerClient->m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_WEAPON_SHOTGUN)
		{
			if (g_pCurrentlyProcessedPlayerClient->m_iAmmoBullets<=30)
				glColor4f(1.f,0.f,0.f,1.f);
			else
				glColor4f(0.f,1.f,0.f,1.f);
			
			glRasterPos2f(135.f, st_Input::Inst()->GetScreenHeight()-20.f);
			g_Fonts.glPrint("%d",g_pCurrentlyProcessedPlayerClient->m_iAmmoBullets);
		}
		else
		if (g_pCurrentlyProcessedPlayerClient->m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_WEAPON_ROCKET_LAUNCHER)
		{
			if (g_pCurrentlyProcessedPlayerClient->m_iAmmoRockets<=5)
				glColor4f(1.f,0.f,0.f,1.f);
			else
				glColor4f(0.f,1.f,0.f,1.f);
			glRasterPos2f(135.f, st_Input::Inst()->GetScreenHeight()-20.f);
			g_Fonts.glPrint("%d",g_pCurrentlyProcessedPlayerClient->m_iAmmoRockets);
		}
		else
		if (g_pCurrentlyProcessedPlayerClient->m_dwCurrentlyHeldWeapon == PLAYER_HOLDING_WEAPON_PLASMA)
		{
			if (g_pCurrentlyProcessedPlayerClient->m_iAmmoShells<=5)
				glColor4f(1.f,0.f,0.f,1.f);
			else
				glColor4f(0.f,1.f,0.f,1.f);
			glRasterPos2f(135.f, st_Input::Inst()->GetScreenHeight()-20.f);
			g_Fonts.glPrint("%d",g_pCurrentlyProcessedPlayerClient->m_iAmmoShells);
		}
	}



		


		/*if (g_pCurrentlyProcessedPlayerClient->m_bRedArmor)
		{
			if (g_pCurrentlyProcessedPlayerClient->m_fTimeHoldingRedArmor>15.f)
				glColor4f(1.f,0.f,0.f,1.f);
			else
				glColor4f(0.f,1.f,0.f,1.f);
				

			// make it blink if going to end
			if (g_pCurrentlyProcessedPlayerClient->m_fTimeHoldingRedArmor>15.f)
			{
				CWave wave;
				wave.SetParams(WF_SIN,0.8f,1.2f,0.f,2.f);

				float fTemp = wave.Compute(st_Timer::Inst()->TimerGetTimeFAKE());

				if (fTemp>0.5f)
				{
					glRasterPos2f(12.f, -98.f);
					g_Fonts.glPrint("Red Armor");
				}
			}
			else
			{
				glRasterPos2f(12.f, -98.f);
				g_Fonts.glPrint("Red Armor");
			}
		}
		
		




		
		if (g_pCurrentlyProcessedPlayerClient->m_bYellowArmor)
		{
			if (g_pCurrentlyProcessedPlayerClient->m_fTimeHoldingYellowArmor>15.f)
				glColor4f(1.f,0.f,0.f,1.f);
			else
				glColor4f(0.f,1.f,0.f,1.f);
				

			// make it blink if going to end
			if (g_pCurrentlyProcessedPlayerClient->m_fTimeHoldingYellowArmor>15.f)
			{
				CWave wave;
				wave.SetParams(WF_SIN,0.8f,1.2f,0.f,2.f);

				float fTemp = wave.Compute(st_Timer::Inst()->TimerGetTimeFAKE());

				if (fTemp>0.5f)
				{
					glRasterPos2f(30.f, -98.f);
					g_Fonts.glPrint("Yellow Armor");
				}
			}
			else
			{
				glRasterPos2f(30.f, -98.f);
				g_Fonts.glPrint("Yellow Armor");
			}
		}*/

}
	



		/*glColor4f(0.f,0.f,1.f,1.f);
		glRasterPos2f(-56.f, -98.f);
		g_Fonts.glPrint("Shells:");

		if (g_pCurrentlyProcessedPlayerClient->m_iAmmoBullets<=30)
			glColor4f(1.f,0.f,0.f,1.f);
		else
			glColor4f(0.f,1.f,0.f,1.f);
		glRasterPos2f(-42.f, -98.f);
		g_Fonts.glPrint("%d",g_pCurrentlyProcessedPlayerClient->m_iAmmoBullets);*/

		
		if (st_Engine::Inst()->m_iGameState==1) // FPS mode
		if (g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->m_iContentsOfTouchedGround & (CONTENTS_SLIME | CONTENTS_LAVA))
		{
			/*glColor4f(1.f,0.f,0.f,1.f);
			glRasterPos2f(-5.f, 0.f);
			g_Fonts.glPrint("Ouch!!!");*/
		}



		if (st_Engine::Inst()->m_iGameState==0) // Board mode
		{

			float fTemp;
			

			glColor4f(1.f,1.f,1.f,1.f);
			


			// add combo if needed
			static char combo[400];

			if (g_pCheckersBoard->m_iBoardGameState==GAME_BOARD_STATE_CHOOSE_TARGET_SPELL)
			{
				int iTempIndex = g_pCheckersBoard->BoardsSquaresUnits[g_pCheckersBoard->m_iChosenUnitX][g_pCheckersBoard->m_iChosenUnitY].m_iIndexToPlayersArray;

				if (iTempIndex>=0)
				{
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_FIREBALL)
						strcpy(&combo[0],"FIREBALL Spell! choose target.");
						else
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_SWITCH_PLACES_OWN)
					{
						if (g_pCheckersBoard->m_iChosenSpellTargetNum==0)
							strcpy(&combo[0],"SWITCH PLACES OWN Spell! choose first target.");
						else
							strcpy(&combo[0],"SWITCH PLACES OWN Spell! choose second target.");
					}
					else
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_SWITCH_PLACES_ENEMY)
					{
						if (g_pCheckersBoard->m_iChosenSpellTargetNum==0)
							strcpy(&combo[0],"SWITCH PLACES ENEMY Spell! choose first target.");
						else
							strcpy(&combo[0],"SWITCH PLACES ENEMY Spell! choose second target.");
					}
					else
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_CHARM)
						strcpy(&combo[0],"CHARM Spell! choose target.");
					else
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_PARALYSE)
						strcpy(&combo[0],"PARALYSE Spell! choose target.");
					else
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_HEAL)
						strcpy(&combo[0],"HEAL Spell! choose target.");
					else
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_SHOCKWAVE)
						strcpy(&combo[0],"SHOCKWAVE Spell! click anywhere on the board.");
					
					

					fTemp = ((float) (strlen(&combo[0])));
					fTemp/=2.f;
					fTemp*=13.f;

					glRasterPos2f( (st_Input::Inst()->GetScreenWidth()/2)-fTemp,g_pGUIManager->m_pTopMenu->m_iPosY+134);
					g_Fonts.glPrint(&combo[0]);
				}
			}
			else
			if (g_pCheckersBoard->m_iBoardGameState==GAME_BOARD_STATE_SHOWING_SPELL_ANIMATION)
			{
				int iTempIndex = g_pCheckersBoard->BoardsSquaresUnits[g_pCheckersBoard->m_iChosenUnitX][g_pCheckersBoard->m_iChosenUnitY].m_iIndexToPlayersArray;

				if (iTempIndex>=0)
				{
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_FIREBALL)
						strcpy(&combo[0],"FIREBALL Spell!");
						else
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_SWITCH_PLACES_OWN)
						strcpy(&combo[0],"SWITCH PLACES OWN Spell!");
					else
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_SWITCH_PLACES_ENEMY)
						strcpy(&combo[0],"SWITCH PLACES ENEMY Spell!");
					else
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_SHOCKWAVE)
						strcpy(&combo[0],"SHOCKWAVE Spell!");
					else
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_CHARM)
						strcpy(&combo[0],"CHARM Spell!");
					else
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_PARALYSE)
						strcpy(&combo[0],"PARALYSE Spell!");
					else
					if (g_pCheckersBoard->m_ppGameUnits[iTempIndex]->m_pPlayerClient->m_iSpellType==SPELL_TYPE_HEAL)
						strcpy(&combo[0],"HEAL Spell!");
						
					

					fTemp = ((float) (strlen(&combo[0])));
					fTemp/=2.f;
					fTemp*=13.f;

					glRasterPos2f( (st_Input::Inst()->GetScreenWidth()/2)-fTemp,g_pGUIManager->m_pTopMenu->m_iPosY+134);
					g_Fonts.glPrint(&combo[0]);
				}

			}
			else
			if (g_pCheckersBoard->m_iBoardGameState==GAME_BOARD_STATE_CHOOSE_TARGET_COMBO)
			{
				strcpy(&combo[0],&g_pCheckersBoard->caPlayerInfoLine[0]);
				strcpy(&combo[strlen(g_pCheckersBoard->caPlayerInfoLine)]," - Choose Next Combo Target!");

				
				if (g_pCheckersBoard->m_fComboMoveTimeLeft>666.f)
					strcpy(&combo[strlen(combo)]," 3");
				else
				if (g_pCheckersBoard->m_fComboMoveTimeLeft>333.f)
					strcpy(&combo[strlen(combo)]," 2");
				else
				if (g_pCheckersBoard->m_fComboMoveTimeLeft>0.f)
					strcpy(&combo[strlen(combo)]," 1");

				fTemp = ((float) (strlen(&combo[0])));
				fTemp/=2.f;
				fTemp*=13.f;

				glRasterPos2f( (st_Input::Inst()->GetScreenWidth()/2)-fTemp,g_pGUIManager->m_pTopMenu->m_iPosY+134);
				g_Fonts.glPrint(&combo[0]);
			}
			else
			{

				fTemp = ((float) (strlen(&g_pCheckersBoard->caPlayerInfoLine[0])));
				fTemp/=2.f;
				fTemp*=13.f;

				glRasterPos2f( (st_Input::Inst()->GetScreenWidth()/2)-fTemp,g_pGUIManager->m_pTopMenu->m_iPosY+134);
				g_Fonts.glPrint(&g_pCheckersBoard->caPlayerInfoLine[0]);
			}

			if (g_pCheckersBoard->m_iBoardGameState!=GAME_BOARD_STATE_MENU)
			{		
				if (g_pCheckersBoard->m_iSelectedUnitExperiencePoints>=0)
				{
					if (g_pCheckersBoard->m_iSelectedUnitExperiencePoints==0)
						glColor4f(0.f,0.f,0.f,1.f);
					else
					if (g_pCheckersBoard->m_iSelectedUnitExperiencePoints<3)
						glColor4f(0.f,1.f,0.f,1.f);
					else
						glColor4f(1.f,0.f,0.f,1.f);

					glRasterPos2f(g_pGUIManager->m_pBoardCharacterMenu->m_iPosX+128.f
					,g_pGUIManager->m_pBoardCharacterMenu->m_iPosY+280.f);

					g_Fonts.glPrint("%d",g_pCheckersBoard->m_iSelectedUnitExperiencePoints);							
				}



				if (g_pCheckersBoard->m_iSelectedUnitHitPoints>=0)
				{
					if (g_pCheckersBoard->m_iSelectedUnitHitPoints==0)
						glColor4f(0.f,0.f,0.f,1.f);
					else
					if (g_pCheckersBoard->m_iSelectedUnitHitPoints<3)
						glColor4f(0.f,1.f,0.f,1.f);
					else
						glColor4f(1.f,0.f,0.f,1.f);

					glRasterPos2f(g_pGUIManager->m_pBoardCharacterMenu->m_iPosX+120.f
					,g_pGUIManager->m_pBoardCharacterMenu->m_iPosY+255.f);

					g_Fonts.glPrint("%d",g_pCheckersBoard->m_iSelectedUnitHitPoints);							
				}


				

				
				
				

									
			/*	if (g_pCheckersBoard->m_iBoardGameState==GAME_BOARD_STATE_CHOOSE_TARGET_COMBO)
				{
					glColor4f(1.f,0.f,0.f,1.f);
					glRasterPos2f(40.f, 160.f);
					g_Fonts.glPrint("COMBO MODE");
					glRasterPos2f(40.f, 180.f);
					g_Fonts.glPrint("Make Next Move!");


					glColor4f(1.f,0.f,0.f,1.f);
					glRasterPos2f(55.f, 200.f);
					if (g_pCheckersBoard->m_fComboMoveTimeLeft>666.f)
						g_Fonts.glPrint("3",g_pCheckersBoard->m_fComboMoveTimeLeft);
					else
					if (g_pCheckersBoard->m_fComboMoveTimeLeft>333.f)
						g_Fonts.glPrint("2",g_pCheckersBoard->m_fComboMoveTimeLeft);
					else
					if (g_pCheckersBoard->m_fComboMoveTimeLeft>0.f)
						g_Fonts.glPrint("1",g_pCheckersBoard->m_fComboMoveTimeLeft);

				}*/
			
				

				/*glColor4f(1.f,0.f,0.f,1.f);
				glRasterPos2f(50.f,160.f);
				g_Fonts.glPrint("PM Past:%d",g_pCheckersBoard->m_CheckersAI.m_iCurrentPossibleMovesPAST);
				glColor4f(1.f,0.f,0.f,1.f);
				glRasterPos2f(50.f,180.f);
				g_Fonts.glPrint("PM Future:%d",g_pCheckersBoard->m_CheckersAI.m_iCurrentPossibleMovesFUTURE);*/
			}
			

		}


		g_pGUIManager->RenderText();


		glPopAttrib();


		glMatrixMode(GL_MODELVIEW); 
		glPopMatrix(); 

		glMatrixMode(GL_PROJECTION); 
		glPopMatrix(); 

}


//////////////////
// DebugDrawQuad()
//////////////////
void CRenderer::DebugDrawQuad(CTexture* pTex,CVector3f& a,CVector3f& b,CVector3f& c,CVector3f& d,bool bBlending)
{   



	//glDepthRange(1.f,1.f);

	glDepthRange(0.f,1.f);

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);


	glEnable(GL_TEXTURE_2D);

	pTex->Bind();

	if (bBlending)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE,GL_ONE);
				
		////////

		//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
	}
	else
		glDisable(GL_BLEND);

		

	glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);			glVertex3f( a.v[0],a.v[1],a.v[2]);
    glTexCoord2f(1.0f, 1.0f);			glVertex3f( b.v[0],b.v[1],b.v[2]);
    glTexCoord2f(1.0f, 0.0f);			glVertex3f( c.v[0],c.v[1],c.v[2]);
	glTexCoord2f(0.0f, 0.0f);			glVertex3f( d.v[0],d.v[1],d.v[2]);
	glEnd();
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

//////////////////
// DebugDrawQuadUsedRenderer()
//////////////////
void CRenderer::DebugDrawQuadUsedRenderer(CTexture* pTex,CVector3f& a,CVector3f& b,CVector3f& c,CVector3f& d,bool bBlending)
{   
	glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);			glVertex3f( a.v[0],a.v[1],a.v[2]);
    glTexCoord2f(1.0f, 1.0f);			glVertex3f( b.v[0],b.v[1],b.v[2]);
    glTexCoord2f(1.0f, 0.0f);			glVertex3f( c.v[0],c.v[1],c.v[2]);
	glTexCoord2f(0.0f, 0.0f);			glVertex3f( d.v[0],d.v[1],d.v[2]);
	glEnd();
	
/*	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);*/
}



////////////////////////////////
// DebugDrawLineLoop()
///////////////////////////////
void CRenderer::DebugDrawLineLoop(CVector3f* verts,int iNum)
{
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINE_LOOP);			
	for (int i=0;i<iNum;i++)
		glVertex3f( verts[i].v[0],verts[i].v[1],verts[i].v[2]);
	glEnd();
}

////////////////////////////////
// DebugDrawLineLoopINDICES()
///////////////////////////////
void CRenderer::DebugDrawLineLoopINDICES(CVector3f* verts,UINT* indices,int iIndicesNum)
{
	glDisable(GL_TEXTURE_2D);

	
	for (int i=0;i<iIndicesNum/3;i++)
	{
		glBegin(GL_LINE_LOOP);			
		glVertex3f( verts[indices[i*3]].v[0],verts[indices[i*3]].v[1],verts[indices[i*3]].v[2]);
		glVertex3f( verts[indices[i*3 +1]].v[0],verts[indices[i*3 +1]].v[1],verts[indices[i*3 +1]].v[2]);
		glVertex3f( verts[indices[i*3 +2]].v[0],verts[indices[i*3 +2]].v[1],verts[indices[i*3 +2]].v[2]);
		glEnd();
	}
	

}

/////////////////////
// DebugDrawLine()
/////////////////////
void CRenderer::DebugDrawLine(CVector3f& vStart, CVector3f& vEnd)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINE_LOOP);								// Begin Drawing Quads
	glVertex3f( vStart.v[0],vStart.v[1],vStart.v[2]);
	glVertex3f( vEnd.v[0],vEnd.v[1],vEnd.v[2]);
	glEnd();
}

void CRenderer::DebugDrawBBox(CVector3f& vMin, CVector3f& vMax)
{
	CVector3f vStart,vEnd;

	CVector3f v1(vMin.v[0],vMin.v[1],vMax.v[2]);
	CVector3f v2(vMax.v[0],vMin.v[1],vMax.v[2]);
	CVector3f v3(vMax.v[0],vMin.v[1],vMin.v[2]);
	CVector3f v4(vMin.v[0],vMin.v[1],vMin.v[2]);
	CVector3f v5(vMin.v[0],vMax.v[1],vMax.v[2]);
	CVector3f v6(vMax.v[0],vMax.v[1],vMax.v[2]);
	CVector3f v7(vMax.v[0],vMax.v[1],vMin.v[2]);
	CVector3f v8(vMin.v[0],vMax.v[1],vMin.v[2]);


	DebugDrawLine(v1,v2);
	DebugDrawLine(v1,v4);
	DebugDrawLine(v1,v5);
	DebugDrawLine(v2,v3);
	DebugDrawLine(v3,v4);
	DebugDrawLine(v2,v6);
	DebugDrawLine(v5,v8);
	DebugDrawLine(v7,v8);
	DebugDrawLine(v6,v5);
	DebugDrawLine(v6,v7);
	DebugDrawLine(v4,v8);
	DebugDrawLine(v3,v7);

	
}


//////////////////
// DebugDrawPoly()
//////////////////
void CRenderer::DebugDrawPoly(CVector3f* verts, int iNum)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_POLYGON);
	//glBegin(GL_LINE_LOOP);
	for (int i=0;i<iNum;i++)
		glVertex3f( verts[i].v[0],verts[i].v[1],verts[i].v[2]);
	glEnd();
}


void CRenderer::DebugDrawTri(CTexture* pTex,CVector3f& a,CVector3f& b,CVector3f& c)
{
/*	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);*/

	glDepthRange(1.f,1.f);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);


	glEnable(GL_TEXTURE_2D);

	glBegin(GL_TRIANGLES);
    glTexCoord2f(0.0f, 1.0f);			glVertex3f( a.v[0],a.v[1],a.v[2]);
    glTexCoord2f(1.0f, 1.0f);			glVertex3f( b.v[0],b.v[1],b.v[2]);
    glTexCoord2f(0.0f, 0.0f);			glVertex3f( c.v[0],c.v[1],c.v[2]);
	glEnd();
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}



//////////////////////
// DebugDrawNormal()
//////////////////////
void CRenderer::DebugDrawNormal(CVector3f& origin,CVector3f& direction,float length)
{

	CVector3f target;
	target = origin + (direction*length);

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);								// Begin Drawing Quads
	glColor3f(1.f,0.f,0.f);
	glVertex3f( origin.v[0],origin.v[1],origin.v[2]);
	glColor3f(0.f,1.f,0.f);
	glVertex3f( target.v[0],target.v[1],target.v[2]);

	glEnd();

	glPopAttrib();
}

CLight* CRenderer::AddLight(void)
{
	for (int i=0;i<m_iUsedLights;i++)
		if (!(m_ppLights[i]->m_bAlive))
		{
			m_ppLights[i]->m_bAlive=true;
			return m_ppLights[i];
		}

	// need to make new place

	if (!m_ppLights[m_iUsedLights])
		m_ppLights[m_iUsedLights] = new CLight;


	m_ppLights[m_iUsedLights]->m_bAlive=true;

	m_iUsedLights++;
	return m_ppLights[m_iUsedLights-1];

	
}

/*void CRenderer::ClearLights(void)
{
	m_iUsedLights = 0;
}*/

void CRenderer::InitTexUnits(void)
 {
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

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
 }