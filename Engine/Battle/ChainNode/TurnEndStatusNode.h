#pragma once
#include "Interface/ICheckNode.h"

namespace Wannabe
{
	class BattleContext;
}

class TurnEndStatusNode : public ICheckNode
{
public:
	virtual ~TurnEndStatusNode() override = default;
	std::vector<CombatEffect> Check(const CombatEffect& effect, Wannabe::BattleContext& context) override;
};