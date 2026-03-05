#include "TurnStartEvent.h"
#include "Actor/Actor.h"
#include "Enum/CombatType.h"
#include "Battle/BattleContext.h"

#include "Interface/IBattleLevel.h"

#include "Battle/AI/EnemyAI.h"

TurnStartEvent::TurnStartEvent(Wannabe::Actor* actor)
	:m_pTarget(actor)
{
}

void TurnStartEvent::OnStart(Wannabe::BattleContext& context)
{
    if (m_pTarget == nullptr)
        return;

    auto& level = context.GetBattleLevel();
    if (m_pTarget->GetTeam() == Wannabe::Team::Player)
    {
        context.SetBattleState(BattleState::CommandSelect);
    }
    else
    {
        auto cmd = EnemyAI::Decide(m_pTarget, level.GetPlayerParty());
        if (cmd)
            level.PushCommand(std::move(cmd));

        context.SetBattleState(BattleState::Animation);
    }
}

bool TurnStartEvent::Update(Wannabe::BattleContext&, float)
{
    return true;
}