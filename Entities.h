#ifndef YOEL_ENTITIES_H
#define YOEL_ENTITIES_H

#pragma once

#include "Math3d.h"
#include "MD3.h"
#include "MD3MeshManager.h"
#include "MD3Manager.h"
#include "SmartPointer.h"

#include "PhysicsModel.h"
#include "PlayerClient.h"

//#include "Q3AMap.h"

#include <vector>

#include "Q3MapDefenitions.h"

// i need to find a solution for entities that aren't using model from the map itself

extern int g_iNumberOfDynamicEntitiesDrawn;

#include "Bot.h"

typedef std::vector<int> LEAFS_INDICES_VECTOR;
typedef std::vector<int>::iterator LEAFS_INDICES_VECTOR_I;


/*enum DynamicEntityType
{
	kNotSet,   // not set yet
	kPlayer,     // a player!
	kProjectile  // a projectile - missile / fireball/ etc.
};*/

#define DYNAMIC_ENTITY_TYPE_NOT_SET     0   // not set yet
#define DYNAMIC_ENTITY_TYPE_PLAYER      1     // a player!
#define DYNAMIC_ENTITY_TYPE_PROJECTILE  2  // a projectile - missile / fireball/ etc.




class CPhysicsModel;
class CPlayerClient;
class CBot;
// an entity abstract interface 
class CEntity
{
public:
	CEntity() {m_dwDynamicEntityType=DYNAMIC_ENTITY_TYPE_NOT_SET;m_pBot=NULL;m_pBrush=NULL;
				m_pPlayerClient = NULL;m_uiCDFrame = 0;m_fCollidedTime = 0.f; m_uiDrawnFrame = 0;
				m_pModel = NULL;m_iAbsoluteEntitiesIndex=-1;m_pPhysicsModel = NULL;};
	virtual ~CEntity() {SafeDelete(m_pModel);SafeDelete(m_pBot)};
	
	virtual void OnTouched(void) = 0;  // if this entity was touched by a dynamic entity
	virtual void OnTouch(void) = 0;  // if this entity is touching a dynamic entity or a map brush
	virtual void Draw(bool bPPLused) = 0;     // draw the entity
	virtual void Update(void) = 0;   // update values such as respawn object
	virtual void UpdatePhysics(void) = 0;
	



	//virtual void HitSurface(DWORD dwHitContents, DWORD dwHitFlags) = 0;
	
	float   m_fCollidedTime;
	unsigned int m_uiDrawnFrame;
	unsigned int m_uiCDFrame;


	void DrawBoundingBox(void);


	/// dynamic entities stuff

	// (only dynamic entities links are updated per frame)
	// todo: only update links on position change


	void LinkToLeafs(void);
	bool IsLeafIndexInVectorCurrent(int iFindLeaf);
	bool IsLeafIndexInVectorPrevious(int iFindLeaf);




	LEAFS_INDICES_VECTOR inLeafs;                // current frame leafs
	LEAFS_INDICES_VECTOR inLeafsTEMP;            // last frame leafs

	///////////////////////////////////


	CBot*          m_pBot;                // ai bot

	CPlayerClient* m_pPlayerClient;       // player client - only used by dynamic entity that represents a player (human or AI)
	CPhysicsModel *m_pPhysicsModel;       // physics model - used by all dynamic entities the are NOT a player (projectiles for example)
	int iModelNum;                 // in case of a map model, this is the index to the model in the map m_pModels array
	CMD3* m_pModel;                // a MD3 Object

	CVector3f vModelBBoxCenter;

	tBSPBrush* m_pBrush;           // a dynamic brush (used mostly for dynamic entities)
	
	BBox     m_BBox;               // bounding box

	CVector3f m_vPos;	           // position of entity

	DWORD m_dwDynamicEntityType;   // if it's a dynamic entity, this represents what kind it is (player/projectile/etc.) 

	int m_iAbsoluteEntitiesIndex;  // used mainly for players, and serves as an index to the map dynamic entities pointers array

	
	bool m_bActive; // is entity active
};


// jump pad
class CEntityJumpPad : public CEntity
{
public:
	CEntityJumpPad() {iModelNum = 0;};
	~CEntityJumpPad() {};
	void OnTouched(void);
	void OnTouch(void) {}; // since a jumppad doesn't move, it won't touch any dynamic entity

	void Draw(bool bPPLused) {};
	void Update(void) {};
	void UpdatePhysics(void) {};

	//void HitSurface(DWORD dwHitContents, DWORD dwHitFlags) {};

	CVector3f vTarget;	
};


// jump teleporter
class CEntityTeleporter : public CEntity
{
public:
	CEntityTeleporter() {iModelNum = 0;};
	~CEntityTeleporter() {};
	void OnTouched(void);
	void OnTouch(void) {}; // since a teleporter doesn't move, it won't touch any dynamic entity

	void Draw(bool bPPLused) {};
	void Update(void) {};
	void UpdatePhysics(void) {};

	
	CVector3f vTarget;
};


// trigger hurt
class CEntityHurt : public CEntity
{
public:
	CEntityHurt() {iModelNum = 0;};
	~CEntityHurt() {};
	void OnTouched(void);
	void OnTouch(void) {}; // since a hurt entity doesn't move, it won't touch any dynamic entity

	void Draw(bool bPPLused) {};
	void Update(void) {};
	void UpdatePhysics(void) {};
	

	int iDamage;
};


#define ENTITY_COLLECTABLE_AMMO_BULLETS (1<<0)
#define ENTITY_COLLECTABLE_AMMO_SHELLS  (1<<1)
#define ENTITY_COLLECTABLE_AMMO_ROCKETS (1<<2)
#define ENTITY_COLLECTABLE_ARMOR_BODY   (1<<3)
#define ENTITY_COLLECTABLE_HEALTH       (1<<4)
#define ENTITY_COLLECTABLE_ARMOR_SHARD  (1<<5)
#define ENTITY_COLLECTABLE_COMBAT_ARMOR (1<<6)

// collectable items
class CEntityCollectable : public CEntity
{
public:
	CEntityCollectable() {iModelNum = -1; m_bActive = true;m_fTimeSinceCollected=0.f;};
	~CEntityCollectable() {};
	void OnTouched(void);
	void OnTouch(void) {}; // since a collectable doesn't move, it won't touch any dynamic entity

	void Draw(bool bPPLused);
	void Update(void);
	void UpdatePhysics(void) {};
		

	CVector3f vTarget;
	DWORD m_dwItemType;

	float m_fTimeSinceCollected;

	//bool m_bActive;
};


// dynamic entities



// a player
class CEntityPlayer : public CEntity
{
public:
	
	CEntityPlayer();
	~CEntityPlayer() {SafeDeleteArray(m_pBrush->m_pPlanes);SafeDelete(m_pPlayerClient);};
	void OnTouched(void) {};
	void OnTouch(void) {}; // push other player is handled inside the q3map, no need to start passing variables here...
	void Draw(bool bPPLused);	
	void UpdatePhysics(void); // updates physics -- used only for FPS characters
	void UpdateModel(void);   // update the model position according to the update physics
	void UpdateLinking(void);



	void Update(void);        // update -- used only for board characters

	void ForcePosition(CVector3f vPos);

	void FakeAI(CVector3f& vTarget); // in case of human vs. pc
	void GetInputFromNetwork(void); // in case of human vs. network

	bool IsLeafIndexInVectorCurrent(int iFindLeaf);
	bool IsLeafIndexInVectorPrevious(int iFindLeaf);

	bool GetNetworked(void) { return m_bNetworked;};
	void SetNetworked(bool bSet) { m_bNetworked = bSet;};

	CVector3f vTarget;
	
	bool m_bNetworked;

	
	//float     fPrevPosTime;

	//DWORD m_dwItemType;

	//float m_fTimeSinceCollected;
		
	//bool m_bActive;
};



#define PROJECTILE_TYPE_NOTSET 0
#define PROJECTILE_TYPE_ROCKET 1
#define PROJECTILE_TYPE_PLASMA 2
#define PROJECTILE_TYPE_BULLET 3

#define ROCKET_EXPLOSION_TIME 1000.f
#define PLASMA_EXPLOSION_TIME 500.f
#define BULLET_EXPLOSION_TIME 500.f


//#define MARK_STAY_TIME 3000.f
// original
#define MARK_STAY_TIME 100000.f
//#define MARK_STAY_TIME 50000.f



#define ROCKET_EXPLOSION_RADIUS 200.f


// a projectile
class CEntityProjectile : public CEntity
{
public:
	
	CEntityProjectile();
	~CEntityProjectile() {SafeDelete(m_pPhysicsModel);SafeDeleteArray(m_pBrush->m_pPlanes);SafeDelete(m_pPlayerClient);};
	void OnTouched(void) {};
	void OnTouch(void); // should be called only when this entity hit a player, or a regular map brush
	void Draw(bool bPPLused);
	void Update(void) {};
	void UpdatePhysics(void);

	void SetMD3s(void);

	void SetTryMove(CVector3f& vSet);

	bool m_bLeaveNoMark;

	CVector3f m_vTryMoveDirection;

	CVector3f vTarget;
	DWORD m_dwProjectileType;

	CVector3f m_vNormalAtCollision;

	float m_fExplosionCountdown;
	float m_fMarkCountdown;


	CMD3* m_pExplosion;                // if the projectile explodes, this is what is displayed
	CMD3* m_pMark;


	float m_fTimeSinceCollected;
	
	
};


#endif //YOEL_ENTITIES_H