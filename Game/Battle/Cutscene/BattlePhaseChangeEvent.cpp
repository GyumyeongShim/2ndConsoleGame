#include <memory>

#include "BattlePhaseChangeEvent.h"

#include "Battle/BattleContext.h"
#include "Battle/Cutscene/LogEvent.h"
#include "Battle/System/CutScenePlayer.h"
#include "Level/BattleLevel.h"

BattlePhaseChangeEvent::BattlePhaseChangeEvent(BattleState nextState)
    :m_eNextState(nextState)
{
}

void BattlePhaseChangeEvent::OnStart(Wannabe::BattleContext& context)
{
    auto& cutscene = context.GetCutscenePlayer();
    BattleLog systemLog;
    systemLog.eLogType = LogType::Free;
    systemLog.wstrTxt = L"=== Phase Change ===";
    cutscene.Push(std::make_unique<LogEvent>(systemLog));
   
    BattleLog log;
    log.eLogType = LogType::Free;
    switch (m_eNextState)
    {
    case BattleState::CommandSelect:
        log.wstrTxt = L"행동을 선택하세요.";
        break;

    case BattleState::TurnCheck:
        log.wstrTxt = L"턴 체크 진행...";
        break;

    case BattleState::Animation:
        log.wstrTxt = L"전투 연출 중...";
        break;

    default:
        break;
    }

    cutscene.Push(std::make_unique<LogEvent>(log));
}

bool BattlePhaseChangeEvent::Update(Wannabe::BattleContext& context, float fDeltaTime)
{
    //m_Elapsed += fDeltaTime;

    //// 콘솔이므로 짧게 유지
    //return m_Elapsed > 0.3f;
    return true;
}

void BattlePhaseChangeEvent::OnEnd(Wannabe::BattleContext& context)
{
    context.SetBattleState(m_eNextState);
}

bool BattlePhaseChangeEvent::IsValid(Wannabe::BattleContext& context) const
{
    return true;
}
