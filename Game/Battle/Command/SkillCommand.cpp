#include "SkillCommand.h"
#include "Manager/DataManager.h"
#include "Util/Utill.h"

#include "Battle/System/CutScenePlayer.h"
#include "Battle/Cutscene/LogEvent.h"
#include "Battle/Cutscene/DamageEvent.h"
#include "Battle/Cutscene/DelayEvent.h"
#include "Battle/Cutscene/DeathEvent.h"

#include "Component/DisplayComponent.h"
#include "Level/BattleLevel.h"

SkillCommand::SkillCommand(Wannabe::Actor* instigator, Wannabe::Actor* pTarget, int iSkillTid)
    :m_iSkillTid(iSkillTid)
{
    m_pInstigator = instigator;
    m_pTarget = pTarget;
    m_eType = CommandType::Skill;
}

bool SkillCommand::Execute(Wannabe::BattleContext& context)
{
    if (context.IsValidActor(m_pTarget) == false || context.IsValidActor(m_pInstigator) == false)
        return false;

    auto& resolver = context.GetResolver();
    auto& processor = context.GetEventProcessor();

    const ActionData* skill = DataManager::Get().GetActionData(m_iSkillTid);
    if (skill == nullptr)
        return false;

    ActCheckResult act = resolver.CanAct(m_pInstigator);
    if (act.bCanAct == false)
    {
        BattleLog log;
        log.eLogType = LogType::Free;
        log.wstrTxt = act.wstrReason;
        context.GetCutscenePlayer().Push(std::make_unique<LogEvent>(log));
        return false;
    }

    int targetCnt = skill->iMaxTargetCnt;
    ActionTargetType targetType = skill->targetType;
    // Å¸°Ù ¼±Á¤
    auto targets = context.GetResolver().ResolveTargets(context, m_pInstigator, m_pTarget, targetType, targetCnt);
    for (auto* target : targets)
    {
        auto results = resolver.ResolveAction(m_pInstigator, target, skill->vecEffects, skill->iPower);

        for (const auto& result : results)
            processor.ProcessCombatEffectResult(context, result);
    }

    return true;
}