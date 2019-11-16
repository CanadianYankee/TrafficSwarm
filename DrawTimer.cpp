#include "pch.h"
#include "DrawTimer.h"

CDrawTimer::CDrawTimer(void) : 
	m_SecondsPerCount(0.0), 
	m_DeltaTime(-1.0), 
	m_BaseTime(0), 
	m_PausedTime(0), 
	m_PrevTime(0), 
	m_CurrTime(0), 
	m_bStopped(false)
{
	__int64 frequency;
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	m_SecondsPerCount = 1.0 / (double)frequency;

	Reset();
}


CDrawTimer::~CDrawTimer(void)
{
}

void CDrawTimer::Reset(void)
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = 0;
	m_bStopped  = false;
}

void CDrawTimer::Start()
{
	if( m_bStopped )
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		m_PausedTime += (startTime - m_StopTime);	

		m_PrevTime = startTime;
		m_StopTime = 0;
		m_bStopped  = false;
	}
}

void CDrawTimer::Stop()
{
	if( !m_bStopped )
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_StopTime = currTime;
		m_bStopped  = true;
	}
}

void CDrawTimer::Tick()
{
	if( m_bStopped )
	{
		m_DeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_CurrTime = currTime;

	// Time difference between this frame and the previous.
	m_DeltaTime = (m_CurrTime - m_PrevTime)*m_SecondsPerCount;

	// Prepare for next frame.
	m_PrevTime = m_CurrTime;

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if(m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}

// Returns the total time elapsed since Reset() was called, NOT counting any
// time when the clock is stopped.
float CDrawTimer::TotalTime() const
{
	return (float)((((m_bStopped ? m_StopTime : m_CurrTime) - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
}

float CDrawTimer::DeltaTime() const
{
	return (float)m_DeltaTime;
}
