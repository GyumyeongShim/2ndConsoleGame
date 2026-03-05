#include <memory>
#include "DamageEvent.h"

#include "Actor/Actor.h"
#include "Battle/BattleContext.h"
#include "Battle/BattleEventProcessor.h"
#include "Battle/BattleLogSystem.h"

#include "Battle/System/CutscenePlayer.h"

#include "Battle/Cutscene/DeathEvent.h"

#include "Component/StatComponent.h"
#include "Component/DisplayComponent.h"

DamageEvent::DamageEvent(Wannabe::Actor* pAtker, Wannabe::Actor* pTarget, int iDmg, bool bCritical /*= 0*/)
	: m_pAtker(pAtker), m_pTarget(pTarget), m_iDmg(iDmg), m_bCritical(bCritical)
{

}

void DamageEvent::OnStart(Wannabe::BattleContext& context)
{
    m_fElapsed = 0.f;

    if (context.IsValidActor(m_pTarget) == false)
        return;

    BattleLog log;
    log.wstrAtkerName = m_pAtker->GetDisplay()->GetOriginName();
    log.wstrTargetName = m_pTarget->GetDisplay()->GetOriginName();
    log.eLogType = LogType::Damage;
    log.iValue = m_iDmg;
    log.bCritical = m_bCritical;
    log.bMiss = false;
    context.GetLogSystem().AddBattleLog(log);
}

bool DamageEvent::Update(Wannabe::BattleContext& context, float fDeltaTime)
{
    m_fElapsed += fDeltaTime;
    return m_fElapsed > 0.2f;
}