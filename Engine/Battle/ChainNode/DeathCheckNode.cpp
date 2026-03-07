#include "DeathCheckNode.h"

#include <vector>

#include "Actor/Actor.h"
#include "Enum/CombatType.h"
#include "Battle/BattleContext.h"
#include "Component/StatComponent.h"

std::vector<CombatEffect> DeathCheckNode::Check(const CombatEffect& effect, Wannabe::BattleContext& context)
{
    std::vector<CombatEffect> vec;
    auto* target = effect.pTarget;
    if (target == nullptr)
        return vec;

    if (context.IsValidActor(target) == false)
        return vec;

    auto* stat = target->GetStat();
    if (stat == nullptr)
        return vec;

    if (stat->IsDead() == true)
        return vec;

    CombatEffect result;
    result.eCombatEffectType = CombatEffectType::None;
    result.pAtker = effect.pAtker;
    result.pTarget = target;

    vec.emplace_back(std::move(result));
    return vec;
}