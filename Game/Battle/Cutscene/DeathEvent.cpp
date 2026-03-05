#include "DeathEvent.h"
#include "Battle/BattleContext.h"

#include "Actor/Actor.h"

#include "Component/StatComponent.h"

DeathEvent::DeathEvent(Wannabe::Actor* pTarget)
    :m_pTarget(pTarget)
{
}

void DeathEvent::OnStart(Wannabe::BattleContext& context)
{
    context.GetEventProcessor().MarkForRemoval(m_pTarget);
}

bool DeathEvent::Update(Wannabe::BattleContext& context, float fDeltaTime)
{
    return true;
}