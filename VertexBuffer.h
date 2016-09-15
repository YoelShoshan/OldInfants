#ifndef YOEL_VERTEXBUFFER_H
#define YOEL_VERTEXBUFFER_H

#pragma once

#include <windows.h>
#include "Shader.h"
#include "Sort.h"

//#include "ZedTracker.h"

void CreateGridIndices(UINT* IndexArray,int xdim,int ydim,int startindex,int startvert);



///////////////////////////////////////////////////////////

#define VB_HAS_VERTS     (1<<0)
#define VB_HAS_NORMALS   (1<<1)
#define VB_HAS_TEXCOORDS (1<<2)
#define VB_HAS_LMCOORDS  (1<<3)
#define VB_HAS_COLOR     (1<<4)
#define VB_HAS_INDICES   (1<<5)
#define VB_HAS_TANGENTS  (1<<6)
#define VB_HAS_BINORMALS (1<<7)



#define VB_HAS_BEZIER_VERTS     (1<<8)
#define VB_HAS_BEZIER_NORMALS   (1<<9)
#define VB_HAS_BEZIER_TEXCOORDS (1<<10)
#define VB_HAS_BEZIER_LMCOORDS  (1<<11)
#define VB_HAS_BEZIER_COLORS    (1<<12)


#define VB_REGULAR      1
#define VB_BEZIER_PATCH 2 
#define VB_MESH         3
#define VB_BILLBOARD    4


////////////////////////////////////////////////
// class CVertexBuffer
// *****************
// this is what the renderer loves to render ;)
////////////////////////////////////////////////

class CVertexBuffer
{
public:

	/////////////////
	// Constructer
	/////////////////
	CVertexBuffer();

	/////////////////
	// Destructer
	/////////////////
	~CVertexBuffer();

	/////////////////////
	// AllocIndexBuffer()
	/////////////////////
	void AllocIndexBuffer(int iIndexCount);

	/////////////////////////////////////////////////////////////////////
	// Alloc()
	// accepts all the VB_HAS_blah types (except from the indices type)
	/////////////////////////////////////////////////////////////////////
	void Alloc(DWORD dwVBType, int iVertexCount);

	//////////////////////////////////////////////////////
	// SetSize()
	// Set the amount of vertices/texcoords/normals/etc.
	/////////////////////////////////////////////////////
	void SetSize(int iSize){m_iSize=iSize;};

	//////////////////////////////////////////////////////
	// GetSize()
	// Get the amount of vertices/texcoords/normals/etc.
	/////////////////////////////////////////////////////
	//int  GetSize(void);
	inline int   GetSize(void) const 
	{
		return m_iSize;
	}

	//////////////////////////////////////////////////////
	// Set()
	// set one of the pointers
	/////////////////////////////////////////////////////
	void Set(DWORD dwVBType,void* pSet);


	void SetAllNull(void);

	//////////////////////////////////////////////////////
	// Get()
	// get one of the pointers
	/////////////////////////////////////////////////////
	//void * Get(DWORD dwVBType);

	inline void * Get(DWORD dwVBType) const 
	{
	
	switch(dwVBType)
		{

		case VB_HAS_TANGENTS:
			return m_pTangents;			
		break;

		case VB_HAS_BINORMALS:
			return m_pBinormals;			
		break;

		case VB_HAS_VERTS:
			return m_pVertices;			
		break;

		case VB_HAS_COLOR:
			return m_pColors;			
		break;

		case VB_HAS_NORMALS:
			return m_pNormals;			
		break;

		case VB_HAS_TEXCOORDS:
			return m_pTexCoords;			
		break;

		case VB_HAS_LMCOORDS:
			return m_pLMCoords;			
		break;



		case VB_HAS_BEZIER_VERTS:
			return m_pOriginalBezierPatchVertices;			
		break;

		case VB_HAS_BEZIER_COLORS:
			return m_pOriginalBezierPatchColors;			
		break;

		case VB_HAS_BEZIER_NORMALS:
			return m_pOriginalBezierPatchNormals;			
		break;

		case VB_HAS_BEZIER_TEXCOORDS:
			return m_pOriginalBezierPatchTexCoords;			
		break;

		case VB_HAS_BEZIER_LMCOORDS:
			return m_pOriginalBezierPatchLMCoords;			
		break;
		default:
			return NULL;
		break;

		}

// Pay attention to this

//if (dwVBType == VB_HAS_VERTS)
  //return m_pVertices;
//if (dwVBType == VB_HAS_COLOR)
//	return m_pColors;
//if (dwVBType == VB_HAS_NORMALS)
//  return m_pNormals;
//if (dwVBType == VB_HAS_TEXCOORDS)
//  return m_pTexCoords;
//if (dwVBType == VB_HAS_LMCOORDS)
//  return m_pLMCoords;


//if (dwVBType == VB_HAS_BEZIER_VERTS)
	//return m_pOriginalBezierPatchVertices;
//if (dwVBType == VB_HAS_BEZIER_COLORS)
	//return m_pOriginalBezierPatchColors;
//if (dwVBType == VB_HAS_BEZIER_NORMALS)
	//return m_pOriginalBezierPatchNormals;
//if (dwVBType == VB_HAS_BEZIER_TEXCOORDS)
	//return m_pOriginalBezierPatchTexCoords;
//if (dwVBType == VB_HAS_BEZIER_LMCOORDS)
	//return m_pOriginalBezierPatchLMCoords;
//return NULL;


}

	//////////////////////////////////////////////////////
	// SetIndicesCount()
	// set the number of indices
	/////////////////////////////////////////////////////	
	void SetIndexCount(int iIndexCount) {m_iIndexCount = iIndexCount;};

	//////////////////////////////////////////////////////
	// GetIndexNum()
	// set the number of indices
	/////////////////////////////////////////////////////
	int   GetIndexNum(void);

	//////////////////////////////////////////////////////
	// SetIndices()
	// set the pointer to the indices
	/////////////////////////////////////////////////////
	void SetIndices(UINT* pSet){m_piIndices=pSet;};


	//////////////////////////////////////////////////////
	// GetIndices()
	// set the pointer to the indices
	/////////////////////////////////////////////////////
	UINT * GetIndices(void);
	
	//////////////////////////////////////////////////////
	// VBHas()
	// set the type flags
	/////////////////////////////////////////////////////
	//bool VBHas(DWORD dwVBType);

	inline bool VBHas(DWORD dwVBType) const 
	{

		if (m_dwVBType & dwVBType)
			return true;

		return false;
	}

	//////////////////////////////////////////////////////
	// SetLMTexID()
	// set the LightMap texture ID
	/////////////////////////////////////////////////////
	void SetLMTexID(UINT uiLMTexID);

	UINT GetLMTexID(void);

	//////////////////////////////////////////////////////
	// SetFaceType()
	/////////////////////////////////////////////////////
	void SetFaceType(DWORD dwFaceType);


	CShader* GetShader(void);

	//void SetShaderIndex(CShader* pSet);
	void SetShaderSP(CShader* pSet);


	void preTransformBillboard(void);


	DWORD GetFaceType(void);

	DWORD GetVBHasType(void);
	void SetVBHasType(DWORD dwSet);


	void SetBezierPatchDimentions(int iX, int iY){m_iBezierPatchXSize=iX;m_iBezierPatchYSize=iY;};
	int  GetBezierPatchDimX(void){return m_iBezierPatchXSize;};
	int  GetBezierPatchDimY(void){return m_iBezierPatchYSize;};

    
	void ReMakeBezierPatch(int iTess);

	int GetCurrentlyTesselatedLOD(void){return m_iCurrentlyTesselatedLOD;};
	
	// operates on self
	void FindAndUseGridIndices(void); // assumes already allocated enough space for indices


	void FillSpecialWorldMatrix(float *pWorldMatrix);


	bool m_bAlwaysDraw;

    
	bool      m_bSpecialWorldMatrix;
	bool      m_bIgnoreCameraMatrix;

	float*    m_pSpecialWorldMatrix;

	bool GetSpecialCounter(void);
	void SetSpecialCounter(void);
	
	float GetTimeStarted(void);

	void DisableSpecialCounter(void);


	void SetBaseTexture(CTexture* pSet);
	CTexture* GetBaseTexture(void);

	void SetNormalTexture(CTexture* pSet);
	CTexture* GetNormalTexture(void);

	void SetSpecularTexture(CTexture* pSet);
	CTexture* GetSpecularTexture(void);



	void CalculateTangentAndBinormalArrays(void);


	bool m_bUsingPPL;

	bool m_bUsingPPLwithNormalShaders;

private:

	CVector3f* m_pVertices;
	CColor4*   m_pColors;
	CVector3f* m_pNormals;

	int m_iBezierPatchXSize;
	int m_iBezierPatchYSize;

	CVector3f* m_pOriginalBezierPatchVertices;
	CVector3f* m_pOriginalBezierPatchNormals;
	CVector2f* m_pOriginalBezierPatchTexCoords;
	CVector2f* m_pOriginalBezierPatchLMCoords;
	CColor4*   m_pOriginalBezierPatchColors;

	int m_iCurrentlyTesselatedLOD;


	CVector3f* m_pTangents;
	CVector3f* m_pBinormals;


	CSmartPointer<CTexture> m_spBaseTexture;
	CSmartPointer<CTexture> m_spNormalTexture;
	CSmartPointer<CTexture> m_spSpecularTexture;

	//CVector3f* m_pSTangents;
	//CVector3f* m_pTTangents;

	UINT*      m_piIndices;
	CVector2f* m_pTexCoords;
	CVector2f* m_pLMCoords;
	int m_iSize;
	int m_iIndexCount;
	
	DWORD m_dwVBType; 
	DWORD m_dwFaceType;
	
	CSmartPointer<CShader> m_spShader;

	
	float m_fTimeStarted;
	bool  m_bSpecialCounter;


	UINT m_uiLMTextureID;

	
};



class CVertexBufferPTR : public tSortInfo
{
public:
	CVertexBufferPTR() {};
	~CVertexBufferPTR() {};

	void SetSort(UINT uiSort){m_uiSort = uiSort;} ;

	CVertexBuffer* GetVB(void) {return m_pVB;};
	void SetVB(CVertexBuffer* pVB) {m_pVB=pVB;};
private:
	CVertexBuffer* m_pVB;
};


#endif
