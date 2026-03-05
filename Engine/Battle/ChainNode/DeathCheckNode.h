#pragma once
#include "Interface/ICheckNode.h"

namespace Wannabe
{
	class BattleContext;
}

class DeathCheckNode : public ICheckNode //사망 이벤트 예약, 제거 예약, 반응 체인 연결, 연출 트리거 가능성
{
public:
	virtual ~DeathCheckNode() override = default;
	std::vector<CombatEffect> Check(const CombatEffect& effect, Wannabe::BattleContext& context) override;
};