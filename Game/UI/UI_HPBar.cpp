#include<algorithm>

#include "UI_HPBar.h"
#include "Engine/Engine.h"
#include "Render/RenderSystem.h"
#include "Math/Color.h"
#include "UI/UI.h"
#include "Actor/BattleActor.h"

#include "Component/StatComponent.h"

using namespace Wannabe;

UI_HPBar::UI_HPBar(BattleActor* owner, int idx, Team eTeam)
    :super(L"", Color::White), m_pOwner(owner), m_iIdx(idx), m_eWhatTeam(eTeam)
{
}

void UI_HPBar::Tick(float fDeltaTime)
{
    if (m_pOwner == nullptr)
        return;

    if (m_pOwner->IsDestroyRequested())
    {
        Destroy();
        return;
    }

    RecalculateViewportPosition();

    super::Tick(fDeltaTime);
}

void UI_HPBar::Draw(Wannabe::RenderSystem& renderSys)
{
    if (m_pOwner == nullptr || m_pOwner->IsDestroyRequested())
        return;

    if (m_pOwner->GetComponent<StatComponent>() == nullptr)
        return;

    if (m_cachedViewportPos.y < 0)
        return;

    m_iCurHp = m_pOwner->GetComponent<StatComponent>()->GetHp();
    m_iMaxHp = m_pOwner->GetComponent<StatComponent>()->GetMaxHp();

    // 0 방지
    if (m_iMaxHp <= 0)
        m_iMaxHp = 1;

    // 퍼센트 계산
    float hpPercent = static_cast<float>(m_iCurHp) / m_iMaxHp;

    // 바 길이 계산
    int ratio = static_cast<int>(hpPercent * m_iBarWidth);
    if (ratio < 0)
        ratio = 0;
    else if (ratio > m_iBarWidth)
        ratio = m_iBarWidth;

    // HP 표기
    std::wstring bar;
    bar += L"[";
    for (int i = 0; i < m_iBarWidth; ++i)
        bar += (i < ratio) ? L"■" : L" ";
    bar += L"] ";

    bar += std::to_wstring(m_iCurHp);
    bar += L"/";
    bar += std::to_wstring(m_iMaxHp);

    // 색상 변화
    if (hpPercent > 0.6f) 
        m_Color = Color::Green;
    else if (hpPercent > 0.3f) 
        m_Color = Color::Yellow;
    else 
        m_Color = Color::Red;

    renderSys.DrawUI(bar.c_str(), m_cachedViewportPos, m_Color, m_SortingOrder);
}

void UI_HPBar::RecalculateViewportPosition()
{
    if (m_pOwner == nullptr || m_pRenderSystem == nullptr)
        return;

    Vector2 actorPos = m_pOwner->GetBattleScreenPosition();

    m_cachedViewportPos = actorPos + Vector2(0, -2);
}

void UI_HPBar::Init()
{
    if (m_pOwner == nullptr)
        return;

    m_SortingOrder = 102;
    m_pRenderSystem = &Engine::Get().GetRenderSystem();

    super::UpdateViewportTransform();
}