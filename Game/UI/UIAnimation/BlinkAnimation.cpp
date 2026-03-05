#include "BlinkAnimation.h"
#include "UI/UI_TargetCursor.h"

using namespace Wannabe;

BlinkAnimation::BlinkAnimation(UI_TargetCursor* owner, float blinkInterval)
	:m_pOwner(owner), m_fInterval(blinkInterval)
{
}

void BlinkAnimation::Tick(float fDeltaTime)
{
    m_fTimer += fDeltaTime;

    if (m_fTimer < m_fInterval)
        return;

    m_fTimer = 0.f;

    m_bVisible = !m_bVisible;

    if (m_pOwner == nullptr)
        return;

    m_pOwner->SetActive(m_bVisible);
}
