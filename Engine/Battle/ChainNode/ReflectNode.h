#pragma once
#include "Interface/ICheckNode.h"

namespace Wannabe
{
	class BattleContext;
}

class ReflectNode : public ICheckNode
{
public:
	virtual ~ReflectNode() override = default;
	std::vector<CombatEffect> Check(const CombatEffect& effect, Wannabe::BattleContext& context) override;
};