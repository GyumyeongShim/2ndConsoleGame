#include "DeathCheckNode.h"

#include <vector>

#include "Actor/Actor.h"
#include "Enum/CombatType.h"
#include "Battle/BattleContext.h"
#include "Component/StatComponent.h"
#include "Interface/ICutsceneEvent.h"

using namespace Wannabe;

std::vector<CombatEffect> DeathCheckNode::Check(const CombatEffect& effect, Wannabe::BattleContext& context)
{
    std::vector<CombatEffect> vec;
    auto* target = effect.pTarget;
    if (target == nullptr)
        return vec;

    if (context.IsValidActor(target) == false)
        return vec;

    auto* stat = effect.pTarget->GetComponent<StatComponent>();
    if (stat == nullptr)
        return vec;

    if (stat->IsDead() == true)
        return vec;

    // 사망 로그 및 연출은 이미 ApplyDmg에서 처리되었으므로, 여기서는 판정만 수행
    auto playerParty = context.GetPlayerParty(effect.pTarget);
    auto enemyParty = context.GetEnemyParty(effect.pTarget);

    bool bAllPlayersDead = std::all_of(playerParty.begin(), playerParty.end(),
        [](Actor* a)
        {
            return (a == nullptr ||
                a->IsDestroyRequested() ||
                a->GetComponent<StatComponent>() == nullptr ||
                a->GetComponent<StatComponent>()->IsDead());
        });

    bool bAllEnemiesDead = std::all_of(enemyParty.begin(), enemyParty.end(),
        [](Actor* a) 
        { 
            return (a == nullptr ||
                a->IsDestroyRequested() ||
                a->GetComponent<StatComponent>() == nullptr ||
                a->GetComponent<StatComponent>()->IsDead());
        });

    if (bAllEnemiesDead || bAllPlayersDead)
    {
        BattleState nextState = bAllEnemiesDead ? BattleState::Victory : BattleState::Defeat;
        context.SetBattleState(nextState);

        return vec;
    }

    CombatEffect result;
    result.eCombatEffectType = CombatEffectType::None;
    result.pAtker = effect.pAtker;
    result.pTarget = target;

    vec.emplace_back(std::move(result));
    return vec;
}