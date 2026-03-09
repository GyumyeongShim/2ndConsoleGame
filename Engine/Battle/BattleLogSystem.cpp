#include "BattleLogSystem.h"

#include "Enum/CombatType.h"
#include "Actor/Actor.h"
#include "Component/DisplayComponent.h"

void BattleLogSystem::AddLog(const std::wstring& log)
{
    if (log.empty() == true)
        return;

    m_dqLog.emplace_back(log);
    while (m_dqLog.size() > m_maxLog)
        m_dqLog.pop_front();
}

void BattleLogSystem::ClearLog()
{
    m_dqLog.clear();
}

bool BattleLogSystem::IsEmptyLog()
{
    return m_dqLog.empty();
}

std::wstring BattleLogSystem::PopFrontLog()
{
    if (m_dqLog.empty())
        return L"";

    std::wstring frontLog = m_dqLog.front();
    m_dqLog.pop_front();
    return frontLog;
}

const std::deque<std::wstring>& BattleLogSystem::GetLog() const
{
    return m_dqLog;
}

std::wstring BattleLogSystem::GetLatestLog() const
{
    if (m_dqLog.empty())
        return L"";

    return m_dqLog.back();
}