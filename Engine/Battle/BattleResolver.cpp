#include "BattleResolver.h"

#include <algorithm>
#include <random>

#include "Battle/BattleContext.h"
#include "Util/Utill.h"
#include "Actor/Actor.h"
#include "Component/StatComponent.h"
#include "Component/StatusComponent.h"
#include "Component/DisplayComponent.h"

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

        auto status = pTarget->GetComponent<StatusComponent>();
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
        effect.eSource = DamageSource::BasicAtk;
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

            bool bIsBeneficial = (effectData.eType == CombatEffectType::Heal || effectData.eType == CombatEffectType::Buff || 
                                 (effectData.eType == CombatEffectType::ApplyStatus && 
                                  (effectData.eStatusType == StatusType::AtkUp || 
                                   effectData.eStatusType == StatusType::DefUp || 
                                   effectData.eStatusType == StatusType::SpdUp ||
                                   effectData.eStatusType == StatusType::Regen)));

            if (!bIsBeneficial && CheckMiss(pAtker, pTarget))
            {
                effect.bMiss = true;
                result.vecCombatEffect.emplace_back(effect);
                continue;
            }

            switch (effectData.eType)
            {
            case CombatEffectType::Damage:
            {
                int finalPower = (effectData.iPower > 0) ? effectData.iPower : iPower;
                int damage = CalcSkillDamage(pAtker, pTarget, finalPower, effectData.iRatio);
                if (CheckCritical(pAtker, pTarget))// 치명 체크
                {
                    damage = static_cast<int>(damage * 1.5f);
                    effect.bCritical = true;
                }

                effect.iValue = damage;
                break;
            }

            case CombatEffectType::Heal:
            {
                int finalPower = (effectData.iPower > 0) ? effectData.iPower : iPower;
                int heal = CalcSkillHeal(pAtker, pTarget, finalPower, effectData.iRatio);
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

            result.vecCombatEffect.emplace_back(effect);
        }

        results.emplace_back(std::move(result));
        return results;
    }

    std::vector<CombatEffectResult> BattleResolver::ResolveStatusEffects(Actor* pTarget)
    {
        std::vector<CombatEffectResult> results;
        if (IsValidActor(pTarget) == false)
            return results;

        auto* pStatus = pTarget->GetComponent<StatusComponent>();
        if (pStatus == nullptr)
            return results;

        std::vector<CombatEffect> rawEffects = pStatus->OnTurnStart();
        if (rawEffects.empty() == true)
            return results;

        CombatEffectResult result;
        for (auto& effect : rawEffects)
        {
            if (IsValidActor(effect.pTarget) == false)
                continue;

            // 효과 타입에 따른 추가 세부 계산
            switch (effect.eCombatEffectType)
            {
            case CombatEffectType::Damage:
            {
                // 독 데미지 등도 스탯이나 내성에 의해 감쇄될 수 있다면 여기서 계산
                // 현재는 고정 데미지 형태라면 iValue를 그대로 유지
                // 예: effect.iValue = CalcStatusDamage(effect.iValue, pTarget);
                break;
            }

            case CombatEffectType::Heal:
            {
                // 재생(Regeneration) 효과 등
                break;
            }

            default:
                break;
            }

            effect.eSource = DamageSource::StatusTick;

            result.vecCombatEffect.emplace_back(effect);
        }

        if (result.vecCombatEffect.empty() == false)
            results.emplace_back(std::move(result));

        return results;
    }

    std::vector<BattleLog> BattleResolver::ResolveStatusExpiration(Actor* pTarget)
    {
        std::vector<BattleLog> vecLog;
        auto* pStatus = pTarget->GetComponent<StatusComponent>();
        if (pStatus == nullptr)
            return vecLog;

        // 지속 시간 감소
        pStatus->CountDownStatus();

        // 만료된 상태이상
        std::vector<StatusState> expiredStates = pStatus->GetExpiredStatusState();
        for (const auto& state : expiredStates)
        {
            BattleLog log;
            log.wstrTargetName = pTarget->GetComponent<DisplayComponent>()->GetOriginName();
            log.wstrTxt = GetStatusToString(state.eStatusType) + L" 효과가 사라졌습니다.";
            log.eLogType = LogType::StatusExpire;

            vecLog.emplace_back(log);
        }

        return vecLog;
    }

    bool BattleResolver::IsValidActor(Actor* pActor)
    {
        if (pActor == nullptr)
            return false;

        if (pActor->IsDestroyRequested())
            return false;

        if (pActor->GetComponent<StatusComponent>() == nullptr)
            return false;

        return true;
    }

    int BattleResolver::CalcDmg(Actor* pAtker, Actor* pTarget)
    {
        int dmg = pAtker->GetComponent<StatComponent>()->GetAtk() - pTarget->GetComponent<StatComponent>()->GetDef();
        return std::max(1, dmg);
    }

    int BattleResolver::CalcSkillDamage(Actor* pAtaker, Actor* pTarget, int iPower, int iRatio)
    {
        if (IsValidActor(pAtaker) == false || IsValidActor(pTarget) == false)
            return 0;

        int atk = pAtaker->GetComponent<StatComponent>()->GetAtk();
        int def = pTarget->GetComponent<StatComponent>()->GetDef();

        // 1. 공격력 기본 수치 (Power와 Ratio 적용)
        float rawDamage = atk * (iPower * 0.01f) * (iRatio * 0.01f);

        // 2. 로그 스케일 방어력 적용: Damage = Raw * (100 / (100 + Def))
           // 방어력이 100이면 데미지 50% 감소, 300이면 75% 감소하는 구조
        float damageReduction = 100.0f / (100.0f + def);
        float finalBase = rawDamage * damageReduction;

        // 3. 랜덤 보정 및 치명타
        float random = Util::Random(0.9f, 1.1f);
        if (CheckCritical(pAtaker, pTarget)) 
            finalBase *= 1.5f;

        return std::max(1, (int)(finalBase * random));
    }

    int BattleResolver::CalcSkillHeal(Actor* pAtaker, Actor* pTarget, int iPower, int iRatio)
    {
        if (IsValidActor(pAtaker) == false || IsValidActor(pTarget) == false)
            return 0;

        auto* atkerStat = pAtaker->GetComponent<StatComponent>();
        auto* targetStat = pTarget->GetComponent<StatComponent>();

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

    std::vector<Actor*> BattleResolver::ResolveTargets(Wannabe::BattleContext& context, Actor* pAtker, Actor* pTarget, ActionTargetType eTargetType, int iMaxTargetCnt)
    {
        std::vector<Actor*> result;
        if (IsValidActor(pAtker) == false)
            return result;

        std::vector<Actor*> temp;
        switch (eTargetType)
        {
        case ActionTargetType::Self:
        case ActionTargetType::SingleAlly:
        case ActionTargetType::AllAlly:
            temp = context.IsEnemy(pAtker) ? context.GetEnemyParty(pAtker) : context.GetPlayerParty(pAtker);
            break;

        case ActionTargetType::SingleEnemy:
        case ActionTargetType::AllEnemy:
        case ActionTargetType::RandomEnemy:
            temp = context.IsEnemy(pAtker) ? context.GetPlayerParty(pAtker) : context.GetEnemyParty(pAtker);
            break;

        default:
            return result;
        }

        // 2. 유효하지 않거나 죽은 대상 필터링
        temp.erase(
            std::remove_if(temp.begin(), temp.end(),
                [this](Actor* actor)
        {
            return !IsValidActor(actor) || actor->GetComponent<StatComponent>()->IsDead();
        }),
            temp.end());

        // 3. 타입별 최종 타겟 확정
        switch (eTargetType)
        {
        case ActionTargetType::Self:
            if (!pAtker->GetComponent<StatComponent>()->IsDead())
                result.emplace_back(pAtker);
            break;

        case ActionTargetType::SingleEnemy:
        case ActionTargetType::SingleAlly:
            if (IsValidActor(pTarget))
            {
                auto it = std::find(temp.begin(), temp.end(), pTarget);
                if (it != temp.end())
                    result.emplace_back(pTarget);
            }
            break;

        case ActionTargetType::AllEnemy:
        case ActionTargetType::AllAlly:
            result = temp;
            break;

        case ActionTargetType::RandomEnemy:
        {
            if (temp.empty())
                break;

            int count = std::min((int)temp.size(), iMaxTargetCnt);
            // 셔플 로직
            for (int i = 0; i < (int)temp.size(); ++i)
            {
                int sour = Util::Random(0, (int)temp.size() - 1);
                int dest = Util::Random(0, (int)temp.size() - 1);
                std::swap(temp[sour], temp[dest]);
            }

            for (int i = 0; i < count; ++i)
                result.emplace_back(temp[i]);
            break;
        }
        }

        return result;
    }

    bool BattleResolver::CalcRunSucess()
    {
        return Util::Random(0, 100) < 50;
    }

    bool BattleResolver::CheckMiss(Actor* pAtker, Actor* pTarget)
    {
        if (IsValidActor(pAtker) == false || IsValidActor(pTarget) == false)
            return true;

        float accuracy = static_cast<float>(pAtker->GetComponent<StatComponent>()->GetAccuracy());
        float evasion = static_cast<float>(pTarget->GetComponent<StatComponent>()->GetEvasion());
        float HitChance = std::max(0.05f, accuracy - evasion);

        return Util::Random(0.0f, 100.0f) > HitChance;
    }

    bool BattleResolver::CheckCritical(Actor* pAtker, Actor* pTarget)
    {
        if (IsValidActor(pAtker) == false)
            return false;

        float critChance = static_cast<float>(pAtker->GetComponent<StatComponent>()->GetCritChance());
        float critResist = static_cast<float>(pTarget->GetComponent<StatComponent>()->GetCritResist());
        float crit = std::max(0.0f, critChance - critResist);

        return Util::Random(0.0f, 1.0f) <= crit;
    }
}