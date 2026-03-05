#include <iostream>
#include <vector>
#include <algorithm>

#include "TurnManager.h"
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
            if (actor->IsDestroyRequested() == true)
                continue;

            if (actor->GetStat()->IsDead() == true)
                continue;

            if (actor->GetStat()->IsTurnMax())
            {
                // 단순히 찼는지 확인하는 게 아니라, 누가 더 많이 찼는지(Overcharge) 비교
                float fTurnCnt = actor->GetStat()->GetTurnCnt();
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
            if (actor->GetStat() == nullptr || actor->GetStat()->IsDead())
                continue;

            // 상태이상: Stun이면 턴 스킵
            if (actor->GetStatus() && actor->GetStatus()->HasStatus(StatusType::Stun))
            {
                // 스턴이면 턴 리셋 대신 진행만
                actor->GetStat()->ResetTurn();
                continue;
            }

            // 턴 카운트 증가
            actor->GetStat()->ProgressTurn();
        }
    }

    void TurnManager::TurnEnd()
    {
        if (m_CurActor == nullptr)
            return;

        if (m_CurActor->GetStat())
            m_CurActor->GetStat()->ResetTurn();

        m_CurActor = nullptr;
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
            if (actor == nullptr)
                continue;
            
            if (actor->IsDestroyRequested())
                continue;
            
            if (actor->GetStat()->IsDead())
                continue;

            displayQueue.push_back(actor);
        }

        // 높은 순서대로 정렬
        std::sort(displayQueue.begin(), displayQueue.end(), [](Actor* a, Actor* b)
            {
                return a->GetStat()->GetTurnCnt() > b->GetStat()->GetTurnCnt();
            });

        return displayQueue;
    }
}