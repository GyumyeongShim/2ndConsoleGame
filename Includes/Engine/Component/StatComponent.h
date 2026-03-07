#pragma once
#include <string>
#include <vector>

#include "Core/Common.h"
#include "Math/Color.h"
#include "Item/Item.h"

namespace Wannabe
{
    struct StatData
    {
        int iLevel;
        int iMaxHp;
        int iHp;
        int iAtk;
        int iDef;
        int iSpd;

        // 확률 및 전투 보정 능력치 (단위: % 또는 0.1%)
        int iAccuracy;      // 명중률
        int iEvasion;       // 회피율
        int iCritChance;    // 치명타 확률
        int iCritResist;    // 치명타 저항 (상대의 치명타 확률을 상쇄)

        int iMaxExp;
        int iExp;

        int iTurnCnt;
        int iMaxTurnCnt;
    };

    struct EquipmentBonus
    {
        int atk = 0;
        int def = 0;
        int hp = 0;
        int spd = 0;

        int accuracy = 0;    // 명중 보정
        int evasion = 0;     // 회피 보정
        int critChance = 0;  // 치명타 확률 보정
        int critResist = 0;  // 치명타 저항 보정
    };

    class WANNABE_API StatComponent
    {
    public:
        StatComponent* Clone() const { return new StatComponent(*this); } //깊은 복사
        void Update(float fDeltaTime);

        void SetStatByData(const StatData& data);

        void ProgressTurn(float spdMultiple = 1.0f);
        void ResetTurn();

        void ModifyHP(int iHp);
        int ApplyDmg(int iValue);
        int ApplyHeal(int iValue);
        void CalcExp(const int exp);
        void LevelUp();

        void AddEquipmentModifier(int atk, int def, int hp, int speed);
        void RemoveEquipmentModifier(int atk, int def, int hp, int speed);

        bool IsLevelUp() const { return m_iExp >= m_iMaxExp; }
        bool IsTurnMax() const { return m_fTurnCnt >= m_iMaxTurnCnt; }
        bool IsDead() const { return m_iHp <= 0; }

        int GetExp() const { return m_iExp; }
        int GetHp() const { return m_iHp; }
        int GetAtk() const { return m_iAtk + m_EquipBonus.atk; }
        int GetDef() const { return m_iDef + m_EquipBonus.def; }
        int GetSpd() const { return m_iSpd + m_EquipBonus.spd; }
        int GetMaxHp() const { return m_iMaxHp + m_EquipBonus.hp; }
        int GetAccuracy() const { return m_iAccuracy + m_EquipBonus.accuracy; }
        int GetEvasion() const { return m_iEvasion + m_EquipBonus.evasion; }
        int GetCritChance() const { return m_iCritChance + m_EquipBonus.critChance; }
        int GetCritResist() const { return m_iCritResist + m_EquipBonus.critResist; }
        float GetTurnCnt() const { return m_fTurnCnt; }
        StatData GetStatData() const;

    private:
        int m_iLevel = 1;
        int m_iHp = 0;
        int m_iMaxHp = 0;

        int m_iAtk = 0;
        int m_iDef = 0;
        int m_iSpd = 0;
        
        int m_iAccuracy = 0;
        int m_iEvasion = 0;
        int m_iCritChance = 0;
        int m_iCritResist = 0;
        
        int m_iExp = 0;
        int m_iMaxExp = 0;

        float m_fTurnCnt = 1;
        int m_iMaxTurnCnt = 10;

        EquipmentBonus m_EquipBonus;
    };
}
