#pragma once
#include <string>
#include <vector>
#include <algorithm>

#include "Core/Common.h"
#include "Component/Component.h"
#include "Math/Color.h"
#include "Enum/ItemType.h"

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

    class WANNABE_API StatComponent : public Component
    {
    public:
        StatComponent* Clone() const { return new StatComponent(*this); } //깊은 복사
        
        virtual void Update(float fDeltaTime) override;

        void SetStatByData(const StatData& data);

        void ProgressTurn(float spdMultiple = 1.0f);
        void ResetTurn();

        int ApplyDmg(int iValue);
        int ApplyHeal(int iValue);
        void CalcExp(const int exp);
        void LevelUp();

        void AddEquipmentModifier(const StatModifier& bonus);
        void RemoveEquipmentModifier(const StatModifier& bonus);

        bool IsLevelUp() const { return m_iExp >= m_iMaxExp; }
        bool IsTurnMax() const { return m_fTurnCnt >= m_iMaxTurnCnt; }
        bool IsDead() const { return m_iHp <= 0; }

        int GetLevel() const { return m_iLevel; }
        int GetExp() const { return m_iExp; }
        int GetHp() const { return m_iHp; }
        int GetAtk() const { return m_iAtk + m_EquipBonus.iAtk; }
        int GetDef() const { return m_iDef + m_EquipBonus.iDef; }
        int GetSpd() const { return m_iSpd + m_EquipBonus.iSpd; }
        int GetMaxHp() const { return m_iMaxHp + m_EquipBonus.iHp; }
        int GetAccuracy() const { return m_iAccuracy + m_EquipBonus.iAccuracy; }
        int GetEvasion() const { return m_iEvasion + m_EquipBonus.iEvasion; }
        int GetCritChance() const { return m_iCritChance + m_EquipBonus.iCritChance; }
        int GetCritResist() const { return m_iCritResist + m_EquipBonus.iCritResist; }
        float GetTurnCnt() const { return m_fTurnCnt; }
        StatData GetStatData() const;

    private:
        template<typename T>
        T Clamp(T val, T min, T max)
        {
            if (val < min) return min;
            if (val > max) return max;
            return val;
        }

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

        StatModifier m_EquipBonus;
    };
}