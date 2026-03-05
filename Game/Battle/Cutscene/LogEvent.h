#pragma once
#include <string>

#include "Interface/ICutsceneEvent.h"
#include "Enum/CombatType.h"

class LogEvent : public Wannabe::ICutsceneEvent
{
public:
    LogEvent(const BattleLog& log);

    void OnStart(Wannabe::BattleContext& context) override;
    bool Update(Wannabe::BattleContext& context, float fDeltaTime) override;

private:
    void SetDuration(float duration);
    std::wstring FormatLog(Wannabe::BattleContext& context, const BattleLog& log);
    std::wstring BuildDamageLog(const BattleLog& log);
    std::wstring BuildHealLog(const BattleLog& log);
    std::wstring BuildCriticalLog(const BattleLog& log);
    std::wstring BuildMissLog(const BattleLog& log);
    std::wstring BuildStatusLog(const BattleLog& log);
    std::wstring BuildStatusExpireLog(const BattleLog& log);  
    std::wstring BuildDeathLog(const BattleLog& log);

private:
    BattleLog m_eBattleLog;
    std::wstring m_wstrLog;
    float m_fElapsed = 0.f;
    float m_fDuration = 1.0f;

    bool m_bStarted = false;
    bool m_bFinished = false;
};