#pragma once
#include "Interface/ICutsceneEvent.h"
#include "Math/Vector2.h"

namespace Wannabe 
{ 
    class Actor; 
}

class MoveEvent : public Wannabe::ICutsceneEvent
{
public:
    MoveEvent(Wannabe::Actor* pActor, const Wannabe::Vector2& targetPos, float fDuration);

    virtual void OnStart(Wannabe::BattleContext& context) override;
    virtual bool Update(Wannabe::BattleContext& context, float fDeltaTime) override;
    virtual bool IsFinished() const override { return m_fElapsed >= m_fDuration; }
    virtual CutsceneEventType GetEventType() override
    {
        m_eCutsceneEventType = CutsceneEventType::Move;
        return m_eCutsceneEventType;
    };

private:
    Wannabe::Actor* m_pActor = nullptr;
    Wannabe::Vector2 m_startPos;
    Wannabe::Vector2 m_targetPos;

    float m_fDuration = 0.f;
    float m_fElapsed = 0.f;
};