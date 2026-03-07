#include "TurnEndStatusNode.h"

#include <vector>

#include "Actor/Actor.h"
#include "Enum/CombatType.h"
#include "Battle/BattleContext.h"
#include "Component/StatusComponent.h"

using namespace Wannabe;

std::vector<CombatEffect> TurnEndStatusNode::Check(const CombatEffect& effect, Wannabe::BattleContext& context)
{
    std::vector<CombatEffect> vec;
    if (effect.eCombatEffectType == CombatEffectType::None)
        return vec;

    if (context.IsValidActor(effect.pTarget) == false)
        return vec;

    auto* status = effect.pTarget->GetComponent<StatusComponent>();
    if (status == nullptr)
        return vec;

    if (status->HasStatus(StatusType::Counter) == true)
    {
        CombatEffect result;
        result.eCombatEffectType = effect.eCombatEffectType;
        result.pAtker = effect.pTarget; // 공수교대
        result.pTarget = effect.pAtker;

        result.iValue = effect.iValue / 2;
        result.eStatus = effect.eStatus;
        result.iDuration = effect.iDuration;

        vec.emplace_back(std::move(result));
    }

    return vec;
}
