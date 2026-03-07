#include <string>
#include <memory>

#include "DamageEvent.h"

#include "Enum/CombatType.h"

#include "Math/Vector2.h"
#include "Math/Color.h"

#include "Actor/Actor.h"
#include "Battle/BattleContext.h"
#include "Battle/System/CutscenePlayer.h"

#include "Component/StatComponent.h"
#include "Component/DisplayComponent.h"

#include "Render/RenderSystem.h"
#include "Level/BattleLevel.h"
#include "UI/UI_FloatingText.h"

DamageEvent::DamageEvent(Wannabe::Actor* pAtker, Wannabe::Actor* pTarget, int iDmg, bool bCritical /*= 0*/)
	: m_pAtker(pAtker), m_pTarget(pTarget), m_iDmg(iDmg), m_bCritical(bCritical)
{

}

void DamageEvent::OnStart(Wannabe::BattleContext& context)
{
    m_fElapsed = 0.f;

    if (context.IsValidActor(m_pTarget) == false)
        return;

    Wannabe::Vector2 targetPos = m_pTarget->GetPosition();
    std::wstring dmgStr = std::to_wstring(m_iDmg);

    Wannabe::Color textColor = m_bCritical ? Wannabe::Color::Red : Wannabe::Color::White;
    if (m_bCritical) 
        dmgStr = L"CRITICAL! " + dmgStr;

    auto pDmgText = std::make_unique<UI_FloatingText>(dmgStr, m_pTarget->GetPosition(), textColor);

    context.GetBattleLevel().AddActor(std::move(pDmgText));

    // 타격 연출 (Camera Shake)
    RenderSystem& renderSys = Engine::Get().GetRenderSystem();
    if (m_bCritical)
    {
        renderSys.Shake(0.2f, 1.2f); // 0.2초 동안 강도 1.2로 크게 흔들기
    }
    else
    {
        renderSys.Shake(0.1f, 0.6f); // 0.1초 동안 살짝 흔들기
    }
}

bool DamageEvent::Update(Wannabe::BattleContext& context, float fDeltaTime)
{
    m_fElapsed += fDeltaTime;
    return m_fElapsed > 0.2f;
}