#include <algorithm>
#include <random>

#include "BattleResolver.h"
#include "Util/Utill.h"
#include "Actor/Actor.h"

#include "Component/StatComponent.h"
#include "Component/StatusComponent.h"

namespace Wannabe
{
    ActCheckResult BattleResolver::CanAct(Actor* pTarget)
    {
        ActCheckResult result{ true, L"" };
        if (pTarget == nullptr || pTarget->IsDestroyRequested())
        {
            result.bCanAct = false;
            result.wstrReason = L"Invalid";
            return result;
        }

        auto status = pTarget->GetStatus();
        if (status == nullptr)
            return result;

        if (status->HasStatus(StatusType::Stun))
        {
            result.bCanAct = false;
            result.wstrReason = L"기절";
            return result;
        }

        if (status->HasStatus(StatusType::Freeze))
        {
            result.bCanAct = false;
            result.wstrReason = L"빙결";
            return result;
        }

        if (status->HasStatus(StatusType::Shock))
        {
            if (Util::Random(0, 100) < 50)
            {
                result.bCanAct = false;
                result.wstrReason = L"감전";
            }
        }

        return result;
    }

    CombatEffectResult BattleResolver::ResolveBasicAtk(Actor* pAtker, Actor* pTarget)
    {
        CombatEffectResult result;
        if (IsValidActor(pAtker) == false || IsValidActor(pTarget) == false)
            return result;

        CombatEffect effect;
        effect.eCombatEffectType = CombatEffectType::Damage;
        effect.pAtker = pAtker;
        effect.pTarget = pTarget;

        if (CheckMiss(pAtker, pTarget) == true) // 명중 판정
        {
            effect.bMiss = true;
            result.vecCombatEffect.emplace_back(effect);
            return result;
        }

        int dmg = CalcDmg(pAtker, pTarget);
        if (CheckCritical(pAtker, pTarget) == true) // 크리 판정
        {
            dmg = static_cast<int>(dmg * 1.2f);
            effect.bCritical = true;
        }
        effect.iValue = dmg;
        result.vecCombatEffect.emplace_back(effect);
        return result;
    }

    std::vector<CombatEffectResult> BattleResolver::ResolveAction(Actor* pAtker, Actor* pTarget, const std::vector<CombatEffectData>& data, int iPower)
    {
        std::vector<CombatEffectResult> results;

        if (IsValidActor(pAtker) == false || IsValidActor(pTarget) == false)
            return results;

        CombatEffectResult result;
        for (const auto& effectData : data)
        {
            CombatEffect effect;
            effect.pAtker = pAtker;
            effect.pTarget = pTarget;
            effect.eCombatEffectType = effectData.eType;
            if (CheckMiss(pAtker, pTarget))// --- 명중 판정 ---
            {
                effect.bMiss = true;
                result.vecCombatEffect.push_back(effect);
                continue;
            }

            switch (effectData.eType)
            {
            case CombatEffectType::Damage:
            {
                int damage = CalcSkillDamage(pAtker,pTarget,iPower,effectData.iRatio);
                if (CheckCritical(pAtker, pTarget))// 크리티컬 판정
                {
                    damage = static_cast<int>(damage * 1.2f);
                    effect.bCritical = true;
                }

                effect.iValue = damage;
                break;
            }

            case CombatEffectType::Heal:
            {
                int heal = CalcSkillHeal(pAtker,pTarget, iPower,effectData.iRatio);
                effect.iValue = heal;
                break;
            }

            case CombatEffectType::ApplyStatus:
            {
                effect.eStatus = effectData.eStatusType;
                effect.iDuration = effectData.iDuration;
                effect.iValue = effectData.iRatio; // 성공 확률 등
                break;
            }

            case CombatEffectType::RemoveStatus:
            {
                effect.eStatus = effectData.eStatusType;
                break;
            }

            case CombatEffectType::Buff:
            case CombatEffectType::Debuff:
            {
                effect.eStatus = effectData.eStatusType;
                effect.iDuration = effectData.iDuration;
                effect.iValue = effectData.iValue;
                break;
            }

            default:
                break;
            }

            result.vecCombatEffect.push_back(effect);
        }

        results.push_back(std::move(result));
        return results;
    }

    void BattleResolver::ResolveStatus(Actor* pActer, Actor* pTarget, StatusType eType, int iDuration, int iValue)
    {
        if (IsValidActor(pTarget) == false)
            return;

        auto* status = pTarget->GetStatus();
        if (status == nullptr)
            return;

        status->AddStatus(eType, iDuration, iValue, pActer);
    }

    bool BattleResolver::IsValidActor(Actor* pActor)
    {
        if (pActor == nullptr)
            return false;

        if (pActor->IsDestroyRequested())
            return false;

        if (pActor->GetStat() == nullptr)
            return false;

        return true;
    }

    int BattleResolver::CalcDmg(Actor* pAtker, Actor* pTarget)
    {
        int dmg = pAtker->GetStat()->GetAtk() - pTarget->GetStat()->GetDef();
        return std::max(1, dmg);
    }

    int BattleResolver::CalcSkillDamage(Actor* pAtaker, Actor* pTarget, int iPower, int iRatio)
    {
        if (IsValidActor(pAtaker) == false || IsValidActor(pTarget) == false)
            return 0;

        int atk = pAtaker->GetStat()->GetAtk();
        int def = pTarget->GetStat()->GetDef();

        // 1. 기본 공격력 기반
        float base = atk * (iPower * 0.01f);

        // 2. 스킬 개별 계수
        base *= (iRatio * 0.01f);

        // 3. 방어력 완전 차감이 아닌 완화 적용
        base -= def * 0.5f;

        base = std::max(1.0f, base);

        // 4. 랜덤 보정
        float random = Util::Random(0.9f, 1.1f);

        // 5. 치명타
        float crit = CheckCritical(pAtaker, pTarget) ? 1.5f : 1.0f;

        int dmg = static_cast<int>(base * random * crit);

        return std::max(1, dmg);
    }

    int BattleResolver::CalcSkillHeal(Actor* pAtaker, Actor* pTarget, int iPower, int iRatio)
    {
        if (IsValidActor(pAtaker) == false || IsValidActor(pTarget) == false)
            return 0;

        auto* atkerStat = pAtaker->GetStat();
        auto* targetStat = pTarget->GetStat();

        if (atkerStat == nullptr || targetStat == nullptr)
            return 0;

        float base = static_cast<float>(iPower);
        float AtkBonus = atkerStat->GetAtk() * 0.1f;
        float DefBonus = targetStat->GetDef() * 0.05f;

        float heal = base + AtkBonus + DefBonus;

        heal *= (iRatio / 100.0f);
        if (heal < 0.f)
            heal = 0.f;

        return static_cast<int>(heal);
    }

    bool BattleResolver::CheckMiss(Actor* pAtker, Actor* pTarget)
    {
        if (IsValidActor(pAtker) == false || IsValidActor(pTarget) == false)
            return true;

        float accuracy = static_cast<float>(pAtker->GetStat()->GetAccuracy());
        float evasion = static_cast<float>(pTarget->GetStat()->GetEvasion());
        float HitChance = std::max(0.05f, accuracy - evasion);

        return Util::Random(0.0f, 100.0f) > HitChance;
    }

    bool BattleResolver::CheckCritical(Actor* pAtker, Actor* pTarget)
    {
        if (IsValidActor(pAtker) == false)
            return false;

        float critChance = static_cast<float>(pAtker->GetStat()->GetCritChance());
        float critResist = static_cast<float>(pTarget->GetStat()->GetCritResist());
        float crit = std::max(0.0f, critChance - critResist);

        return Util::Random(0.0f, 1.0f) <= crit;
    }
}