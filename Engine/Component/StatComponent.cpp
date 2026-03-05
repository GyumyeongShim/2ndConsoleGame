#include "StatComponent.h"

#include "Util/Utill.h"

namespace Wannabe
{
	void StatComponent::Update(float fDeltaTime)
	{
		m_fTurnCnt += fDeltaTime;

		if (m_fTurnCnt > m_iMaxTurnCnt)
			m_fTurnCnt = static_cast<float>(m_iMaxTurnCnt);
	}

	void StatComponent::SetStatByData(const StatData& data)
	{
		// data -> 멤버 변수로 복사하도록 수정
		m_iLevel = data.iLevel;
		m_iMaxHp = data.iMaxHp;
		m_iHp = data.iHp;
		m_iAtk = data.iAtk;
		m_iDef = data.iDef;

		m_iMaxExp = data.iMaxExp;
		m_iExp = data.iExp;

		m_fTurnCnt = static_cast<float>(data.iTurnCnt);
		m_iMaxTurnCnt = data.iMaxTurnCnt;
	}

	void StatComponent::ProgressTurn(float spdMultiple)
	{
		float speed = 1.0f + (m_EquipBonus.spd * 0.1f);
		m_fTurnCnt += speed;
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
}