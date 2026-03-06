#include <vector>

#include "AsciiParticleEvent.h"

#include "Engine/Engine.h"
#include "Battle/BattleContext.h"
#include "Math/Color.h"

using namespace Wannabe;

AsciiParticleEvent::AsciiParticleEvent(const Wannabe::Vector2& pos, int iParticleCnt, float fDuration)
    : m_pos(pos), m_fDuration(fDuration), m_fElapsed(0.f), m_iParticleCnt(iParticleCnt)
{
}

void AsciiParticleEvent::OnStart(Wannabe::BattleContext& context)
{
    m_fElapsed = 0.f;

    InitParticles();
}

bool AsciiParticleEvent::Update(Wannabe::BattleContext& context, float fDeltaTime)
{
    m_fElapsed += fDeltaTime;

    UpdateParticles(fDeltaTime);
    
    RenderSystem& renderSys = Engine::Get().GetRenderSystem();
    for (const auto& p : m_particles)
    {
        if (!p.bAlive)
            continue;

        std::wstring txt(1, static_cast<wchar_t>(p.Symbol));
        renderSys.DrawUI(txt,Vector2((int)p.Position.x, (int)p.Position.y),
            Color::RightCyan,100 // Particle Layer Sorting Order (높게)
        );
    }

    bool bTimeOver = (m_fElapsed >= m_fDuration);
    bool bAllDead = IsAllParticleDead();

    return !(bTimeOver && bAllDead);
}

bool AsciiParticleEvent::IsValid(Wannabe::BattleContext& context) const
{
    return m_particles.empty() == false;
}

bool AsciiParticleEvent::IsFinished() const
{
    return m_fElapsed >= m_fDuration;
}

void AsciiParticleEvent::InitExplosionParticle(std::vector<AsciiParticle>& particles, const Wannabe::Vector2& pos, int iParticleCnt)
{
    particles.clear();
    particles.reserve(iParticleCnt);

    const float PI = 3.141592f;

    for (int i = 0; i < iParticleCnt; i++)
    {
        float angle = ((float)rand() / RAND_MAX) * 2.f * PI;
        float speed = 3.f + ((float)rand() / RAND_MAX) * 5.f;

        AsciiParticle p;

        p.Position = pos;

        p.Velocity = Vector2(cos(angle) * speed,sin(angle) * speed);

        // 약한 중력 (JRPG 스타일은 과장된 물리보다 살짝 느린 게 좋다)
        p.Acceleration = Vector2(0.f, 2.5f);

        p.LifeTime = 0.f;
        p.MaxLifeTime = 1.2f + ((float)rand() / RAND_MAX) * 0.8f;

        particles.push_back(p);
    }
}

void AsciiParticleEvent::InitParticles()
{
    m_particles.clear();
    m_particles.reserve(m_iParticleCnt);

    const float PI = 3.141592f;

    for (int i = 0; i < m_iParticleCnt; i++)
    {
        float angle = ((float)rand() / RAND_MAX) * 2.f * PI;

        float speed = 3.f + ((float)rand() / RAND_MAX) * 5.f;

        AsciiParticle p;

        p.Position = m_pos;

        p.Velocity = Vector2(
            cosf(angle) * speed,
            sinf(angle) * speed
        );

        p.Acceleration = Vector2(0.f, 2.8f);

        p.MaxLifeTime = 1.2f + ((float)rand() / RAND_MAX) * 0.8f;

        p.Symbol = "*+o."[rand() % 4];

        m_particles.push_back(p);
    }
}

void AsciiParticleEvent::UpdateParticles(float fDeltaTime)
{
    for (auto& p : m_particles)
    {
        if (!p.bAlive)
            continue;

        p.LifeTime += fDeltaTime;

        if (p.LifeTime > p.MaxLifeTime)
        {
            p.bAlive = false;
            continue;
        }

        p.Velocity += p.Acceleration * fDeltaTime;
        p.Position += p.Velocity * fDeltaTime;
    }
}

bool AsciiParticleEvent::IsAllParticleDead() const
{
    for (const auto& p : m_particles)
    {
        if (p.bAlive)
            return false;
    }

    return true;
}