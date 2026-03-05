#include "DelayEvent.h"

DelayEvent::DelayEvent(float delay)
	:m_fDelay(delay)
{
}

bool DelayEvent::Update(Wannabe::BattleContext& context, float fDeltaTime)
{
    m_fElapsed += fDeltaTime;
    return m_fElapsed >= m_fDelay;
}