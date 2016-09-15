#include <Windows.h>
#include "Entities.h"


/*#include "PhysicsModel.h"
#include "PlayerClient.h"*/
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

#include "Engine.h"

#include "WeaponsSounds.h"
extern CWeaponsSound g_WeaponsSounds;

#include "Communication.h"
extern CCommunication g_Communication;

#include "TextMessages.h"
extern CTextMessages g_TextMessages;

/*void CEntity::HitSurface(DWORD dwHitContents, DWORD dwHitFlags)
{
};*/

void CEntity::DrawBoundingBox(void)
{
/*	if (!m_pBrush)
		return;

	CPlane* planes = m_pBrush->m_pPlanes;*/

	st_Renderer::Inst()->DebugDrawBBox(m_vPos+m_BBox.min,m_vPos+m_BBox.max);

/*	CVector3f v1,v2,v3,v4,v5,v6,v7,v8;

	v1.Set(planes[0].d,planes[3].d,planes[4].d);
	v2.Set(planes[1].d,planes[2].d,planes[5].d);


	st_Renderer::Inst()->DebugDrawBBox(v1,v2);*/

}


void CEntity::LinkToLeafs(void)
{	
	/*if (  (inLeafs.size() == 0) && (inLeafsTEMP.size() == 0))
		return;*/

	int inLeafsSize = inLeafs.size();
	int inLeafsTEMPSize = inLeafsTEMP.size();

	if ( (inLeafsSize == 0) && (inLeafsTEMPSize == 0))
		return;



	bool bSame = true;

	if (inLeafsSize ==inLeafsTEMPSize)
	{
		for (int i=0;i<inLeafs.size();i++)
		{
	
			if (inLeafs[i]!=inLeafsTEMP[i])
			{
				bSame = false;
			break;
			}
		}
	}
	else
		bSame=false;

	if (bSame)
		return;



	tBSPLeaf* leaf;

	ENTITIES_VECTOR_I iterator;
	

//	BREAKPOINT(inLeafs.size() != inLeafsTEMP.size());

	// everything that is in the CURRENT but not in the PREVIOUS must be added

    for (int i=0;i<inLeafs.size();i++)
	{
		bool bFind;
		bFind = false; // didn't find
		

		leaf = & g_pQuake3Map->m_pLeafs[inLeafs[i]];


		if (IsLeafIndexInVectorPrevious(inLeafs[i])) //  if it's also in the previous
			continue; // then we don't need to add it


		// it's NOT on the previous, so we need to add

		for (int j=0;j<leaf->DynamicEntities.size();j++) // only add it if it's not already there
		{
			if (leaf->DynamicEntities[j] == this)
				bFind = true;			
		}
		
		if (!bFind)
			g_pQuake3Map->m_pLeafs[inLeafs[i]].DynamicEntities.push_back(this);		
	}


	// everything that is in the PREVIOUS but not in the CURRENT must be erased


	for (int i=0;i<inLeafsTEMP.size();i++)
	{
		leaf = & g_pQuake3Map->m_pLeafs[inLeafsTEMP[i]];


		bool bFind;
		bFind=false;

		if (IsLeafIndexInVectorCurrent(inLeafsTEMP[i])) //  if it's also in the current
			continue; // then we don't need to add it


		// it's the dynamic entities list is empty we don't need to continue

		if (leaf->DynamicEntities.empty())
			continue;

		iterator = leaf->DynamicEntities.begin();

		while (iterator != leaf->DynamicEntities.end())
		{
			if ( (*iterator) == this)
			{
				bFind=true;
				break;
			}

			iterator++;
		}


		// debug

		int iDebug1=g_pQuake3Map->m_pLeafs[inLeafsTEMP[i]].DynamicEntities.size();
		
		//if (iterator != leaf->DynamicEntities.end())
		if (bFind)
			g_pQuake3Map->m_pLeafs[inLeafsTEMP[i]].DynamicEntities.erase(iterator);

		int iDebug2=g_pQuake3Map->m_pLeafs[inLeafsTEMP[i]].DynamicEntities.size();
		
		int blah;
		blah=4;

	}

}



bool CEntity::IsLeafIndexInVectorCurrent(int iFindLeaf)
{
	for (int i=0;i<inLeafs.size();i++)
	{
		if (inLeafs[i]==iFindLeaf)
			return true;
	}

	return false; 
}

bool CEntity::IsLeafIndexInVectorPrevious(int iFindLeaf)
{
	for (int i=0;i<inLeafsTEMP.size();i++)
	{
		if (inLeafsTEMP[i]==iFindLeaf)
			return true;
	}

	return false; 
}


void CEntityJumpPad::OnTouched(void)
{	
	/*
	//CVector3f vNewGravity = g_vGravity * 0.0001f;
	CVector3f vNewGravity = g_vGravity;

	//SetVelocity
	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->SetVelocity(CVector3f(0.f,0.f,0.f));

    //	Vy = sqrt(2.g.(y_target - y0))
    //	Vx = (x_target - x0).g / Vy
    //	Vz = (z_target - z0).g / Vy
	//  thanks sBibi ;)

	CVector3f vJumpPadVelocity(0.f,0.f,0.f);

	
	// original
	//vJumpPadVelocity.v[1] = sqrtf(2* ((-vNewGravity.v[1])*1.f)*(vTarget.v[1] - g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetPosition().v[1]));
	//vJumpPadVelocity.v[0] = (vTarget.v[0] - g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetPosition().v[0]) * ((-vNewGravity.v[1])*1.f) / vJumpPadVelocity.v[1];
	//vJumpPadVelocity.v[2] = (vTarget.v[2] - g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetPosition().v[2]) * ((-vNewGravity.v[1])*1.f)/ vJumpPadVelocity.v[1];
	


	// perhaps i need to try using the model BBox center instead
	vJumpPadVelocity.v[1] = sqrtf(2* ((-vNewGravity.v[1])*1.f)*(vTarget.v[1] - vModelBBoxCenter.v[1]));
	vJumpPadVelocity.v[0] = (vTarget.v[0] - vModelBBoxCenter.v[0]) * ((-vNewGravity.v[1])*1.f) / vJumpPadVelocity.v[1];
	vJumpPadVelocity.v[2] = (vTarget.v[2] - vModelBBoxCenter.v[2]) * ((-vNewGravity.v[1])*1.f)/ vJumpPadVelocity.v[1];
	


	*/

	g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_JUMPPAD,m_vPos);

	if (vTarget==CVector3f(0.f,0.f,0.f))
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->SetVelocity(CVector3f(0.f,0.f,0.f));
	else
	{
		g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->SetVelocity(vTarget);

		if (g_pCurrentlyProcessedBot)
		{
			if (!g_pCurrentlyProcessedBot->m_bJumpingWithJumpPad)
			{
				g_pCurrentlyProcessedBot->m_bJumpingWithJumpPad = true;
				g_pCurrentlyProcessedBot->m_fTimeJumpingWithJumppad = 0.f;
			}
		}
	}

	//BREAKPOINT(1);
}


void CEntityTeleporter::OnTouched(void)
{	
	g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_TELEPORTER,m_vPos);

	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->SetPosition(vTarget + CVector3f(0.f,g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->getRadius(),0.f));

	g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->ChangePositionNow(true);
}

void CEntityHurt::OnTouched(void)
{	  
 	//g_pCurrentlyProcessedPlayerClient->m_iLife -= iDamage;
	g_pCurrentlyProcessedPlayerClient->m_iLife += iDamage;

	if (g_pCurrentlyProcessedPlayerClient->m_iLife<0)
		g_pCurrentlyProcessedPlayerClient->m_iLife = 0;

}


void CEntityCollectable::Update(void)
{


	// make the object rotate around themselves
	/**/
		CMatrix m1,m2;
		m1.MatMakeRot(DEG2RAD(-90.f),DEG2RAD(90.f + st_Timer::Inst()->TimerGetTimeFAKE()/4.f),0.f);				
		m2.MatMakeMove(m_vPos.v[0],m_vPos.v[1],m_vPos.v[2]);
		m1 = m2 * m1;
		float pfOpenglMatrix[16];
		m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);
		m_pModel->Update(true,false,&pfOpenglMatrix[0]);
	/**/
       

	if (!m_bActive)
	{
		m_fTimeSinceCollected+=st_Timer::Inst()->GetDeltaTimeFAKE()/1000.f;
		if (m_fTimeSinceCollected>40.f)
		{
			m_bActive=true;
			m_fTimeSinceCollected=0.f;
		}
	}
}

void CEntityCollectable::OnTouched(void)
{	

	if (!m_bActive) // this collectable isn't active right now
		return;




	CVector3f vPlayerMin,vPlayerMax;

	vPlayerMin = g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetPosition() - CVector3f(40.f,40.f,40.f);
	vPlayerMax = g_pCurrentlyProcessedPlayerClient->GetPhysicsModel()->GetPosition() + CVector3f(40.f,40.f,40.f);

	bool bPlayerIntesect=false;

	CVector3f vModelMin,vModelMax;

	vModelMin.Set(m_pModel->m_pMD3Shared->m_pBones[0].mins[0],m_pModel->m_pMD3Shared->m_pBones[0].mins[2],-m_pModel->m_pMD3Shared->m_pBones[0].mins[1]);
	vModelMax.Set(m_pModel->m_pMD3Shared->m_pBones[0].maxs[0],m_pModel->m_pMD3Shared->m_pBones[0].maxs[2],-m_pModel->m_pMD3Shared->m_pBones[0].maxs[1]);


	vModelMin+= m_vPos;
	vModelMax+= m_vPos;

	if (BBoxesIntersect(vPlayerMin,vPlayerMax,vModelMin,vModelMax))
		bPlayerIntesect=true;

	/*m_bActive = false;*/

	if (bPlayerIntesect)
	{		
		

		switch(m_dwItemType)
		{
		
		case ENTITY_COLLECTABLE_ARMOR_BODY:		
		m_bActive = false;
		m_fTimeSinceCollected = 0.f;
		// add body armor to player
		g_pCurrentlyProcessedPlayerClient->m_bRedArmor = true;
		g_pCurrentlyProcessedPlayerClient->m_fTimeHoldingRedArmor = 0.f;
		g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_PICK_UP,m_vPos);
		break;

		case ENTITY_COLLECTABLE_COMBAT_ARMOR:		
		m_bActive = false;
		m_fTimeSinceCollected = 0.f;
		// add combat armor to player
		g_pCurrentlyProcessedPlayerClient->m_bYellowArmor = true;
		g_pCurrentlyProcessedPlayerClient->m_fTimeHoldingYellowArmor = 0.f;
		g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_PICK_UP,m_vPos);
		break;

		case ENTITY_COLLECTABLE_AMMO_BULLETS:		
			m_bActive = false;
			m_fTimeSinceCollected = 0.f;
			g_pCurrentlyProcessedPlayerClient->m_iAmmoBullets+=50;
			g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_PICK_UP,m_vPos);
		break;

		case ENTITY_COLLECTABLE_AMMO_ROCKETS:		
			m_bActive = false;
			m_fTimeSinceCollected = 0.f;
			g_pCurrentlyProcessedPlayerClient->m_iAmmoRockets+=20;
			g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_PICK_UP,m_vPos);
		break;
	
		case ENTITY_COLLECTABLE_AMMO_SHELLS:
			m_bActive = false;
			m_fTimeSinceCollected = 0.f;
			g_pCurrentlyProcessedPlayerClient->m_iAmmoShells+=50;
			g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_PICK_UP,m_vPos);
		break;		
		
		case ENTITY_COLLECTABLE_HEALTH:
			if (g_pCurrentlyProcessedPlayerClient->m_iLife<100)
			{
				m_bActive = false;
				m_fTimeSinceCollected = 0.f;
				g_pCurrentlyProcessedPlayerClient->m_iLife+=25;
				if (g_pCurrentlyProcessedPlayerClient->m_iLife>100)
					g_pCurrentlyProcessedPlayerClient->m_iLife=100;
				g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_PICK_UP,m_vPos);
			}
		break;		

		case ENTITY_COLLECTABLE_ARMOR_SHARD:
			if (g_pCurrentlyProcessedPlayerClient->m_iMana<200)
			{
				m_bActive = false;
				m_fTimeSinceCollected = 0.f;
				g_pCurrentlyProcessedPlayerClient->m_iMana+=20;
				if (g_pCurrentlyProcessedPlayerClient->m_iMana>200)
					g_pCurrentlyProcessedPlayerClient->m_iMana=200;
				g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_PICK_UP,m_vPos);
			}
		break;		
		}
	}
}

void CEntityCollectable::Draw(bool bPPLused)
{
	if (m_bActive)
	if (m_uiDrawnFrame!=st_Renderer::Inst()->m_uiFrameCounter) // RESTORE RESTORE RESTORE
	{
		m_uiDrawnFrame=st_Renderer::Inst()->m_uiFrameCounter;
		m_pModel->PushToRenderer(false);
	}
}


// entity player


CEntityPlayer::CEntityPlayer()
{
	m_bNetworked=false;

	m_bActive = true;

	m_dwDynamicEntityType=DYNAMIC_ENTITY_TYPE_PLAYER;

	iModelNum = -1;

	m_pPlayerClient = new CPlayerClient;

	m_vPos.Set(0.f,0.f,0.f);

	/*m_BBox.min.Set(-15.f,-25.f,-15.f);
	m_BBox.max.Set(15.f,35.f,15.f);*/

	m_BBox.min.Set(-15.f,-35.f,-15.f);
	m_BBox.max.Set(15.f,35.f,15.f);


	m_pBrush = new tBSPBrush;


	m_pBrush->numOfBrushSides = 6;
	m_pBrush->textureID = -2;

	m_pBrush->m_pPlanes = new CPlane[6];	
	MakeBrushFromBoundingBox(m_BBox.min+m_vPos,m_BBox.max+m_vPos,m_pBrush->m_pPlanes);

	m_pBot = new CBot;
	

};


void CEntityPlayer::Draw(bool bPPLused)
{
	if (m_uiDrawnFrame!=st_Renderer::Inst()->m_uiFrameCounter) // RESTORE RESTORE RESTORE
	{
		m_uiDrawnFrame=st_Renderer::Inst()->m_uiFrameCounter;	
		m_pPlayerClient->m_pPlayerModel->DrawPlayer(bPPLused);
		g_iNumberOfDynamicEntitiesDrawn++;
	}
}

void CEntityPlayer::UpdateModel(void)   // update the model position according to the update physics
{
	m_pPlayerClient->m_pPlayerModel->m_vWorldPosition = m_pPlayerClient->GetPhysicsModel()->GetPosition();

	m_vPos = m_pPlayerClient->m_pPlayerModel->m_vWorldPosition;

	MakeBrushFromBoundingBox(m_BBox.min+m_vPos,m_BBox.max+m_vPos,m_pBrush->m_pPlanes);

	m_pPlayerClient->Update();


	
	//todo: move this
	m_pPlayerClient->UpdateCharacterAnimation();
}

void CEntityPlayer::UpdateLinking(void)
{
	inLeafsTEMP.clear(); //now inLeafsTEMP hold a list of what was BEFORE this frame

	for (int i=0;i<inLeafs.size();i++)
	{
		inLeafsTEMP.push_back(inLeafs[i]);
	}

	//BREAKPOINT(inLeafs.size()>100);

	inLeafs.clear(); // and inLeafs hold a list of what is in THIS frame

	// here here here - 
	g_pQuake3Map->FindLeafs(inLeafs,m_pPlayerClient->GetPhysicsModel()->GetPosition(),m_pPlayerClient->GetPhysicsModel()->getRadius(),0);
}

// this actually updates both physics and model... (calls UpdateModel() ) ...
void CEntityPlayer::UpdatePhysics(void)
{	
	//
	CPlayerClient* pPlayerClientTEMP = g_pCurrentlyProcessedPlayerClient;
	CBot         * pBotTEMP = g_pCurrentlyProcessedBot;
	//

	g_pCurrentlyProcessedPlayerClient = m_pPlayerClient;
	g_pCurrentlyProcessedBot          = m_pBot;
	

	m_pPlayerClient->GetPhysicsModel()->m_iContentsOfTouchedGround = 0;
	m_pPlayerClient->GetPhysicsModel()->m_iFlagsOfTouchedGround = 0;

	m_pBrush->m_iAbsoluteEntityIndex = m_iAbsoluteEntitiesIndex;


	// a hack for q3dm17

	if (m_pPlayerClient->GetPhysicsModel()->GetPosition().v[1]<-700.f)
		//m_pPlayerClient->Die();
		m_pPlayerClient->m_iLife = 0;



	/*if (g_pQuake3Map->m_bFightOver && m_pPlayerClient->m_iLife>=0)
		m_pPlayerClient->GetPhysicsModel()->tryPlayerMove(CVector3f(0.f,0.f,0.f),this,!m_bNetworked);
	else
		m_pPlayerClient->GetPhysicsModel()->tryPlayerMove(m_pPlayerClient->m_vDesiredMove,this,!m_bNetworked);*/

	m_pPlayerClient->GetPhysicsModel()->tryPlayerMove(m_pPlayerClient->m_vDesiredMove,this,!m_bNetworked);
			
	m_pPlayerClient->GetPhysicsModel()->updateDistanceToFloor(this);




	/**/

	UpdateModel();

	

	if (st_Engine::Inst()->m_iGameState==0) // we moved back to board game
		return;


	UpdateLinking();

	
	

	/**/

	//
	g_pCurrentlyProcessedPlayerClient = pPlayerClientTEMP;
	g_pCurrentlyProcessedBot          = pBotTEMP;
	//

}

void CEntityPlayer::Update(void)
{
	m_pPlayerClient->GetPhysicsModel()->SetPosition(m_vPos);	
	m_pPlayerClient->m_pPlayerModel->m_vWorldPosition = m_pPlayerClient->GetPhysicsModel()->GetPosition();
	m_vPos = m_pPlayerClient->m_pPlayerModel->m_vWorldPosition;




	//todo: move this
	m_pPlayerClient->UpdateCharacterAnimation();
}

void CEntityPlayer::ForcePosition(CVector3f vPos)
{
	m_vPos = vPos;
	m_pPlayerClient->m_pPlayerModel->m_vWorldPosition = m_vPos;
	m_pPlayerClient->GetPhysicsModel()->SetPosition(m_vPos);	
	m_pPlayerClient->m_pPlayerModel->m_vWorldPosition = m_vPos;
	
}

void CEntityPlayer::GetInputFromNetwork(void)
{
	m_bNetworked=true;

	//
	CPlayerClient* pPlayerClientTEMP = g_pCurrentlyProcessedPlayerClient;	
	//

	g_pCurrentlyProcessedPlayerClient = m_pPlayerClient;
	
	if	(st_TimerI->GetTimeSinceLastGameLogicHappened() >= GAME_LOGIC_MILISECONDS_INTERVAL)
	{
		/*
		if (!pPlayerClientTEMP->m_bDying && !pPlayerClientTEMP->m_bFinishedDying && !pPlayerClientTEMP->m_bStartDie)
			m_pBot->FindNextMove(vTarget);
			*/

		m_pPlayerClient->SetDesiredMove(CVector3f(0.f,0.f,0.f));


		static bool bAtLeastOne=false; // original wasn't static

		m_pPlayerClient->fOldTime      = TIMER_GET_TIME;
		m_pPlayerClient->vOldPosition = m_pPlayerClient->GetPhysicsModel()->GetPosition();
		m_pPlayerClient->fOldAngX     = m_pPlayerClient->GetPhysicsModel()->m_fAngX;
		m_pPlayerClient->fOldAngY     = m_pPlayerClient->GetPhysicsModel()->m_fAngY;

		while (g_Communication.RecieveFPSBattleMessage())
			{
				if (g_Communication.m_FPSBattleData.dwPlayerActions & FPS_BATTLE_MESSAGE_SHOOT)
				{
					m_pPlayerClient->Shoot(true,false);
					continue;
				}

				bAtLeastOne=true;
				//m_pPlayerClient->vOldPosition = m_pPlayerClient->GetPhysicsModel()->GetPosition();
							
				CVector3f vNewPos = g_Communication.m_FPSBattleData.vPlayerPosition;
				//CVector3f vDifference = vNewPos - m_pPlayerClient->GetPhysicsModel()->GetPosition();

				//ForcePosition(vNewPos);

				m_pPlayerClient->GetPhysicsModel()->m_fAngX = g_Communication.m_FPSBattleData.fPlayerAngX;
				m_pPlayerClient->GetPhysicsModel()->m_fAngY = g_Communication.m_FPSBattleData.fPlayerAngY;
	
				m_pPlayerClient->m_pPlayerModel->m_fAngX = m_pPlayerClient->GetPhysicsModel()->m_fAngX;
				m_pPlayerClient->m_pPlayerModel->m_fAngY = m_pPlayerClient->GetPhysicsModel()->m_fAngY;

				m_pPlayerClient->GetPhysicsModel()->m_iContentsOfTouchedGround = 0;
				m_pPlayerClient->GetPhysicsModel()->m_iFlagsOfTouchedGround = 0;

				// backup info

				m_pPlayerClient->vLastRecievedPosition = vNewPos;
				m_pPlayerClient->vLastRecievedVelocity = g_Communication.m_FPSBattleData.vVelocity;
				m_pPlayerClient->vLastRecievedAcceleration = g_Communication.m_FPSBattleData.vAcceleration;
				//m_pPlayerClient->vLastRecievedDesiredPosition = g_Communication.m_FPSBattleData.vDesiredPosition;
				m_pPlayerClient->fLastRecievedAngX = m_pPlayerClient->m_pPlayerModel->m_fAngX;
				m_pPlayerClient->fLastRecievedAngY = m_pPlayerClient->m_pPlayerModel->m_fAngY;
				m_pPlayerClient->fLastRecievedTime = TIMER_GET_TIME;

				
				
			}


		if (!bAtLeastOne) // this means that we didn't get any message in this game logic frame
		{			
			m_pPlayerClient->vLastRecievedPosition = g_Communication.m_FPSBattleData.vPlayerPosition;
		}

	}
	
	m_pPlayerClient->GetPhysicsModel()->m_iContentsOfTouchedGround = 0;
	m_pPlayerClient->GetPhysicsModel()->m_iFlagsOfTouchedGround = 0;

	if (g_pQuake3Map->m_bRecievedFirstUDPMessage)
	{
		float fT = (TIMER_GET_TIME-m_pPlayerClient->fLastRecievedTime)*(1/GAME_LOGIC_MILISECONDS_INTERVAL);

		// original
		/*if (fT>1.f)
			fT=1.f;*/

		/*if (fT>3.5f)
			fT=3.5f;*/

		if (fT>1.f) // it means we passed the liner interpolation part and now we need to really estimate
		{
			 
			m_pPlayerClient->GetPhysicsModel()->tryPlayerMove(CVector3f(0.f,0.f,0.f),this,true);
			//m_pPlayerClient->GetPhysicsModel()->tryPlayerMove(m_pPlayerClient->vLastRecievedDesiredPosition,this,true);
			
			m_pPlayerClient->GetPhysicsModel()->updateDistanceToFloor(this);
		}
		else // a simple linear interpolation
		{
			// interpolate position
			CVector3f vInterpolatedPosition = m_pPlayerClient->vOldPosition +
				(((m_pPlayerClient->vLastRecievedPosition-m_pPlayerClient->vOldPosition) * fT)  );
			ForcePosition(vInterpolatedPosition);
			// interpolate rotation
			//m_pPlayerClient->m_pPlayerModel->m_fAngX = m_pPlayerClient->fLastRecievedAngX;
			//m_pPlayerClient->m_pPlayerModel->m_fAngY = m_pPlayerClient->fLastRecievedAngY;
			m_pPlayerClient->m_pPlayerModel->m_fAngX = m_pPlayerClient->fOldAngX +
				(( m_pPlayerClient->fLastRecievedAngX - m_pPlayerClient->fOldAngX) * fT);
			m_pPlayerClient->m_pPlayerModel->m_fAngY = m_pPlayerClient->fOldAngY +
				(( m_pPlayerClient->fLastRecievedAngY - m_pPlayerClient->fOldAngY) * fT);

			m_pPlayerClient->GetPhysicsModel()->SetVelocity(m_pPlayerClient->vLastRecievedVelocity);
			m_pPlayerClient->GetPhysicsModel()->SetAcceleration(m_pPlayerClient->vLastRecievedAcceleration);
		}

		
		
		

		m_pPlayerClient->GetPhysicsModel()->m_dwPlayerState = g_Communication.m_FPSBattleData.dwPlayerState;

		
	}


		// debug debug
		//ForcePosition(m_pPlayerClient->vLastRecievedPosition);
	//}

	//
	g_pCurrentlyProcessedPlayerClient = pPlayerClientTEMP;	
	//
}

void CEntityPlayer::FakeAI(CVector3f& vTarget)
{
	//
	CPlayerClient* pPlayerClientTEMP = g_pCurrentlyProcessedPlayerClient;
	CBot         * pBotTEMP = g_pCurrentlyProcessedBot;
	//

	g_pCurrentlyProcessedPlayerClient = m_pPlayerClient;
	g_pCurrentlyProcessedBot          = m_pBot;


	m_pBot->SetBotPlayerClient(m_pPlayerClient); // should only be done once, but i'm doing it all the time



	if	(st_TimerI->GetTimeSinceLastGameLogicHappened() >= GAME_LOGIC_MILISECONDS_INTERVAL)
	{
		if (!pPlayerClientTEMP->m_bDying && !pPlayerClientTEMP->m_bFinishedDying && !pPlayerClientTEMP->m_bStartDie)
			m_pBot->FindNextMove(vTarget);
	}


	//
	g_pCurrentlyProcessedPlayerClient = pPlayerClientTEMP;
	g_pCurrentlyProcessedBot          = pBotTEMP;
	//


}



CEntityProjectile::CEntityProjectile()
{

	m_bLeaveNoMark = false;

	m_vNormalAtCollision.Set(0.f,0.f,-1.f);

	m_dwProjectileType = 0;

	m_vTryMoveDirection.Set(0.f,0.f,0.f);

	m_bActive = true;

	m_dwDynamicEntityType=DYNAMIC_ENTITY_TYPE_PROJECTILE;


	//m_fExplosionCountdown = ROCKET_EXPLOSION_TIME;
	m_fExplosionCountdown = PLASMA_EXPLOSION_TIME;

	m_fMarkCountdown = MARK_STAY_TIME;

	// MD3 models loading
    
	iModelNum = -1;

	
	m_pModel = NULL;

	m_pExplosion = NULL;

	m_pMark = NULL;



	m_pPhysicsModel = new CPhysicsModel(10.f);

	m_vPos.Set(0.f,0.f,0.f);

	/*m_BBox.min.Set(-15.f,-25.f,-15.f);
	m_BBox.max.Set(15.f,35.f,15.f);*/

	m_BBox.min.Set(-10.f,-15.f,-10.f);
	m_BBox.max.Set(10.f,15.f,10.f);


	m_pBrush = new tBSPBrush;

	m_pBrush->numOfBrushSides = 6;
	m_pBrush->textureID = -2;
	m_pBrush->m_iAbsoluteEntityIndex = -1;

	m_pBrush->m_pPlanes = new CPlane[6];	
	MakeBrushFromBoundingBox(m_BBox.min+m_vPos,m_BBox.max+m_vPos,m_pBrush->m_pPlanes);

	//m_pBot = new CBot;
	

};


void CEntityProjectile::SetMD3s(void)
{
	// i need to check in each one if it isn't ALREADY

	SafeDelete(m_pModel);
	SafeDelete(m_pExplosion);
	SafeDelete(m_pMark);

	if (m_dwProjectileType==PROJECTILE_TYPE_ROCKET)
	{
		// the rocket model
		m_pModel = new CMD3;
		m_pModel->Load("models/ammo/rocket/rocket.md3",NULL,NULL,false);

		// the explosion model
		m_pExplosion = new CMD3;
		// original
		m_pExplosion->Load("models/weaphits/boom01.md3",NULL,"rocketExplosion",false);
		//m_pExplosion->Load("models/weaphits/boom01.md3",NULL,"smokePuff");
		//m_pExplosion->Load("models/weaphits/boom01.md3",NULL,"viewBloodBlend");
		//m_pExplosion->Load("models/weaphits/boom01.md3",NULL,"waterBubble");
		//m_pExplosion->Load("models/weaphits/boom01.md3",NULL,"powerups/battleSuit");
		//m_pExplosion->Load("models/weaphits/boom01.md3",NULL,"powerups/invisibility");
		//m_pExplosion->Load("models/weaphits/boom01.md3",NULL,"teleportEffect");
		
		
		
		
		

		// the mark model
		m_pMark = new CMD3;
		m_pMark->Load("models/weaphits/boom01.md3",NULL,"Quake3Art/hole_lg_mrk",false);

	}
	else
	if (m_dwProjectileType==PROJECTILE_TYPE_PLASMA)
	{
		// the rocket model
		m_pModel = new CMD3;
		m_pModel->Load("models/weapons2/plasma/plasma_flash.md3",NULL,NULL,false);

		// the explosion model
		m_pExplosion = new CMD3;
		m_pExplosion->Load("models/weaphits/boom01.md3",NULL,"plasmaExplosion",false);	
		
		// the mark model
		m_pMark = new CMD3;
		m_pMark->Load("models/weaphits/boom01.md3",NULL,"gfx/damage/plasma_mrk",false);
		
	}
	else
	if (m_dwProjectileType==PROJECTILE_TYPE_BULLET)
	{
		
		// the rocket model
		m_pModel = NULL;

		// the explosion model
		m_pExplosion = new CMD3;
		m_pExplosion->Load("models/weaphits/bullet.md3",NULL,"bulletExplosion",false);					

		// the mark model
		m_pMark = new CMD3;
		//m_pMark->Load("models/weaphits/boom01.md3",NULL,"gfx/damage/bullet_mrk");
		m_pMark->Load("models/weaphits/boom01.md3",NULL,"gfx/damage/plasma_mrk",false);
	}


	// force allocation of the world matrices in models

	/*static float pfOpenglMatrix[16];
	static CMatrix m1;

	m1.MatMakeUnit();

	m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);

	if (m_pModel)
		m_pModel->Update(true,false,&pfOpenglMatrix[0]);

	if (m_pExplosion)
		m_pExplosion->Update(true,false,&pfOpenglMatrix[0]);*/


}

void CEntityProjectile::SetTryMove(CVector3f& vSet)
{
	m_vTryMoveDirection = vSet;

}

void CEntityProjectile::Draw(bool bPPLused)
{

	static CTraceRequest g_RayTraceInput;
	static CTraceResult  g_RayTraceOutput;

	if (m_bActive)
	{
		if (m_uiDrawnFrame!=st_Renderer::Inst()->m_uiFrameCounter) // RESTORE RESTORE RESTORE
		{
			m_uiDrawnFrame=st_Renderer::Inst()->m_uiFrameCounter;
			m_pModel->PushToRenderer(false);
			g_iNumberOfDynamicEntitiesDrawn++;
		}	
	}
	else
	{
		if ( (m_fExplosionCountdown>0.f))
		{
			if (m_uiDrawnFrame!=st_Renderer::Inst()->m_uiFrameCounter) // RESTORE RESTORE RESTORE
			{
				m_uiDrawnFrame=st_Renderer::Inst()->m_uiFrameCounter;
		

				g_RayTraceInput.dwTraceType = TRACE_TYPE_SPHERE;
				g_RayTraceInput.dwTraceCausedBy = TRACE_CAUSED_BY_TEST;
				g_RayTraceInput.fRadius = 7.f;
		
				g_RayTraceInput.vRayStart.Set(g_Camera1.m_Pos);
				g_RayTraceInput.vRayEnd.Set(m_vPos);

				g_RayTraceInput.iAbsoluteEntityIndexCause = -1;
		
				// RESTORE RESTORE RESTORE
				//g_pQuake3Map->Trace(&g_RayTraceInput,&g_RayTraceOutput,false,false,NULL);

				// DEBUG DEBUG DEBUG
				g_pQuake3Map->Trace(&g_RayTraceInput,&g_RayTraceOutput,false,true,NULL);
				

				/**/

				if (g_RayTraceOutput.Fraction==1.f)
					m_pExplosion->SetAlwaysDraw(TRUE);					
				else
					m_pExplosion->SetAlwaysDraw(FALSE);					



				// RESTORE RESTORE RESTORE
				m_pExplosion->PushToRenderer(false);

			
				g_iNumberOfDynamicEntitiesDrawn++;



				// i canceled all marks...
				/*if (!m_bLeaveNoMark)
				if (m_dwProjectileType==PROJECTILE_TYPE_ROCKET) // only rocket mark looks ok
				if (m_fMarkCountdown>0.f)
				{
					m_pMark->PushToRenderer(false);

					m_pMark->SetAlwaysDraw(FALSE);					
			
					g_iNumberOfDynamicEntitiesDrawn++;
				}*/


			}				
		}
		else
		{	
			// i canceled all marks...
			/*if (!m_bLeaveNoMark)
			if (m_dwProjectileType==PROJECTILE_TYPE_ROCKET) // only rocket mark looks ok
			if (m_fMarkCountdown>0.f)
			if (m_uiDrawnFrame!=st_Renderer::Inst()->m_uiFrameCounter) // RESTORE RESTORE RESTORE
			{
				m_uiDrawnFrame=st_Renderer::Inst()->m_uiFrameCounter;
				m_pMark->PushToRenderer(false);

				m_pMark->SetAlwaysDraw(FALSE);					
			
				g_iNumberOfDynamicEntitiesDrawn++;
			}*/

		}
		
		
		
	}
}



void CEntityProjectile::UpdatePhysics(void)
{	

	static CMatrix m1;
	static CMatrix m2;
	static CMatrix m3;
	static float pfOpenglMatrix[16];
	
	if (m_bActive)
	{

		CVector3f vSaveTryMoveDirection = m_vTryMoveDirection;

		CVector3f vTempNormal(0.f,0.f,0.f);

		vTempNormal = m_pPhysicsModel->tryProjectileMove(m_vTryMoveDirection,this);

		if (!(vTempNormal==CVector3f(0.f,0.f,0.f)))
			m_vNormalAtCollision = vTempNormal;
	
		m_vTryMoveDirection = vSaveTryMoveDirection;
	
		m_vPos = m_pPhysicsModel->GetPosition();


		// update rocket
	
		float fProjectileAngX,fProjectileAngY;

		GetLookAtAngles(CVector3f(0.f,0.f,0.f),m_vTryMoveDirection,fProjectileAngX,fProjectileAngY);

		

		// hmm - some issue with x angle



		m1.MatMakeRot(DEG2RAD(0.f),DEG2RAD(fProjectileAngY+90.f),DEG2RAD(fProjectileAngX) );	
		m2.MatMakeRot(DEG2RAD(st_Timer::Inst()->TimerGetTimeFAKE()*0.2f),DEG2RAD(0.f),DEG2RAD(0.f) );	

		m1 = m1 * m2;
			
		m3.MatMakeMove(m_vPos.v[0],m_vPos.v[1],m_vPos.v[2]);
	

		// RESTORE RESTORE RESTORE
		m1 = m3 * m1;

		
		m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);
		m_pModel->Update(true,false,&pfOpenglMatrix[0]);
	}
	else
	{

		if (m_fExplosionCountdown>=0.f)
		{
			m_fExplosionCountdown -= st_Timer::Inst()->GetDeltaTimeFAKE();
			if (m_fExplosionCountdown<=0.f)
			{
				m_fExplosionCountdown=-0.1f;
				m_fMarkCountdown = MARK_STAY_TIME;
			}

				m_fMarkCountdown = MARK_STAY_TIME;
				float fExpAngX,fExpAngY;

				GetLookAtAngles(CVector3f(0.f,0.f,0.f),m_vNormalAtCollision,fExpAngX,fExpAngY); 

				CMatrix m1,m2;

				m1.MatMakeRot(DEG2RAD(0.f),DEG2RAD(fExpAngY+90.f),DEG2RAD(fExpAngX) );	
				m2.MatMakeMove(m_vPos.v[0],m_vPos.v[1],m_vPos.v[2]);	
				m1 = m2 * m1;
				

				m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);
	
				m_pMark->Update(true,false,&pfOpenglMatrix[0]);
		}

		if (m_fMarkCountdown>0.f)
		{
			m_fMarkCountdown -= st_Timer::Inst()->GetDeltaTimeFAKE();;

			/*
			if (m_fMarkCountdown<1000.f)
			{
				float fTemp = m_fMarkCountdown/4.f;

				byte bTemp = (byte) (floorf(fTemp));
				m_pMark->SetSubMeshesColorArrays(bTemp,bTemp,bTemp,bTemp);			
				//m_pMark->SetSubMeshesColorArrays(0,0,0,0);			
			}
			*/
		}

			
			
	}
	

	if (m_dwProjectileType==PROJECTILE_TYPE_ROCKET)
	{
		m_BBox.min.Set(-10.f,-15.f,-10.f);
		m_BBox.max.Set(10.f,15.f,10.f);

		m_pPhysicsModel->setRadius(10.f);
	}
	else
	if (m_dwProjectileType==PROJECTILE_TYPE_PLASMA)
	{
		m_BBox.min.Set(-5.f,-7.5f,-5.f);
		m_BBox.max.Set(5.f,7.5f,5.f);

		m_pPhysicsModel->setRadius(5.f);
	}
	else
	if (m_dwProjectileType==PROJECTILE_TYPE_BULLET)// ????
	{
		m_BBox.min.Set(-5.f,-7.5f,-5.f);
		m_BBox.max.Set(5.f,7.5f,5.f);

		m_pPhysicsModel->setRadius(5.f);
	}
	

	MakeBrushFromBoundingBox(m_BBox.min+m_vPos,m_BBox.max+m_vPos,m_pBrush->m_pPlanes);

	/**/

	// i should REALLY only do this if m_vPos has changed from last frame...

	inLeafsTEMP.clear(); //now inLeafsTEMP hold a list of what was BEFORE this frame

	for (int i=0;i<inLeafs.size();i++)
	{
		inLeafsTEMP.push_back(inLeafs[i]);
	}
	
	inLeafs.clear(); // and inLeafs hold a list of what is in THIS frame
	g_pQuake3Map->FindLeafs(inLeafs,m_pPhysicsModel->GetPosition(),m_pPhysicsModel->getRadius(),0);

	/**/
}




void CEntityProjectile::OnTouch(void)
{

	// sound

	if (m_dwProjectileType==PROJECTILE_TYPE_ROCKET)
		g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_ROCKET_HIT,m_vPos);
	else
	if (m_dwProjectileType==PROJECTILE_TYPE_PLASMA)
		g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_PLASMA_HIT,m_vPos);

	/*if (m_dwProjectileType==PROJECTILE_TYPE_ROCKET)
		g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_ROCKET_HIT,m_vPos);
	else
	if (m_dwProjectileType==PROJECTILE_TYPE_PLASMA)
		g_WeaponsSounds.PlaySound3D(WEAPON_SOUND_PLASMA_HIT,m_vPos);*/



	// activate damage + physics on nearby players
	// for now only consider radius (IGNORE the fact that some wall might be in the way)

	float fDistanceFromPlayer;

	for (int i=0;i<g_pQuake3Map->m_iPlayersNum;i++)
	{
		fDistanceFromPlayer = DISTANCE(g_pQuake3Map->m_ppAllDynamicEntities[i]->m_vPos,m_vPos);

		if (m_dwProjectileType==PROJECTILE_TYPE_ROCKET)
		{

            
			if (fDistanceFromPlayer<ROCKET_EXPLOSION_RADIUS) // if inside the damage radius
			{
				// cause life damage to players in proportion to their distance
				g_pQuake3Map->m_ppAllDynamicEntities[i]->m_pPlayerClient->m_iLife-=(1 - fDistanceFromPlayer/ROCKET_EXPLOSION_RADIUS) * 40*g_pQuake3Map->m_ppAllDynamicEntities[i]->m_pPlayerClient->m_fReduceDamageFactor;
				g_pQuake3Map->m_ppAllDynamicEntities[i]->m_pPlayerClient->PlayPainSound((1 - fDistanceFromPlayer/ROCKET_EXPLOSION_RADIUS) * 40*g_pQuake3Map->m_ppAllDynamicEntities[i]->m_pPlayerClient->m_fReduceDamageFactor);

				// cause a physics reaction to damage
				CVector3f vExplosionPushPlayer = (g_pQuake3Map->m_ppAllDynamicEntities[i]->m_vPos - m_vPos).Normalize();

				// try to raise y
				// perhaps do "if y>0"

				if (fDistanceFromPlayer<60.f)
				{
					vExplosionPushPlayer.v[0]*=2.f;
					vExplosionPushPlayer.v[1]*=6.f;
					vExplosionPushPlayer.v[2]*=2.f;
				}
				else
					vExplosionPushPlayer.v[1]*=4.f;
			

				g_pQuake3Map->m_ppAllDynamicEntities[i]->m_pPlayerClient->GetPhysicsModel()->m_vOutsideForce+= vExplosionPushPlayer * 160.f * (1 - fDistanceFromPlayer/ROCKET_EXPLOSION_RADIUS);
			}
		}
		else
		{
			// some other projetile that causes damage to surrounding
		}

	}
	

	// de-activate entity, and start explosion

	static CMatrix m1,m2,m3;
	static float pfOpenglMatrix[16];
		
	m_bActive = false;

	

	// update explosion - should only happen once


	if ((m_dwProjectileType==PROJECTILE_TYPE_PLASMA) ||
		(m_dwProjectileType==PROJECTILE_TYPE_BULLET)	)
	{
		float fExpAngX,fExpAngY;

		GetLookAtAngles(CVector3f(0.f,0.f,0.f),m_vNormalAtCollision,fExpAngX,fExpAngY); 

		m1.MatMakeRot(DEG2RAD(0.f),DEG2RAD(fExpAngY+90.f),DEG2RAD(fExpAngX) );	
		m2.MatMakeMove(m_vPos.v[0],m_vPos.v[1],m_vPos.v[2]);	
		m1 = m2 * m1;
	}
	else
	if (m_dwProjectileType==PROJECTILE_TYPE_ROCKET)
	{
		// since autosprite will handle it, no rotation is needed
		m1.MatMakeMove(m_vPos.v[0],m_vPos.v[1],m_vPos.v[2]);	
	}


	m1.ConvertToOpenglMatrix(&pfOpenglMatrix[0]);
	
	m_pExplosion->Update(true,false,&pfOpenglMatrix[0]);
	m_pMark->Update(true,false,&pfOpenglMatrix[0]);
	

	if (!m_pExplosion->m_pMeshes[0].m_VB.GetSpecialCounter())
		m_pExplosion->SetSpecialCounter();
}