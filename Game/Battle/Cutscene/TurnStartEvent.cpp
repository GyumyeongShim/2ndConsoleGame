#include "TurnStartEvent.h"

#include "Enum/CombatType.h"
#include "Actor/Actor.h"
#include "Battle/BattleContext.h"
#include "Battle/BattleResolver.h"

#include "Interface/IBattleLevel.h"

#include "Battle/AI/EnemyAI.h"
#include "Component/StatusComponent.h"

TurnStartEvent::TurnStartEvent(Wannabe::Actor* actor)
	:m_pTarget(actor)
{
}

void TurnStartEvent::OnStart(Wannabe::BattleContext& context)
{
    if (m_pTarget == nullptr)
        return;

    // 상태 이상 틱 처리 (데미지 등)
    auto* pStatus = m_pTarget->GetComponent<Wannabe::StatusComponent>();
    if (pStatus == nullptr)
        return;

    auto statusEffects = context.GetResolver().ResolveStatusEffects(m_pTarget);
    for (const auto& eff : statusEffects)
    {
        // 여기서 입자 이펙트, 사운드, HP 감소, 사망 체크가 연쇄적으로 일어남
        context.GetEventProcessor().ProcessCombatEffectResult(context, eff);
    }
}

bool TurnStartEvent::Update(Wannabe::BattleContext&, float)
{
    return true;
}