#pragma once
#include "Actor/Actor.h"
#include "Data/ActionData.h"

class Actor;
class BattleContext;
class ProjectileActor : public Wannabe::Actor
{
public:
    ProjectileActor(Wannabe::Actor* pInstigator, Wannabe::Actor* pTarget, float fDuration);

    virtual void Tick(float fDeltaTime) override;

private:
    Wannabe::Actor* m_pInstigator = nullptr;
    Wannabe::Actor* m_pTarget = nullptr;

    float m_fDuration = 0.f;
    float m_fElapsed = 0.f;
};

