#include "TurnEndEvent.h"
#include "Actor/Actor.h"
#include "Battle/BattleContext.h"
#include "Interface/IBattleEventFactory.h"
#include "Battle/System/CutScenePlayer.h"
#include "Component/StatComponent.h"
#include "Battle/Cutscene/LogEvent.h"
#include "Battle/Cutscene/DeathEvent.h"
#include "Battle/Cutscene/BattlePhaseChangeEvent.h"

TurnEndEvent::TurnEndEvent(Wannabe::Actor* actor)
    :m_pTarget(actor)
{
}
void TurnEndEvent::OnStart(Wannabe::BattleContext& context)
{
    if (m_pTarget == nullptr) 
        return;

    auto expirationLogs = context.GetResolver().ResolveStatusExpiration(m_pTarget);
    for (const auto& log : expirationLogs)
        context.GetCutscenePlayer().Push(std::make_unique<LogEvent>(log));

    // 사망 체크
    auto* stat = m_pTarget->GetComponent<Wannabe::StatComponent>();
    if (stat && stat->IsDead())
    {
        // 이미 죽었다면 사망 연출 Push
        context.GetCutscenePlayer().Push(std::make_unique<DeathEvent>(m_pTarget));

        // Processor에 제거 예약
        context.GetEventProcessor().MarkForRemoval(m_pTarget);
    }
}

bool TurnEndEvent::Update(Wannabe::BattleContext&, float)
{
    return true;
}