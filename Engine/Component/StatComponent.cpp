#include "StatComponent.h"

#include "Util/Utill.h"

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
		m_fTurnCnt = 0;
	}

	void StatComponent::ModifyHP(int iHp)
	{
		m_iHp += iHp;
		if (m_iHp < 0)
			m_iHp = 0;

		if (m_iHp > GetMaxHp())
			m_iHp = GetMaxHp();
	}

	int StatComponent::ApplyDmg(int iValue)
	{
		int dmg = std::max(0, iValue - GetDef());
		m_iHp -= dmg;
		if (m_iHp < 0) 
			m_iHp = 0;

		return dmg;
	}

	int StatComponent::ApplyHeal(int iValue)
	{
		int origin = m_iHp;
		m_iHp += iValue;
		if (m_iHp > GetMaxHp())
			m_iHp = GetMaxHp();

		return m_iHp - origin;
	}

	void StatComponent::CalcExp(const int exp)
	{
		m_iExp += exp;
	}

	void StatComponent::LevelUp()
	{
		++m_iLevel;

		// 기본 성장
		m_iMaxHp += 2;
		m_iHp = m_iMaxHp;

		m_iAtk += 1;
		if (m_iLevel % 2 == 0)
			m_iDef += 1;

		m_iExp = 0;
		m_iMaxExp = m_iLevel * 100;
	}

	void StatComponent::AddEquipmentModifier(int atk, int def, int hp, int speed)
	{
		m_EquipBonus.atk += atk;
		m_EquipBonus.def += def;
		m_EquipBonus.hp += hp;
		m_EquipBonus.spd += speed;
	}

	void StatComponent::RemoveEquipmentModifier(int atk, int def, int hp, int speed)
	{
		m_EquipBonus.atk -= atk;
		m_EquipBonus.def -= def;
		m_EquipBonus.hp -= hp;
		m_EquipBonus.spd -= speed;
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