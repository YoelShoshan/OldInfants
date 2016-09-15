#ifndef YOEL_WEAPONS_SOUND_H
#define YOEL_WEAPONS_SOUND_H

#pragma once

#include "SoundManager.h"
extern CSoundManager g_SoundManager;

#include "Math3D.h"

#define WEAPON_SOUND_ROCKET_SHOOT  0
#define WEAPON_SOUND_ROCKET_FLY    1
#define WEAPON_SOUND_ROCKET_HIT    2
#define WEAPON_SOUND_PLASMA_SHOOT  3
#define WEAPON_SOUND_PLASMA_FLY    4
#define WEAPON_SOUND_PLASMA_HIT    5
#define WEAPON_SOUND_SHOTGUN_SHOOT 6
#define WEAPON_SOUND_JUMPPAD       7
#define WEAPON_SOUND_TELEPORTER    8
#define WEAPON_SOUND_PICK_UP       9
#define WEAPON_SOUND_MAX           10


class CWeaponsSound
{
public:
	CWeaponsSound() {m_ppWeaponSoundSamples=NULL;};
	~CWeaponsSound() {for (int i=0;i<WEAPON_SOUND_MAX;i++) g_SoundManager.FreeSound(m_ppWeaponSoundSamples[i]);};
	void Init(void);

	void PlaySound(int iNum);
	void PlaySound3D(int iNum,CVector3f& vPos);

	FSOUND_SAMPLE** m_ppWeaponSoundSamples;
};


#endif //YOEL_WEAPONS_SOUND_H