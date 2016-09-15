#ifndef YOEL_COLLISION_DETECTION_H
#define YOEL_COLLISION_DETECTION_H

#pragma once

#define st_CD CSingleton<CCD>

#include <Windows.h>
#include "Math3d.h"

class CCD
{
public:
	float GetFreeDistanceForMove(CVector3f& vStart, CVector3f& vDir, float fDesiredT);
};



#endif // YOEL_COLLISION_DETECTION_H