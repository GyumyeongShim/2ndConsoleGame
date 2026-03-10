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

    float effectRatio = sinf(ratio * 3.141592f);
    float jumpHeight = 5.0f;
    float curX = m_startPos.x + (m_targetPos.x - m_startPos.x) * effectRatio;
    float curY = m_startPos.y + (m_targetPos.y - m_startPos.y) * effectRatio - (effectRatio * jumpHeight);

    m_pActor->SetPosition({ (int)curX, (int)curY });

    if (m_fElapsed >= m_fDuration)
    {
        // 종료 시 확실히 시작 지점으로 되돌림
        m_pActor->SetPosition(m_startPos);
        return false;
    }

    return m_fElapsed < m_fDuration;
}