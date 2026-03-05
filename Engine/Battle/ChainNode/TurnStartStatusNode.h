#pragma once
#include "Interface/ICheckNode.h"

namespace Wannabe
{
	class BattleContext;
}

class TurnStartStatusNode : public ICheckNode
{
public:
	virtual ~TurnStartStatusNode() override = default;
	std::vector<CombatEffect> Check(const CombatEffect& effect, Wannabe::BattleContext& context) override;
};