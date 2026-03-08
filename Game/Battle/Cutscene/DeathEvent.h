#pragma once
#include "Interface/ICutsceneEvent.h"
namespace Wannabe
{
    class Actor;
    class BattleContext;
}

class DeathEvent : public Wannabe::ICutsceneEvent
{
public:
    DeathEvent(Wannabe::Actor* pTarget);

    void OnStart(Wannabe::BattleContext& context) override;
    bool Update(Wannabe::BattleContext& context, float fDeltaTime) override;
    virtual CutsceneEventType GetEventType() override
    {
        m_eCutsceneEventType = CutsceneEventType::Death;
        return m_eCutsceneEventType;
    };

private:
    Wannabe::Actor* m_pTarget;
};
