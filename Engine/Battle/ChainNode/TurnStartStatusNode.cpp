#include "TurnStartStatusNode.h"

#include <vector>

#include "Actor/Actor.h"
#include "Enum/CombatType.h"
#include "Battle/BattleContext.h"
#include "Component/StatusComponent.h"

std::vector<CombatEffect> TurnStartStatusNode::Check(const CombatEffect& effect, Wannabe::BattleContext& context)
{
    std::vector<CombatEffect> vec;

    if (effect.eCombatEffectType != CombatEffectType::ApplyStatus)
        return vec;

    if (effect.pTarget == nullptr || context.IsValidActor(effect.pTarget) == false)
        return vec;

    auto* statusComp = effect.pTarget->GetStatus();
    if (statusComp == nullptr)
        return vec;

    CombatEffect result;
    if (statusComp->HasStatus(StatusType::Counter) == true)
    {
        CombatEffect result;
        result.eCombatEffectType = CombatEffectType::ApplyStatus;
        result.pAtker = effect.pTarget;
        result.pTarget = effect.pAtker;

        result.eStatus = effect.eStatus;
        result.iDuration = effect.iDuration;

        result.iValue = effect.iValue / 2;

        vec.emplace_back(std::move(result));
    }
    return vec;
}
