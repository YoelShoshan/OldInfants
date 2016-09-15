#include "VertexBuffer.h"

#include "Bezier.h"

///////////////////////////
#include "ZedTracker.h"
///////////////////////////

void CreateGridIndices(UINT* IndexArray,int xdim,int ydim,int startindex,int startvert)
{

	int t_DEBUG_BiggestVertNum = 0;

	UINT index=0;
	UINT vertex=xdim;	

	for (int y=1;y<ydim;y++)
	{
		for (int x=0;x<xdim-1;x++)
		{
			IndexArray[startindex+index++]=vertex + startvert;
			if ((vertex+startvert)>t_DEBUG_BiggestVertNum)
				t_DEBUG_BiggestVertNum = vertex+startvert;				
			IndexArray[startindex+index++]=vertex+1 + startvert;
			if ((vertex+1 + startvert)>t_DEBUG_BiggestVertNum)
				t_DEBUG_BiggestVertNum = vertex+1 + startvert;				
			IndexArray[startindex+index++]=vertex-xdim + startvert;
			if ((vertex-xdim + startvert)>t_DEBUG_BiggestVertNum)
				t_DEBUG_BiggestVertNum = vertex-xdim + startvert;				



			IndexArray[startindex+index++]=vertex-xdim + startvert;
			if ((vertex-xdim + startvert)>t_DEBUG_BiggestVertNum)
				t_DEBUG_BiggestVertNum = vertex-xdim + startvert;				
			IndexArray[startindex+index++]=vertex+1 + startvert;
			if ((vertex+1 + startvert)>t_DEBUG_BiggestVertNum)
				t_DEBUG_BiggestVertNum = vertex+1 + startvert;				
			IndexArray[startindex+index++]=vertex+1-xdim + startvert;
			if ((vertex+1-xdim + startvert)>t_DEBUG_BiggestVertNum)
				t_DEBUG_BiggestVertNum = vertex+1-xdim + startvert;				

			vertex++;
		}
		vertex++;
	}
}


// TODO: Add color and normals handling

CVertexBuffer::CVertexBuffer()
{

	m_bUsingPPLwithNormalShaders = false;

	m_bUsingPPL = false;

	m_fTimeStarted = 0.f;
	m_bSpecialCounter = false;


	m_piIndices=NULL;
	m_pVertices=NULL;
	m_pColors=NULL;
    m_pNormals=NULL;

	m_piIndices = NULL;
	m_pTexCoords=NULL;
	m_pLMCoords=NULL;


	m_pOriginalBezierPatchVertices=NULL;
	m_pOriginalBezierPatchNormals=NULL;
	m_pOriginalBezierPatchTexCoords=NULL;
	m_pOriginalBezierPatchLMCoords=NULL;
	m_pOriginalBezierPatchColors=NULL;

	m_iSize = 0;
	m_iIndexCount = 0;

//	m_uiTextureID = 0;

	//m_ppShader = NULL;
	m_uiLMTextureID = 0;

	m_iBezierPatchXSize=0;
	m_iBezierPatchYSize=0;

	m_dwFaceType=VB_REGULAR;
	
	
	m_dwVBType |= VB_HAS_VERTS | VB_HAS_INDICES;


	m_bSpecialWorldMatrix = false;

	m_bIgnoreCameraMatrix = false;

	m_bAlwaysDraw = false;

	m_pSpecialWorldMatrix = NULL;
}



CVertexBuffer::~CVertexBuffer()
{
	if (m_piIndices!=NULL)
		SafeDeleteArray(m_piIndices);
	
	if (m_pColors!=NULL)
		SafeDeleteArray(m_pColors);

	if (m_pVertices!=NULL)
		SafeDeleteArray(m_pVertices);

	if (m_pNormals!=NULL)
		SafeDeleteArray(m_pNormals);
	
	if (m_pTexCoords!=NULL)
		SafeDeleteArray(m_pTexCoords);

	if (m_pLMCoords!=NULL)
		SafeDeleteArray(m_pLMCoords);

	SafeDeleteArray(m_pOriginalBezierPatchVertices);
	SafeDeleteArray(m_pOriginalBezierPatchNormals);
	SafeDeleteArray(m_pOriginalBezierPatchTexCoords);
	SafeDeleteArray(m_pOriginalBezierPatchLMCoords);
	SafeDeleteArray(m_pOriginalBezierPatchColors);

	SafeDeleteArray(m_pSpecialWorldMatrix);
}




void CVertexBuffer::SetSpecialCounter(void)
{
	m_bSpecialCounter = true;
	m_fTimeStarted = st_Timer::Inst()->TimerGetTimeSecondsFAKE();
}

bool CVertexBuffer::GetSpecialCounter(void) 
{ 
	return m_bSpecialCounter;
};

float CVertexBuffer::GetTimeStarted(void) 
{ 
	return m_fTimeStarted;
};


void CVertexBuffer::DisableSpecialCounter(void) 
{ 
	m_bSpecialCounter = false;
	m_fTimeStarted=0.f;
};

void CVertexBuffer::FillSpecialWorldMatrix(float *pWorldMatrix)
{
	if (!m_pSpecialWorldMatrix)
		m_pSpecialWorldMatrix = new float[16];

	memcpy(m_pSpecialWorldMatrix,pWorldMatrix,sizeof(float)*16);
	/*for (int i=0;i<16;i++)
		m_pSpecialWorldMatrix[i] = pWorldMatrix[i];*/
}

void CVertexBuffer::AllocIndexBuffer(int iIndexCount)
{
	m_iIndexCount = iIndexCount;

	m_piIndices = new UINT [m_iIndexCount];
}

void CVertexBuffer::Alloc(DWORD dwVBType, int iVertexCount)
{	
	m_dwVBType=0;
	m_dwVBType|= dwVBType;

	m_iSize = iVertexCount;

	if (m_dwVBType & VB_HAS_VERTS)
		m_pVertices = new CVector3f [m_iSize];

	if (m_dwVBType & VB_HAS_TANGENTS)
	{
		m_pTangents = new CVector3f [m_iSize];
		m_bUsingPPL = true;
	}

	if (m_dwVBType & VB_HAS_BINORMALS)
		m_pBinormals = new CVector3f [m_iSize];

	if (m_dwVBType & VB_HAS_COLOR)
 		m_pColors = new CColor4 [m_iSize];

	if (m_dwVBType & VB_HAS_NORMALS)
		m_pNormals = new CVector3f [m_iSize];

	if (m_dwVBType & VB_HAS_TEXCOORDS)
		m_pTexCoords = new CVector2f [m_iSize];

	if (m_dwVBType & VB_HAS_LMCOORDS)
		m_pLMCoords = new CVector2f [m_iSize];		



	


    
	if (m_dwVBType & VB_HAS_BEZIER_VERTS)
		m_pOriginalBezierPatchVertices = new CVector3f [m_iSize];

	if (m_dwVBType & VB_HAS_BEZIER_NORMALS)
		m_pOriginalBezierPatchNormals = new CVector3f [m_iSize];

	if (m_dwVBType & VB_HAS_BEZIER_TEXCOORDS)
		m_pOriginalBezierPatchTexCoords = new CVector2f [m_iSize];

	if (m_dwVBType & VB_HAS_BEZIER_LMCOORDS)
		m_pOriginalBezierPatchLMCoords = new CVector2f [m_iSize];

	if (m_dwVBType & VB_HAS_BEZIER_COLORS)
		m_pOriginalBezierPatchColors = new CColor4 [m_iSize];

	



}





void CVertexBuffer::SetLMTexID(UINT uiLMTexID)
{
	m_uiLMTextureID = uiLMTexID;
}

void CVertexBuffer::SetFaceType(DWORD dwFaceType)
{
	m_dwFaceType = dwFaceType;
}



UINT CVertexBuffer::GetLMTexID(void)
{
	return m_uiLMTextureID;
}




UINT * CVertexBuffer::GetIndices(void)
{
  return m_piIndices;
}

DWORD CVertexBuffer::GetFaceType(void)
{
	return m_dwFaceType;
}

DWORD CVertexBuffer::GetVBHasType(void)
{
	return m_dwVBType;
}

void CVertexBuffer::SetVBHasType(DWORD dwSet)
{
	m_dwVBType = dwSet;
}



int   CVertexBuffer::GetIndexNum(void)
{
	return m_iIndexCount;
}

/*int   CVertexBuffer::GetSize(void)
{
	return m_iSize;
}*/


/*UINT CVertexBuffer::GetShaderIndex(void)
{
	return m_uiShaderIndex;
}

void CVertexBuffer::SetShaderIndex(UINT uiShaderIndexNum)
{
	m_uiShaderIndex = uiShaderIndexNum;
}*/

/*void CVertexBuffer::SetShaderIndex(CShader* pSet)
{
	m_pShader = pSet;
}*/

void CVertexBuffer::SetShaderSP(CShader* pSet)
{
	m_spShader = pSet;
}

CShader* CVertexBuffer::GetShader(void)
{
	return (CShader*) m_spShader;
}



void CVertexBuffer::SetAllNull(void)
{
	m_pVertices = NULL;
	m_pColors = NULL;
	m_pNormals = NULL;
	m_pTexCoords = NULL;
	m_pLMCoords = NULL;


}

void CVertexBuffer::Set(DWORD dwVBType,void* pSet)
{
	if (dwVBType == VB_HAS_VERTS)
	{
		m_pVertices = (CVector3f*)pSet;
		return;
	}

	if (dwVBType == VB_HAS_TANGENTS)
	{
		m_pTangents = (CVector3f*)pSet;
		return;
	}

	if (dwVBType == VB_HAS_BINORMALS)
	{
		m_pBinormals = (CVector3f*)pSet;
		return;
	}

	if (dwVBType == VB_HAS_COLOR)
	{
		m_pColors = (CColor4*)pSet;
		return;
	}
	if (dwVBType == VB_HAS_NORMALS)
	{
		m_pNormals = (CVector3f*)pSet;;
		return;
	}
	if (dwVBType == VB_HAS_TEXCOORDS)
	{
		m_pTexCoords = (CVector2f*)pSet;;
		return;
	}
	if (dwVBType == VB_HAS_LMCOORDS)
	{
		m_pLMCoords = (CVector2f*)pSet;;
		return;
	}
}

void CVertexBuffer::preTransformBillboard(void)
{
	CVector3f	t,t2,t3,t4;
	CMatrix     M;
	M.MatMakeUnit();


	for (int v=0;v<m_iIndexCount;v+=6) 
	{	
		t = m_pVertices[m_piIndices[v]];
		t2 = m_pVertices[m_piIndices[v+1]];
		t3 = m_pVertices[m_piIndices[v+2]];
		t4 = (t2+t3)*0.5f;

		t2 = t2-t;
		t2.Normalize();
		t3 = t-t3;
		t3.Normalize();
		t=t2^t3;


		M.m[0][0] = t2.v[0];
		M.m[1][0] = t2.v[1];
		M.m[2][0] = t2.v[2];

		M.m[0][1] = t3.v[0];
		M.m[1][1] = t3.v[1];
		M.m[2][1] = t3.v[2];

		M.m[0][2] = t.v[0];
		M.m[1][2] = t.v[1];
		M.m[2][2] = t.v[2];


		m_pVertices[m_piIndices[v]]   = t4 + (M*(m_pVertices[m_piIndices[v]]-t4));
		m_pVertices[m_piIndices[v+1]] = t4 + (M*(m_pVertices[m_piIndices[v+1]]-t4));
		m_pVertices[m_piIndices[v+2]] = t4 + (M*(m_pVertices[m_piIndices[v+2]]-t4));
		m_pVertices[m_piIndices[v+5]] = t4 + (M*(m_pVertices[m_piIndices[v+5]]-t4));


	}

}

void CVertexBuffer::ReMakeBezierPatch(int iTess)
{
	// delete

	SafeDeleteArray(m_pVertices);
	SafeDeleteArray(m_pTexCoords);
	SafeDeleteArray(m_pLMCoords);
	SafeDeleteArray(m_piIndices);

	SafeDeleteArray(m_pNormals);



	// alloc

	m_iSize = (m_iBezierPatchXSize/2)*(m_iBezierPatchYSize/2)*(iTess+1)*(iTess+1);

	m_pVertices  = new CVector3f[m_iSize];
	m_pNormals   = new CVector3f[m_iSize];
	m_pTexCoords = new CVector2f[m_iSize];
	m_pLMCoords  = new CVector2f[m_iSize];

	//m_iIndexCount = (iTess)*(iTess)*6*(m_iBezierPatchXSize/2)*(m_iBezierPatchYSize/2);

	m_iIndexCount = ((iTess+1)*(m_iBezierPatchXSize/2)-1)*((iTess+1)*(m_iBezierPatchYSize/2)-1)*6;


	m_piIndices  = new UINT[m_iIndexCount];

	m_iCurrentlyTesselatedLOD = iTess;

	EvaluateQuadricBezierPatch(m_pOriginalBezierPatchVertices,m_pOriginalBezierPatchNormals,m_pOriginalBezierPatchTexCoords,m_pOriginalBezierPatchLMCoords,
		m_iCurrentlyTesselatedLOD,m_iCurrentlyTesselatedLOD,
		m_pVertices,m_pNormals, m_pTexCoords,m_pLMCoords,
		m_iBezierPatchXSize,m_iBezierPatchYSize,m_piIndices);

}



// assumes I've already allocated enough space for indices (which is 6*(xsize-1)*(ysize-1))
void CVertexBuffer::FindAndUseGridIndices(void) 
{
	CreateGridIndices(m_piIndices,m_iBezierPatchXSize,m_iBezierPatchYSize,0,0);
}


void CVertexBuffer::SetBaseTexture(CTexture* pSet)
{
	m_spBaseTexture = pSet;
}

CTexture* CVertexBuffer::GetBaseTexture(void)
{
	return (CTexture*) m_spBaseTexture;
}


void CVertexBuffer::SetNormalTexture(CTexture* pSet)
{
	m_spNormalTexture = pSet;
}

CTexture* CVertexBuffer::GetNormalTexture(void)
{
	return (CTexture*) m_spNormalTexture;
}


void CVertexBuffer::SetSpecularTexture(CTexture* pSet)
{
	m_spSpecularTexture = pSet;
}

CTexture* CVertexBuffer::GetSpecularTexture(void)
{
	return (CTexture*) m_spSpecularTexture;
}

//void CalculateTangentArray(long vertexCount, const CVector3f *vertex, const CVector3f *normal, const CVector2f *texcoord,   long triangleCount, const Triangle *triangle, Vector4D *tangent)
//const{
void CVertexBuffer::CalculateTangentAndBinormalArrays(void)
	{
	
		for (long a = 0; a < m_iIndexCount/3; a++) 
		{    
			UINT i1 = m_piIndices[a*3+0]; 
			UINT i2 = m_piIndices[a*3+1]; 
			UINT i3 = m_piIndices[a*3+2]; 
			const CVector3f& v1 = m_pVertices[i1];    
			const CVector3f& v2 = m_pVertices[i2];    
			const CVector3f& v3 = m_pVertices[i3];    
			const CVector2f& w1 = m_pTexCoords[i1];   
			const CVector2f& w2 = m_pTexCoords[i2];    
			const CVector2f& w3 = m_pTexCoords[i3];   
			float x1 = v2.v[0] - v1.v[0];    
			float x2 = v3.v[0] - v1.v[0];     
			float y1 = v2.v[1] - v1.v[1];    
			float y2 = v3.v[1] - v1.v[1];   
			float z1 = v2.v[2] - v1.v[2];    
			float z2 = v3.v[2] - v1.v[2];    
			float s1 = w2.v[0] - w1.v[0];    
			float s2 = w3.v[0] - w1.v[0];   
			float t1 = w2.v[1] - w1.v[1];     
			float t2 = w3.v[1] - w1.v[1];   
			float r = 1.0F / (s1 * t2 - s2 * t1);     
			CVector3f sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);    
			CVector3f tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);   
			m_pTangents[i1] += sdir;   
			m_pTangents[i2] += sdir;    
			m_pTangents[i3] += sdir;     
			m_pBinormals[i1] += tdir;    
			m_pBinormals[i2] += tdir;     
			m_pBinormals[i3] += tdir; 

			//if (bGenNormals) 
			//{ 
			//	CVector3 n=Cross(v1-v2,v3-v2); 
			//	pNormals[i1]+=n; 
			//	pNormals[i2]+=n; 
			//	pNormals[i3]+=n; 
			//} 
		}    
		for (long a = 0; a < m_iSize; a++)    
		{  
			//if (bGenNormals) 
			//	pNormals[a]=Normalize(pNormals[a]); 

			CVector3f& n = m_pNormals[a]; 
			CVector3f& t = m_pTangents[a];   
			m_pTangents[a] = ((t - n * (n * t))).Normalize(); 

			float f=(((n ^ t) * m_pBinormals[a]) < 0.0F) ? -1.0F : 1.0F; 

			m_pBinormals[a]=(((n^t).Normalize())*f).Normalize(); 

		}


/*		void CalculateTangentArray(const CVector3f *vertex, const CVector3f *normal, const CVector2f *texcoord,
		long triangleCount, const Triangle *triangle, Vector4D *tangent)*/
		/*
{
	CVector3f *tan1 = new CVector3f[m_iSize * 2];
	CVector3f *tan2 = tan1 + m_iSize;
	
	//ClearMemory(tan1, m_iSize * sizeof(CVector3f) * 2);
	ZeroMemory(tan1,m_iSize * sizeof(CVector3f) * 2);
	
	for (long a = 0; a < m_iIndexCount/3; a++)
	{
		long i1 = m_piIndices[a*3+0];
		long i2 = m_piIndices[a*3+1];
		long i3 = m_piIndices[a*3+2];
		
		const CVector3f& v1 = m_pVertices[i1];
		const CVector3f& v2 = m_pVertices[i2];
		const CVector3f& v3 = m_pVertices[i3];
		
		const CVector2f& w1 = m_pTexCoords[i1];
		const CVector2f& w2 = m_pTexCoords[i2];
		const CVector2f& w3 = m_pTexCoords[i3];
		
		float x1 = v2.v[0] - v1.v[0];
		float x2 = v3.v[0] - v1.v[0];
		float y1 = v2.v[1] - v1.v[1];
		float y2 = v3.v[1] - v1.v[1];
		float z1 = v2.v[2] - v1.v[2];
		float z2 = v3.v[2] - v1.v[2];
		
		float s1 = w2.v[0] - w1.v[0];
		float s2 = w3.v[0] - w1.v[0];
		float t1 = w2.v[1] - w1.v[1];
		float t2 = w3.v[1] - w1.v[1];
		
		float r = 1.0F / (s1 * t2 - s2 * t1);
		CVector3f sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);
		CVector3f tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);
		
		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;
		
		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
		
		//triangle++;
	}
	
	long count = m_iSize;
	for (long a = 0; a < count; a++)
	{
		//const CVector3f& n = m_pNormals[a];
		//const CVector3f& t = tan1[a];

		CVector3f& n = m_pNormals[a];
		CVector3f& t = tan1[a];
		
		// Gram-Schmidt orthogonalize
		m_pTangents[a] = (t - n * (n * t)).Normalize();
		
		// Calculate handedness
		m_pTangents[a].w = (n % t * tan2[a] < 0.0F) ? -1.0F : 1.0F;
 		
	}
	
	delete[] tan1;
}*/


/*
     for (long a = 0; a <  m_iIndexCount/3; a++) 
     {    
          UINT i1=m_piIndices[a*3+0]; 
          UINT i2=m_piIndices[a*3+1]; 
          UINT i3=m_piIndices[a*3+2]; 
          const CVector3f& v1 = m_pVertices[i1];    
          const CVector3f& v2 = m_pVertices[i2];    
          const CVector3f& v3 = m_pVertices[i3];    
          const CVector2f& w1 = m_pTexCoords[i1];   
          const CVector2f& w2 = m_pTexCoords[i2];    
          const CVector2f& w3 = m_pTexCoords[i3];   
          float x1 = v2.v[0] - v1.v[0];    
          float x2 = v3.v[0] - v1.v[0];     
          float y1 = v2.v[1] - v1.v[1];    
          float y2 = v3.v[1] - v1.v[1];   
          float z1 = v2.v[2] - v1.v[2];    
          float z2 = v3.v[2] - v1.v[2];    
          float s1 = w2.v[0] - w1.v[0];    
          float s2 = w3.v[0] - w1.v[0];   
          float t1 = w2.v[1] - w1.v[1];     
          float t2 = w3.v[1] - w1.v[1];   
          float r = 1.0F / (s1 * t2 - s2 * t1);     
          CVector3f sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);    
          CVector3f tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);   
          m_pTangents[i1] += sdir;   
          m_pTangents[i2] += sdir;    
          m_pTangents[i3] += sdir;     
          m_pBinormals[i1] += tdir;    
          m_pBinormals[i2] += tdir;     
          m_pBinormals[i3] += tdir; 
 
          //if (bGenNormals) 
          { 
               //CVector3f n=Cross(v1-v2,v3-v2); 
			  CVector3f n= (v1-v2)^(v3-v2); 
               m_pNormals[i1]+=n; 
               m_pNormals[i2]+=n; 
               m_pNormals[i3]+=n; 
          } 
     }    
     for (long a = 0; a < m_iSize; a++)    
     {  
          //if (bGenNormals) 
               m_pNormals[a]=(m_pNormals[a].Normalize()); 
 
          CVector3f& n = m_pNormals[a]; 
          CVector3f& t = m_pTangents[a];   

          m_pTangents[a] = ((t - n * (n * t))).Normalize(); 
 
          float f=(((n ^ t) * m_pBinormals[a]) < 0.0F) ? -1.0F : 1.0F; 
 
          //m_pBinormals[a]=Normalize(n^t)*f; 
		  m_pBinormals[a]=((n^t).Normalize())*f; 
 
     } 

*/

}
