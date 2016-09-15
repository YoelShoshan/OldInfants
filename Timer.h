#ifndef YOEL_TIMER_H
#define YOEL_TIMER_H

#pragma once



#define st_Timer CSingleton<CTimer>
#define st_TimerI CSingleton<CTimer>::Inst()


#define TIMER_DELTA_TIME_FAKE st_TimerI->GetDeltaTimeFAKE()
#define TIMER_DELTA_TIME      st_TimerI->GetDeltaTime()

#define TIMER_GET_TIME_FAKE   st_TimerI->TimerGetTimeFAKE()
#define TIMER_GET_TIME        st_TimerI->TimerGetTimeF()


//#define GAME_LOGIC_MILISECONDS_INTERVAL 100.f
//#define GAME_LOGIC_MILISECONDS_INTERVAL 100.f

//#define GAME_LOGIC_MILISECONDS_INTERVAL 33.f
#define GAME_LOGIC_MILISECONDS_INTERVAL 19.f

#define GAME_LOGIC_MILISECONDS_INTERVAL_UDP_SEND 19.f


//#define BIGGEST_TIME_INTERVAL 300.f
#define BIGGEST_TIME_INTERVAL 50.f

class CTimer			 											// Create A Structure For The Timer Information
{
public:
	void TimerInit(void);
    float TimerGetTimeF();
	float TimerGetTimeSecondsF();

	float TimerGetTimeFAKE();
	float TimerGetTimeSecondsFAKE();

	void  SetFakeSlowMotion(float fSet) {m_fFakeSlowMotion=fSet;};
	float GetFakeSlowMotion(void) {return m_fFakeSlowMotion;};

	float GetDeltaTime(void) {return m_fDeltaTime;};
	float GetDeltaTimeFAKE(void) {if (m_fDeltaTimeFAKE<BIGGEST_TIME_INTERVAL) return m_fDeltaTimeFAKE; else return BIGGEST_TIME_INTERVAL;};

	void ZeroTimeSinceLastGameLogicHappend(void);

	float GetTimeSinceLastGameLogicHappened(void) { return m_fTimeSinceLastGameLogicHappened;};
	void  SetTimeSinceLastGameLogicHappened(float fSet) { m_fTimeSinceLastGameLogicHappened = fSet;};

	void ZeroTimeSinceLastGameLogicHappend_UDP_SEND(void);

	float GetTimeSinceLastGameLogicHappened_UDP_SEND(void) { return m_fTimeSinceLastGameLogicHappened_UDP_SEND;};
	void  SetTimeSinceLastGameLogicHappened_UDP_SEND(float fSet) { m_fTimeSinceLastGameLogicHappened_UDP_SEND = fSet;};


	void Update(void);
	

	float m_fTimerStart, m_fFPS, m_fDeltaTime, m_fNow;
	float m_fTimerStartFAKE,m_fDeltaTimeFAKE,m_fNowFAKE;
	float m_fTimeForFrame;
	float m_fTotalTimeFAKE;

	

	// seperating game logic from rendering

	float m_fTimeSinceLastGameLogicHappened;

	float m_fTimeSinceLastGameLogicHappened_UDP_SEND;

private:
	__int64       frequency;									// Timer Frequency
	float         resolution;									// Timer Resolution
	unsigned long mm_timer_start;								// Multimedia Timer Start Value
	unsigned long mm_timer_elapsed;							// Multimedia Timer Elapsed Time
	bool			performance_timer;							// Using The Performance Timer?
	__int64       performance_timer_start;					// Performance Timer Start Value
	__int64       performance_timer_elapsed;					// Performance Timer Elapsed Time

	float m_fFakeSlowMotion;      // Result = realtime / fFakeSlowMotion

};													// Structure Is Named timer



#endif // YOEL_TIMER_H


