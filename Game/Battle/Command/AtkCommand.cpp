#include "AtkCommand.h"

#include "Util/Utill.h"
#include "Battle/BattleContext.h"
#include "Battle/BattleResolver.h"
#include "Battle/BattleEventProcessor.h"
#include "Battle/System/CutScenePlayer.h"
#include "Battle/Cutscene/LogEvent.h"

AtkCommand::AtkCommand(Wannabe::Actor* instigator, Wannabe::Actor* pTarget)
{
    m_pInstigator = instigator;
    m_pTarget = pTarget;
    m_eType = CommandType::Atk;
}

void AtkCommand::Execute(Wannabe::BattleContext& context)
{
    if(context.IsValidActor(m_pInstigator) == false || context.IsValidActor(m_pTarget) == false)
        return;

    Wannabe::BattleResolver& resolver = context.GetResolver();
    ActCheckResult act = resolver.CanAct(m_pInstigator);
    if (act.bCanAct == false)
    {
        BattleLog log;
        log.eLogType = LogType::Free;
        log.wstrTxt = act.wstrReason;
        context.GetCutscenePlayer().Push(std::make_unique<LogEvent>(log));
        return;
    }

    CombatEffectResult skill = resolver.ResolveBasicAtk(m_pInstigator, m_pTarget);
    context.GetEventProcessor().ProcessCombatEffectResult(context, skill);
}