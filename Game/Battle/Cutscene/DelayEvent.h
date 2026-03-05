#pragma once
#include "Interface/ICutsceneEvent.h"

class DelayEvent : public Wannabe::ICutsceneEvent
{
public:
    DelayEvent(float delay);
    virtual ~DelayEvent() override = default;

    virtual bool Update(Wannabe::BattleContext& context, float fDeltaTime) override;

private:
    float m_fDelay = 0.f;
    float m_fElapsed = 0.f; //경과 시간
};