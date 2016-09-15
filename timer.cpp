#include "timer.h"
#include <windows.h>

///////////////////////////
#include "ZedTracker.h"
///////////////////////////

void CTimer::TimerInit(void)										// Initialize Our Timer (Get It Ready)
{

	m_fTimeSinceLastGameLogicHappened = GAME_LOGIC_MILISECONDS_INTERVAL + 20.f; // make it a little more then the interval needed, that way first frame will consider input/AI
	m_fTimeSinceLastGameLogicHappened_UDP_SEND = GAME_LOGIC_MILISECONDS_INTERVAL_UDP_SEND + 20.f; // make it a little more then the interval needed, that way first frame will consider input/AI

	m_fDeltaTime = 0.f;
	m_fDeltaTimeFAKE = 0.f;


	// restore 
	m_fFakeSlowMotion = 1.f;
	


	m_fTotalTimeFAKE = 0;
	//memset(&timer, 0, sizeof(Timer));						// Clear Our Timer Structure

	// Check To See If A Performance Counter Is Available
	// If One Is Available The Timer Frequency Will Be Updated
	if (!QueryPerformanceFrequency((LARGE_INTEGER *) &frequency))
	{
		// No Performace Counter Available
		performance_timer	= FALSE;					// Set Performance Timer To FALSE
		mm_timer_start	= timeGetTime();			// Use timeGetTime() To Get Current Time
		resolution		= 1.0f/1000.0f;				// Set Our Timer Resolution To .001f
		frequency			= 1000;						// Set Our Timer Frequency To 1000
		mm_timer_elapsed	= mm_timer_start;		// Set The Elapsed Time To The Current Time
	}
	else
	{
		// Performance Counter Is Available, Use It Instead Of The Multimedia Timer
		// Get The Current Time And Store It In performance_timer_start
		QueryPerformanceCounter((LARGE_INTEGER *) &performance_timer_start);
		performance_timer			= TRUE;				// Set Performance Timer To TRUE
		// Calculate The Timer Resolution Using The Timer Frequency
		resolution				= (float) (((double)1.0f)/((double)frequency));
		// Set The Elapsed Time To The Current Time
		performance_timer_elapsed	= performance_timer_start;
	}
	m_fTimerStart = TimerGetTimeFAKE();
}

float CTimer::TimerGetTimeF()										// Get Time In Milliseconds
{
	__int64 time;											// time Will Hold A 64 Bit Integer

	if (performance_timer)							// Are We Using The Performance Timer?
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &time);	// Grab The Current Performance Time
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return ( (float) ( time - performance_timer_start) * resolution)*1000.0f;
	}
	else
	{
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return( (float) ( timeGetTime() - mm_timer_start) * resolution)*1000.0f;
	}
}

float CTimer::TimerGetTimeSecondsF()
{
	__int64 time;											// time Will Hold A 64 Bit Integer

	if (performance_timer)							// Are We Using The Performance Timer?
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &time);	// Grab The Current Performance Time
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return ( (float) ( time - performance_timer_start) * resolution);
	}
	else
	{
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return( (float) ( timeGetTime() - mm_timer_start) * resolution);
	}

}

float CTimer::TimerGetTimeFAKE()
{	
	return m_fTotalTimeFAKE;  // original
}



float CTimer::TimerGetTimeSecondsFAKE()
{
	return m_fTotalTimeFAKE/1000.f; //original
}

void CTimer::ZeroTimeSinceLastGameLogicHappend(void)
{
	m_fTimeSinceLastGameLogicHappened = 0.f;
}

void CTimer::ZeroTimeSinceLastGameLogicHappend_UDP_SEND(void)
{
	m_fTimeSinceLastGameLogicHappened_UDP_SEND = 0.f;

}

void CTimer::Update(void)
{
	m_fNow =  TimerGetTimeF();
		m_fNowFAKE = TimerGetTimeFAKE();
	m_fDeltaTime = m_fNow - m_fTimerStart;
		m_fDeltaTimeFAKE = m_fNowFAKE - m_fTimerStartFAKE;


	m_fFPS = 1000.0 / m_fDeltaTime;

	m_fTimeForFrame = 1000.f / m_fFPS;

	m_fTimerStart = TimerGetTimeF();
		m_fTimerStartFAKE = TimerGetTimeFAKE();

	if (m_fFakeSlowMotion>=0.f)
		m_fDeltaTimeFAKE = m_fDeltaTime / m_fFakeSlowMotion;   // this didn't work well for camera rotation...
	else
		m_fDeltaTimeFAKE = m_fDeltaTime;

	m_fTotalTimeFAKE+=m_fDeltaTimeFAKE;


	m_fTimeSinceLastGameLogicHappened+=m_fDeltaTime;
	m_fTimeSinceLastGameLogicHappened_UDP_SEND+=m_fDeltaTime;
}
