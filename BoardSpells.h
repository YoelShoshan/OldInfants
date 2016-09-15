#ifndef YOEL_BOARD_SPELLS_H
#define YOEL_BOARD_SPELLS_H

#pragma once


#define SPELL_TYPE_SWITCH_PLACES_OWN   0 //
#define SPELL_TYPE_SWITCH_PLACES_ENEMY 1 //
#define SPELL_TYPE_FIREBALL            2 //
#define SPELL_TYPE_SHOCKWAVE           3 //
#define SPELL_TYPE_CHARM               4 //
#define SPELL_TYPE_PARALYSE            5 //
#define SPELL_TYPE_HEAL                6 //
#define SPELL_TYPE_ENERGY_FIELD        7
#define SPELL_TYPE_TELEKINESIS         8
#define SPELL_TYPE_VORTEX              9
#define SPELL_TYPE_LASER               10
#define SPELL_TYPE_ARMAGEDDON          11
#define SPELL_TYPE_MAX                 12


#define SPELL_EFFECTS_SELF_UNIT                  1
#define SPELL_EFFECTS_SELF_ONE_REST_OF_OWN_ARMY  2
#define SPELL_EFFECTS_SELF_TWO_REST_OF_OWN_ARMY  3
#define SPELL_EFFECTS_ENEMY_UNIT                 4
#define SPELL_EFFECTS_ENEMY_TWO                  5
#define SPELL_EFFECTS_EMPTY_SQUARE               6
#define SPELL_EFFECTS_AUTO                       7


#define SPELL_WAY_DOESNT_MATTER         0
#define SPELL_WAY_DIRECT_LINE_OF_SIGHT  1



class CBoardSpell
{	
public:
	CBoardSpell() { m_iSpell=0;m_iEffects=SPELL_EFFECTS_SELF_UNIT;m_iRange=1000;};

	int m_iSpell;
	int m_iEffects;
	int m_iRange;
	int m_iWay;
};






#endif //YOEL_BOARD_SPELLS_H