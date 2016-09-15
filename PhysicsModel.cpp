#include "PhysicsModel.h"

extern CVector3f g_vGravity;

//#include "Defines.h"


extern bool g_bBotDistanceToFloorTooBig;

#include "Q3AMap.h"
extern CQuake3BSP* g_pQuake3Map;
//extern bool      g_bPlayerFlying;

#include "LogFile.h"
extern CLogFile g_LogFile;

#include "Engine.h"

extern bool g_bTestCollisionDetection;
extern bool g_bFinalDirectionIsOppositeThenDesired;

/*extern int g_iShootTestFlags;
extern int g_iShootTestContents;*/

#include "md3.h"
extern CMD3* g_pWeaponModel;

extern CVector3f g_vWeaponPos;

#include "Timer.h"

#include "GMath.h"


#include "SoundManager.h"
extern CSoundManager g_SoundManager;

CPhysicsModel::CPhysicsModel()
{
	Init(20.f); // default radius = 20.f
    	
}

CPhysicsModel::CPhysicsModel(float fSphereSize)
{
	Init(fSphereSize);
}

CPhysicsModel::~CPhysicsModel()
{
    	
}

void CPhysicsModel::Init(float fSphereSize)
{
	m_vOutsideForce.Set(0.f,0.f,0.f);

	m_dwPlayerState = kNotSet;

	//m_vOutsidersForce.Set(0.f,0.f,0.f);
	m_fRadius = fSphereSize;
	//m_vPos.Set(0.f,0.f,0.f);
	m_vVelocity.Set(0.f,0.f,0.f);
	m_vAcceleration.Set(0.f,0.f,0.f);
	m_bPlayerInAir=false;
	m_fTimePlayerInAir=0.f;
	m_fDistanceToFloor=0.f;
	m_bChangePositionNow=false;

	m_bJumping=false;
	m_bStartJumping=false;

	m_bLanding = false;

	m_bPlayerFlying = false;
	//m_bPlayerFlying = true;



	m_bClimbingStair = false;
	m_fClimbingPhase = 0.f; // [0.f,1.f]
	//m_fClimbingStepNum = 3.f;
	m_fClimbingStepNum = 6.f;
	//m_fClimbingStepNum = 8.f;
	//m_fClimbingStepNum = 16.f;



	m_vLookDirection.Set(0.f,0.f,0.f);
	m_fAngX =0.f;
	m_fAngY =0.f;

	m_dwMoveRotFlags = 0;


	RotateYSpeed=3.f;
	RotateXSpeed=0.5f;
	RotateZSpeed=1.f;
}

void CPhysicsModel::SetSoundSamples(FSOUND_SAMPLE** pSet)
{ 
	m_ppSoundSamples = pSet;
}


CVector3f CPhysicsModel::GetLookDirection(void)
{

	m_vLookDirection = GetVectorFromAngles(m_fAngX,m_fAngY);

	return m_vLookDirection;

}

float CPhysicsModel::computeDistanceToFloor(CVector3f& vPos, CEntity* pCausedByEntity)
{
	CTraceRequest OnGroundMoveInput;
	CTraceResult     OnGroundMoveOutput;

	OnGroundMoveInput.dwTraceType = TRACE_TYPE_SPHERE;
	
	//// ONLY WORKS IF YOU'RE _ABOVE_

	OnGroundMoveInput.fRadius = m_fRadius*0.75f;

	OnGroundMoveInput.vRayStart = vPos + CVector3f(0.f,m_fRadius+1.f,0.f);
	OnGroundMoveInput.vRayEnd   = vPos + CVector3f(0.f,-m_fRadius,0.f);


	OnGroundMoveInput.dwTraceCausedBy = TRACE_CAUSED_BY_TEST;

	//g_pQuake3Map->Trace(&OnGroundMoveInput,&OnGroundMoveOutput,false,-1); // should be like this
	g_pQuake3Map->Trace(&OnGroundMoveInput,&OnGroundMoveOutput,false,true,pCausedByEntity);
	
	m_fDistanceToFloor = DISTANCE(OnGroundMoveOutput.EndPoint,m_vPos);

	return DISTANCE(OnGroundMoveOutput.EndPoint,m_vPos);
}


bool CPhysicsModel::lineOfSight(CVector3f& vTarget)
{
	static	CTraceRequest MoveSafeInput; // remember to make it a member
	static   CTraceResult  MoveSafeOutput;


	// test move

	/*MoveSafeInput.dwTraceType = TRACE_TYPE_SPHERE;
	MoveSafeInput.fRadius = m_fRadius*0.10;*/
	MoveSafeInput.dwTraceType = TRACE_TYPE_LINE;
	MoveSafeInput.dwTraceCausedBy = TRACE_CAUSED_BY_TEST;
	MoveSafeInput.vRayStart = m_vPos;
	MoveSafeInput.vRayEnd   = vTarget;

	g_pQuake3Map->Trace(&MoveSafeInput,&MoveSafeOutput,false,false,NULL); // remember it was -2 here - means that it should not activate anything or sth like that

	if (MoveSafeOutput.Fraction!=1.f)
		return false;
	
	return true;
}

float CPhysicsModel::wouldMoveLeadToPit(CVector3f& vPos, CVector3f& vTarget, CVector3f& vFinal)
{
		/**************************************************/
		// check if move is safe (won't lead to a fall)

		static	CTraceRequest MoveSafeInput; // remember to make it a member
		static   CTraceResult  MoveSafeOutput;


		// test move

		MoveSafeInput.dwTraceType = TRACE_TYPE_SPHERE;
		//MoveSafeInput.fRadius = m_fRadius*0.75;
		MoveSafeInput.fRadius = m_fRadius*1.25;
		MoveSafeInput.dwTraceCausedBy = TRACE_CAUSED_BY_TEST;
		MoveSafeInput.vRayStart = m_vPos;
		//MoveSafeInput.vRayEnd   = m_vPos + (vTarget-m_vPos).Normalize() * 0.03f*st_Timer::Inst()->GetDeltaTimeFAKE();
		MoveSafeInput.vRayEnd   = vTarget;

		g_pQuake3Map->TraceAndResponse(&MoveSafeInput,&MoveSafeOutput,0,NULL);


		vFinal = MoveSafeOutput.EndPoint;



		// check height on that location
		
		
		MoveSafeInput.fRadius = m_fRadius;
		MoveSafeInput.vRayStart = MoveSafeOutput.EndPoint;
		MoveSafeInput.vRayEnd   = MoveSafeOutput.EndPoint + CVector3f(0.f,-3000.f,0.f);
		MoveSafeInput.dwTraceCausedBy = TRACE_CAUSED_BY_TEST;
		g_pQuake3Map->Trace(&MoveSafeInput,&MoveSafeOutput,false,true,NULL);
		
	
		float fDistanceToFloor = DISTANCE(MoveSafeOutput.EndPoint,m_vPos);

		//float fDistanceToFloor = computeDistanceToFloor(MoveSafeOutput.EndPoint,-1);


		 return fDistanceToFloor;
		
		/**************************************************/
}

float CPhysicsModel::updateDistanceToFloor( CEntity* pCausedByEntity)
{
	CTraceRequest OnGroundMoveInput;
	CTraceResult     OnGroundMoveOutput;

	OnGroundMoveInput.dwTraceType = TRACE_TYPE_SPHERE;
	
	//// ONLY WORKS IF YOU'RE _ABOVE_

	OnGroundMoveInput.fRadius = m_fRadius*0.75f;

	OnGroundMoveInput.vRayStart = m_vPos + CVector3f(0.f,m_fRadius+1.f,0.f);
	OnGroundMoveInput.vRayEnd   = m_vPos + CVector3f(0.f,-m_fRadius,0.f);


	OnGroundMoveInput.dwTraceCausedBy = TRACE_CAUSED_BY_TEST;

	//g_pQuake3Map->Trace(&OnGroundMoveInput,&OnGroundMoveOutput,false,-1); // should be like this
	g_pQuake3Map->Trace(&OnGroundMoveInput,&OnGroundMoveOutput,false,true,pCausedByEntity);
	
	m_fDistanceToFloor = DISTANCE(OnGroundMoveOutput.EndPoint,m_vPos);


	// RESTORE RESTORE RESTORE 17 06 04
	/*
	if (m_fDistanceToFloor<10.f)
	{
		m_iContentsOfTouchedGround = OnGroundMoveOutput.HitContents;
		m_iFlagsOfTouchedGround = OnGroundMoveOutput.HitFlags;
	}
	else
	{
		m_iContentsOfTouchedGround = 0;
		m_iFlagsOfTouchedGround = 0;
	}
	*/
	///

	OnGroundMoveInput.vRayStart = m_vPos;
	OnGroundMoveInput.vRayEnd   = m_vPos + CVector3f(0.f,-3000.f,0.f);
	OnGroundMoveInput.dwTraceCausedBy = TRACE_CAUSED_BY_PLAYER;
	OnGroundMoveInput.fRadius = m_fRadius;
	g_pQuake3Map->Trace(&OnGroundMoveInput,&OnGroundMoveOutput,false,true,NULL);

	m_fDistanceToFloor = DISTANCE(OnGroundMoveOutput.EndPoint,m_vPos);


	st_Engine::Inst()->m_fHeightAboveGround = m_fDistanceToFloor;

	

	//if (m_fDistanceToFloor>EPSILON)
	if (m_fDistanceToFloor>3.25f)
	{
		m_bPlayerInAir = true;
		m_fTimePlayerInAir += st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;
	}
	else
	{
		m_bPlayerInAir = false;
		m_fTimePlayerInAir =0.f;
	}

	return m_fDistanceToFloor;
}



CVector3f CPhysicsModel::tryProjectileMove(CVector3f& vMoveVelocity,  CEntity* pCausedByEntity)
{
	static CTraceRequest PhysicsModelMoveTraceInput; // remember to make it a member
	static CTraceResult  PhysicsModelMoveTraceOutput;


	CVector3f vDesiredPosition;
	CVector3f vOriginalDir;


	CVector3f vReturnNormal(0.f,0.f,0.f);


	if ( GetChangePositionNow()) // if need to change position now (teleporter) don't try the move
	{
		m_vVelocity.v[0] = m_vVelocity.v[1] = m_vVelocity.v[2] = 0.f;
		ChangePositionNow(false);
		return vReturnNormal;			
	}

	// just a try - some limits for both position and velocity
	/**/

	if (m_vVelocity.v[0] > 10000.f || m_vVelocity.v[0] < -10000.f)
		m_vVelocity.v[0] = 0.f;
	if (m_vVelocity.v[1] > 10000.f || m_vVelocity.v[1] < -10000.f)
		m_vVelocity.v[1] = 0.f;
	if (m_vVelocity.v[2] > 10000.f || m_vVelocity.v[2] < -10000.f)
		m_vVelocity.v[2] = 0.f;

	if (m_vPos.v[0] > 10000.f || m_vPos.v[0] < -10000.f)
		m_vPos.v[0] = 0.f;
	if (m_vPos.v[1] > 10000.f || m_vPos.v[1] < -10000.f)
		m_vPos.v[1] = 0.f;
	if (m_vPos.v[2] > 10000.f || m_vPos.v[2] < -10000.f)
		m_vPos.v[2] = 0.f;
	
	/**/


	vMoveVelocity*= 100.f*st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;		
	

	// set delta-time in seconds
		float fDeltaTime = st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;
	
		// if very small then make it zero
		if (m_vVelocity==CVector3f(0.f,0.f,0.f))
			m_vVelocity.Set(0.f,0.f,0.f);
	
		// the total velocity
		CVector3f vTotalVelocity(0.f,0.f,0.f);
	
		// add this move velocity to the total velocity
		vTotalVelocity+=vMoveVelocity;

		m_vVelocity -= m_vVelocity * st_Timer::Inst()->GetDeltaTimeFAKE()*0.01f * 0.97f; // friction for 3rd
	

		// s = s0 + v0*t + 1/2 * a * t^2

		vDesiredPosition = m_vPos + (m_vVelocity*fDeltaTime) + 
								 vTotalVelocity*(0.5f  * fDeltaTime * fDeltaTime )  ;


		// original normalized direction (used afterwards in the anti-shaking code)
		vOriginalDir = (vDesiredPosition - m_vPos).Normalize();


		//v=v0+at   --   update the velocity
		m_vVelocity = m_vVelocity + vMoveVelocity;

		// ading outside force (ex: got hit by something)

		m_vVelocity+=m_vOutsideForce;

		m_vOutsideForce.Set(0.f,0.f,0.f);


		PhysicsModelMoveTraceInput.dwTraceType = TRACE_TYPE_SPHERE;
		PhysicsModelMoveTraceInput.fRadius = m_fRadius;
		PhysicsModelMoveTraceInput.dwTraceCausedBy = TRACE_CAUSED_BY_PROJECTILE;
		PhysicsModelMoveTraceInput.vRayStart.Set(m_vPos);
		PhysicsModelMoveTraceInput.vRayEnd.Set(vDesiredPosition);

		g_pQuake3Map->Trace(&PhysicsModelMoveTraceInput,&PhysicsModelMoveTraceOutput,false,true,pCausedByEntity); // no collision response
		

/*		if (PhysicsModelMoveTraceOutput.Fraction==1.f)*/
			m_vPos = PhysicsModelMoveTraceOutput.EndPoint;



			//if (PhysicsModelMoveTraceOutput.Fraction!=1.f)
			vReturnNormal = PhysicsModelMoveTraceOutput.CollisionPlane.n;


			return vReturnNormal;


/*		else
		{
			PhysicsModelMoveTraceInput.dwTraceType = TRACE_TYPE_SPHERE;
			PhysicsModelMoveTraceInput.fRadius = 0.1f;
			PhysicsModelMoveTraceInput.dwTraceCausedBy = TRACE_CAUSED_BY_PROJECTILE;
			PhysicsModelMoveTraceInput.vRayStart.Set(m_vPos);
			PhysicsModelMoveTraceInput.vRayEnd.Set(vDesiredPosition);	

			g_pQuake3Map->Trace(&PhysicsModelMoveTraceInput,&PhysicsModelMoveTraceOutput,false,false,NULL); // no collision response

			m_vPos = PhysicsModelMoveTraceOutput.EndPoint;
		}*/

}


void CPhysicsModel::tryPlayerMove(CVector3f& vMoveVelocity,  CEntity* pCausedByEntity,bool bMove)
{
	static CVector3f vPosBackup;
	
	if (!bMove)
		vPosBackup = m_vPos;

	

	static CTraceRequest CPhysicsModelMoveTraceInput; // remember to make it a member
	static CTraceResult  CPhysicsModelMoveTraceOutput;

	bool bOverComeBumps;

	CVector3f vDesiredPosition;
	CVector3f vOriginalDir;





	// just a try
	/**/

	if (m_vVelocity.v[0] > 10000.f || m_vVelocity.v[0] < -10000.f)
		m_vVelocity.v[0] = 0.f;
	if (m_vVelocity.v[1] > 10000.f || m_vVelocity.v[1] < -10000.f)
		m_vVelocity.v[1] = 0.f;
	if (m_vVelocity.v[2] > 10000.f || m_vVelocity.v[2] < -10000.f)
		m_vVelocity.v[2] = 0.f;

	if (m_vPos.v[0] > 10000.f || m_vPos.v[0] < -10000.f)
		m_vPos.v[0] = 0.f;
	if (m_vPos.v[1] > 10000.f || m_vPos.v[1] < -10000.f)
		m_vPos.v[1] = 0.f;
	if (m_vPos.v[2] > 10000.f || m_vPos.v[2] < -10000.f)
		m_vPos.v[2] = 0.f;
	
	/**/




	if (!m_bClimbingStair)
	{
		// change the move according to the player "Strength"
		if (!m_bPlayerFlying) // if in air then less strength can be applied by player
		{
			// debug debug
			if (m_bPlayerInAir) // in air
				vMoveVelocity*= 100.f*st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;		
				//vMoveVelocity*= st_Timer::Inst()->GetDeltaTimeFAKE()*0.01f;		
			else // on ground
				vMoveVelocity*= 3.f*st_Timer::Inst()->GetDeltaTimeFAKE();
				//vMoveVelocity*= st_Timer::Inst()->GetDeltaTimeFAKE()*0.3f;
		}
		else
		{
			vMoveVelocity*= 25.f;

		}
	
		// set delta-time in seconds
		float fDeltaTime = st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;
	
		// if very small then make it zero
		if (m_vVelocity==CVector3f(0.f,0.f,0.f))
			m_vVelocity.Set(0.f,0.f,0.f);
	
		// the total velocity
		CVector3f vTotalVelocity(0.f,0.f,0.f);
	
		// add this move velocity to the total velocity
		vTotalVelocity+=vMoveVelocity;
	
	
	
		// simplified friction... ;)

		if (!m_bPlayerFlying) // walking-mode
		{
			if (!m_bPlayerInAir) // really walking (not falling/in air)
			{
				//m_vVelocity -= m_vVelocity * st_Timer::Inst()->GetDeltaTimeFAKE()*0.01f * 0.85f; // friction for fps
				m_vVelocity -= m_vVelocity * st_Timer::Inst()->GetDeltaTimeFAKE()*0.01f * 0.97f; // friction for 3rd
			}
		}
		else                  // flying-mode
			//m_vVelocity -= m_vVelocity * st_Timer::Inst()->GetDeltaTimeFAKE()*0.01f * 0.35f; // friction for fps
			m_vVelocity -= m_vVelocity * st_Timer::Inst()->GetDeltaTimeFAKE()*0.01f * 0.35f; // friction for 3rd
	

		// s = s0 + v0*t + 1/2 * a * t^2

		vDesiredPosition = m_vPos + (m_vVelocity*fDeltaTime) + 
								 vTotalVelocity*(0.5f  * fDeltaTime * fDeltaTime )  ;


		// original normalized direction (used afterwards in the anti-shaking code)
		vOriginalDir = (vDesiredPosition - m_vPos).Normalize();


		//v=v0+at   --   update the velocity
		m_vVelocity = m_vVelocity + vMoveVelocity;


		if (!m_bPlayerFlying)
			m_vVelocity = m_vVelocity + (g_vGravity* fDeltaTime);


		// jumping...

		/*if (!m_bPlayerInAir && !m_bStartJumping)
			m_bJumping = false;*/


		//if (m_bJumping && m_fTimePlayerInAir<0.05f)
		if (m_bStartJumping)
		{
			//m_dwSounds|=SOUND_FLAG_JUMP;
			//g_SoundManager.PlaySample(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_JUMP],100);
			g_SoundManager.PlaySample3D(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_JUMP],m_vPos,200);
			
			

			m_vVelocity.v[1]=300.f;
			//m_vVelocity.v[1]=200.f;
			m_bJumping=true;	
			m_bStartJumping = false;
		}
		else
		{
			if (m_fDistanceToFloor<EPSILON*2.f && m_vVelocity.v[1]<0.f)
				m_bJumping=false;
		}

		if (m_bJumping && m_fTimePlayerInAir>2.f)
			m_bJumping=false;


		//if (m_bPlayerInAir && m_fDistanceToFloor<40.f && m_vVelocity.v[1]<0.f)
		//if (m_bPlayerInAir && m_fDistanceToFloor<40.f && m_vVelocity.v[1]<-20.f)
		if (m_bPlayerInAir && m_fDistanceToFloor<10.f && m_vVelocity.v[1]<0.f && m_fTimePlayerInAir>0.5f)
		{
			m_bJumping=false;
			m_bLanding=true;

			//g_SoundManager.PlaySample(m_ppSoundSamples[CLIENT_SOUND_SAMPLE_LAND],100);

		}
		else
		{
			m_bLanding=false;
		}


		// ading outside force (ex: got hit by something)

		m_vVelocity+=m_vOutsideForce;

		m_vOutsideForce.Set(0.f,0.f,0.f);
		




		/******/


			m_dwPlayerState = 0; 


	if (m_bPlayerFlying) // flying
	{
		m_dwPlayerState |= kFlying;
	}
	else // not flying
	{

/*		if (vMoveVelocity==CVector3f(0.f,0.f,0.f) && !m_bJumping)
			m_dwPlayerState |= kIdle;
		else*/
		{
			if (!m_bPlayerInAir)
			{
				if (m_bJumping)
				{
					if (m_dwMoveRotFlags & PLAYER_MOVE_Z_BACKWARDS)
						m_dwPlayerState |= kJumpingB;
					else
						m_dwPlayerState |= kJumping;
				}
				else
				if (m_dwMoveRotFlags & PLAYER_MOVE_Z_FORWARD)
					m_dwPlayerState |= kRunning;
				else
				if (m_dwMoveRotFlags & PLAYER_MOVE_Z_BACKWARDS)
					m_dwPlayerState |= kMovingBackwards;
				else
				if (m_dwMoveRotFlags & PLAYER_MOVE_X_LEFT)
					m_dwPlayerState |= kStrafeLeft;
				else
				if (m_dwMoveRotFlags & PLAYER_MOVE_X_RIGHT)
					m_dwPlayerState |= kStrafeRight;
				else
				m_dwPlayerState |= kIdle;
			}
			else
			{
				if (m_bLanding)
					m_dwPlayerState |= kLanding;
				else
				if (!m_bJumping)
					m_dwPlayerState |= kStanding;
			}
		}

	}
	

	//m_dwMoveRotFlags = 0;



		/******/


		/// if CollisionDetection is disabled, just get to the desired destination
		if (!g_bTestCollisionDetection)
		{
			m_vPos = vDesiredPosition;
			if (!bMove)
				vPosBackup = m_vPos;
			return;
		}



		/////////////////////////////////////////////////

	

		

	
		CPhysicsModelMoveTraceInput.dwTraceType = TRACE_TYPE_SPHERE;
		CPhysicsModelMoveTraceInput.fRadius = m_fRadius;
		CPhysicsModelMoveTraceInput.dwTraceCausedBy = TRACE_CAUSED_BY_PLAYER;
		CPhysicsModelMoveTraceInput.vRayStart.Set(m_vPos);
		CPhysicsModelMoveTraceInput.vRayEnd.Set(vDesiredPosition);


		bOverComeBumps=true;

		if (m_bPlayerInAir)
			bOverComeBumps=false;
		
		if (m_fTimePlayerInAir>0.2f)
			bOverComeBumps=false;

		if (vMoveVelocity==CVector3f(0.f,0.f,0.f))
		{
			bOverComeBumps=false;

			// still need to know ON what i'm standing

			static CTraceRequest StandingOnWhatInput;
			static CTraceResult  StandingOnWhatOutput;

			StandingOnWhatInput.dwTraceType = TRACE_TYPE_SPHERE;
			StandingOnWhatInput.fRadius = m_fRadius;
			StandingOnWhatInput.dwTraceCausedBy = TRACE_CAUSED_BY_PLAYER;
			StandingOnWhatInput.vRayStart.Set(m_vPos);
			StandingOnWhatInput.vRayEnd.Set(m_vPos - CVector3f(0.f,5.f,0.f));
			g_pQuake3Map->Trace(&StandingOnWhatInput,&StandingOnWhatOutput,false,true, pCausedByEntity); // trying the step up version

		}
	}
	
	// try a step up
	static CTraceRequest StepUpInput;
	static CTraceResult StepUpOutput;


	if (!m_bClimbingStair && bOverComeBumps)
	{
        // MOVE UP
		StepUpInput.dwTraceType = TRACE_TYPE_SPHERE;
		StepUpInput.fRadius = m_fRadius;
		StepUpInput.dwTraceCausedBy = TRACE_CAUSED_BY_PLAYER;
		StepUpInput.vRayStart = m_vPos;
		StepUpInput.vRayEnd = m_vPos + CVector3f(0.f,25.f,0.f);
		g_pQuake3Map->Trace(&StepUpInput,&StepUpOutput,true,true,pCausedByEntity); // trying the step up version
				
		StepUpInput.vRayStart = StepUpOutput.EndPoint;
		StepUpInput.vRayEnd   =StepUpOutput.EndPoint + (vDesiredPosition - m_vPos);
		g_pQuake3Map->TraceAndResponse(&StepUpInput,&StepUpOutput,0,pCausedByEntity); // move the ofsseted move
				
		StepUpInput.vRayStart = StepUpOutput.EndPoint;
		StepUpInput.vRayEnd = StepUpOutput.EndPoint - CVector3f(0.f,25.f,0.f);

		g_pQuake3Map->Trace(&StepUpInput,&StepUpOutput,true,true,pCausedByEntity); // move back down


	}

    
	// remember to move the overcomebump field from traceandrespone()
	bool bSuccessOnFirstMove = g_pQuake3Map->TraceAndResponse(&CPhysicsModelMoveTraceInput,&CPhysicsModelMoveTraceOutput,0,pCausedByEntity);

	if ( GetChangePositionNow()) // if need to change position now (teleporter) don't try the move
	{
		ChangePositionNow(false);
		if (!bMove)
			vPosBackup = m_vPos;
		return;			
	}

	float fStepUpMoveDistance = DISTANCE(m_vPos,StepUpOutput.EndPoint);
	float fNoStepUpMoveDistance = DISTANCE(m_vPos,CPhysicsModelMoveTraceOutput.EndPoint);



	if (!m_bClimbingStair)
	if ( bOverComeBumps &&  
		(fStepUpMoveDistance > fNoStepUpMoveDistance+0.1f))
	{	
		m_bClimbingStair = true;
		m_fClimbingPhase=0.f;


		//m_fClimbingStepNum = st_Timer::Inst()->GetDeltaTimeFAKE() * 0.21f;
		//m_fClimbingStepNum = st_Timer::Inst()->GetDeltaTimeFAKE() * 0.42f;

		//m_fClimbingStepNum = st_Timer::Inst()->GetDeltaTimeFAKE() * 0.42f;
		
		/*m_fClimbingStepNum = 85.71 / st_Timer::Inst()->GetDeltaTimeFAKE();

		if (m_fClimbingStepNum<1.f)
			m_fClimbingStepNum = 1.f;

		if (m_fClimbingStepNum>12.f)
			m_fClimbingStepNum = 12.f;


		m_fClimbingStepNum = floorf(m_fClimbingStepNum);*/



		/**/

		// MOVE UP
		// perhaps i should try lines
		StepUpInput.dwTraceType = TRACE_TYPE_SPHERE;
		StepUpInput.dwTraceCausedBy = TRACE_CAUSED_BY_PLAYER;
		//StepUpInput.dwTraceType = TRACE_TYPE_LINE;
		StepUpInput.fRadius = m_fRadius;

		StepUpInput.vRayStart = m_vPos;
		StepUpInput.vRayEnd = m_vPos + CVector3f(0.f,25.f,0.f);
		//g_pQuake3Map->Trace(&StepUpInput,&StepUpOutput,true); // trying the step up version
		g_pQuake3Map->TraceAndResponse(&StepUpInput,&StepUpOutput,0,pCausedByEntity); // trying the step up version
				



		//StepUpInput.dwTraceType = TRACE_TYPE_SPHERE;
		StepUpInput.vRayStart = StepUpOutput.EndPoint;
		//StepUpInput.vRayEnd   =StepUpOutput.EndPoint + (vDesiredPosition - m_vPos);
		StepUpInput.vRayEnd   = StepUpOutput.EndPoint + (vDesiredPosition - m_vPos) +
									(vDesiredPosition - m_vPos).Normalize()*12.f;

		g_pQuake3Map->TraceAndResponse(&StepUpInput,&StepUpOutput,0,pCausedByEntity); // move the ofsseted move
				

		//StepUpInput.dwTraceType = TRACE_TYPE_LINE;
		StepUpInput.vRayStart = StepUpOutput.EndPoint;
		StepUpInput.vRayEnd = StepUpOutput.EndPoint - CVector3f(0.f,25.f,0.f);

		//g_pQuake3Map->Trace(&StepUpInput,&StepUpOutput,true); // move back down
		g_pQuake3Map->TraceAndResponse(&StepUpInput,&StepUpOutput,0,pCausedByEntity); // move back down


		//
		/*g_iShootTestFlags = StepUpOutput.HitFlags;
		g_iShootTestContents = StepUpOutput.HitContents;*/
		//


		// now move till next stair

		CVector3f vDesiredFalttenedMove = (vDesiredPosition - m_vPos).Normalize();
		vDesiredFalttenedMove.v[1] = 0.f;
		StepUpInput.vRayStart = StepUpOutput.EndPoint;
		StepUpInput.vRayEnd   = StepUpOutput.EndPoint + vDesiredFalttenedMove*10.f;

		g_pQuake3Map->TraceAndResponse(&StepUpInput,&StepUpOutput,0,pCausedByEntity); // move till next step to make a continues process


		/**/

		m_vStartClimbPos = m_vPos;
		m_vClimbMove = (StepUpOutput.EndPoint - m_vPos);

	}
		
	if (m_bClimbingStair)
	{
		m_fClimbingPhase+= 1.f/m_fClimbingStepNum; 
		if (m_fClimbingPhase>1.f)
		{
			m_fClimbingPhase = 0.f;
			m_bClimbingStair = false;
			if (!bMove)
				vPosBackup = m_vPos;
			return;
			//tryPlayerMove(vMoveVelocity);
		}
		else
		{
			m_vPos = m_vStartClimbPos + (m_vClimbMove * m_fClimbingPhase);
			/*g_iShootTestFlags = CPhysicsModelMoveTraceOutput.HitFlags;
			g_iShootTestContents = CPhysicsModelMoveTraceOutput.HitContents;*/
			if (!bMove)
				vPosBackup = m_vPos;
			return;
		}

		/*m_vPos = m_vStartClimbPos + (m_vClimbMove * m_fClimbingPhase);
		if (m_fClimbingPhase>1.f)
		{
			m_fClimbingPhase = 0.f;
			m_bClimbingStair = false;
		}

		return;*/
	}





	// the final ACTUAL direction (effected by slides etc. - used by the anti-shaking code)
	CVector3f vNewDir = (CPhysicsModelMoveTraceOutput.EndPoint - m_vPos).Normalize();

	//if (vOriginalDir * vNewDir<0)
	if (vOriginalDir * vNewDir<EPSILON*3.f)
		g_bFinalDirectionIsOppositeThenDesired = true;
	else 
		g_bFinalDirectionIsOppositeThenDesired = false;


	// if not blocked by anything OR blocked, but not going back in the opposite direction (would cause a "shake")
	if (  (bSuccessOnFirstMove    ||   (vOriginalDir * vNewDir>=0)))
		m_vPos = CPhysicsModelMoveTraceOutput.EndPoint;


	if (!bMove)
		m_vPos = vPosBackup;

}

void CPhysicsModel::UpdateInterfaceWeaponPos(void)
{
	float a,b,c,d;

	CWave wave1,wave2;

	CMatrix m1,m2;
	float pfOpenglMatrix[16];

	if (m_dwPlayerState & kIdle)
		{
					/*

			a = m_vVelocity.Length() * 0.0002f;
			b = 0.004f;
			c = m_vVelocity.Length() * 0.0001f;
			d = 0.002f;
	
			g_vWeaponPos.v[0] = g_vWeaponPos.v[0] + a * sinf(st_Timer::Inst()->TimerGetTimeFAKE()* b);
			g_vWeaponPos.v[1] = g_vWeaponPos.v[1] + c * sinf(st_Timer::Inst()->TimerGetTimeFAKE()* d);

			g_pWeaponModel->Update(true,false,CVector3f(13.f + g_vWeaponPos.v[0],-15.f + g_vWeaponPos.v[1],-30.f),CVector3f(0.f,100.f,0.f),CVector3f(0.f,0.f,0.f),CVector3f(0.f,0.f,0.f));*/

			wave1.SetParams(WF_SIN,0.8,1.2,0,0.2);
			wave2.SetParams(WF_SIN,0.f,0.25f,0.f,0.25);

			g_vWeaponPos.v[0] = wave1.Compute(st_Timer::Inst()->TimerGetTimeFAKE());
			g_vWeaponPos.v[1] = wave2.Compute(st_Timer::Inst()->TimerGetTimeFAKE());

			m1.MatMakeMove(13.f + g_vWeaponPos.v[0],-15.f + g_vWeaponPos.v[1],-30.f);

			//m2.MatMakeRot(0.f,100.f,0.f);
			m2.MatMakeRot(DEG2RAD(-90.f),DEG2RAD(90.f),0.f);

			// restore restore
			m1 = m1 * m2;

			m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);

			g_pWeaponModel->Update(true,true,&pfOpenglMatrix[0]);
	
			

		}
	else

		if (m_dwPlayerState & kRunning)
		{

			
			a = 0.0002f;
			b = 0.004f;
			
			

			//wave1.SetParams(wavetype,base,amplitude,phase,freq)

			wave1.SetParams(WF_SIN,0.8,10.2,0,1.2);
			wave2.SetParams(WF_SIN,0.f,4.f,0.f,1.25);

			float yaw = wave1.Compute(st_Timer::Inst()->TimerGetTimeFAKE());
			float pitch = wave2.Compute(st_Timer::Inst()->TimerGetTimeFAKE());

			m1.MatMakeMove(13.f + g_vWeaponPos.v[0],-15.f + g_vWeaponPos.v[1],-30.f);
			m2.MatMakeRot(DEG2RAD(-90.f+yaw),DEG2RAD(90.f+pitch),0.f);


			// restore restore
			m1 = m1 * m2;

			m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);
	
			g_pWeaponModel->Update(true,true,&pfOpenglMatrix[0]);

		}
}


CVector3f CPhysicsModel::GetDesiredPosition(int iPlayerMode)
 {
	 CVector3f vDesiredPosition = m_vPos;

	     //CVector3f TempCameraSpeed;
	 float fTempCameraSpeed;

	 CMatrix m_mCameraRotate,m_mCameraMove,m_mCamera;

	m_mCameraRotate.MatMakeUnit();
	m_mCameraMove.MatMakeUnit();


	//if (RotateYLeft)
	if (m_dwMoveRotFlags & PLAYER_ROT_Y_LEFT)
	{
		m_fAngY+=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed / 15.f;
	}

	//if (RotateYRight)
	if (m_dwMoveRotFlags & PLAYER_ROT_Y_RIGHT)
	{
		m_fAngY-=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed  / 15.f;
	}

	if (m_fAngY<0.f) m_fAngY +=360.f;
	if (m_fAngY>360.f) m_fAngY -=360.f;


	//if (RotateXDown)
	if (m_dwMoveRotFlags & PLAYER_ROT_X_DOWN)
	{
		if (m_fAngX>-89.f) m_fAngX-=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateXSpeed /15.f;		
	}

	//if (RotateXUp)
	if (m_dwMoveRotFlags & PLAYER_ROT_X_UP)
	{
		if (m_fAngX<89.f) m_fAngX+=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed /15.f;
	}



	/*if (m_dwMoveRotFlags & PLAYER_ROT_Z_LEFT)
	{
		zang+=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed / 15.f;
	}

	if (m_dwMoveRotFlags & PLAYER_ROT_Z_RIGHT)
	{
		zang-=st_Timer::Inst()->GetDeltaTimeFAKE() * RotateYSpeed  / 15.f;
	}*/
	
	
	TempPoint.v[0]=TempPoint.v[1]=TempPoint.v[2]=0.f;

	
	fTempCameraSpeed = 1.f;
	


	//fCameraSpeed*=st_Timer::Inst()->GetDeltaTimeFAKE();

	
	if (m_dwMoveRotFlags & PLAYER_MOVE_Z_FORWARD)
	{
		TempPoint.v[2]-=1.f;
	}

	
	if (m_dwMoveRotFlags & PLAYER_MOVE_Z_BACKWARDS)
	{
		TempPoint.v[2]+=1.f;
	}

	//if (y>0.3f) 
	if (m_dwMoveRotFlags & PLAYER_MOVE_Y_DOWN)
	{
		TempPoint.v[1]-=1.f;
	}
	//if (y<3.9f)
	if (m_dwMoveRotFlags & PLAYER_MOVE_Y_UP)
	{
		TempPoint.v[1]+=1.f;
	}


	if (m_dwMoveRotFlags & PLAYER_MOVE_X_LEFT)
	{
		TempPoint.v[0]-=1.f;
	}

	if (m_dwMoveRotFlags & PLAYER_MOVE_X_RIGHT)
	{
		TempPoint.v[0]+=1.f;
	}
	

	// make m_mCameraMove matrix contain all translation needed
	m_mCameraMove.MatMakeMove(TempPoint.v[0],TempPoint.v[1],TempPoint.v[2]);

	
	// make m_mCameraRotate matrix will contain all rotation needed
	if (iPlayerMode == PLAYER_MODE_FLY)
	{
		m_mCameraRotate.MatMakeRot(DEG2RAD(m_fAngX),DEG2RAD(m_fAngY),DEG2RAD(0));
	}
	else 
	if (iPlayerMode == PLAYER_MODE_WALK)// a "walk"
	{
		m_mCameraRotate.MatMakeRot(DEG2RAD(0),DEG2RAD(m_fAngY),DEG2RAD(0));
	}
	else // unknown
	{
		return m_vPos;
	}

	
	// TODO:
	// an optimization would be to create a method inside CMatrix that does:
	// 1. CMatrix::MatMakeRot() --> this needs to create the matrix in ONE step - without creating
	//                               small (mX*mY*mZ)
	// 2. CMatrix::MatMakeMoveAndRotate(x,y,z,m_fAngX,m_fAngY,zAng);

	m_mCamera = m_mCameraRotate*m_mCameraMove; // m_mCamera has entire WORLD -> CAMERA matrix now

	TempPoint=m_mCamera*TempPoint;


	// actually change the position of the camera

	///m_Pos+=TempPoint;
	vDesiredPosition+=TempPoint;


	// Basic Restrictions

	if (m_fAngX < -70.f)
		m_fAngX = -70.f;
	if (m_fAngX > 70.f)
		m_fAngX = 70.f;

	
	m_vLastDesiredPosition = vDesiredPosition;
	return vDesiredPosition;
 }