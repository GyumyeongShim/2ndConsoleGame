#include <algorithm>
#include <vector>

#include "BattleEventProcessor.h"
#include "Actor/Actor.h"

#include "Battle/BattleContext.h"

#include "../Game/Battle/System/CutscenePlayer.h"
#include "Interface/IBattleEventFactory.h"
#include "Interface/IRemoveActorCallback.h"

#include "Component/StatusComponent.h"
#include "Component/DisplayComponent.h"
#include "Component/StatComponent.h"

namespace Wannabe
{
    static constexpr int iMaxDepth = 3;

    BattleEventProcessor::BattleEventProcessor(IBattleEventFactory* factory, IRemoveActorCallback* callback)
        : m_pEventFactory(factory), m_pRemoveCallback(callback)
    {
    }

    void BattleEventProcessor::ProcessCombatEffectResult(BattleContext& context, const CombatEffectResult& result, int iDepth /*= 0*/)
    {
        if (iDepth > iMaxDepth)
            return;

        for (const auto& effect : result.vecCombatEffect)
        {
            if (effect.pAtker == nullptr || effect.pTarget == nullptr)
                continue;

            ApplyEffect(context, effect);

            // 연쇄 반응 체크
            auto nextEffects = context.GetCheckChain().Run(effect, context);
            if (!nextEffects.empty())
            {
                CombatEffectResult next;
                next.vecCombatEffect = nextEffects;
                ProcessCombatEffectResult(context,next, iDepth + 1);
            }
        }
    }

    void BattleEventProcessor::OnTurnStart(BattleContext& context, Actor* pTarget)
    {
        if (context.IsValidActor(pTarget) == false)
            return;

        auto status = pTarget->GetStatus();
        if (status == nullptr)
            return;

        
        const auto& states = status->GetCurStatusState();
        for (const auto& state : states)
        {
            if (state.period.eType == CombatEffectType::None)
                continue;

            CombatEffect effect;
            effect.pAtker = state.pAtker;
            effect.pTarget = pTarget;
            effect.eCombatEffectType = state.period.eType;
            effect.iValue = state.period.iValue * state.iStackCnt;

            CombatEffectResult temp;
            temp.vecCombatEffect.emplace_back(effect);

            ProcessCombatEffectResult(context, temp, 0);
        }
    }

    void BattleEventProcessor::OnTurnEnd(BattleContext& context, Actor* pTarget)
    {
        if (context.IsValidActor(pTarget) == false)
            return;

        auto status = pTarget->GetStatus();
        if (status == nullptr)
            return;

        // 1. 상태 지속시간 감소
        status->CountDownStatus();

        // 2. 만료된 상태 추출
        std::vector<StatusState> expiredStates = status->GetExpiredStatusState();

        for (const auto& state : expiredStates)
        {
            // 상태 종료 로그 생성
            BattleLog log;
            log.wstrAtkerName = state.pAtker->GetDisplay()->GetOriginName();
            log.wstrTargetName = pTarget->GetDisplay()->GetOriginName();
            log.eLogType = LogType::StatusExpire;
            log.iValue = 0;
            log.bCritical = false;
            log.bMiss = false;

            context.GetCutscenePlayer().Push(m_pEventFactory->CreateLog(log));
        }

        // 3. 상태 종료 후 사망 체크 (예: 디버프 해제 후 자해형 효과 등 고려)
        auto stat = pTarget->GetStat();
        if (stat && stat->IsDead())
        {
            MarkForRemoval(pTarget);
        }
    }

    void BattleEventProcessor::ApplyDmg(BattleContext& context, const CombatEffect& effect)
    {
        Actor* atker = effect.pAtker;
        Actor* target = effect.pTarget;
        if (atker == nullptr || target == nullptr)
            return;

        auto stat = target->GetStat();
        if (stat == nullptr)
            return;

        int dmg = stat->ApplyDmg(effect.iValue);
        bool bIsDead = stat->IsDead();
        
        // 투사체 연출, 직선
        Vector2 startPos = atker->GetPosition();
        Vector2 endPos = target->GetPosition();
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateVisualEffect(
            startPos, endPos, EffectMovementType::Parabola, L'*', Color::Yellow, 0.8f
        ));

        // 로그 출력
        BattleLog log;
        log.wstrAtkerName = atker->GetDisplay()->GetOriginName();
        log.wstrTargetName = target->GetDisplay()->GetOriginName();
        log.eLogType = LogType::Damage;
        log.iValue = dmg;
        log.bCritical = effect.bCritical;
        log.bMiss = effect.bMiss;
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateLog(log));
        
        // 입자 연출, 50개의 입자가 1.0초간 비산
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateAsciiParticle(endPos, 50, 1.0f)); 
        
        // 연출
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateDmg(atker, target, dmg, effect.bCritical));
        
        // 사망
        if (stat->IsDead())
        {
            context.GetCutscenePlayer().Push(m_pEventFactory->CreateDeath(target));
        }
        
        // 턴 변경
        context.GetCutscenePlayer().Push(m_pEventFactory->CreatePhaseChange(BattleState::Log));
    }

    void BattleEventProcessor::ApplyHeal(BattleContext& context, const CombatEffect& effect)
    {
        Actor* target = effect.pTarget;
        if (target == nullptr)
            return;

        auto stat = effect.pTarget->GetStat();
        if (stat == nullptr)
            return;

        int heal = stat->ApplyHeal(effect.iValue);

        /// 로그 출력
        BattleLog log;
        log.wstrTargetName = target->GetDisplay()->GetOriginName();
        log.eLogType = LogType::Heal;
        log.iValue = heal;
        log.bCritical = effect.bCritical;
        log.bMiss = effect.bMiss;
        auto event = m_pEventFactory->CreateLog(log);
        context.GetCutscenePlayer().Push(std::move(event));
    }

    void BattleEventProcessor::ApplyStatus(BattleContext& context, const CombatEffect& effect)
    {
        Actor* target = effect.pTarget;
        if (target == nullptr)
            return;

        auto status = effect.pTarget->GetStatus();
        if (status == nullptr)
            return;

        bool applied = status->AddStatus(effect.eStatus,effect.iDuration,effect.iValue,effect.pAtker);
        if (applied == false)
            return;

        BattleLog log;
        log.wstrAtkerName = effect.pAtker->GetDisplay()->GetOriginName();
        log.wstrTargetName = effect.pTarget->GetDisplay()->GetOriginName();
        log.eLogType = LogType::StatusApply;
        log.iValue = effect.iValue;
        log.bCritical = false;
        log.bMiss = false;

        // 로그 출력
        auto event = m_pEventFactory->CreateLog(log);
        context.GetCutscenePlayer().Push(std::move(event));
    }

    void BattleEventProcessor::ApplyEffect(BattleContext& context, const CombatEffect& effect)
    {
        switch (effect.eCombatEffectType)
        {
        case CombatEffectType::Damage:
            ApplyDmg(context, effect);
            break;

        case CombatEffectType::Heal:
            ApplyHeal(context, effect);
            break;

        case CombatEffectType::ApplyStatus:
            ApplyStatus(context, effect);
            break;
        }
    }

    void BattleEventProcessor::MarkForRemoval(Actor* pTarget)
    {
        if (pTarget)
            m_vecPendingRemoval.emplace_back(pTarget);
    }

    void BattleEventProcessor::FlushRemoval()
    {
        for (Actor* actor : m_vecPendingRemoval)
        {
            if (m_pRemoveCallback)
                m_pRemoveCallback->OnRemoveActor(actor);
        }
        m_vecPendingRemoval.clear();
    }

    void BattleEventProcessor::RemoveActor(Actor* actor)
    {
        if (actor == nullptr)
            return;

        if (actor->GetStat()->IsDead())
            return;

        if (actor->IsDestroyRequested())
            return;

        if (m_pRemoveCallback)
            m_pRemoveCallback->OnRemoveActor(actor);
    }

    void BattleEventProcessor::SetEventFactory(IBattleEventFactory* factory)
    {
        m_pEventFactory = factory;
    }

    void BattleEventProcessor::SetRemoveCallback(IRemoveActorCallback* callback)
    {
        m_pRemoveCallback = callback;
    }
}