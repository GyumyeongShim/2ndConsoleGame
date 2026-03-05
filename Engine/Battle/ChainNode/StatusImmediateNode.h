#pragma once
#include "Interface/ICheckNode.h"

namespace Wannabe
{
	class BattleContext;
}

class StatusImmediateNode : public ICheckNode
{
public:
	virtual ~StatusImmediateNode() override = default;
	std::vector<CombatEffect> Check(const CombatEffect& effect, Wannabe::BattleContext& context) override;
};