#include "TurnEndEvent.h"
#include "Actor/Actor.h"
#include "Battle/BattleContext.h"

TurnEndEvent::TurnEndEvent(Wannabe::Actor* actor)
    :m_pTarget(actor)
{
}
void TurnEndEvent::OnStart(Wannabe::BattleContext& context)
{
    context.GetEventProcessor().OnTurnEnd(context, m_pTarget);
}

bool TurnEndEvent::Update(Wannabe::BattleContext&, float)
{
    return true;
}