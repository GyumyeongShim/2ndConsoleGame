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

private:
    Wannabe::Actor* m_pTarget;
};
