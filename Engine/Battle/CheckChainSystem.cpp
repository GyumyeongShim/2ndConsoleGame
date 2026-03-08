#include "CheckChainSystem.h"

#include <vector>

#include "Enum/CombatType.h"
#include "Actor/Actor.h"
#include "Battle/BattleContext.h"
#include "Battle/ChainNode/ReflectNode.h"
#include "Battle/ChainNode/CounterNode.h"
#include "Battle/ChainNode/DeathCheckNode.h"
#include "Battle/ChainNode/HealReflectNode.h"
#include "Battle/ChainNode/StatusImmediateNode.h"
#include "Battle/ChainNode/TurnEndStatusNode.h"
#include "Battle/ChainNode/TurnStartStatusNode.h"
#include "Component/StatusComponent.h"

void Wannabe::CheckChainSystem::Init()
{
    m_vecNodes.emplace_back(std::make_unique<ReflectNode>());
    m_vecNodes.emplace_back(std::make_unique<CounterNode>());
    m_vecNodes.emplace_back(std::make_unique<DeathCheckNode>());
    m_vecNodes.emplace_back(std::make_unique<HealReflectNode>());
    m_vecNodes.emplace_back(std::make_unique<StatusImmediateNode>());
    m_vecNodes.emplace_back(std::make_unique<TurnEndStatusNode>());
    m_vecNodes.emplace_back(std::make_unique<TurnStartStatusNode>());
}

std::vector<CombatEffect> Wannabe::CheckChainSystem::Run(const CombatEffect& effect, BattleContext& context)
{
    std::vector<CombatEffect> result;
    if (effect.eCombatEffectType == CombatEffectType::None)
        return result;

    for (auto& node : m_vecNodes)
    {
        if (node == nullptr)
            continue;

        auto out = node->Check(effect, context);
        result.insert(result.end(), out.begin(), out.end());
    }

    return result;
}