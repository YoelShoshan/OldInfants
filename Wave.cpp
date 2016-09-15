#include "Wave.h"

CWave::CWave()
{
}

CWave::CWave(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	SetParams(uiWaveType,fBase,fAmplitude,fPhase,fFreq);
}


CWave::~CWave()
{
	m_uiWaveType  = WF_SIN;
	m_fBase       = 0.5f;
	m_fAmplitude  = 0.f;
	m_fPhase      = 0.f;
	m_fFreq       = 0.f;

}

void CWave::SetParams(UINT uiWaveType,float fBase,float fAmplitude,float fPhase,float fFreq)
{
	m_uiWaveType  = uiWaveType;
	m_fBase       = fBase;
	m_fAmplitude  = fAmplitude;
	m_fPhase      = fPhase;
	m_fFreq       = fFreq;
}

float CWave::Compute(float fTime)
{
	//// ONLY FOR DEBUG!!!!!!!!
	fTime/=1000.f;
	//////////////////////////

	if (m_uiWaveType==WF_SIN)
		return ComputeSin(fTime);
	else
		if (m_uiWaveType==WF_TRIANGLE)
			return ComputeTriangle(fTime);
		else
			if (m_uiWaveType==WF_SQUARE)
				return ComputeSquare(fTime);
			else
				if (m_uiWaveType==WF_SAWTOOTH)
					return ComputeSawTooth(fTime);
				else
					if (m_uiWaveType==WF_INVERSESAWTOOTH)
						return ComputeInverseSawTooth(fTime);
					else
						if (m_uiWaveType==WF_NOISE)
							return ComputeNoise(fTime);
						else
							return ComputeSin(fTime);
}

float CWave::ComputeSin(float fTime)
{
	fTime = m_fPhase + m_fFreq*fTime;
	fTime -= floorf(fTime);
	fTime = sinf (fTime*TWOPI);
	return m_fBase + m_fAmplitude*fTime;
}


float CWave::ComputeTriangle(float fTime)
{
	fTime = m_fPhase + m_fFreq*fTime;
	fTime -= floorf(fTime);
	fTime = (fTime < 0.5f)? 2.0f*fTime : 2.0f-2.0f*fTime;
	return m_fBase + m_fAmplitude*fTime;
}
float CWave::ComputeSawTooth(float fTime)
{
	fTime = m_fPhase + m_fFreq*fTime;
	fTime -= floorf(fTime);
	return m_fBase + m_fAmplitude*fTime;
}
float CWave::ComputeInverseSawTooth(float fTime)
{
	fTime = m_fPhase + m_fFreq*fTime;
	fTime -= floorf(fTime);
	fTime=  1.0f-fTime;
	return m_fBase + m_fAmplitude*fTime;
}
float CWave::ComputeSquare(float fTime)
{
	fTime = m_fPhase + m_fFreq*fTime;
	fTime -= floorf(fTime);
	fTime= (fTime < 0.5) ? 1.0f : -1.0f;
	return m_fBase + m_fAmplitude*fTime;
}
float CWave::ComputeNoise(float fTime)
{
	fTime = m_fPhase + m_fFreq*fTime;
	fTime -= floorf(fTime);
	fTime=(float)rand()*(1.0f/(float)RAND_MAX);
	//fTime=(((float)rand()+(float)rand())/2.0f) / (float)RAND_MAX;

	return m_fBase + m_fAmplitude*fTime;
	////return fTime;
}

