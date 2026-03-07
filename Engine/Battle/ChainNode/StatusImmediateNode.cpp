#include "StatusImmediateNode.h"

#include <vector>

#include "Actor/Actor.h"
#include "Enum/CombatType.h"
#include "Battle/BattleContext.h"
#include "Component/StatusComponent.h"

std::vector<CombatEffect> StatusImmediateNode::Check(const CombatEffect& effect, Wannabe::BattleContext& context)
{
    std::vector<CombatEffect> vec;

    if (effect.eCombatEffectType != CombatEffectType::ApplyStatus)
        return vec;

    CombatEffect result;
    result.eCombatEffectType = CombatEffectType::Damage;
    result.pAtker = effect.pAtker;
    result.pTarget = effect.pTarget;
    result.iValue = effect.iValue / 2;

    if (effect.pTarget->GetStatus()->HasStatus(StatusType::Counter) == true)
        vec.emplace_back(std::move(result));

    return vec;
}
