#ifndef YOEL_FRUSTUM_H
#define YOEL_FRUSTUM_H

#pragma once

#include <Windows.h>
#include <gl/GL.h>

// in case of the general opengl camera frustum

#define FRUSTUM_RIGHT_PLANE	  0		// The RIGHT side of the frustum
#define FRUSTUM_LEFT_PLANE	  1		// The LEFT	 side of the frustum
#define FRUSTUM_BOTTOM_PLANE  2		// The BOTTOM side of the frustum
#define FRUSTUM_TOP_PLANE	  3		// The TOP side of the frustum
#define FRUSTUM_BACK_PLANE	  4		// The BACK	side of the frustum
#define FRUSTUM_FRONT_PLANE	  5			// The FRONT side of the frustum

#include "Math3d.h"


#define COMPLETE_OUT 0
#define INTERSECT    1
#define COMPLETE_IN  2

#define FRUSTUM_CHECK_AT_LEAST_PARTLY_IN 0
#define FRUSTUM_CHECK_OUT                1


#define MAX_PORTAL_CLIPVERTS 100


class CFrustum
{

public:
	CFrustum() {m_pPlanes=NULL;m_iPlanesNum=0;};
	~CFrustum() {SafeDeleteArray(m_pPlanes);}

	void FillFromOpenGLFrustum(void);

	bool TestPoint(CVector3f& p1);

	bool DebugTestPointArray(CVector3f* pPoints,int iNum);

	int BoxInFrustum(float x, float y, float z, float sizeX, float sizeY, float sizeZ);

	CVector3f* ClipPolygon(CVector3f* pOriginal,int iNum, int& iFinalNum);

	int PolygonInFrustum(CVector3f* pVerts, int iNum);

	void GenerateFrustumFromCameraAndPortalPoints(CVector3f& pCameraPosition,CVector3f* pVerts,int iNum,bool bReverse, bool bDraw);

	void DrawFrustumSource(CVector3f& pCameraPosition,CVector3f* pVerts,int iNum,bool bReverse);

	void DebugDrawFrustum(void);

	//void TestPoint();

	//hmmm - perhaps i need to add an options to fill this frustum from the opengl frustum

	void AllocPlanes(int iNum) { m_pPlanes = new CPlane[iNum];m_iPlanesNum=iNum;}

	int m_iPlanesNum;
	CPlane* m_pPlanes;

	// temp buffers for clipping
	CVector3f temp1[100];
	CVector3f temp2[100];
	BOOL bTemp1[100]; // in=true,out=false
	BOOL bTemp2[100]; // in=true,out=false
};



#endif //YOEL_FRUSTUM_H