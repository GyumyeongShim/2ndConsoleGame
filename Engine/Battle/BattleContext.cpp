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

        if (pTarget->GetComponent<StatComponent>()== nullptr)
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

    BattleState BattleContext::GetBattleState()
    {
        if (m_pLevel == nullptr)
            return BattleState::Start;

        return m_pLevel->GetBattleState();
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