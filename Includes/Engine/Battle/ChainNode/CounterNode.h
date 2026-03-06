#pragma once
#include "Interface/ICheckNode.h"

namespace Wannabe
{
	class BattleContext;
}

class CounterNode : public ICheckNode
{
public:
	virtual ~CounterNode() override = default;
	std::vector<CombatEffect> Check(const CombatEffect& effect, Wannabe::BattleContext& context) override;
};
