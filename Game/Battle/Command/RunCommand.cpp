#include "RunCommand.h"

#include "Battle/BattleContext.h"
#include "Battle/BattleResolver.h"
#include "Battle/BattleEventProcessor.h"
#include "Battle/System/CutScenePlayer.h"
#include "Battle/Cutscene/LogEvent.h"

RunCommand::RunCommand(Wannabe::Actor* self)
{
}

void RunCommand::Execute(Wannabe::BattleContext& context)
{
    if (!context.IsValidActor(m_pInstigator)) 
        return;
    // 1. 도망 성공 확률 계산 (예: 아군 평균 속도 vs 적군 평균 속도)
    bool bSuccess = context.GetResolver().CalcRunSucess(context);

    BattleLog log;
    if (bSuccess)
    {
        log.eLogType = LogType::Free;
        log.wstrTxt = L"성공적으로 도망쳤습니다!";
        context.GetCutscenePlayer().Push(std::make_unique<LogEvent>(log));

        // 전투 종료 상태로 변경
        context.SetBattleState(BattleState::Result);
    }
    else
    {
        log.wstrTxt = L"도망에 실패했습니다!";
        context.GetCutscenePlayer().Push(std::make_unique<LogEvent>(log));
    }
}