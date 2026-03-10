#include "TurnManager.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include "Actor/Actor.h"
#include "Component/StatComponent.h"
#include "Component/StatusComponent.h"

namespace Wannabe
{
    TurnManager* TurnManager::instance = nullptr;

    TurnManager::TurnManager()
    {
        instance = this;
    }

    Actor* TurnManager::GetNextActor(const std::vector<Actor*>& players, const std::vector<Actor*>& enemies)
    {
        m_vecActors.clear();
        m_CurActor = nullptr;

        m_vecActors.insert(m_vecActors.end(), players.begin(), players.end());
        m_vecActors.insert(m_vecActors.end(), enemies.begin(), enemies.end());
        if (m_vecActors.empty())
            return nullptr;

        float fMaxTurn = 0.0f;
        for (auto actor : m_vecActors)
        {
            if (actor == nullptr || actor->IsDestroyRequested() == true)
                continue;
      
            auto* stat = actor->GetComponent<StatComponent>();
            if (stat == nullptr || stat->IsDead() == true)
                continue;

            if (stat->IsTurnMax())
            {
                // 단순히 찼는지 확인하는 게 아니라, 누가 더 많이 찼는지(Overcharge) 비교
                float fTurnCnt = stat->GetTurnCnt();
                if (fTurnCnt > fMaxTurn)
                {
                    fMaxTurn = fTurnCnt;
                    m_CurActor = actor;
                }
            }
        }

        return m_CurActor;
    }

    void TurnManager::ProgressTurns()
    {
        if (m_vecActors.empty())
            return;

        for (Wannabe::Actor* actor : m_vecActors)
        {
            if (actor == nullptr || actor->IsDestroyRequested())
                continue;

            auto* stat = actor->GetComponent<StatComponent>();
            if (stat == nullptr || stat->IsDead() == true)
                continue;

            // 상태이상: Stun이면 턴 스킵
            auto* status = actor->GetComponent<StatusComponent>();
            if (status && status->HasStatus(StatusType::Stun))
            {
                // 스턴이면 턴 리셋 대신 진행만
                stat->ResetTurn();
                continue;
            }

            // 턴 카운트 증가
            stat->ProgressTurn();
        }
    }

    void TurnManager::TurnEnd()
    {
        if (m_CurActor == nullptr || m_CurActor->IsDestroyRequested())
            return;

        if (m_CurActor->GetComponent<StatComponent>())
            m_CurActor->GetComponent<StatComponent>()->ResetTurn();

        m_CurActor = nullptr;
    }

    void TurnManager::ResetTurns()
    {
        if (m_vecActors.empty())
            return;

        for (Wannabe::Actor* actor : m_vecActors)
        {
            if (actor == nullptr)
                continue;

            if (actor->IsDestroyRequested())
                continue;

            auto* stat = actor->GetComponent<StatComponent>();
            if (stat == nullptr || stat->IsDead() == true)
                continue;

            stat->ResetTurn();
        }
    }

    TurnManager& TurnManager::Get()
    {
        if (!instance)
        {
            std::cerr << "TurnManager::Get() instance is Null\n";
            __debugbreak();
        }

        return *instance;
    }

    const std::vector<Actor*> TurnManager::GetTurnQueue()
    {
        std::vector<Actor*> displayQueue;
        for (auto actor : m_vecActors) 
        {
            if (actor == nullptr || actor->IsDestroyRequested())
                continue;

            if (actor->GetComponent<StatComponent>() == nullptr)
                continue;

            if (actor->GetComponent<StatComponent>()->IsDead())
                continue;

            displayQueue.emplace_back(actor);
        }

        // 높은 순서대로 정렬
        std::sort(displayQueue.begin(), displayQueue.end(), [](Actor* a, Actor* b)
            {
                return a->GetComponent<StatComponent>()->GetTurnCnt() > b->GetComponent<StatComponent>()->GetTurnCnt();
            });

        return displayQueue;
    }
}