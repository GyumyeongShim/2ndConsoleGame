#pragma once
#include <vector>

namespace Wannabe
{
    class BattleContext;
}

struct CombatEffect;

class ICheckNode
{
public:
    virtual ~ICheckNode() = default;
    virtual std::vector<CombatEffect> Check(const CombatEffect& effect, Wannabe::BattleContext& context) = 0;
};