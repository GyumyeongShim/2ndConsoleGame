#include <vector>

#include "StatusImmediateNode.h"

#include "Actor/Actor.h"

#include "Enum/CombatType.h"
#include "Battle/BattleContext.h"

#include "Component/StatusComponent.h"

std::vector<CombatEffect> StatusImmediateNode::Check(const CombatEffect& effect, Wannabe::BattleContext& context)
{
    std::vector<CombatEffect> vec;

    if (effect.eCombatEffectType != CombatEffectType::ApplyStatus)
        return vec;

    if (effect.pTarget->GetStatus()->HasStatus(StatusType::Counter) == true)
        return vec;

    CombatEffect reflect;
    reflect.eCombatEffectType = CombatEffectType::Damage;
    reflect.pAtker = effect.pTarget;
    reflect.pTarget = effect.pAtker;
    reflect.iValue = effect.iValue / 2;

    vec.emplace_back(std::move(reflect));
    return vec;
}
