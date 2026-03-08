#pragma once
#include "Interface/ICutsceneEvent.h"
#include "Enum/CombatType.h"

namespace Wannabe
{
    class BattleContext;
    class Actor;
}

class DamageEvent : public Wannabe::ICutsceneEvent
{
public:
    DamageEvent(Wannabe::Actor* pAtker, Wannabe::Actor* pTarget, int iDmg, bool bCritical = false,DamageSource eSource = DamageSource::BasicAtk);

    void OnStart(Wannabe::BattleContext& context) override;
    bool Update(Wannabe::BattleContext& context, float fDeltaTime) override;
    virtual CutsceneEventType GetEventType() override
    {
        m_eCutsceneEventType = CutsceneEventType::Damage;
        return m_eCutsceneEventType;
    };

private:
    Wannabe::Actor* m_pAtker;
    Wannabe::Actor* m_pTarget;

    bool m_bCritical = false;
    int m_iDmg = 0;
    float m_fElapsed = 0.f;
    DamageSource m_eDamageSource = DamageSource::BasicAtk;
};