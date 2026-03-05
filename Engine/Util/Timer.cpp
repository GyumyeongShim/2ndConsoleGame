#include "Timer.h"

namespace Wannabe
{
	Timer::Timer(float targerTime)
		:m_fTargetTime(targerTime)
	{
	}

	void Timer::Tick(float fDeltaTime)
	{
		m_fElapedTime += fDeltaTime;
	}

	void Timer::Reset()
	{
		m_fElapedTime = 0.0f;
	}

	bool Timer::IsTimeOut() const
	{
		return m_fElapedTime >= m_fTargetTime;
	}

	void Timer::SetTargerTime(float newTargetTime)
	{
		m_fTargetTime = newTargetTime;
	}
}
