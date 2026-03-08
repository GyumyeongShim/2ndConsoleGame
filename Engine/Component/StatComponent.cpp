#include "StatComponent.h"

#include "Util/Utill.h"
#include "Enum/ItemType.h"

namespace Wannabe
{
	void StatComponent::Update(float fDeltaTime)
	{
		float speedBonus = 1.0f + (GetSpd() * 0.1f);
		m_fTurnCnt += speedBonus * fDeltaTime;

		if (m_fTurnCnt > static_cast<float>(m_iMaxTurnCnt))
			m_fTurnCnt = static_cast<float>(m_iMaxTurnCnt);
	}

	void StatComponent::SetStatByData(const StatData& data)
	{
		m_iLevel = data.iLevel;
		m_iMaxHp = data.iMaxHp;
		m_iHp = data.iHp;
		m_iAtk = data.iAtk;
		m_iDef = data.iDef;
		m_iSpd = data.iSpd;

		m_iAccuracy = data.iAccuracy;
		m_iEvasion = data.iEvasion;
		m_iCritChance = data.iCritChance;
		m_iCritResist = data.iCritResist;

		m_iMaxExp = data.iMaxExp;
		m_iExp = data.iExp;

		m_fTurnCnt = static_cast<float>(data.iTurnCnt);
		m_iMaxTurnCnt = data.iMaxTurnCnt;
	}

	void StatComponent::ProgressTurn(float spdMultiple)
	{
		float speedBonus = 1.0f + (GetSpd() * 0.1f);
		m_fTurnCnt += speedBonus * spdMultiple;
	}

	void StatComponent::ResetTurn()
	{
		m_fTurnCnt = 0.0f;
	}

	int StatComponent::ApplyDmg(int iValue)
	{
		int finalDmg = std::max(0, iValue - GetDef());

		int origin = m_iHp;
		m_iHp = Clamp(m_iHp - finalDmg, 0, GetMaxHp());

		return origin - m_iHp;
	}

	int StatComponent::ApplyHeal(int iValue)
	{
		int origin = m_iHp;
		m_iHp = Clamp(m_iHp + iValue, 0, GetMaxHp());

		return m_iHp - origin;
	}

	void StatComponent::CalcExp(const int exp)
	{
		m_iExp += exp;
		// 레벨업 체크 루프 (한 번에 여러 레벨이 오를 수도 있음)
		while (m_iExp >= m_iMaxExp)
		{
			LevelUp();
		}
	}

	void StatComponent::LevelUp()
	{
		m_iExp -= m_iMaxExp;
		++m_iLevel;

		// 기본 성장
		m_iMaxHp += 20;
		m_iHp = m_iMaxHp;

		m_iAtk += 1;
		if (m_iLevel % 2 == 0)
			m_iDef += 1;

		m_iExp = 0;
		m_iMaxExp = m_iLevel * 100;
	}

	void StatComponent::AddEquipmentModifier(const StatModifier& bonus)
	{
		m_EquipBonus.iAtk += bonus.iAtk;
		m_EquipBonus.iDef += bonus.iDef;
		m_EquipBonus.iHp += bonus.iHp;
		m_EquipBonus.iSpd += bonus.iSpd;
		m_EquipBonus.iAccuracy += bonus.iAccuracy;
		m_EquipBonus.iEvasion += bonus.iEvasion;
		m_EquipBonus.iCritChance += bonus.iCritChance;
		m_EquipBonus.iCritResist += bonus.iCritResist;
	}

	void StatComponent::RemoveEquipmentModifier(const StatModifier& bonus)
	{
		m_EquipBonus.iAtk -= bonus.iAtk;
		m_EquipBonus.iDef -= bonus.iDef;
		m_EquipBonus.iHp -= bonus.iHp;
		m_EquipBonus.iSpd -= bonus.iSpd;
		m_EquipBonus.iAccuracy -= bonus.iAccuracy;
		m_EquipBonus.iEvasion -= bonus.iEvasion;
		m_EquipBonus.iCritChance -= bonus.iCritChance;
		m_EquipBonus.iCritResist -= bonus.iCritResist;
	}

	StatData StatComponent::GetStatData() const
	{
		StatData data;

		data.iLevel = m_iLevel;
		data.iMaxHp = m_iMaxHp;
		data.iHp = m_iHp;
		data.iAtk = m_iAtk;
		data.iDef = m_iDef;
		data.iSpd = m_iSpd;

		data.iAccuracy = m_iAccuracy;
		data.iEvasion = m_iEvasion;
		data.iCritChance = m_iCritChance;
		data.iCritResist = m_iCritResist;

		data.iMaxExp = m_iMaxExp;
		data.iExp = m_iExp;

		data.iTurnCnt = static_cast<int>(m_fTurnCnt);
		data.iMaxTurnCnt = m_iMaxTurnCnt;

		return data;
	}
}