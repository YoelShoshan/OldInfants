#ifndef YOEL_BOT_H
#define YOEL_BOT_H

#pragma once

#include "PlayerClient.h"


enum
{
	kNoSteering = 0,      // just running towards player
	kSteeringLeftMode,    // encountered a problem, trying to rotate left all the time
	kSteeringRightMode    // encountered a problem, trying to rotate right all the time
};


class CPlayerClient;
class CBot
{
public:
	CBot();
	~CBot();


	void FindNextMove(CVector3f& vTarget);

	void InitStats(void);
	void SetBotPlayerClient(CPlayerClient* pSet) { m_pBotPlayerClient = pSet;};
	void UpdateTimes(void);
    
	bool m_bJumpingWithJumpPad;
	float m_fTimeJumpingWithJumppad;

	int m_iSteeringMode;
	float m_fTimeSinceDecidedToSteer;


	bool m_bJumpingSelf;



	// bot qualities

	float m_fAimingAcuracy;

	float m_fSteerAngX,m_fSteerAngY;

private:

	CPlayerClient* m_pBotPlayerClient;

};

#endif // YOEL_BOT_H

