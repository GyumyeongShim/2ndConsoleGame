#include <vector>

#include "CheckChainSystem.h"

#include "Enum/CombatType.h"
#include "Battle/BattleContext.h"

#include "Battle/ChainNode/ReflectNode.h"
#include "Battle/ChainNode/CounterNode.h"
#include "Battle/ChainNode/DeathCheckNode.h"

void Wannabe::CheckChainSystem::Init()
{
    m_vecNodes.emplace_back(std::make_unique<ReflectNode>());
    m_vecNodes.emplace_back(std::make_unique<CounterNode>());
    m_vecNodes.emplace_back(std::make_unique<DeathCheckNode>());
}

std::vector<CombatEffect> Wannabe::CheckChainSystem::Run(const CombatEffect& effect, BattleContext& context)
{
    std::vector<CombatEffect> result;

    for (auto& node : m_vecNodes)
    {
        auto out = node->Check(effect, context);
        result.insert(result.end(), out.begin(), out.end());
    }

    return result;
}