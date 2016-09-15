#include "WeaponsSounds.h"

#include "SoundManager.h"
extern CSoundManager g_SoundManager;

void CWeaponsSound::Init(void)
{
	m_ppWeaponSoundSamples = new FSOUND_SAMPLE*[WEAPON_SOUND_MAX];

	for (int i=0;i<WEAPON_SOUND_MAX;i++)
		m_ppWeaponSoundSamples[i] = NULL;


	g_SoundManager.LoadSample("sound/weapons/rocket/rocklf1a.wav",m_ppWeaponSoundSamples[WEAPON_SOUND_ROCKET_SHOOT]);
	g_SoundManager.LoadSample("sound/weapons/rocket/rockfly.wav",m_ppWeaponSoundSamples[WEAPON_SOUND_ROCKET_FLY]);
	g_SoundManager.LoadSample("sound/weapons/rocket/rocklx1a.wav",m_ppWeaponSoundSamples[WEAPON_SOUND_ROCKET_HIT]);



	g_SoundManager.LoadSample("sound/weapons/plasma/hyprbf1a.wav",m_ppWeaponSoundSamples[WEAPON_SOUND_PLASMA_SHOOT]);
	g_SoundManager.LoadSample("sound/weapons/plasma/lasfly.wav",m_ppWeaponSoundSamples[WEAPON_SOUND_PLASMA_FLY]);
	g_SoundManager.LoadSample("sound/weapons/plasma/plasmx1a.wav",m_ppWeaponSoundSamples[WEAPON_SOUND_PLASMA_HIT]);

	g_SoundManager.LoadSample("sound/weapons/shotgun/sshotf1b.wav",m_ppWeaponSoundSamples[WEAPON_SOUND_SHOTGUN_SHOOT]);

	// world

	g_SoundManager.LoadSample("sound/world/jumppad.wav",m_ppWeaponSoundSamples[WEAPON_SOUND_JUMPPAD]);
	g_SoundManager.LoadSample("sound/world/telein.wav",m_ppWeaponSoundSamples[WEAPON_SOUND_TELEPORTER]);

	g_SoundManager.LoadSample("sound/items/respawn1.wav",m_ppWeaponSoundSamples[WEAPON_SOUND_PICK_UP]);

	
	

}


void CWeaponsSound::PlaySound(int iNum)
{
	g_SoundManager.PlaySample(m_ppWeaponSoundSamples[iNum],100);
}

void CWeaponsSound::PlaySound3D(int iNum,CVector3f& vPos)
{	
	g_SoundManager.PlaySample3D(m_ppWeaponSoundSamples[iNum],vPos,255);

}