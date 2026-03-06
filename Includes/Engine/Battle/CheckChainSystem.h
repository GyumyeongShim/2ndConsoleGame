#pragma once
#include <vector>
#include <memory>

#include "Interface/ICheckNode.h"

namespace Wannabe
{
    class CheckChainSystem
    {
    public:
        CheckChainSystem() = default;
        CheckChainSystem(const CheckChainSystem&) = delete;
        CheckChainSystem& operator=(const CheckChainSystem&) = delete;
        CheckChainSystem(CheckChainSystem&&) noexcept = default;
        CheckChainSystem& operator=(CheckChainSystem&&) noexcept = default;

    public:
        void Init();
        std::vector<CombatEffect> Run(const CombatEffect& effect, BattleContext& context);

    private:
        std::vector<std::unique_ptr<ICheckNode>> m_vecNodes;
    };
}