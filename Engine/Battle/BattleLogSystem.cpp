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

void BattleLogSystem::AddBattleLog(const BattleLog& log)
{
    std::wstring result;

    switch (log.eLogType)
    {
    case LogType::Damage:
        if (log.wstrAtkerName.empty() == false && log.wstrTargetName.empty() == false)
            result = log.wstrAtkerName + L" → " + log.wstrTargetName + L"에게 " + std::to_wstring(log.iValue) + L" 피해";
        break;

    case LogType::Heal:
        if (log.wstrTargetName.empty() == false)
            result = log.wstrTargetName + L" " + std::to_wstring(log.iValue) + L" 회복";
        break;

    case LogType::CriticalDamage:
        result = L"치명타! " + std::to_wstring(log.iValue);
        break;

    case LogType::Miss:
        if (log.wstrAtkerName.empty() == false)
        result = log.wstrAtkerName + L" 공격이 빗나갔다!";
        break;

    case LogType::Death:
        if (log.wstrTargetName.empty() == false)
        result = log.wstrTargetName + L" 쓰러졌다.";
        break;

    default:
        break;
    }

    if (!result.empty())
        AddLog(result);
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