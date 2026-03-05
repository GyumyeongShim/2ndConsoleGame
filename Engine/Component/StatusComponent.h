#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "Common/Common.h"
#include "Interface/ICutsceneEvent.h"
#include "Enum/CombatType.h"

namespace Wannabe
{
	class Actor;
	class BattleContext;

	struct PeriodEffect
	{
		CombatEffectType eType = CombatEffectType::None;
		int iValue = 0;
	};

	struct StatusState
	{
		StatusType eStatusType; // 독, 기절, 화상 등

		int iDuration=0; // 남은 턴
		int iValue =0; // 수치
		int iStackCnt = 1; //중첩 수
		Actor* pAtker = nullptr;
		PeriodEffect period;
	};

	struct StatusRule
	{
		bool bStackable = false;
		bool bRefreshDurationOnApply = true;
		bool bAccumulateValue = false; // 스택 시 값 누적 여부
	};

	static const StatusRule& GetStatusRule(StatusType type)
	{
		static std::unordered_map<StatusType, StatusRule> table =
		{
			{ StatusType::Poison,  { true,  true,  true  } },
			{ StatusType::Burn,    { true,  true,  true  } },
			{ StatusType::Stun,    { false, true,  false } },
			{ StatusType::Thorns,  { false, true,  false } },
			{ StatusType::Counter, { false, true,  false } },
		};

		return table[type];
	}

	class WANNABE_API StatusComponent //상태 이상 처리하는 컴포넌트
	{
	public:
		bool AddStatus(StatusType eType, int iDuration, int iValue, Actor* pIntstigator);
		void Update(float fDeltaTime); // 매 턴 호출, deltaTime 기반으로 턴 진행
		void CountDownStatus(); // 턴이 끝날 때마다 상태 지속 시간 감소
		void ResetStatus(); // 모든 상태 초기화

		bool HasStatus(StatusType eState);
		void SetOwner(Actor* pOwner) { m_pOwner = pOwner; }
		bool IsStackable(StatusType eStatusType) const { return GetStatusRule(eStatusType).bStackable; }
		const std::vector<StatusState> GetCurStatusState() { return m_vecStatusState; }
		const std::vector<StatusState> GetExpiredStatusState();
		std::wstring GetStatusToString(StatusType eType) const;

	private:
		void RemoveExpiredStatus(); // 지속 시간이 0이 된 상태 제거

	private:
		Actor* m_pOwner = nullptr;
		std::vector<StatusState> m_vecStatusState;
	};
}