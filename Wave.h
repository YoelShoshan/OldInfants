#ifndef YOEL_WAVE_H
#define YOEL_WAVE_H

#pragma once


#include "defines.h"


// Wave Functions

#define WF_NONE            0
#define WF_SIN             1
#define WF_TRIANGLE        2
#define WF_SQUARE          3
#define WF_SAWTOOTH        4
#define WF_INVERSESAWTOOTH 5
#define WF_NOISE           6


/////////////////////////////////////////////
// class CWave
// *****************
// a general wave class
//////////////////////////////////////////////

class CWave
{
public:
	CWave();
	CWave(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq);
	~CWave();

	void SetParams(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq);

	float ComputeSin(float fTime);
	float ComputeTriangle(float time);
	float ComputeSquare(float time);
	float ComputeSawTooth(float time);
	float ComputeInverseSawTooth(float time);
	float ComputeNoise(float time);

	float Compute(float fTime);

	UINT  m_uiWaveType;
	float m_fBase;
	float m_fAmplitude;
	float m_fPhase;
	float m_fFreq;
};



#endif // YOEL_WAVE_H