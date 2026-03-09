#include "MoveEvent.h"
#include "Actor/Actor.h"

MoveEvent::MoveEvent(Wannabe::Actor* pActor, const Wannabe::Vector2& targetPos, float fDuration)
    : m_pActor(pActor), m_targetPos(targetPos), m_fDuration(fDuration)
{
}

void MoveEvent::OnStart(Wannabe::BattleContext& context)
{
    if (m_pActor)
        m_startPos = m_pActor->GetPosition();

    m_fElapsed = 0.f;
}

bool MoveEvent::Update(Wannabe::BattleContext& context, float fDeltaTime)
{
    if (m_pActor == nullptr) 
        return false;

    m_fElapsed += fDeltaTime;
    float ratio = m_fElapsed / m_fDuration;
    if (ratio > 1.0f) 
        ratio = 1.0f;

    // 선형 보간(Lerp) 계산
    float curX = m_startPos.x + (m_targetPos.x - m_startPos.x) * ratio;
    float curY = m_startPos.y + (m_targetPos.y - m_startPos.y) * ratio;

    m_pActor->SetPosition({ (int)curX, (int)curY });

    return m_fElapsed < m_fDuration;
}