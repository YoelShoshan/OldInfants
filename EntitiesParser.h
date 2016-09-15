#ifndef YOEL_ENTITIES_PARSER_H
#define YOEL_ENTITIES_PARSER_H

#pragma once

#include "Singleton.h"
#include "Math3D.h"


// This is our lumps enumeration
enum eEntitiesTypes
{
	kNotSupported = 0,          // something i don't support
    kInfoPlayerDeatchMatch,		// "info_player_deathmatch"
    kTriggerPush,				// "trigger_push"
    kTargetPosition,			// "target_position"
	kTriggerTeleport,           // "trigger_teleport"
	kTriggerHurt,				// "trigger_hurt"
	kAmmoBullets,				// "ammo_bullets"
	kAmmoShells,				// "ammo_shells"
	kAmmoRockets,               // "ammo_rockets"
	kArmorBody,                 // "item_armor_body"   
	kCombatArmor,               // "item_armor_combat"
	kHealth,					// "item_health"
	kArmorShard					// "item_armor_shard"
};


// a single simplified Entity - it's your job to Fill the angles and expand the Entities info ;)

typedef struct sGeneralEntity
{
	unsigned int uiEntityType;
	CVector3f vOrigin;
	int      iAngle;
	int      iModelIndex;
	int      iDamage;
	char     caTargetName[5]; // if this is a target, then it's name, if it's a trigger push then the target we're aiming at
};

#define st_EntitiesParser CSingleton<CEntitiesParser>

#define MAX_ENTITIES 4000

class CEntitiesParser {

public:

	// Our CEntitiesParser Constructer
	CEntitiesParser();	

	
	void SetEntitiesString(char * InputString);
	
	// We dump the entities into a file in order to examine how to write a better parser in the future ;)
	void ExportEntitiesToFile(void);

	// Here we load the Entities info
	void Load(void);

	// Puts Entity Origin number [iNum] into Vector
	void GetPlayerPosition(int iNum,CVector3f &Vector);


	int FillPlayerPositions(CVector3f*& pStartPositions);


	sGeneralEntity m_pEntities[MAX_ENTITIES];
	int            m_iEntitiesNum;



	int m_iStartPositions;

private:
	// Our beloved Entities String !
	char * m_cpEntities;
    		
};

#endif // YOEL_ENTITIES_PARSER_H
