#include "HealReflectNode.h"

#include <vector>

#include "Actor/Actor.h"
#include "Enum/CombatType.h"
#include "Battle/BattleContext.h"
#include "Component/StatusComponent.h"

using namespace Wannabe;

std::vector<CombatEffect> HealReflectNode::Check(const CombatEffect& effect, Wannabe::BattleContext& context)
{
    std::vector<CombatEffect> vec;

    if (effect.eCombatEffectType != CombatEffectType::ApplyStatus)
        return vec;

    if (effect.pTarget == nullptr || effect.pAtker == nullptr)
        return vec;

    auto* status = effect.pTarget->GetComponent<StatusComponent>();
    if (status == nullptr)
        return vec;

    if (status->HasStatus(StatusType::Counter) == true)
    {
        CombatEffect result;
        result.eCombatEffectType = CombatEffectType::ApplyStatus;
        result.pAtker = effect.pTarget;
        result.pTarget = effect.pAtker;
        result.eStatus = effect.eStatus;
        result.iDuration = effect.iDuration;
        result.iValue = effect.iValue;

        vec.emplace_back(std::move(result));
    }

    return vec;
}