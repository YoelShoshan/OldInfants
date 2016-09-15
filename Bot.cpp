#include "Bot.h"


#include "Timer.h"

#include "PhysicsModel.h"
#include "PlayerClient.h"

extern CPlayerClient *g_pCurrentlyProcessedPlayerClient,*g_pPlayerClient1,*g_pPlayerClient2;

extern CVector3f g_vGravity;

#include <math.h>
#include "Renderer.h"
#include "Timer.h"

#include "q3amap.h"
extern CQuake3BSP* g_pQuake3Map;

extern CBot *g_pCurrentlyProcessedBot, *g_pCurrentlyProcessedBot1, *g_pCurrentlyProcessedBot2;

#include "Camera.h"
extern CCamera g_Camera1;  // the global camera

extern bool g_bBotDistanceToFloorTooBig;
extern bool g_bBotJumpsWithJumpPad;
extern bool g_bDirectLineOfSight;
extern int g_iBotSteeringMode;

CBot::CBot()
{
	InitStats();	
}

CBot::~CBot()
{
	
}

void CBot::FindNextMove(CVector3f& vTarget)
{

	
	UpdateTimes();

	g_bBotJumpsWithJumpPad = m_bJumpingWithJumpPad;



	

	bool bDirectLineOfSight = false;



	
	CVector3f vLookDirection;
		CMatrix m;


		float angX,angY;


	//if (!g_pCurrentlyProcessedPlayerClient->m_bDying && !g_pCurrentlyProcessedPlayerClient->m_bStartDie)
	if (!g_pCurrentlyProcessedPlayerClient->m_bDying && !g_pCurrentlyProcessedPlayerClient->m_bStartDie && !g_pCurrentlyProcessedPlayerClient->m_bFinishedDying)
	{

	
		if (m_bJumpingWithJumpPad)
		{
			if (m_fTimeJumpingWithJumppad>1.f)
			{
				if (m_pBotPlayerClient->GetPhysicsModel()->getDistanceToFloor()<3.25f)
				{
					m_bJumpingWithJumpPad =false;
				}

			}
		}



		// steer to chased player
	
		
	

		//GetLookAtAngles(m_vPos,vTarget,m_pPlayerClient->GetPhysicsModel()->m_fAngX,m_pPlayerClient->GetPhysicsModel()->m_fAngY);
		GetLookAtAngles(m_pBotPlayerClient->GetPhysicsModel()->GetPosition(),vTarget,angX,angY);

		float fDebugX = m_fSteerAngX;
		float fDebugY = m_fSteerAngY;


	
		//m_pPlayerClient->GetPhysicsModel()->m_fAngX = angX;
		//m_pPlayerClient->GetPhysicsModel()->m_fAngY = angY;

		m_fSteerAngY +=  (angX - m_fSteerAngX) * 0.01f*st_Timer::Inst()->GetDeltaTimeFAKE();
		m_fSteerAngY +=  (angY - m_fSteerAngY) * 0.01f*st_Timer::Inst()->GetDeltaTimeFAKE();

		//m_pPlayerClient->GetPhysicsModel()->m_fAngX =  angX;
		//m_pPlayerClient->GetPhysicsModel()->m_fAngY =  angY;
	


		bDirectLineOfSight = m_pBotPlayerClient->GetPhysicsModel()->lineOfSight(vTarget);

		g_bDirectLineOfSight = bDirectLineOfSight;




		// shooting related

		if (bDirectLineOfSight)
			m_pBotPlayerClient->Shoot(false,false);
		


		CVector3f vMissVector(0.f,0.f,0.f);
		float fRandMiss = RANDOM_NUM;

		//fRandMiss-=0.7f;
		
		//fRandMiss-=0.7f;

		/*if (fRandMiss<0.f)
			fRandMiss = 0.f;*/

		float fRandMissX = RANDOM_NUM;
		float fRandMissY = RANDOM_NUM;
		float fRandMissZ = RANDOM_NUM;

		/*fRandMissX-=0.4f;
		fRandMissY-=0.4f;
		fRandMissZ-=0.4f;*/

		fRandMissX+=74.4f;
		fRandMissY+=74.4f;
		fRandMissZ+=74.4f;

		if (fRandMissX<0.f)
			fRandMissX=0.f;
		if (fRandMissY<0.f)
			fRandMissY=0.f;
		if (fRandMissZ<0.f)
			fRandMissZ=0.f;

		if (fRandMiss>0.f)
		{
			if ( (fRandMissX>0.f) || (fRandMissY>0.f) || (fRandMissZ>0.f))
			{
				vMissVector.Set(fRandMissX*1.1f,fRandMissY*1.1f,fRandMissZ*1.1f);
				vMissVector.Normalize();
			}
		}

	{


		CTraceRequest g_BotRayTraceInput;
		CTraceResult g_BotRayTraceOutput;

		g_BotRayTraceInput.dwTraceType = TRACE_TYPE_LINE;
		g_BotRayTraceInput.dwTraceCausedBy = TRACE_CAUSED_BY_SHOOT;
		g_BotRayTraceInput.fRadius = 0.f;
		
		g_BotRayTraceInput.vRayStart.Set(g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->m_vWeaponCenter);

		
		g_BotRayTraceInput.vRayEnd.Set(g_pCurrentlyProcessedPlayerClient->m_pPlayerModel->m_vWeaponCenter);
		g_BotRayTraceInput.vRayEnd+= (vMissVector+g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetLookDirection()) * 3000.f;

		g_BotRayTraceInput.iAbsoluteEntityIndexCause = m_pBotPlayerClient->m_iAbsoluteEntityIndex;

		g_pQuake3Map->Trace(&g_BotRayTraceInput,&g_BotRayTraceOutput,false,true,NULL);
	}

		//BREAKPOINT(bDirectLineOfSight);
		


		 // end of shooting related

		// check if move is safe (won't lead to a fall)
		// i still need to solve the angX issue

		

		CVector3f vFinalPosition;

		//if (m_pBot->m_fTimeSinceDecidedToSteer>3.f)
		//if (m_pBot->m_fTimeSinceDecidedToSteer>5.f)
		if (m_fTimeSinceDecidedToSteer>7.f)
		{

			// get into steering mode because of collision with objects

			vLookDirection.Set(0.f,0.f,-1.f);
			m.MatMakeRot(DEG2RAD(m_fSteerAngX),DEG2RAD(m_fSteerAngY),DEG2RAD(0.f));
			vLookDirection = m*vLookDirection;			
			float fHeightOnTarget = m_pBotPlayerClient->GetPhysicsModel()->wouldMoveLeadToPit(m_pBotPlayerClient->GetPhysicsModel()->GetPosition(),m_pBotPlayerClient->GetPhysicsModel()->GetPosition() + vLookDirection * 90.f,vFinalPosition);



			if ((DISTANCE(m_pBotPlayerClient->GetPhysicsModel()->GetPosition(),vFinalPosition)<0.01f))
			{
				m_iSteeringMode = kSteeringRightMode;
			}


			/*if (fHeightOnTarget>3.25)
			{
				// get into steering mode because of a pit

				CVector3f vPosIfSteerLeft,vPosIfSteerRight;

				vLookDirection.Set(0.f,0.f,-1.f);
				m.MatMakeRot(DEG2RAD(m_fSteerAngX),DEG2RAD(m_fSteerAngY-10.f),DEG2RAD(0.f));
				vLookDirection = m*vLookDirection;			
				vPosIfSteerLeft = m_pBotPlayerClient->GetPhysicsModel()->GetPosition() + vLookDirection * 90.f;


				vLookDirection.Set(0.f,0.f,-1.f);
				m.MatMakeRot(DEG2RAD(m_fSteerAngX),DEG2RAD(m_fSteerAngY+10.f),DEG2RAD(0.f));
				vLookDirection = m*vLookDirection;			
				vPosIfSteerRight = m_pBotPlayerClient->GetPhysicsModel()->GetPosition() + vLookDirection * 90.f;

				if (DISTANCE(m_pBotPlayerClient->GetPhysicsModel()->GetPosition(),vPosIfSteerLeft) > DISTANCE(m_pBotPlayerClient->GetPhysicsModel()->GetPosition(),vPosIfSteerRight))
					m_iSteeringMode = kSteeringLeftMode;
				else
					m_iSteeringMode = kSteeringRightMode;
		
				m_fTimeSinceDecidedToSteer = 0.f;
			}*/
		}

	
		g_iBotSteeringMode = m_iSteeringMode;
		
		
		float fDistanceToFloorInTargetFarStep;
		float fDistanceToFloorInTargetCloseStep;

		
		bool bStopRotateSearch = false;

		int iCountStop=0;


		
		while (1)
		{			
				vLookDirection.Set(0.f,0.f,-1.f);
				m.MatMakeRot(DEG2RAD(m_fSteerAngX),DEG2RAD(m_fSteerAngY),DEG2RAD(0.f));
				vLookDirection = m*vLookDirection;			
				//bWouldFallToPit = 	m_pPlayerClient->GetPhysicsModel()->wouldMoveLeadToPit(m_vPos,m_vPos + vLookDirection * 60.f);

				

				fDistanceToFloorInTargetFarStep = 	m_pBotPlayerClient->GetPhysicsModel()->wouldMoveLeadToPit(m_pBotPlayerClient->GetPhysicsModel()->GetPosition(),m_pBotPlayerClient->GetPhysicsModel()->GetPosition() + vLookDirection * 90.f,vFinalPosition);
				fDistanceToFloorInTargetCloseStep = 	m_pBotPlayerClient->GetPhysicsModel()->wouldMoveLeadToPit(m_pBotPlayerClient->GetPhysicsModel()->GetPosition(),m_pBotPlayerClient->GetPhysicsModel()->GetPosition() + vLookDirection * 50.f,vFinalPosition);



				if (bDirectLineOfSight && fDistanceToFloorInTargetFarStep<400.f) // directlineofsight
				{
					m_iSteeringMode == kNoSteering;
					break;
				}

				// move is safe and we actually moved
				if (fDistanceToFloorInTargetFarStep<400.f && (DISTANCE(m_pBotPlayerClient->GetPhysicsModel()->GetPosition(),vFinalPosition)>30.f))
					break;

				
				

				iCountStop++;

					
				if (m_iSteeringMode == kSteeringRightMode)
					m_fSteerAngY-=5.f;// * st_Timer::Inst()->GetDeltaTimeFAKE()*0.01f;				
				else
				if (m_iSteeringMode == kSteeringLeftMode)
					m_fSteerAngY+=5.f;// * st_Timer::Inst()->GetDeltaTimeFAKE()*0.01f;				
				else
				{
					// what to do on no steering mode here?
				}


				if (iCountStop>2)
					break;

				// temp debug
				//break;
			}
		



			if (fDistanceToFloorInTargetFarStep<400.f) // if move would lead to ground (Safe)
			{
				g_bBotDistanceToFloorTooBig = false;
				// move safe

				
				vLookDirection.Normalize();

				// if player on ground
				if (!m_pBotPlayerClient->GetPhysicsModel()->getPlayerInAir())
					//m_pBotPlayerClient->SetDesiredMove(vLookDirection *  0.0002f * st_Timer::Inst()->GetDeltaTimeFAKE());
					//m_pBotPlayerClient->SetDesiredMove(vLookDirection *  0.0013f);

					m_pBotPlayerClient->SetDesiredMove(vLookDirection *  0.05f);
					//m_pBotPlayerClient->SetDesiredMove(vLookDirection.Normalize());
				else
				{
					if (m_bJumpingWithJumpPad)
					{
						m_pBotPlayerClient->SetDesiredMove(CVector3f(0.f,0.f,0.f));
					}
					else
					{
						//m_pBotPlayerClient->SetDesiredMove(vLookDirection * 0.03f*st_Timer::Inst()->GetDeltaTimeFAKE() * 0.005f);
						//m_pBotPlayerClient->SetDesiredMove(vLookDirection * 0.002f);
						//m_pBotPlayerClient->SetDesiredMove(vLookDirection * 0.0002f  * st_Timer::Inst()->GetDeltaTimeFAKE());
						m_pBotPlayerClient->SetDesiredMove(vLookDirection *  0.0013f);

						// original
						//m_pBotPlayerClient->SetDesiredMove(vLookDirection * -0.6f*st_Timer::Inst()->GetDeltaTimeFAKE() * 0.005f);
						//m_pBotPlayerClient->SetDesiredMove(vLookDirection * -0.3f*st_Timer::Inst()->GetDeltaTimeFAKE() * 0.005f);

						//m_pBotPlayerClient->SetDesiredMove(vLookDirection.Normalize());
						
					}

				}

			}
			else // if move would lead to falling too much
			{	
				g_bBotDistanceToFloorTooBig = true;
				// move not safe - would lead to falling
				m_pBotPlayerClient->SetDesiredMove(CVector3f(0.f,0.f,0.f));
			}
		}
		else // if dying/dead
		{
			m_pBotPlayerClient->SetDesiredMove(CVector3f(0.f,0.f,0.f));
			return;
		}
		

		///

		// temp always look at player

		//if (!g_pCurrentlyProcessedPlayerClient->m_bDying && !g_pCurrentlyProcessedPlayerClient->m_bStartDie)
		if (bDirectLineOfSight && !g_pCurrentlyProcessedPlayerClient->m_bDying && !g_pCurrentlyProcessedPlayerClient->m_bStartDie)		
		{
			GetLookAtAngles(m_pBotPlayerClient->GetPhysicsModel()->GetPosition(),vTarget,angX,angY);
			m_pBotPlayerClient->GetPhysicsModel()->m_fAngX = angX;
			m_pBotPlayerClient->GetPhysicsModel()->m_fAngY = angY;						
		}
		else
		{
			m_pBotPlayerClient->GetPhysicsModel()->m_fAngX = m_fSteerAngX;
			m_pBotPlayerClient->GetPhysicsModel()->m_fAngY = m_fSteerAngY;
		}

		m_pBotPlayerClient->m_pPlayerModel->m_fAngX = m_pBotPlayerClient->GetPhysicsModel()->m_fAngX;
		m_pBotPlayerClient->m_pPlayerModel->m_fAngY = m_pBotPlayerClient->GetPhysicsModel()->m_fAngY;

		



	//CVector3f vLookAt;

	//vLookAt.Set(0.f,0.f,-1.f);

	//CMatrix m;
	//m.MatMakeRot(m_pPlayerClient->m_pPlayerModel->m_fAngX,m_pPlayerClient->m_pPlayerModel->m_fAngY,0.f);

	//vLookAt = m * vLookAt;

	//m_pPlayerClient->SetDesiredMove(vLookAt * 0.04f*st_Timer::Inst()->GetDeltaTimeFAKE());
	



	// jump

//	if (vTarget.v[1] > m_vPos.v[1] + 5.f)
//	if (!m_pPlayerClient->GetPhysicsModel()->getPlayerInAir() || m_pPlayerClient->GetPhysicsModel()->getDistanceToFloor()<2.f)
//		{
//			m_pPlayerClient->GetPhysicsModel()->SetJumping(true);
//			m_pPlayerClient->GetPhysicsModel()->SetStartJumping(true);
//		}



	m_pBotPlayerClient->GetPhysicsModel()->m_iContentsOfTouchedGround = 0;
	m_pBotPlayerClient->GetPhysicsModel()->m_iFlagsOfTouchedGround = 0;


	// decide which animation would make more sense

	float fDifference = m_pBotPlayerClient->GetPhysicsModel()->m_fAngY - m_fSteerAngY;

	if (fDifference>-40.f && fDifference<40.f)
		m_pBotPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags = PLAYER_MOVE_Z_FORWARD;
	else
	if (fDifference>40.f && fDifference<120.f)
		m_pBotPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags = PLAYER_MOVE_X_RIGHT;
	else
	if (fDifference<-40.f && fDifference>-120.f)
		m_pBotPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags = PLAYER_MOVE_X_LEFT;
	else
		m_pBotPlayerClient->GetPhysicsModel()->m_dwMoveRotFlags = PLAYER_MOVE_Z_BACKWARDS;
		

	
}

void CBot::InitStats(void)
{
	m_bJumpingWithJumpPad = false;
	m_fTimeJumpingWithJumppad = 0.f;
	m_bJumpingSelf = false;
	m_iSteeringMode = kNoSteering;
	m_fTimeSinceDecidedToSteer = 0.f;

	m_fAimingAcuracy = 0.85f;

	m_fSteerAngX = 0.f;
	m_fSteerAngY = 0.f;
}


void CBot::UpdateTimes(void)
{
	if (m_bJumpingWithJumpPad)
		m_fTimeJumpingWithJumppad+=st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;

	m_fTimeSinceDecidedToSteer+=st_Timer::Inst()->GetDeltaTimeFAKE()*0.001f;
}
