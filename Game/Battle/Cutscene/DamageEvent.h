#pragma once
#include "Interface/ICutsceneEvent.h"

namespace Wannabe
{
    class BattleContext;
    class Actor;
}

class DamageEvent : public Wannabe::ICutsceneEvent
{
public:
    DamageEvent(Wannabe::Actor* pAtker, Wannabe::Actor* pTarget, int iDmg, bool bCritical = false);

    void OnStart(Wannabe::BattleContext& context) override;
    bool Update(Wannabe::BattleContext& context, float fDeltaTime) override;

private:
    Wannabe::Actor* m_pAtker;
    Wannabe::Actor* m_pTarget;

    bool m_bCritical = false;
    int m_iDmg = 0;
    float m_fElapsed = 0.f;
};