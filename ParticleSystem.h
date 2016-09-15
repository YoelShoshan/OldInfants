#ifndef YOEL_PARTICLE_SYSTEM_H
#define YOEL_PARTICLE_SYSTEM_H

#pragma once

#include <Windows.h>

#include "SmartPointer.h"
#include "ReferenceCount.h"

#include "Math3d.h"
#include "Texture.h"

#include <vector>

#define PS_MANAGER_MAXIMUM_SYSTEMS 500

class CParticle
{
public:

	CVector3f m_vPos;
	CVector3f m_vVelocity;
	CVector3f m_vAccel;

	//CTexture *m_pTex
	float m_fLife; // 0==dead,100==full
	bool m_bActive;
};


#define PARTICLE_SYSTEM_TYPE_FIRE                   1
#define PARTICLE_SYSTEM_TYPE_SMOKE                  2
#define PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE          3 // used only for players burning
#define PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE_FASTER   4 
#define PARTICLE_SYSTEM_TYPE_MAGIC_BLUE             5
#define PARTICLE_SYSTEM_TYPE_MAGIC_RED              6


typedef std::vector<CParticle> PARTICLES_VECTOR;
typedef std::vector<CParticle>::iterator PARTICLES_VECTOR_I;

class CParticleSystem
{

public:

	CParticleSystem();
	~CParticleSystem();
	
	void Init(DWORD dwType,int iParticlesMax);

	void UpdateParticles(void);

	void DebugDrawParticles(void);

	void SetParticlePos(CVector3f* vSet,bool bFollowAnimation);
	CVector3f GetParticlePos(void) { return (*m_pvStartPosition);};
	void AddToCenterPos(CVector3f& vAdd) { (*m_pvStartPosition)+=vAdd;};

	void SetOffsetPos(CVector3f vSet) {m_vOffsetPos = vSet;} ;

	void SetRevive() { m_bRevive=true;};
	void SetDie() { m_bRevive=false;};

	bool GetEntireSystemDead(void){ return m_bEntireSystemDead;};
	
	void SetDelayedStart(float fSet) { m_fDelayedStart = fSet;};


	void SetTimeToDeath(float fSet) { m_fTimeToDeath = fSet;};

	DWORD GetType(void) { return m_dwSystemType;};

private:

	bool m_bFirstTimeActivated;

	float m_fParticleSize;

	float m_fDelayedStart;

	float m_fTimeToDeath;

	CVector3f m_vOffsetPos;

	bool m_bRevive;


	int m_iTexturesNum;


	CTexture ** m_ppTextures;
	DWORD m_dwSystemType;

	CVector3f m_vGravity;
	CVector3f m_vRandomRangeS;
	CVector3f m_vRandomRangeE;
	float m_fLifeRegenerateS;
	float m_fLifeRegenerateR;

	int m_iParticlesMax;
	int m_iParticlesNum;

	CVector3f* m_pvStartPosition;

	//CParticle *m_pParticles;
	PARTICLES_VECTOR m_pParticles;

	bool m_bEntireSystemDead;

};


#define st_PSManager CSingleton<CPSManager>
#define st_PSManagerI CSingleton<CPSManager>::Inst()

class CPSManager
{
public:
	CPSManager(); 
	~CPSManager();

	// pvPos should always point to valid mem!!!!
	// i did this to easily animate the system according to an animated point
	CParticleSystem* PushParticleSystem(DWORD dwType,int iParticlesMax, CVector3f* pvPos,bool bFollowAni, CVector3f vOffset,float fDelayTime);

	void Update(void);

	void Draw(void);

	bool IsThereAnyActiveSystem(DWORD dwType);



private:

	int m_iParticleSystemsNum;
	CParticleSystem ** m_ppParticleSystems;

};



#endif // YOEL_PARTICLE_SYSTEM_H