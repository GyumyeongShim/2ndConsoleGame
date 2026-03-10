#include "BattleEventProcessor.h"

#include <algorithm>

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
        {
            // 턴 변경
            context.GetCutscenePlayer().Push(m_pEventFactory->CreatePhaseChange(BattleState::Log));
            return;
        }

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

    void BattleEventProcessor::ApplyDmg(BattleContext& context, const CombatEffect& effect)
    {
        Actor* atker = effect.pAtker;
        Actor* target = effect.pTarget;
        if (atker == nullptr || target == nullptr)
            return;

        auto stat = target->GetComponent<StatComponent>();
        if (stat == nullptr)
            return;

        if(effect.bMiss == true) // 회피 판정
        {
            // 1. 시각적 연출 추가 (MISS 텍스트가 타겟 위치에서 위로 튀어오름)
           Vector2 startPos = target->GetPosition();
           Vector2 endPos = { int(startPos.x + 2.0f), int(startPos.y - 1.0f) }; // 약간 옆으로 이동하며 종료
            context.GetCutscenePlayer().Push(m_pEventFactory->CreateVisualEffect(
                startPos, endPos, EffectMovementType::Parabola, L'M', Color::Yellow, 0.8f
            ));

            // 로그 출력
            BattleLog log;
            log.wstrAtkerName = atker->GetComponent<DisplayComponent>()->GetOriginName();
            log.eLogType = LogType::Miss;
            context.GetCutscenePlayer().Push(m_pEventFactory->CreateLog(log));

            // 턴 변경
            context.GetCutscenePlayer().Push(m_pEventFactory->CreatePhaseChange(BattleState::Log));
            return;
        }

        int dmg = stat->ApplyDmg(effect.iValue);

        Vector2 startPos = atker->GetPosition();
        Vector2 endPos = target->GetPosition();

        // 이동 연출
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateMove(atker, endPos, 0.3f));

        // 공격자 애니 (atk타입)
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateAsciiAnimation(atker, EAniType::Attack));

        // 투사체 연출, 직선
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateVisualEffect(
            startPos, endPos, EffectMovementType::Parabola, L'*', Color::Yellow, 0.8f
        ));

        // 피격자 애니메이션
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateAsciiAnimation(target, EAniType::Hit));

        // 로그 출력
        BattleLog log;
        log.wstrAtkerName = atker->GetComponent<DisplayComponent>()->GetOriginName();
        log.wstrTargetName = target->GetComponent<DisplayComponent>()->GetOriginName();
        log.eLogType = LogType::Damage;
        log.iValue = dmg;
        log.bCritical = effect.bCritical;
        log.bMiss = effect.bMiss;
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateLog(log));
        
        // 입자 연출, 50개의 입자가 1.0초간 비산
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateAsciiParticle(endPos, 50, 1.0f)); 
        
        // 데미지 연출
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateDmg(atker, target, dmg, effect.bCritical));
        
        // 사망
        if (stat->IsDead())
        {
            int exp = stat->GetStatData().iMaxExp;
            context.AddEarnedExp(exp);

            // 사망 애니메이션
            context.GetCutscenePlayer().Push(m_pEventFactory->CreateAsciiAnimation(target, EAniType::Death));

            // 사망 이벤트
            context.GetCutscenePlayer().Push(m_pEventFactory->CreateDeath(target));
        }
    }

    void BattleEventProcessor::ApplyHeal(BattleContext& context, const CombatEffect& effect)
    {
        Actor* target = effect.pTarget;
        if (target == nullptr)
            return;

        auto stat = effect.pTarget->GetComponent<StatComponent>();
        if (stat == nullptr)
            return;

        int heal = stat->ApplyHeal(effect.iValue);

        // 시각적 연출 (하단에서 위로 솟구치는 하트나 + 기호)
        Vector2 pos = target->GetPosition();
        Vector2 startPos = { pos.x, pos.y + 1 };
        Vector2 endPos = { pos.x, pos.y - 2 };

        context.GetCutscenePlayer().Push(m_pEventFactory->CreateVisualEffect(
            startPos, endPos, EffectMovementType::Straight, L'+', Color::Green, 0.6f
        ));

        /// 로그 출력
        BattleLog log;
        log.wstrTargetName = target->GetComponent<DisplayComponent>()->GetOriginName();
        log.eLogType = LogType::Heal;
        log.iValue = heal;
        log.bCritical = effect.bCritical;
        log.bMiss = effect.bMiss;
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateLog(log));
    }

    void BattleEventProcessor::ApplyStatus(BattleContext& context, const CombatEffect& effect)
    {
        Actor* target = effect.pTarget;
        if (target == nullptr)
            return;

        auto status = effect.pTarget->GetComponent<StatusComponent>();
        if (status == nullptr)
            return;

        bool applied = status->AddStatus(effect.eStatus,effect.iDuration,effect.iValue,effect.pAtker);
        if (applied == false)
            return;

        // 2. 시각적 연출 (타겟 머리 위에서 반짝이는 느낌)
        Vector2 pos = target->GetPosition();
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateVisualEffect(
            pos, { pos.x, pos.y - 1 }, EffectMovementType::Parabola, L'!', Color::Cyan, 0.5f
        ));

        BattleLog log;
        log.wstrAtkerName = effect.pAtker->GetComponent<DisplayComponent>()->GetOriginName();
        log.wstrTargetName = effect.pTarget->GetComponent<DisplayComponent>()->GetOriginName();
        log.wstrTxt = GetStatusToString(effect.eStatus);
        log.eLogType = LogType::StatusApply;
        log.iValue = effect.iValue;
        log.bCritical = false;
        log.bMiss = false;

        // 로그 출력
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateLog(log));

        // 4. 입자 연출 (상태 이상 특유의 흩뿌려지는 느낌)
        context.GetCutscenePlayer().Push(m_pEventFactory->CreateAsciiParticle(pos, 20, 0.7f));
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

        if (actor->IsDestroyRequested())
            return;

        if (actor->GetComponent<StatComponent>() == nullptr)
            return;

        if (actor->GetComponent<StatComponent>()->IsDead())
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