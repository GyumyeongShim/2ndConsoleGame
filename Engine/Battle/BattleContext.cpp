#include "BattleContext.h"

#include <memory>
#include <random>

#include "Util/Utill.h"
#include "Actor/Actor.h"
#include "Enum/ActionType.h"
#include "Interface/IBattleLevel.h"
#include "Interface/IBattleEventFactory.h"
#include "Interface/IRemoveActorCallback.h"
#include "Battle/BattleLogSystem.h"
#include "Battle/BattleEventProcessor.h"
#include "Component/StatComponent.h"
#include "Component/StatusComponent.h"

namespace Wannabe
{
    BattleContext::BattleContext(
        BattleLogSystem* logSystem,CutscenePlayer* cutscenePlayer,RemoveActorFunc removeFunc,
        IBattleLevel* level,IBattleEventFactory* factory,IRemoveActorCallback* callback)
        :m_pBattleLogSystem(logSystem), m_pCutscenePlayer(cutscenePlayer), m_RemoveFunc(removeFunc), m_pLevel(level),m_pEventFactory(factory)
    {
        m_CheckChain.Init();
        m_BattleEventProcessor = std::make_unique<BattleEventProcessor>(factory, callback);
    }

    bool BattleContext::IsValidActor(Actor* pTarget) const
    {
        if (pTarget == nullptr)
            return false;

        if (pTarget->IsDestroyRequested())
            return false;

        if (pTarget->GetStat() == nullptr)
            return false;

        return true;
    }

    void BattleContext::SpawnActor(Actor* actor)
    {
        if (actor == nullptr)
            return;

        if (m_pLevel)
            m_pLevel->AddActor(std::unique_ptr<Actor>(actor));
    }

    void BattleContext::EndBattle()
    {
        //todo "도망"일때 해당 함수를 진행한다
    }

    bool BattleContext::IsEnemy(Actor* actor) const
    {
        if (actor == nullptr)
            return false;

        return actor->GetTeam() == Team::Enemy;
    }

    void BattleContext::SetBattleState(BattleState state)
    {
        if (m_pLevel == nullptr)
            return;

        m_pLevel->RequestBattleStateChange(state);
    }

    void BattleContext::SetBattleLevel(IBattleLevel* level)
    {
        m_pLevel = level;
    }

    void BattleContext::SetEventFactory(IBattleEventFactory* factory)
    {
        m_pEventFactory = factory;

        if (m_BattleEventProcessor == nullptr)
            return;

        m_BattleEventProcessor->SetEventFactory(factory);
    }

    void BattleContext::SetRemoveCallback(BattleEventProcessor::RemoveActorFunc func)
    {
        if (m_BattleEventProcessor)
            m_BattleEventProcessor->SetRemoveCallback(func);
    }

    std::vector<Actor*> BattleContext::ResolveTargets(Actor* pAtker, Actor* pTarget, ActionTargetType eTargetType, int iMaxTargetCnt)
    {
        std::vector<Actor*> result;

        if (IsValidActor(pAtker) == false)
            return result;

        // 대상 진영 추출
        std::vector<Actor*> temp;
        switch (eTargetType)
        {
            case ActionTargetType::Self:
            case ActionTargetType::SingleAlly:
            case ActionTargetType::AllAlly:
                // 아군 진영 추출
                temp = IsEnemy(pAtker) ? GetEnemyParty(pAtker) : GetPlayerParty(pAtker);
                break;

            case ActionTargetType::SingleEnemy:
            case ActionTargetType::AllEnemy:
            case ActionTargetType::RandomEnemy:
                // 적군 진영 추출
                temp = IsEnemy(pAtker) ? GetPlayerParty(pAtker) : GetEnemyParty(pAtker);
                break;

            default:
                return result; // 정의되지 않은 타입은 즉시 반환
        }

        // 죽은 대상 제거
        temp.erase(
            std::remove_if(temp.begin(), temp.end(),
                [](Actor* actor)
        {
            return actor == nullptr || actor->IsDestroyRequested() || actor->GetStat() == nullptr || actor->GetStat()->IsDead();
        }),
            temp.end());

        switch (eTargetType)
        {
        case ActionTargetType::Self:
            if (IsValidActor(pAtker) && !pAtker->GetStat()->IsDead())
            {
                result.emplace_back(pAtker);
            }
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
            if (temp.empty() == true)
                break;

            int count = std::min((int)temp.size(), iMaxTargetCnt);
            for (int i = 0; i < temp.size(); ++i)
            {
                int sour = Util::Random(0, (int)temp.size() - 1);
                int dest = Util::Random(0, (int)temp.size() - 1);
                std::swap(temp[sour], temp[dest]);
            }

            for (int i = 0; i < count; ++i)
                result.emplace_back(temp[i]);
            break;
        }

        default:
            break;
        }

        return result;
    }

    std::vector<Actor*> BattleContext::GetEnemyParty(Actor* actor)
    {
        std::vector<Actor*> result;

        for (auto* battleActor : m_pLevel->GetEnemyParty())
            result.emplace_back(battleActor);

        return result;
    }

    std::vector<Actor*> BattleContext::GetPlayerParty(Actor* actor)
    {
        std::vector<Actor*> result;

        for (auto* battleActor : m_pLevel->GetPlayerParty())
            result.emplace_back(battleActor);

        return result;
    }
}