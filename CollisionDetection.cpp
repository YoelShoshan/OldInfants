#include "CollisionDetection.h"



#include "Doom3Map.h"
extern CDoom3Map g_Doom3Map;

#include "Doom3Area.h"

#include "GMath.h"


float CCD::GetFreeDistanceForMove(CVector3f& vStart, CVector3f& vDir, float fDesiredT)
{
	//vDir.Normalize(); // DON'T

	CVector3f vTarget = (vStart+ (vDir*fDesiredT ));

	CDoom3Area*  pStartArea = g_Doom3Map.FindArea(vStart);
	CDoom3Area*  pEndArea   = g_Doom3Map.FindArea(vTarget);

	if (!pStartArea)
		return 0.f;
	
	/*if (!pEndArea)
		return 0.f;*/

	CVector3f* verts;

	CVector3f vTempCollision;

	float fFinalDistance=10000.f;

	float fTempDistance=0.f;


	//bool bSameArea;



	if ( (!pEndArea))
	{
		for (int i=0;i<pStartArea->m_iVBNum;i++)
		{
			verts = (CVector3f*) pStartArea->m_ppVBs[i]->Get(VB_HAS_VERTS);

			// find distance to plane
			fTempDistance = RayPlaneIntersectionT(vStart,vDir, pStartArea->m_pVBPlanes[i]);

			// find point of intersection on plane
			vTempCollision = GetRayPoint(vStart,vDir,fTempDistance);

			/////////////////////////////
			// REMEMBER EPSILONS!!!!
			/////////////////////////////
			
			//bool PointInPolygon(CVector3f& Point, CVector3f* pVerts,int iNum)

			if (PointInPolygon(vTempCollision, verts, pStartArea->m_ppVBs[i]->GetSize()))
			{
				//if (  (fTempDistance>0.f) && (fTempDistance <= fDesiredT) ) 
				if (  abs(fTempDistance) <= abs(fDesiredT) ) 
				{
					fFinalDistance = fTempDistance;
				}

			}

			// if vTempCollision is INSIDE that polygon
			// {
			//   if (fTempDistance is <= fDesiredT)  // we are going to hit it
			//   {  
			//      if (fTempDistance <  fFinalDistance)
			//          fFinalDistance = fTempDistance;
			//   }
			// }
            			
		}

		if (fFinalDistance!=10000.f)
		{
			if (fFinalDistance<0)
				return fFinalDistance+EPSILON*3;

			if (fFinalDistance>0)
				return fFinalDistance-EPSILON;

			return 0.f;
		}
		else
			return 0.f;
		//return 0.f;
		//return fDesiredT;
	}
	else
	{
		// just go... - here the recursion should end
		return fDesiredT;
		//return 0.f;
	}
}