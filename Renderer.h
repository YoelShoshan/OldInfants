#ifndef YOEL_RENDERER_H
#define YOEL_RENDERER_H

#pragma once

#include "VertexBuffer.h"



//#include "ZedTracker.h"

// renderer states - used for optimization (less opengl state changes)

#define RENDERER_STATE_DEPTHFUNC_EQUAL   (1<<0)
#define RENDERER_STATE_DEPTHFUNC_LEQUAL  (1<<1)
#define RENDERER_STATE_DEPTHFUNC_ALWAYS  (1<<2)

#define RENDERER_STATE_ALPHAFUNC_GT0     (1<<3)
#define RENDERER_STATE_ALPHAFUNC_LT128   (1<<4)
#define RENDERER_STATE_ALPHAFUNC_GE128   (1<<5)
#define RENDERER_STATE_ALPHAFUNC_NONE    (1<<6)

#define RENDERER_STATE_NOCULL            (1<<7)
#define RENDERER_STATE_CULLFRONT         (1<<8)
#define RENDERER_STATE_CULLBACK          (1<<9)




#define ANCHOR_X_MIDDLE  (1<<0)
#define ANCHOR_X_MAX     (1<<1)
#define ANCHOR_Y_MIDDLE  (1<<2)
#define ANCHOR_Y_MAX     (1<<3)


#define RENDERER_MAX_LIGHTS 50

// TODO: Fine tuning of those is quite important...
#define VB_MAX 14000
//#define VB_VERTICES_MAX 4000
#define VB_VERTICES_MAX 8000

#define st_Renderer CSingleton<CRenderer>
#define st_RendererI CSingleton<CRenderer>::Inst()

#include "Light.h"

class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	
	void InitGL(void); // inits gl related issues

	void Push(CVertexBuffer* PushMe); // push into the queue

	void RenderVB(CVertexBuffer* pVB,bool bIsSky,bool bIgnoreVB); // render a single VertexBuffer

	void RenderVBppl(CVertexBuffer* pVB);

	void SetupShader(CShader* pShader,CVertexBuffer* pVB,bool bIgnoreVB);

	//// TODO: maybe I should make iCurTexUnit and pVB into member variables inside CRenderer, so I won't have 
	////       to pass them all the time.
	void SetupShaderPass(CShaderPass* pShaderPass,CShader* pShader,int iCurTexUnit,CVertexBuffer* pVB,bool bIgnoreVB);
	
	// render everything that's inside the queue
	void RenderAll(float * pSpecialTransform,bool bSort);
	
	void Reset(void);

	void Sort(void);

	// Render Sky

	void RenderSky(void);

	void InitSkyVBs(void);
	
	// Debug Stuff

	//void DebugOutPutSortToFile(void);

	UINT GetVBNum(void);

	UINT GetDrawElementsCallsNum(void);


	float m_fTimeSinceLastBezierTessLODCheck;





	/// DEBUG DRAW

	void DrawText(void);

	void DrawFullScreenPic(CTexture* pTex);

	//////////////////////
	// DebugDraw2DQuad()
	//////////////////////
	//void DebugDraw2DQuad(CTexture* pTex,CVector2f& a,CVector2f& b,CVector2f& c,CVector2f& d,bool bBlending);
	void DebugDraw2DQuad(CTexture* pTex,int iLeftX,int iRightX,int iUpY,int iDownY,bool bBlending);


	///////////////////////
	// DrawAnchored2DQuad()
	///////////////////////
	void DrawAnchored2DQuad(CTexture* pTex,DWORD dwAnchor, int iSizeX, int iSizeY,int iOffsetX, int iOffsetY);


	//////////////////////
	// GetAnchoredValues()
	//////////////////////
	void GetAnchoredValues(DWORD dwAnchor,int iSizeX, int iSizeY, int iOffsetX, int iOffsetY, int &iLeftX, int &iRightX,int &iUpY,int &iDownY);


	////////////////////////////
	// DebugDraw2DQuadPercent()
	////////////////////////////
	void DebugDraw2DQuadPrecent(CTexture* pTex,int iLeftX,int iRightX,int iUpY,int iDownY,bool bBlending);

	/*//////////////////////
	// DebugDraw2DQuad()
	//////////////////////
	void DebugDraw2DQuadUsedRenderer(CVector2f& a,CVector2f& b,CVector2f& c,CVector2f& d,bool bBlending);*/


	//////////////////////
	// DebugDraw2DQuad()
	//////////////////////
	void DebugDraw2DQuadInitAllOpenGLStuff(CTexture* pTex,CVector2f& a,CVector2f& b,CVector2f& c,CVector2f& d,bool bBlending);



	//////////////////
	// DebugDrawQuad()
	//////////////////
	void DebugDrawQuad(CTexture* pTex,CVector3f& a,CVector3f& b,CVector3f& c,CVector3f& d,bool bBlending);

	//////////////////
	// DebugDrawQuadUsedRenderer()
	//////////////////
	void DebugDrawQuadUsedRenderer(CTexture* pTex,CVector3f& a,CVector3f& b,CVector3f& c,CVector3f& d,bool bBlending);

	//////////////////
	// DebugDrawPoly()
	//////////////////
	void DebugDrawPoly(CVector3f* verts, int iNum);


	//////////////////
	// DebugDrawTri()
	//////////////////
	void DebugDrawTri(CTexture* pTex,CVector3f& a,CVector3f& b,CVector3f& c);




	////////////////////////////////
	// DebugDrawLineLoop()
	///////////////////////////////
	void DebugDrawLineLoop(CVector3f* verts,int iNum);



	////////////////////////////////
	// DebugDrawLineLoopINDICES()
	///////////////////////////////
	void DebugDrawLineLoopINDICES(CVector3f* verts,UINT* indices,int iIndicesNum);

	/////////////////////
	// DebugDrawLine()
	/////////////////////
	void DebugDrawLine(CVector3f& vStart, CVector3f& vEnd);

	void DebugDrawBBox(CVector3f& vMin, CVector3f& vMax);

	//////////////////////
	// DebugDrawNormal()
	//////////////////////
	void DebugDrawNormal(CVector3f& origin,CVector3f& direction,float length);


	void InitTexUnits(void);

	unsigned int m_iTotalFrameCounter;


	unsigned int m_uiFrameCounter;

	bool m_bPPLused;


	CLight* GetLight(int iNum) { return m_ppLights[iNum];};

	CLight* AddLight(void);

	/*void ClearLights(void);*/
	
private:


	DWORD m_dwOpenGLState; // a bitset of opengl states

	// the big CVertexBuffer pointers queue
	CVertexBufferPTR* m_pVBPTRQueue;

		
	// temp arrays - those temp arrays are used because in many cases the original 
	// values of the vertex buffers elements need to be stay the same as they were
	// when loaded from the bsp file. by using temp color, rgbGen, for example, wouldn't alter 
	// the original values.
	// those arrays are ALSO used by the real-time LOD bezier-patch tesselation mechanism

	byte*       m_pbTempColorArray;      // temp color array 
	CVector2f*  m_pTempTexCoordsArray;   // temp tex coords array
	CVector2f*  m_pTempLMCoordsArray;    // temp tex coords array
	CVector3f*  m_pTempVertexArray;      // temp vertices array
	UINT*       m_puiTempIndicesArray;   // temp indices array


	int   m_iCurrentBezierPatchSubPatchesNum;
	int   m_iCurrentBezierPatchVerticesNum;
	int   m_iCurrentBezierPatchIndicesNum;

	
	
	UINT m_uiVBNum; // the number of ACTIVE vertex buffers in queue
	
	UINT m_uiDrawElementsCallsNum; // num of glDrawElements() calls
	
	int m_iCurTexUnit;       // the CURRENT CARD tex unit being processed
	int m_iCurShaderPass;    // the CURRENT SHADER pass being processed

	DWORD m_dwCurrentVBType; // the CURRENT VertexBuffer type


	// sky 
	CVertexBuffer* m_pSkyVBs;
	//CVertexBuffer m_pSkyVBs[6];

	int m_iCurSkySideDrawn;
	

	

	

	// debug


	int iDebugCounter;

	int iFakeCardTexUnits; // used in order to fake a different tex units num then the card actuall has

	// lights
	int m_iUsedLights;
	CLight** m_ppLights;

};








#endif // YOEL_RENDERER_H