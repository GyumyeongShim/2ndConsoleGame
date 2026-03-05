#include <memory>

#include "ProjectileActor.h"
#include "Actor/Actor.h"

#include "Battle/BattleContext.h"
#include "Battle/System/CutscenePlayer.h"

using namespace Wannabe;

ProjectileActor::ProjectileActor(Actor* pInstigator, Actor* pTarget, float fDuration)
    : Actor(), m_pTarget(pTarget), m_fDuration(fDuration)
{
}

void ProjectileActor::Tick(float fDeltaTime)
{
    m_fElapsed += fDeltaTime;

    if (m_fElapsed >= m_fDuration)
        Destroy();
}