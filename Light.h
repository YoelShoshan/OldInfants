#ifndef YOEL_LIGHT_H
#define YOEL_LIGHT_H

#pragma once

#include "math3d.h"


class CLight
{
public:

	CLight() { m_bAlive=false; m_bFadeDie=false;};

	CVector3f m_vPos;
	CVector3f m_vColor;
	float     m_fRadius;

	bool m_bFadeDie;
	bool m_bAlive;
};














#endif // YOEL_LIGHT_H