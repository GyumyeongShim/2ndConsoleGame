#include <vector>

#include "CounterNode.h"

#include "Actor/Actor.h"

#include "Enum/CombatType.h"
#include "Battle/BattleContext.h"

#include "Component/StatusComponent.h"

std::vector<CombatEffect> CounterNode::Check(const CombatEffect& effect, Wannabe::BattleContext& context)
{
    std::vector<CombatEffect> vec;

    if (effect.eCombatEffectType != CombatEffectType::Damage)
        return vec;

    if (context.IsValidActor(effect.pTarget) == false)
        return vec;

    auto* status = effect.pTarget->GetStatus();
    if (status == nullptr)
        return vec;

    if (status->HasStatus(StatusType::Counter) == false)
        return vec;

    CombatEffect reflect;
    reflect.eCombatEffectType = CombatEffectType::Damage;
    reflect.pAtker = effect.pTarget;
    reflect.pTarget = effect.pAtker;
    reflect.iValue = effect.iValue;

    vec.emplace_back(std::move(reflect));
    return vec;
}
