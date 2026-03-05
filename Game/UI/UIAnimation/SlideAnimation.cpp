#include "SlideAnimation.h"
#include "UI/UI_TurnOrder.h"

using namespace Wannabe;

SlideAnimation::SlideAnimation(UI_TurnOrder* owner, Vector2 targetOffset, float speed)
    : m_pOwner(owner), m_targetOffset(targetOffset), m_fSpd(speed)
{
}
void SlideAnimation::Tick(float fDeltaTime)
{
	if (m_pOwner == nullptr)
		return;

    Vector2 cur = m_pOwner->GetOffset();
    Vector2 diff = m_targetOffset - cur;

    float dist = diff.Length();

    if (dist < 1.f)
    {
        m_pOwner->SetOffset(m_targetOffset);
        m_bFinished = true;
        return;
    }

    Vector2 dir = diff.Normalized();

    m_pOwner->SetOffset(cur + dir * m_fSpd * fDeltaTime);
}
