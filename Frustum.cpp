#include "Frustum.h"

#include <math.h>
#include <windows.h>
#include <GL/gl.h>

///////////////////////////
#include "ZedTracker.h"
///////////////////////////


enum FrustumSide
{
	RIGHT	= 0,
	LEFT	= 1,
	BOTTOM	= 2,
	TOP		= 3,
	BACK	= 4,
	FRONT	= 5	
}; 



CFrustum::CFrustum()
{
	m_iPlanesNum = 0;
	m_pPlanes = NULL;
}

CFrustum::~CFrustum()
{
	SafeDeleteArray(m_pPlanes);
	m_iPlanesNum = 0;
}



void CFrustum::AllocPlanes(int iNum)
{
	m_iPlanesNum = iNum;
	m_pPlanes = new CPlane[m_iPlanesNum];	
}


///////////////////////////////// CALCULATE FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This extracts the frustum from the OpenGL projection and modelview matrix.
/////
///////////////////////////////// CALCULATE FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFrustum::CalculateOpenGLFrustum()
{   
	if (!m_pPlanes)
		AllocPlanes(6);
        

	float   proj[16];								// This will hold our projection matrix
	float   modl[16];								// This will hold our modelview matrix
	float   clip[16];								// This will hold the clipping planes

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
	m_pPlanes[RIGHT].n.v[0] = clip[ 3] - clip[ 0];
	m_pPlanes[RIGHT].n.v[1] = clip[ 7] - clip[ 4];
	m_pPlanes[RIGHT].n.v[2] = clip[11] - clip[ 8];
	m_pPlanes[RIGHT].d = clip[15] - clip[12];

	// Now that we have a normal (A,B,C) and a distance (D) to the plane,
	// we want to normalize that normal and distance.

	// Normalize the RIGHT side
	//NormalizePlane(m_Frustum, RIGHT);
	m_pPlanes[RIGHT].Noralize();

	// This will extract the LEFT side of the frustum
	m_pPlanes[LEFT].n.v[0] = clip[ 3] + clip[ 0];
	m_pPlanes[LEFT].n.v[1] = clip[ 7] + clip[ 4];
	m_pPlanes[LEFT].n.v[2] = clip[11] + clip[ 8];
	m_pPlanes[LEFT].d = clip[15] + clip[12];

	// Normalize the LEFT side
	//NormalizePlane(m_Frustum, LEFT);
	m_pPlanes[LEFT].Noralize();


	// This will extract the BOTTOM side of the frustum
	m_pPlanes[BOTTOM].n.v[0] = clip[ 3] + clip[ 1];
	m_pPlanes[BOTTOM].n.v[1] = clip[ 7] + clip[ 5];
	m_pPlanes[BOTTOM].n.v[2] = clip[11] + clip[ 9];
	m_pPlanes[BOTTOM].d = clip[15] + clip[13];

	// Normalize the BOTTOM side
	//NormalizePlane(m_Frustum, BOTTOM);
	m_pPlanes[BOTTOM].Noralize();

	// This will extract the TOP side of the frustum
	m_pPlanes[TOP].n.v[0] = clip[ 3] - clip[ 1];
	m_pPlanes[TOP].n.v[1] = clip[ 7] - clip[ 5];
	m_pPlanes[TOP].n.v[2] = clip[11] - clip[ 9];
	m_pPlanes[TOP].d = clip[15] - clip[13];

	// Normalize the TOP side
	//NormalizePlane(m_Frustum, TOP);
	m_pPlanes[TOP].Noralize();


	// This will extract the BACK side of the frustum
	m_pPlanes[BACK].n.v[0] = clip[ 3] - clip[ 2];
	m_pPlanes[BACK].n.v[1] = clip[ 7] - clip[ 6];
	m_pPlanes[BACK].n.v[2] = clip[11] - clip[10];
	m_pPlanes[BACK].d = clip[15] - clip[14];

	// Normalize the BACK side
	//NormalizePlane(m_Frustum, BACK);
	m_pPlanes[BACK].Noralize();


	// This will extract the FRONT side of the frustum
	m_pPlanes[FRONT].n.v[0] = clip[ 3] + clip[ 2];
	m_pPlanes[FRONT].n.v[1] = clip[ 7] + clip[ 6];
	m_pPlanes[FRONT].n.v[2] = clip[11] + clip[10];
	m_pPlanes[FRONT].d = clip[15] + clip[14];

	// Normalize the FRONT side
	//NormalizePlane(m_Frustum, FRONT);
	m_pPlanes[FRONT].Noralize();
}


/*
///////////////////////////////// POINT IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This determines if a point is inside of the frustum
/////
///////////////////////////////// POINT IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFrustum::PointInFrustum( float x, float y, float z )
{
	// Go through all the sides of the frustum
	for(int i = 0; i < m_iPlanesNum; i++ )
	{
		// Calculate the plane equation and check if the point is behind a side of the frustum
		if(m_pPlanes[i].n.v[0] * x + m_pPlanes[i].n.v[1] * y + m_pPlanes[i].n.v[2] * z + m_pPlanes[i].d <= 0)
		{
			// The point was behind a side, so it ISN'T in the frustum
			return false;
		}
	}

	// The point was inside of the frustum (In front of ALL the sides of the frustum)
	return true;
}


///////////////////////////////// SPHERE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This determines if a sphere is inside of our frustum by it's center and radius.
/////
///////////////////////////////// SPHERE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFrustum::SphereInFrustum( float x, float y, float z, float radius )
{
	// Go through all the sides of the frustum
	for(int i = 0; i < m_iPlanesNum; i++ )	
	{
		// If the center of the sphere is farther away from the plane than the radius
		if( m_pPlanes[i].n.v[0] * x + m_pPlanes[i].n.v[1] * y + m_pPlanes[i].n.v[2] * z + m_pPlanes[i].d <= -radius )
		{
			// The distance was greater than the radius so the sphere is outside of the frustum
			return false;
		}
	}
	
	// The sphere was inside of the frustum!
	return true;
}


///////////////////////////////// CUBE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This determines if a cube is in or around our frustum by it's center and 1/2 it's length
/////
///////////////////////////////// CUBE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFrustum::CubeInFrustum( float x, float y, float z, float size )
{
	// Basically, what is going on is, that we are given the center of the cube,
	// and half the length.  Think of it like a radius.  Then we checking each point
	// in the cube and seeing if it is inside the frustum.  If a point is found in front
	// of a side, then we skip to the next side.  If we get to a plane that does NOT have
	// a point in front of it, then it will return false.

	// *Note* - This will sometimes say that a cube is inside the frustum when it isn't.
	// This happens when all the corners of the bounding box are not behind any one plane.
	// This is rare and shouldn't effect the overall rendering speed.

	for(int i = 0; i < 6; i++ )
	{
		if(m_pPlanes[i].n.v[0] * (x - size) + m_pPlanes[i].n.v[1] * (y - size) + m_pPlanes[i].n.v[2] * (z - size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x + size) + m_pPlanes[i].n.v[1] * (y - size) + m_pPlanes[i].n.v[2] * (z - size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x - size) + m_pPlanes[i].n.v[1] * (y + size) + m_pPlanes[i].n.v[2] * (z - size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x + size) + m_pPlanes[i].n.v[1] * (y + size) + m_pPlanes[i].n.v[2] * (z - size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x - size) + m_pPlanes[i].n.v[1] * (y - size) + m_pPlanes[i].n.v[2] * (z + size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x + size) + m_pPlanes[i].n.v[1] * (y - size) + m_pPlanes[i].n.v[2] * (z + size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x - size) + m_pPlanes[i].n.v[1] * (y + size) + m_pPlanes[i].n.v[2] * (z + size) + m_pPlanes[i].d > 0)
		   continue;
		if(m_pPlanes[i].n.v[0] * (x + size) + m_pPlanes[i].n.v[1] * (y + size) + m_pPlanes[i].n.v[2] * (z + size) + m_pPlanes[i].d > 0)
		   continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	return true;
}

*/
///////////////////////////////// BOX IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This determines if a BOX is in or around our frustum by it's min and max points
/////
///////////////////////////////// BOX IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

#define V_IN 1
#define V_OUT 2
#define V_INTERSECT 3

int CFrustum::BoxInFrustum( float x, float y, float z, float x2, float y2, float z2) const
{
	BYTE mode=0;					// set IN and OUT bit to 0
	for(int i = 0; i < m_iPlanesNum; i++ )
	{
		mode &= V_OUT;				// clear the IN bit to 0 
		if(m_pPlanes[i].n.v[0] * x  + m_pPlanes[i].n.v[1] * y  + m_pPlanes[i].n.v[2] * z  + m_pPlanes[i].d >= 0)    mode |= V_IN;  // set IN bit to 1
																									else mode |= V_OUT; // set OUT bit to 1
		if (mode == V_INTERSECT) continue;  // if we found a vertex IN for THIS plane and 
											// a vertex OUT of ANY plane continue ( we have enough information to say: 
											// INTERSECT! IF there is not vertex missing from the FRONT of the remaining planes)
		
		if(m_pPlanes[i].n.v[0] * x2 + m_pPlanes[i].n.v[1] * y  + m_pPlanes[i].n.v[2] * z  + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x  + m_pPlanes[i].n.v[1] * y2 + m_pPlanes[i].n.v[2] * z  + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x2 + m_pPlanes[i].n.v[1] * y2 + m_pPlanes[i].n.v[2] * z  + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x  + m_pPlanes[i].n.v[1] * y  + m_pPlanes[i].n.v[2] * z2 + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x2 + m_pPlanes[i].n.v[1] * y  + m_pPlanes[i].n.v[2] * z2 + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x  + m_pPlanes[i].n.v[1] * y2 + m_pPlanes[i].n.v[2] * z2 + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * x2 + m_pPlanes[i].n.v[1] * y2 + m_pPlanes[i].n.v[2] * z2 + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		// if we arrive to this point, then there are two possibilities:
		// there is not vertices in or there is not intersection till know, if 
		// there is a vertice in, continue (we are not over!) 
		if (mode == V_IN) continue;

		// there is not vertex IN front of this plane, so the box is COMPLETE_OUT
		return COMPLETE_OUT;
	}

	// All planes has a vertex IN FRONT so or the box is intersecting or complete IN
	if (mode == V_INTERSECT) return INTERSECT;
			            else return COMPLETE_IN;
									

/*

	static CVector3f vCenter;

	vCenter.v[0] = (x+x2)/2.f;
	vCenter.v[1] = (y+y2)/2.f;
	vCenter.v[2] = (z+z2)/2.f;
    
     float R=x2-x; 
     float S=y2-y; 
     float T=z2-z; 


	 for (int i=0;i<m_iPlanesNum;i++)
	 {
	     float rEff=0.5*(fabs(R*m_pPlanes[i].n.v[0])+fabs(S*m_pPlanes[i].n.v[1])+fabs(T*m_pPlanes[i].n.v[2])); 
          
		 // Box is completely behind 1 plane -> not in Frustum 
          if (m_pPlanes[i].d-m_pPlanes[i].n*vCenter>=rEff) 
               COMPLETE_OUT; 

	 }   
      
     return COMPLETE_IN;
	 
	 // what about intersect?
		 */
	 
}


int CFrustum::BoxInFrustum(CVector3f& vMins, CVector3f& vMaxs) const
{

	BYTE mode=0;					// set IN and OUT bit to 0
	for(int i = 0; i < m_iPlanesNum; i++ )
	{
		mode &= V_OUT;				// clear the IN bit to 0 
		if(m_pPlanes[i].n.v[0] * vMins.v[0]  + m_pPlanes[i].n.v[1] * vMins.v[1]  + m_pPlanes[i].n.v[2] * vMins.v[1]  + m_pPlanes[i].d >= 0)    mode |= V_IN;  // set IN bit to 1
																									else mode |= V_OUT; // set OUT bit to 1
		if (mode == V_INTERSECT) continue;  // if we found a vertex IN for THIS plane and 
											// a vertex OUT of ANY plane continue ( we have enough information to say: 
											// INTERSECT! IF there is not vertex missing from the FRONT of the remaining planes)
		
		if(m_pPlanes[i].n.v[0] * vMaxs.v[0] + m_pPlanes[i].n.v[1] * vMins.v[1]  + m_pPlanes[i].n.v[2] * vMins.v[2]  + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * vMins.v[0]  + m_pPlanes[i].n.v[1] * vMaxs.v[1] + m_pPlanes[i].n.v[2] * vMins.v[2]  + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * vMaxs.v[0] + m_pPlanes[i].n.v[1] * vMaxs.v[1] + m_pPlanes[i].n.v[2] * vMins.v[2]  + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] * vMins.v[0]  + m_pPlanes[i].n.v[1] * vMins.v[1]  + m_pPlanes[i].n.v[2] * vMaxs.v[2] + m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] *vMaxs.v[0]+ m_pPlanes[i].n.v[1] *vMins.v[1] + m_pPlanes[i].n.v[2] *vMaxs.v[2]+ m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] *vMins.v[0] + m_pPlanes[i].n.v[1] *vMaxs.v[1]+ m_pPlanes[i].n.v[2] *vMaxs.v[2]+ m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_pPlanes[i].n.v[0] *vMaxs.v[0]+ m_pPlanes[i].n.v[1] *vMaxs.v[1]+ m_pPlanes[i].n.v[2] *vMaxs.v[2]+ m_pPlanes[i].d >= 0)  mode |= V_IN; else mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		// if we arrive to this point, then there are two possibilities:
		// there is not vertices in or there is not intersection till know, if 
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

	//for (int i=0;i<m_iPlanesNum;i++)
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