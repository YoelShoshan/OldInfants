#include "ParticleSystem.h"

#include "Singleton.h"

#include "Renderer.h"

#include <GL/gl.h>

#include "Camera.h"
extern CCamera g_Camera1;


CParticleSystem::CParticleSystem()
{
	//m_bRevive=false;
	m_bRevive=true;
	m_bEntireSystemDead = true;
	m_iParticlesMax=0;
	m_dwSystemType = 0;
	m_bFirstTimeActivated=true;
	

}

CParticleSystem::~CParticleSystem()
{
	SafeDeleteArray(m_ppTextures);
}

void CParticleSystem::Init(DWORD dwType,int iParticlesMax)
{
	m_bFirstTimeActivated=true;
	m_vOffsetPos.Set(0.f,0.f,0.f);
	m_fTimeToDeath=-10000.f;
	m_fDelayedStart=0.f;

	m_bEntireSystemDead = false;

	m_dwSystemType = dwType;
	m_iParticlesMax = iParticlesMax;
	//m_iParticlesNum = 0; //????
	m_iParticlesNum = iParticlesMax;
	//m_pParticles = new CParticle[m_iParticlesMax];
	m_pParticles.resize(iParticlesMax);

	if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE || m_dwSystemType == PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE_FASTER)
			m_iTexturesNum = 2;

	if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_SMOKE)
			m_iTexturesNum = 1;

	if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_MAGIC_BLUE)
			m_iTexturesNum = 1;

	if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_MAGIC_RED)
			m_iTexturesNum = 1;

	

	



	m_ppTextures = new CTexture*[m_iTexturesNum];
	

	if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE)
	{
		m_ppTextures[0] = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\ParticleSystems\\fire2.tga",false,true,true);
		m_ppTextures[1] = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\ParticleSystems\\smoke2.tga",false,true,true);

		m_vGravity.Set(0.f,-5.f,0.f);
		m_vRandomRangeS.Set(-1.f,1.f,-1.f);
		m_vRandomRangeE.Set(1.f,4.f,1.f);

		m_fParticleSize=10.f;

		m_fLifeRegenerateS = 40.f;
		m_fLifeRegenerateR = 60.f;
	}
	else
	if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE_FASTER)
	{
		m_ppTextures[0] = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\ParticleSystems\\fire2.tga",false,true,true);
		m_ppTextures[1] = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\ParticleSystems\\smoke2.tga",false,true,true);

		m_vGravity.Set(0.f,-5.f,0.f);
		m_vRandomRangeS.Set(-1.f,1.f,-1.f);
		m_vRandomRangeE.Set(1.f,8.f,1.f);

		m_fParticleSize=7.5f;

		m_fLifeRegenerateS = 20.f;
		m_fLifeRegenerateR = 30.f;

	}
	if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_SMOKE)
	{
		//m_ppTextures[0] = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\ParticleSystems\\magic1.tga",false,true,true);
		m_ppTextures[0] = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\ParticleSystems\\smoke2.tga",false,true,true);

		m_vGravity.Set(0.f,-5.f,0.f);
		m_vRandomRangeS.Set(-1.f,1.f,-1.f);
		m_vRandomRangeE.Set(1.f,1.5f,1.f);
		m_fParticleSize=10.f;

		m_fLifeRegenerateS = 40.f;
		m_fLifeRegenerateR = 60.f;
	}
	else
	if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_MAGIC_BLUE)
	{
		//m_ppTextures[0] = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\ParticleSystems\\magic1.tga",false,true,true);
		m_ppTextures[0] = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\ParticleSystems\\blue1.tga",false,true,true);

		m_vGravity.Set(0.f,-5.f,0.f);
		m_vRandomRangeS.Set(-1.f,1.f,-1.f);
		m_vRandomRangeE.Set(1.5f,2.f,1.5f);
		m_fParticleSize=10.f;

		m_fLifeRegenerateS = 40.f;
		m_fLifeRegenerateR = 60.f;
	}
	else
	if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_MAGIC_RED)
	{
		//m_ppTextures[0] = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\ParticleSystems\\magic1.tga",false,true,true);
		m_ppTextures[0] = st_TextureManager::Inst()->LoadTexAndPush("ceGui\\ParticleSystems\\red1.tga",false,true,true);

		m_vGravity.Set(0.f,-5.f,0.f);
		m_vRandomRangeS.Set(-1.f,1.f,-1.f);
		m_vRandomRangeE.Set(1.5f,2.f,1.5f);
		m_fParticleSize=10.f;

		m_fLifeRegenerateS = 40.f;
		m_fLifeRegenerateR = 60.f;
	}
		
		


	//m_vStartPosition.Set(0.f,25.f,0.f);
	m_pvStartPosition = new CVector3f;
	m_pvStartPosition->Set(0.f,25.f,0.f);





	for (int i=0;i<m_iParticlesMax;i++)
	{
		m_pParticles[i].m_bActive=false;
		//m_pParticles[i].m_fLife=100.f;
		//m_pParticles[i].m_fLife= 40 + 60.f * RANDOM_NUM;
		m_pParticles[i].m_fLife= 60.f * RANDOM_NUM;
		//m_pParticles[i].m_fLife= 20.f * RANDOM_NUM;
		m_pParticles[i].m_vPos = (*m_pvStartPosition);
	}

}

void CParticleSystem::UpdateParticles(void)
{	
	
	// ORIGIANL
	if (m_fDelayedStart>0.f)
	{
		m_fDelayedStart-=st_Timer::Inst()->GetDeltaTimeFAKE();
		return;
	}


	if (m_fTimeToDeath>-5000.f) // if activated
		m_fTimeToDeath-=st_Timer::Inst()->GetDeltaTimeFAKE();

	if (m_fTimeToDeath<=0.f && m_fTimeToDeath>-5000.f)
		SetDie();

	float fRandomX,fRandomY,fRandomZ;

	int iCountDeadParticles=0;
	
	for (int i=0;i<m_iParticlesMax;i++)
	{

		float fDebugLife = m_pParticles[i].m_fLife;

		if (m_pParticles[i].m_fLife<0.f)
		{
			m_pParticles[i].m_bActive=false;
			iCountDeadParticles++;
		}

		if ((!m_pParticles[i].m_bActive) && m_bRevive) // revive
		{

			iCountDeadParticles--;

			fRandomX = RANDOM_NUM;
			fRandomY = RANDOM_NUM;
			fRandomZ = RANDOM_NUM;
			
			if (m_vRandomRangeS.v[0]>=0 && m_vRandomRangeE.v[0]>=0)
				fRandomX= (fRandomX*m_vRandomRangeE.v[0])+m_vRandomRangeS.v[0];
			else
			if (m_vRandomRangeS.v[0]<0 && m_vRandomRangeE.v[0]<0)
			{
				fRandomX*=m_vRandomRangeE.v[0];
				fRandomX=-fRandomX;
			}
			else
			{
				fRandomX-=0.5f;
				fRandomX*=m_vRandomRangeE.v[0];				
			}



			if (m_vRandomRangeS.v[1]>=0 && m_vRandomRangeE.v[1]>=0)
				fRandomY= (fRandomY*m_vRandomRangeE.v[1])+m_vRandomRangeS.v[1];
			else
			if (m_vRandomRangeS.v[1]<0 && m_vRandomRangeE.v[1]<0)
			{
				fRandomY*=m_vRandomRangeE.v[1];
				fRandomY=-fRandomY;
			}
			else
			{
				fRandomY-=0.5f;
				fRandomY*=m_vRandomRangeE.v[1];				
			}



			if (m_vRandomRangeS.v[2]>=0 && m_vRandomRangeE.v[2]>=0)
				fRandomZ= (fRandomZ*m_vRandomRangeE.v[2])+m_vRandomRangeS.v[2];
			else
			if (m_vRandomRangeS.v[2]<0 && m_vRandomRangeE.v[2]<0)
			{
				fRandomZ*=m_vRandomRangeE.v[2];
				fRandomZ=-fRandomZ;
			}
			else
			{
				fRandomZ-=0.5f;
				fRandomZ*=m_vRandomRangeE.v[2];				
			}


			


			m_pParticles[i].m_vVelocity.Set(fRandomX+m_vOffsetPos.v[0],fRandomY+m_vOffsetPos.v[1],fRandomZ+m_vOffsetPos.v[2]);			

			m_pParticles[i].m_bActive = true;

			//m_pParticles[i].m_fLife=100.f;

			//m_pParticles[i].m_fLife= 40 + 60.f * RANDOM_NUM;

			if (m_bFirstTimeActivated)
				m_pParticles[i].m_fLife= 100 * RANDOM_NUM;
			else
				m_pParticles[i].m_fLife= 40 + 60.f * RANDOM_NUM;
			
			m_pParticles[i].m_vPos = (*m_pvStartPosition);
		}
		
		
		//m_pParticles[i].m_fLife -=st_Timer::Inst()->GetDeltaTimeFAKE()*0.03f;
		m_pParticles[i].m_fLife -=st_Timer::Inst()->GetDeltaTimeFAKE()*0.09f;

		m_pParticles[i].m_vPos += m_pParticles[i].m_vVelocity * st_Timer::Inst()->GetDeltaTimeFAKE()*0.015f;



	}



	if (!m_bRevive)
		if (iCountDeadParticles>=m_iParticlesMax-1)
			m_bEntireSystemDead = true;

	m_bFirstTimeActivated=false;
}

void CParticleSystem::DebugDrawParticles(void)
{

	/*if (m_fDelayedStart>0.f)
	{
		m_fDelayedStart-=st_Timer::Inst()->GetDeltaTimeFAKE();
		return;
	}*/

	static CVector3f v1,v2,v3,v4;
	static CVector3f newPos;

	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();

	CMatrix* m = g_Camera1.ReturnCameraMatrix();

	for (int i=0;i<m_iParticlesMax;i++)
	{
		if (m_pParticles[i].m_bActive)
		{
			/*newPos = g_Camera1.RotateWithCameraRotationAndTranslateMatrix(m_pParticles[i].m_vPos);*/
			newPos = (*m) * m_pParticles[i].m_vPos;

/*			if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE)
				glColor3f(((m_pParticles[i].m_fLife*0.005f)),((m_pParticles[i].m_fLife*0.005f)),((m_pParticles[i].m_fLife*0.005f)));
			else
			if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_SMOKE)*/
				glColor3f(((m_pParticles[i].m_fLife*0.005f)),((m_pParticles[i].m_fLife*0.005f)),((m_pParticles[i].m_fLife*0.005f)));
			


			v1.Set(newPos + CVector3f(-m_fParticleSize,-m_fParticleSize,0.f));
			v2.Set(newPos + CVector3f(m_fParticleSize,-m_fParticleSize,0.f));
			v3.Set(newPos + CVector3f(m_fParticleSize,m_fParticleSize,0.f));
			v4.Set(newPos + CVector3f(-m_fParticleSize,m_fParticleSize,0.f));


			if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE || m_dwSystemType == PARTICLE_SYSTEM_TYPE_FIRE_TO_SMOKE_FASTER)
			{
				if (m_pParticles[i].m_fLife<40.f)
					st_RendererI->DebugDrawQuad(m_ppTextures[1],v1,v2,v3,v4,true);
				else
					st_RendererI->DebugDrawQuad(m_ppTextures[0],v1,v2,v3,v4,true);
			}
			else
			if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_SMOKE)
				st_RendererI->DebugDrawQuad(m_ppTextures[0],v1,v2,v3,v4,true);
			else
			if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_MAGIC_BLUE)
				st_RendererI->DebugDrawQuad(m_ppTextures[0],v1,v2,v3,v4,true);
			else
			if (m_dwSystemType == PARTICLE_SYSTEM_TYPE_MAGIC_RED)
				st_RendererI->DebugDrawQuad(m_ppTextures[0],v1,v2,v3,v4,true);
				

		}

	}

	SafeDelete(m);
}

void CParticleSystem::SetParticlePos(CVector3f* pvSet,bool bFollowAnimation)
{
	if (bFollowAnimation)
		m_pvStartPosition = pvSet;
	else
		m_pvStartPosition->Set((*pvSet));
}

////////////////////////////////////
//// PARTICLE SYSTEMS MANAGER //////
////////////////////////////////////


CPSManager::CPSManager()
{
	m_iParticleSystemsNum=0;
	m_ppParticleSystems = new CParticleSystem*[PS_MANAGER_MAXIMUM_SYSTEMS];

	for (int i=0;i<PS_MANAGER_MAXIMUM_SYSTEMS;i++)
		m_ppParticleSystems[i] = new CParticleSystem;

}

CPSManager::~CPSManager()
{
	for (int i=0;i<PS_MANAGER_MAXIMUM_SYSTEMS;i++)
		SafeDelete(m_ppParticleSystems[i]);
	SafeDeleteArray(m_ppParticleSystems);
}

CParticleSystem* CPSManager::PushParticleSystem(DWORD dwType,int iParticlesMax, CVector3f* pvPos,bool bFollowAni, CVector3f vOffset,float fDelayTime)
{
	for (int i=0;i<60;i++)
	{
		if (m_ppParticleSystems[i]->GetEntireSystemDead())
		{			
			m_ppParticleSystems[i]->Init(dwType,iParticlesMax);
			m_ppParticleSystems[i]->SetParticlePos(pvPos,bFollowAni);
			m_ppParticleSystems[i]->SetRevive();

			if (fDelayTime>0.f)
				m_ppParticleSystems[i]->SetDelayedStart(fDelayTime);

			return m_ppParticleSystems[i];
		}
	}




	// non found - using too many particle systems.
	return NULL;
}

void CPSManager::Update(void)
{
	for (int i=0;i<PS_MANAGER_MAXIMUM_SYSTEMS;i++)
	{
		if (!(m_ppParticleSystems[i]->GetEntireSystemDead()))
			m_ppParticleSystems[i]->UpdateParticles();		
	}

}

void CPSManager::Draw(void)
{
	for (int i=0;i<PS_MANAGER_MAXIMUM_SYSTEMS;i++)
	{
		if (!m_ppParticleSystems[i]->GetEntireSystemDead())
			m_ppParticleSystems[i]->DebugDrawParticles();
	}
}

bool CPSManager::IsThereAnyActiveSystem(DWORD dwType)
{
	for (int i=0;i<PS_MANAGER_MAXIMUM_SYSTEMS;i++)
	{
		if (!m_ppParticleSystems[i]->GetEntireSystemDead())
			if (m_ppParticleSystems[i]->GetType() == dwType)
				return true;
	}

	return false;
}
