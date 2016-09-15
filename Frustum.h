#ifndef YOEL_FRUSTUM_H
#define YOEL_FRUSTUM_H

#pragma once

#include <windows.h>
#include "Math3D.h"

#define FRUSTUM_CHECK_AT_LEAST_PARTLY_IN 0
#define FRUSTUM_CHECK_OUT                1

// This will allow us to create an object to keep track of our frustum
class CFrustum {

public:


	CFrustum();
	~CFrustum();


	void AllocPlanes(int iNum);

	// Call this every time the camera moves to update the frustum
	void CalculateOpenGLFrustum();


//	bool PointInFrustum(float x, float y, float z);

//	bool SphereInFrustum(float x, float y, float z, float radius);

//	bool CubeInFrustum(float x, float y, float z, float size);*/

	#define COMPLETE_OUT 0
	#define INTERSECT    1
	#define COMPLETE_IN  2

	int BoxInFrustum(float x, float y, float z, float sizeX, float sizeY, float sizeZ) const;

	int BoxInFrustum(CVector3f& vMins, CVector3f& vMaxs) const;

	int PolygonInFrustum(CVector3f* pVerts, int iNum);


private:

	CPlane* m_pPlanes;
	int     m_iPlanesNum;
};

#endif // YOEL_FRUSTUM_H