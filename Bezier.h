#ifndef YOEL_BEZIER_H
#define YOEL_BEZIER_H

#pragma once

#include <windows.h>
#include "Math3D.h"

//#include "ZedTracker.h"

#define DEFAULT_BEZIER_PATCH_TESSELATION_LEVEL 7


CVector3f EvaluateQuadricBezierCurve(float fT, const CVector3f &p0, const CVector3f &p1, const CVector3f &p2);
CVector2f EvaluateQuadricBezierCurve(float fT,CVector2f &p0,CVector2f &p1,CVector2f &p2);

CVector3f evalQuadricBezierDerivative(float t, CVector3f &p0,CVector3f &p1,CVector3f &p2);

// pOriginalVerts and pOriginalTC MUST point to valid 9 CVector3f/CVector2f
// pTargetVerts and pTargetTC MUST point to NULL
// returns number of sub3x3patches created
int EvaluateQuadricBezierPatch(CVector3f* pOriginalVerts, CVector3f* pOriginalNormals, CVector2f* pOriginalTC,CVector2f* pOriginalLMC,
							   int iStepsX, int iStepsY,
							   CVector3f* pTargetVerts, CVector3f* pTargetNormals, CVector2f* pTargetTC,CVector2f* pTargetLMC,
							   int iOriginalPatchDimX, int iOriginalPatchDimY,UINT* puiNewIndices);




class CBezierRoute
{
public:
	CBezierRoute();
	~CBezierRoute();

	void AllocRoute(int iNum);	
    
	// fT should be in the range [0,(m_iVertsNum-1) /2] 
	// (if it's bigger then the last point will be returned)
	CVector3f FindPosInRoute(float fT);


CVector3f* m_pVertsArray;
int        m_iVertsNum;


};










#endif // YOEL_BEZIER_H