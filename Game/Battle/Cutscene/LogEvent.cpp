#include <memory>

#include "LogEvent.h"
#include "Actor/Actor.h"
#include "Battle/BattleContext.h"
#include "Battle/System/CutScenePlayer.h"
#include "Component/DisplayComponent.h"
#include "Battle/BattleLogSystem.h"

LogEvent::LogEvent(const BattleLog& log)
    :m_eBattleLog(log)
{
}

void LogEvent::OnStart(Wannabe::BattleContext& context)
{
    if (m_bStarted)
        return;

    auto formatted = FormatLog(context, m_eBattleLog);
    context.GetLogSystem().AddLog(formatted);

    m_bStarted = true;
}

bool LogEvent::Update(Wannabe::BattleContext& context, float fDeltaTime)
{
    m_fElapsed += fDeltaTime;

    return m_fElapsed >= m_fDuration;
}

void LogEvent::SetDuration(float duration)
{
}

std::wstring LogEvent::FormatLog(Wannabe::BattleContext& context, const BattleLog& log)
{
    switch (log.eLogType)
    {
    case LogType::Damage:
        return BuildDamageLog(log);

    case LogType::Heal:
        return BuildHealLog(log);

    case LogType::CriticalDamage:
        return BuildCriticalLog(log);

    case LogType::Miss:
        return BuildMissLog(log);

    case LogType::StatusApply:
        return BuildStatusLog(log);

    case LogType::StatusExpire:
        return BuildStatusExpireLog(log);

    case LogType::Death:
        return BuildDeathLog(log);

    case LogType::TurnStart:
        return L"=== 턴 시작 ===";

    case LogType::TurnEnd:
        return L"=== 턴 종료 ===";

    case LogType::Free:
        return log.wstrTxt;

    default:
        return L"";
    }
}

std::wstring LogEvent::BuildDamageLog(const BattleLog& log)
{
    if (log.wstrAtkerName.empty() == true)
        return L"";

    if (log.wstrTargetName.empty() == true)
        return L"";

    return log.wstrAtkerName + L" → " + log.wstrTargetName + L"에게 " + std::to_wstring(log.iValue) + L" 피해";
}

std::wstring LogEvent::BuildHealLog(const BattleLog& log)
{
    return log.wstrTargetName + L" " + std::to_wstring(log.iValue) + L" 회복";
}

std::wstring LogEvent::BuildCriticalLog(const BattleLog& log)
{
    return L"치명타! " + std::to_wstring(log.iValue);
}

std::wstring LogEvent::BuildMissLog(const BattleLog& log)
{
    if (log.wstrAtkerName.empty() == true)
        return L"";

    return log.wstrAtkerName + L" 공격이 빗나갔다!";
}

std::wstring LogEvent::BuildStatusLog(const BattleLog& log)
{
    return L"상태 이상 적용";
}

std::wstring LogEvent::BuildStatusExpireLog(const BattleLog& log)
{
    return L"상태 이상 종료";
}

std::wstring LogEvent::BuildDeathLog(const BattleLog& log)
{
    if (log.wstrTargetName.empty() == true)
        return L"";

    return log.wstrTargetName + L" 쓰러졌다.";
}