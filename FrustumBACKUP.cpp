#include <Windows.h>

#include "BasicFrustum.h"

#include "Camera.h"
extern CCamera g_Camera1;  // the global camera

#include <assert.h>
//#include <crtdbg.h>


#include "Singleton.h"
#include "Renderer.h"

void CFrustum::FillFromOpenGLFrustum(void)
{
	float   proj[16];								// This will hold our projection matrix
	float   modl[16];								// This will hold our modelview matrix
	float   clip[16];								// This will hold the clipping planes

	CVector4f FrustumPlanes[6];

	// glGetFloatv() is used to extract information about our OpenGL world.
	// Below, we pass in GL_PROJECTION_MATRIX to abstract our projection matrix.
	// It then stores the matrix into an array of [16].
	glGetFloatv( GL_PROJECTION_MATRIX, proj );

	// By passing in GL_MODELVIEW_MATRIX, we can abstract our model view matrix.
	// This also stores it in an array of [16].
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	// Now that we have our modelview and projection matrix, if we combine these 2 matrices,
	// it will give us our clipping planes.  To combine 2 matrices, we multiply them.

	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	// Now we actually want to get the sides of the frustum.  To do this we take
	// the clipping planes we received above and extract the sides from them.

	// This will extract the RIGHT side of the frustum
	FrustumPlanes[FRUSTUM_RIGHT_PLANE].v[0] = clip[ 3] - clip[ 0];
	FrustumPlanes[FRUSTUM_RIGHT_PLANE].v[1] = clip[ 7] - clip[ 4];
	FrustumPlanes[FRUSTUM_RIGHT_PLANE].v[2] = clip[11] - clip[ 8];
	FrustumPlanes[FRUSTUM_RIGHT_PLANE].v[3] = clip[15] - clip[12];

	// Now that we have a normal (A,B,C) and a distance (D) to the plane,
	// we want to normalize that normal and distance.

	// Normalize the RIGHT side
	FrustumPlanes[FRUSTUM_RIGHT_PLANE].Normalize();
	

	// This will extract the LEFT side of the frustum
	FrustumPlanes[FRUSTUM_LEFT_PLANE].v[0] = clip[ 3] + clip[ 0];
	FrustumPlanes[FRUSTUM_LEFT_PLANE].v[1] = clip[ 7] + clip[ 4];
	FrustumPlanes[FRUSTUM_LEFT_PLANE].v[2] = clip[11] + clip[ 8];
	FrustumPlanes[FRUSTUM_LEFT_PLANE].v[3] = clip[15] + clip[12];

	// Normalize the LEFT side
	FrustumPlanes[FRUSTUM_LEFT_PLANE].Normalize();

	// This will extract the BOTTOM side of the frustum
	FrustumPlanes[FRUSTUM_BOTTOM_PLANE].v[0] = clip[ 3] + clip[ 1];
	FrustumPlanes[FRUSTUM_BOTTOM_PLANE].v[1] = clip[ 7] + clip[ 5];
	FrustumPlanes[FRUSTUM_BOTTOM_PLANE].v[2] = clip[11] + clip[ 9];
	FrustumPlanes[FRUSTUM_BOTTOM_PLANE].v[3] = clip[15] + clip[13];

	// Normalize the BOTTOM side
	FrustumPlanes[FRUSTUM_BOTTOM_PLANE].Normalize();

	// This will extract the TOP side of the frustum
	FrustumPlanes[FRUSTUM_TOP_PLANE].v[0] = clip[ 3] - clip[ 1];
	FrustumPlanes[FRUSTUM_TOP_PLANE].v[1] = clip[ 7] - clip[ 5];
	FrustumPlanes[FRUSTUM_TOP_PLANE].v[2] = clip[11] - clip[ 9];
	FrustumPlanes[FRUSTUM_TOP_PLANE].v[3] = clip[15] - clip[13];

	// Normalize the TOP side
	FrustumPlanes[FRUSTUM_TOP_PLANE].Normalize();

	// This will extract the BACK side of the frustum
	FrustumPlanes[FRUSTUM_BACK_PLANE].v[0] = clip[ 3] - clip[ 2];
	FrustumPlanes[FRUSTUM_BACK_PLANE].v[1] = clip[ 7] - clip[ 6];
	FrustumPlanes[FRUSTUM_BACK_PLANE].v[2] = clip[11] - clip[10];
	FrustumPlanes[FRUSTUM_BACK_PLANE].v[3] = clip[15] - clip[14];

	// Normalize the BACK side
	FrustumPlanes[FRUSTUM_BACK_PLANE].Normalize();

	// This will extract the FRONT side of the frustum
	FrustumPlanes[FRUSTUM_FRONT_PLANE].v[0] = clip[ 3] + clip[ 2];
	FrustumPlanes[FRUSTUM_FRONT_PLANE].v[1] = clip[ 7] + clip[ 6];
	FrustumPlanes[FRUSTUM_FRONT_PLANE].v[2] = clip[11] + clip[10];
	FrustumPlanes[FRUSTUM_FRONT_PLANE].v[3] = clip[15] + clip[14];

	// Normalize the FRONT side
	FrustumPlanes[FRUSTUM_FRONT_PLANE].Normalize();

	
	AllocPlanes(6);

	for (int i=0;i<6;i++)
	{
		m_pPlanes[i].n.v[0] = 	FrustumPlanes[i].v[0];
		m_pPlanes[i].n.v[1] = 	FrustumPlanes[i].v[1];
		m_pPlanes[i].n.v[2] = 	FrustumPlanes[i].v[2];
		m_pPlanes[i].d      =   -FrustumPlanes[i].v[3];
	}
	
}


bool CFrustum::TestPoint(CVector3f& p1)
{
	for (int i=0;i<m_iPlanesNum;i++)
		if (m_pPlanes[i].n*p1<m_pPlanes[i].d)
			return false;

	return true;
}

bool CFrustum::DebugTestPointArray(CVector3f* pPoints,int iNum)
{
	for (int i=0;i<iNum;i++)
	{
		if (TestPoint(pPoints[i]))
			return true;
	}

	return false;
}




#define V_IN 1
#define V_OUT 2
#define V_INTERSECT 3

int CFrustum::BoxInFrustum( float x, float y, float z, float x2, float y2, float z2)
{

	///
	//return COMPLETE_IN;
	///

	assert(m_iPlanesNum==6);

	
	BYTE mode=0;					// set IN and OUT bit to 0
	for(int i = 0; i < 6; i++ )
	{
		mode &= V_OUT;				// clear the IN bit to 0 
		if(m_pPlanes[i].n.v[0] * x  + m_pPlanes[i].n.v[1] * y  + m_pPlanes[i].n.v[2] * z  + m_pPlanes[i].d >= 0)    mode |= V_IN;  // set IN bit to 1
		else mode |= V_OUT; // set OUT bit to 1
		if (mode == V_INTERSECT) continue;  // if we found a vertex IN for THIS plane and 
		// a vertex OUT of ANY plane continue ( we have enough information to say: 
		// INTERSECT! IF there is not vertex missing from the FRONT of the remaining planes)

		if(m_pPlanes[i].n.v[0] * x2 + m_pPlanes[i].n.v[1] * y  + m_pPlanes[i].n.v[2] * z  >= m_pPlanes[i].d )  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x  + m_pPlanes[i].n.v[1] * y2 + m_pPlanes[i].n.v[2] * z  >= m_pPlanes[i].d )  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x2 + m_pPlanes[i].n.v[1] * y2 + m_pPlanes[i].n.v[2] * z  >= m_pPlanes[i].d )  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x  + m_pPlanes[i].n.v[1] * y  + m_pPlanes[i].n.v[2] * z2 >= m_pPlanes[i].d )  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x2 + m_pPlanes[i].n.v[1] * y  + m_pPlanes[i].n.v[2] * z2 >= m_pPlanes[i].d )  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x  + m_pPlanes[i].n.v[1] * y2 + m_pPlanes[i].n.v[2] * z2 >= m_pPlanes[i].d )  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x2 + m_pPlanes[i].n.v[1] * y2 + m_pPlanes[i].n.v[2] * z2 >= m_pPlanes[i].d )  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		// if we arrive to this point, then there are two possibilities:
		// there is not vertices in or there is not intersection till now, if 
		// there is a vertice in, continue (we are not over!) 
		if (mode == V_IN) continue;

		// there is not vertex IN front of this plane, so the box is COMPLETE_OUT
		return COMPLETE_OUT;
	}

	// All planes has a vertex IN FRONT so or the box is intersecting or complete IN
	if (mode == V_INTERSECT) return INTERSECT;
	else return COMPLETE_IN;

	

/*	CVector3f vMins(x,y,z);
	CVector3f vMaxs(x2,y2,z2);


     CVector3f vCenter=(vMins+vMaxs)/2.0f; 
     float R=vMaxs.v[0]-vMins.v[0]; 
     float S=vMaxs.v[1]-vMins.v[1]; 
     float T=vMaxs.v[2]-vMins.v[2]; 
      
 
     for (CONSTFRUSTUMPLANEITERATOR it=m_FrustumPlanes.begin();it!=m_FrustumPlanes.end();it++) 
     { 
          float rEff=0.5*(fabs(R*(*it).vNormal.x)+fabs(S*(*it).vNormal.y)+fabs(T*(*it).vNormal.z)); 
 
          // Box is completely behind 1 plane -> not in Frustum 
          if ((*it).d-(*it).vNormal*vCenter>=rEff) 
               return false; 
     } 
 
     return true;*/
}

int CFrustum::PolygonInFrustum(CVector3f* pVerts, int iNum)
{
	BOOL bTemp1=false;

	for (int i=0;i<m_iPlanesNum;i++)
	{
		bTemp1=false;
		for (int j=0;j<iNum;j++) 
		{
			if (m_pPlanes[i].n*pVerts[j]>m_pPlanes[i].d) // in 
			{				
				bTemp1=true;
				break;
			}
		}

		if (!bTemp1) // all vertices behind this plane
			return  FRUSTUM_CHECK_OUT;		
    }

	return FRUSTUM_CHECK_AT_LEAST_PARTLY_IN;
	
}
	

// remember to use a simplified version of this for IsPolygonIn

CVector3f*  CFrustum::ClipPolygon(CVector3f* pOriginal,int iNum, int& iFinalNum)
{
	int iTemp1Index=0;
	int iTemp2Index=0;

	for (int i=0;i<iNum;i++)
	{
		temp1[i]=pOriginal[i];
		iTemp1Index++;
	}


	bool bTemp=true; //false = temp1->temp2, true = temp2->temp1

	int iFirst=0;
	int iSecond=0;


	bool bAtLeastOne=false;

	i=0;
	/////////////////////////////////////////////////////

	for (i=0;i<m_iPlanesNum;i++)
	{
		if (bTemp)
		{
			iTemp2Index=0;
			bAtLeastOne=false;
			for (int j=0;j<iTemp1Index;j++) // fill boolean array
			{
				if (m_pPlanes[i].n*temp1[j]<m_pPlanes[i].d)
				{
					bTemp1[j]=false;
					
				}
				else
				{
					bTemp1[j]=true;
					bAtLeastOne=true;
				}
					
			}

			for (j=0;j<iTemp1Index;j++)
			{	
				if (j==iTemp1Index-1) // last edge
				{
					iFirst=j;
					iSecond=0;
				}
				else
				{
					iFirst=j;
					iSecond=j+1;
				}

				if (bTemp1[iFirst]==true &&  bTemp1[iSecond]==true)//edge going from in to in
				{

					//BREAKPOINT(iTemp2Index>10);

					temp2[iTemp2Index] = temp1[iSecond];
					iTemp2Index++;

				}
				else
				if (bTemp1[iFirst]==true &&  bTemp1[iSecond]==false)//edge going from in to out
				{					
					//assert(_CrtCheckMemory());
					temp2[iTemp2Index] = RayPlaneIntersection(temp1[iFirst],temp1[iSecond],m_pPlanes[i]);
					iTemp2Index++;		
					//assert(_CrtCheckMemory());
				}
				else
				if (bTemp1[iFirst]==false &&  bTemp1[iSecond]==true)//edge going from out to in
				{
//					assert(_CrtCheckMemory());
					temp2[iTemp2Index] = RayPlaneIntersection(temp1[iFirst],temp1[iSecond],m_pPlanes[i]);
					iTemp2Index++;			
//					assert(_CrtCheckMemory());

					temp2[iTemp2Index] = temp1[iSecond];
					iTemp2Index++;
//					assert(_CrtCheckMemory());
				}
				else //edge going from out to out
				{
					// do nothing
				}


			}

		} // if bTemp==true
		else
			{
				//BREAKPOINT(iTemp2Index>30);
				iTemp1Index=0;
				bAtLeastOne=false;
				for (int j=0;j<iTemp2Index;j++) // fill boolean array
				{
					if (m_pPlanes[i].n*temp2[j]<m_pPlanes[i].d)
					{
						bTemp2[j]=false;						
					}
					else
					{
						bTemp2[j]=true;
						bAtLeastOne=true;
					}
						
				}

				for (j=0;j<iTemp2Index;j++)
				{	
					if (j==iTemp2Index-1) // last edge
					{
						iFirst=j;
						iSecond=0;
					}
					else
					{
						iFirst=j;
						iSecond=j+1;
					}

					if (bTemp2[iFirst]==true &&  bTemp2[iSecond]==true)//edge going from in to in
					{
						temp1[iTemp1Index] = temp2[iSecond];
						iTemp1Index++;
					}
					else
					if (bTemp2[iFirst]==true &&  bTemp2[iSecond]==false)//edge going from in to out
					{
						temp1[iTemp1Index] = RayPlaneIntersection(temp2[iFirst],temp2[iSecond],m_pPlanes[i]);
						iTemp1Index++;					
					}
					else
					if (bTemp2[iFirst]==false &&  bTemp2[iSecond]==true)//edge going from out to in
						{							
							temp1[iTemp1Index] = RayPlaneIntersection(temp2[iFirst],temp2[iSecond],m_pPlanes[i]);
							iTemp1Index++;

							temp1[iTemp1Index] = temp2[iSecond];
							iTemp1Index++;
						}
					else //edge going from out to out
					{
						// do nothing
					}


				}

			} // if bTemp==false

		bTemp=!bTemp;
	}


//	assert(_CrtCheckMemory());

	CVector3f* pFinal=NULL;

	if (bTemp) // the result is in temp1
	{
		iFinalNum=iTemp1Index;
		if (iFinalNum==0)
		{
			return NULL;
		}
		pFinal = new CVector3f[iFinalNum];
		for (i=0;i<iFinalNum;i++)
		{
//			assert(_CrtCheckMemory());
			pFinal[i]=temp1[i];
		}
		return pFinal;
	}
	else       // the result is in temp2
	{
		iFinalNum=iTemp2Index;
		if (iFinalNum==0)
		{
			return NULL;
		}
		pFinal = new CVector3f[iFinalNum];
		for (i=0;i<iFinalNum;i++)
		{
//			assert(_CrtCheckMemory());
			pFinal[i]=temp2[i];
		}
		return pFinal;
	}
	
	
}

void CFrustum::GenerateFrustumFromCameraAndPortalPoints(CVector3f& pCameraPosition,CVector3f* pVerts,int iNum,bool bReverse, bool bDraw)
{
	AllocPlanes(iNum);
	int iFirst=0;
	int iSecond=0;

	if (bReverse==false)
		for (int i=0;i<m_iPlanesNum;i++)
		{
			if (i==m_iPlanesNum-1)
			{
				iFirst=i;
				iSecond=0;
			}
			else
			{
				iFirst=i;
				iSecond=i+1;
			}
			m_pPlanes[i].FindPlane(pCameraPosition,pVerts[iFirst],pVerts[iSecond]);

			/*
			if (bDraw)
			{
				glColor4f(0.f,0.f,1.f,0.01f);
				st_Renderer::Inst()->DebugDrawTri(NULL,pCameraPosition,pVerts[iFirst],pVerts[iSecond]);
			}
			*/
			
		}
	else
		for (int i=0;i<m_iPlanesNum;i++)
		{
			if (i==m_iPlanesNum-1)
			{
				iFirst=i;
				iSecond=0;
			}
			else
			{
				iFirst=i;
				iSecond=i+1;
			}
			m_pPlanes[i].FindPlane(pVerts[iSecond],pVerts[iFirst],pCameraPosition);


			/*
			if (bDraw)
			{
				glColor4f(1.f,1.f,0.f,0.01f);
				st_Renderer::Inst()->DebugDrawTri(NULL,pCameraPosition,pVerts[iSecond],pVerts[iFirst]);
			}
			*/
		}
}


void CFrustum::DrawFrustumSource(CVector3f& pCameraPosition,CVector3f* pVerts,int iNum,bool bReverse)
{
	int iFirst=0;
	int iSecond=0;

		for (int i=0;i<m_iPlanesNum;i++)
		{
			if (i==m_iPlanesNum-1)
			{
				iFirst=i;
				iSecond=0;
			}
			else
			{
				iFirst=i;
				iSecond=i+1;
			}
			
			glColor4f(1.f,1.f,0.f,0.01f);
			st_Renderer::Inst()->DebugDrawTri(NULL,pCameraPosition,pVerts[iFirst],pVerts[iSecond]);
		}
}


void CFrustum::DebugDrawFrustum(void)
{
	/*for (int i=0;i<m_iPlanesNum;i++)
	st_Renderer::Inst()->DebugDrawPoly(NULL,m_pPlanes[i].,num);*/
}