#pragma once
#include <vector>

#include "Interface/ICutsceneEvent.h"
#include "Math/Vector2.h"

namespace Wannabe
{
    class BattleContext;
}

class AsciiParticleEvent : public Wannabe::ICutsceneEvent
{
    struct AsciiParticle
    {
        Wannabe::Vector2 Position;
        Wannabe::Vector2 Velocity;
        Wannabe::Vector2 Acceleration;

        char Symbol = '*';

        float LifeTime = 0.f;
        float MaxLifeTime = 1.5f;

        bool bAlive = true;
    };

public:
    AsciiParticleEvent(const Wannabe::Vector2& pos, int iParticleCnt, float fDuration);

    virtual void OnStart(Wannabe::BattleContext& context) override;
    virtual bool Update(Wannabe::BattleContext& context, float fDeltaTime) override;
    virtual bool IsValid(Wannabe::BattleContext& context) const override;
    virtual bool IsFinished() const override;

private:
    void InitExplosionParticle(std::vector<AsciiParticle>& particles, const Wannabe::Vector2& pos, int iParticleCnt);
    void InitParticles();
    void UpdateParticles(float dt);
    bool IsAllParticleDead() const;

private:
    std::vector<AsciiParticle> m_particles;
    Wannabe::Vector2 m_pos;

    float m_fDuration = 1.5f;
    float m_fElapsed = 0.f;
    int m_iParticleCnt = 0;
};