#include "StatusComponent.h"

#include <algorithm>

#include "Actor/Actor.h"
#include "Component/StatComponent.h"

namespace Wannabe
{
	bool StatusComponent::AddStatus(StatusType eType, int iDuration, int iValue, Actor* pIntstigator)
	{
		const StatusRule& rule = GetStatusRule(eType);

		auto it = std::find_if(m_vecStatusState.begin(), m_vecStatusState.end(),
			[eType](const StatusState& s)
		{
			return s.eStatusType == eType;
		});

		if (it != m_vecStatusState.end())
		{
			if (rule.bStackable == false) //중첩 불가
			{
				if (rule.bRefreshDurationOnApply)
				{
					it->iDuration = iDuration;
				}
				return false;
			}

			// 중첩 처리
			it->iStackCnt++;

			if (rule.bAccumulateValue) // 누적
			{
				it->iValue += iValue;
			}

			if (rule.bRefreshDurationOnApply) // 갱신
			{
				it->iDuration = iDuration;
			}

			return true;
		}

		StatusState newState; //신규 상태이상의 경우
		newState.eStatusType = eType;
		newState.iDuration = iDuration;
		newState.iValue = iValue;
		newState.iStackCnt = 1;
		newState.pAtker = pIntstigator;

		m_vecStatusState.emplace_back(newState);
		return true;
	}

	bool StatusComponent::HasStatus(StatusType eType)
	{
		for (const StatusState& iter : m_vecStatusState)
		{
			if (iter.eStatusType == eType)
				return true;
		}

		return false;
	}
	
	void StatusComponent::CountDownStatus() // 턴이 끝날 때마다 상태 지속 시간 감소
	{
		for(auto& status : m_vecStatusState)
			status.iDuration --;
	}
	
	void StatusComponent::RemoveExpiredStatus() // 지속 시간이 0이 된 상태 제거
	{
		m_vecStatusState.erase(
			std::remove_if(m_vecStatusState.begin(), m_vecStatusState.end(),
				[](const StatusState& status) 
		{
			return status.iDuration <= 0; 
		}),
			m_vecStatusState.end());
	}

	void StatusComponent::ResetStatus()
	{
		m_vecStatusState.clear();
	}

	const std::vector<StatusState> StatusComponent::GetExpiredStatusState()
	{
		std::vector<StatusState> out; //보유한 상태이상에서 제거하고, out에 담는다.

		auto it = m_vecStatusState.begin();
		while (it != m_vecStatusState.end())
		{
			if (it->iDuration <= 0)
			{
				out.emplace_back(*it);
				it = m_vecStatusState.erase(it);
			}
			else
			{
				++it;
			}
		}

		return out;
	}

	std::wstring StatusComponent::GetStatusToString(StatusType eType) const
	{
		switch (eType)
		{
		case StatusType::Burn:
			return L"화상";
		case StatusType::Freeze:
			return L"빙결";
		case StatusType::Poison:
			return L"중독";
		case StatusType::Shock:
			return L"감전";
		case StatusType::Stun:
			return L"기절";
		case StatusType::Sleep:
			return L"수면";
		default:
			return L"";
		}
	}
}