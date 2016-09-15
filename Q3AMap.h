#ifndef YOEL_Q3AMAP_H
#define YOEL_Q3AMAP_H

#pragma once

#include "Texture.h"
#include "Math3D.h"
#include "VertexBuffer.h"
#include "EntitiesParser.h"
#include "Entities.h"

#include <vector>

#include "SurfaceFlags.h"

#include "Q3MapDefenitions.h"

#include "SoundManager.h"
extern CSoundManager g_SoundManager;

//#include "ZedTracker.h"


//#define BIGGEST_CLIMB_SLOPE 0.25f
#define BIGGEST_CLIMB_SLOPE 0.5f

#define MAX_STATIC_ENTITIES_NUM   500
#define MAX_DYNAMIC_ENTITIES_NUM  500

class CQuake3BSP
{

public:

	//////////////////////////// CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	// Constructer
	//////////////////////////// CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	CQuake3BSP();

	//////////////////////////// ~CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	// Destructer
	//////////////////////////// ~CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	~CQuake3BSP();

    //////////////////////////// LoadBSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	// load the q3 bsp map from file
	//////////////////////////// LoadBSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	bool LoadBSP(const char *strFileName);

	//////////////////////////// FillVBArray \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// Fill the VertexBuffer Array
	//////////////////////////// FillVBArray \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void FillVBArray();

	//////////////////////////// PushAllToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// push all the vertexbuffers to the renderer (used mainly for debugging)
	//////////////////////////// PushAllToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void PushAllToRenderer(void);

	//////////////////////////// PushAllBezierPatchesToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// push ONLY the bezier patches
	//////////////////////////// PushAllBezierPatchesToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void PushAllBezierPatchesToRenderer(void);


	//////////////////////////// PushFaceToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// push a single face to the renderer
	//////////////////////////// PushFaceToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void PushFaceToRenderer(UINT uiNum);
	
	//////////////////////////// FindLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// returns the leaf index for a position in space
	//////////////////////////// FindLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	int FindLeaf(const CVector3f &vPos);


	//////////////////////////// FindLeafs \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// fills the recieved leaf indices with the leafs that intersect with the given bounding sphere
	//////////////////////////// FindLeafs \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void FindLeafs(LEAFS_INDICES_VECTOR& fillMe,const CVector3f &vPos,float fRadius,int iNode);

	
	//////////////////////////// IsClusterVisible \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// is "test" cluster is potentially visible from "current" cluster?
	//////////////////////////// IsClusterVisible \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	inline int IsClusterVisible(int current, int test);

	/////////////////////////// RenderLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// render a leaf
	/////////////////////////// RenderLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void RenderLeaf( int iLeaf, bool CheckFrustum );

	/////////////////////////// RenderTree \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// render the BSP tree
	/////////////////////////// RenderTree \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void RenderTree( int Node, bool CheckFrustum );

	////////////////////////// RenderLevel \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// render the Q3 BSP level
	////////////////////////// RenderLevel \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void RenderLevel(const CVector3f &vPos);


	void RenderMapModels(void);

	void DrawMapModelsBBoxes(void);


	void DrawMapLeafsBBoxes(void);

	////////////////////////// SetMapFileName \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// set the map file name
	////////////////////////// SetMapFileName \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void SetMapFileName(const char* pcSet);

	//////////////
	//Debug stuff
	//////////////

	//void OutPutShadersToFile(void);


	////////////////////////
	//Collision Detection
	////////////////////////


	/////////////////////////// TraceAndResponse \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	////// recuresly slides along the planes
	/////////////////////////// TraceAndResponse \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	//bool TraceAndResponse(CTraceRequest* input,CTraceResult* output, int iDepth, int iAbsoluteEntity);
	bool TraceAndResponse(CTraceRequest* input,CTraceResult* output, int iDepth, CEntity* pCausedByEntity);
	
    
	/////////////////////////// Trace \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	////// send a trace down the bsp tree and check against the brushes
	/////////////////////////// Trace \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	//void Trace(CTraceRequest* input,CTraceResult* output,bool bActivateEntities, int iAbsolutEntity);
	void Trace(CTraceRequest* input,CTraceResult* output,bool bActivateEntities,bool bCheckDynamicEntities,CEntity* pCausedByEntity);

	/*void TraceAgainstModel(CTraceRequest* input,CTraceResult* output,int iModelNum);*/

	bool TestModelAgainstLeaf(int iModelNum,int iLeafNum);

	bool TestBBoxAgainstLeaf(CVector3f& vMin,CVector3f& vMax,int iLeafNum);

	



	/////////////////////////// CheckNode \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	////// recurse check the bsp tree by nodes
	/////////////////////////// CheckNode \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void CheckMoveByNode(int iNodeIndex, float fStartFraction, float fEndFraction, CVector3f vStart, CVector3f vEnd,bool bActivateEntities,int iRecurseDepth);


	///////////////////////////// CheckBrush \\\\\\\\\\\\\\\\\\\\\\\\\*
	////// check against the brush
	///////////////////////////// CheckBrush \\\\\\\\\\\\\\\\\\\\\\\\\*
	bool CheckBrush(tBSPBrush *brush);

	

	int GetModelsNum(void) { return m_numOfModels;};


	int GetStartPositionsNum(void){return m_iStartPositionsNum;};
	CVector3f* GetStartPositions(void) { return m_pStartPositions;};

	void FillStaticEntities(void);
	void LinkAllStaticEntitiesToLeafs(void);
	void UpdateAllStaticEntities(void);




	void FillDynamicEntities(CPlayerClient* pPlayer,CPlayerClient* pOponent);
	void UpdateLinkDynamicEntities(void);

	void BruteDrawDynamicEntities();


	bool GetFreeProjectileEntity(CEntity*& pEntity,DWORD dwProjectileType);

	void KillSelf(void);
	void KillOpponent(void);


		/*CVector3f* m_pStartPositions;
	int       m_iStartPositionsNum;*/

	////////////////////////////
	// collision detection
	////////////////////////////
	//CTraceResult m_OutputMoveData; // hold the temporary collision data

	//CVector3f m_vInputRayStart;
	//CVector3f m_vInputRayEnd;


	CTraceRequest *m_CurrentTraceInput;
	CTraceResult     *m_CurrentTraceOutput;

	tBSPBrush *m_pBrushThatWasLastHit;

	tBSPLeaf    *m_pLeafs;         // leafs array
	

	int        m_iStaticEntitiesNum;
	CEntity**  m_ppAllStaticEntities;


	int        m_iDynamicEntitiesNum;
	int        m_iPlayersNum;
	CEntity**  m_ppAllDynamicEntities;	 


	

	FSOUND_SAMPLE* m_pSampleYOUWIN;
	FSOUND_SAMPLE* m_pSampleYOULOST;

	
	float m_fDelayGoingBackToBoard;

	bool m_bFightOver;
	bool m_bRecievedFirstUDPMessage;

private:


	char* m_pcMapFileName;      // map file name

	CVector3f m_CameraPosition; // position of the camera

	int m_numOfVerts;			// number of verts in the model
	int m_numOfFaces;			// number of faces in the model
	int m_numOfTextures;		// number of texture maps
	int m_numOfLightmaps;		// number of light maps

	int m_numOfNodes;           // number of nodes in the BSP tree
	int m_numOfLeafs;           // number of leafs in the BSP tree 
	int m_numOfLeafFaces;       // number of leaf faces
	int m_numOfPlanes;          // number of planes

    int m_numOfLeafBrushes;     // number of leaf brushes
    int m_numOfBrushes;         // number of brushes
    int m_numOfBrushSides;      // number of brush sides

	int m_numOfIndices;         // num of indices on indices on pIndexArray
	int m_numOfCharsInEntities; // num of chars in entities

	int m_numOfModels;
	

	tBSPVertex  *m_pVerts;		// The object's vertices
	tBSPFace    *m_pFaces;		// The faces information of the object

	
	int         *m_pIndexArray;            // The indices array
	char        *m_strEntities;            // our Entities String


	CVertexBuffer * m_pVBArray;    // the vertexbuffers array

	tBSPNode    *m_pNodes;         // nodes array
	

	tBSPBrushORIGINAL *m_pBrushesORIGINAL;         // brushes array
	tBSPBrush		  *m_pBrushes;         // brushes array
	
	tBSPPlane   *m_pPlanesORIGINAL;        // planes array
	CPlane      *m_pPlanes;


	int         *m_pLeafFaces;     // leaf faces indices array
	tBSPVisData  m_clusters;	   // the clusters

	tBSPTexture *m_pTextures;      // BSP texture (filename,content,flags)
    int *m_pLeafBrushes;           // leaf brushes indices array
    
    tBSPBrushORIGINALSide *m_pBrushSides;  // brush sides array


	tBSPModel *m_pModels;

	CSmartPointer<CTexture>* m_sppLightMaps; // smartpointers array of the lightmaps

	UINT m_uiShadersIDs[TEX_MAN_MAX_TEXTURES]; // shader IDs
								
	CBitset m_FacesDrawn;		// The bitset for the faces that have/haven't been drawn

	CVector3f* m_pStartPositions;
	int       m_iStartPositionsNum;


	



	//int        m_iTempCurrentTracedEntity;
	CEntity     *m_pTempCurrentTracedEntity;

		
};


#endif
