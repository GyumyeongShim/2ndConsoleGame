#pragma once
#include <vector>
#include <string>

#include "Core/Common.h"
#include "Enum/CombatType.h"
#include "Enum/ActionType.h"
#include "Enum/ItemType.h"

namespace Wannabe
{
	class Actor;
	class BattleContext;

	class WANNABE_API BattleResolver //순수 계산 용도 데미지, 크리, 명중, 상태 이상, 카운터 등등
	{
	public:
		ActCheckResult CanAct(Actor* pTarget);
		CombatEffectResult ResolveBasicAtk(Actor* pAtker, Actor* pTarget);
		std::vector<CombatEffectResult> ResolveAction(Actor* pAtker, Actor* pTarget, const std::vector<CombatEffectData>& data, int iPower);
		std::vector<CombatEffectResult> ResolveStatusEffects(Actor* pTarget);
		std::vector<BattleLog> ResolveStatusExpiration(Actor* pTarget);

		std::vector<Actor*> ResolveTargets(Wannabe::BattleContext& context, Actor* pAtker, Actor* pTarget, ActionTargetType eTargetType, int iMaxTargetCnt);
		bool CalcRunSucess();
		bool IsValidActor(Actor* pActor);

	private:
		int CalcDmg(Actor* pAtker, Actor* pTarget);
		int CalcSkillDamage(Actor* pAtaker, Actor* pTarget, int iPower, int iRatio);
		int CalcSkillHeal(Actor* pAtaker, Actor* pTarget, int iPower, int iRatio);
		bool CheckMiss(Actor* pAtker, Actor* pTarget);
		bool CheckCritical(Actor* pAtker, Actor* pTarget);
	};
}