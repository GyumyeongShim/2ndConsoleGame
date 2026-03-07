#include "UI_TurnOrder.h"
#include "Render/RenderSystem.h"

#include "Component/DisplayComponent.h"

UI_TurnOrder::UI_TurnOrder(Wannabe::TurnManager* pTurnMgr, Wannabe::Vector2 pos)
    :super(L"", Color::White), m_pTurnMgr(pTurnMgr)
{
}

void UI_TurnOrder::Tick(float fDeltaTime)
{
    super::Tick(fDeltaTime);
    
    RefreshTurnOrder();

    RecalculateViewportPosition();
}

void UI_TurnOrder::Draw(Wannabe::RenderSystem& renderSys)
{   
    Wannabe::Vector2 pos = m_cachedViewportPos;

    // 오른쪽 정렬 보정
    for (int i = 0; i < m_lines.size(); ++i)
    {
        renderSys.DrawUI(m_lines[i], Wannabe::Vector2(pos.x, pos.y + i),m_lineColors[i],m_SortingOrder);
    }
}

void UI_TurnOrder::RecalculateViewportPosition()
{
    if (m_pRenderSystem == nullptr)
        return;

    Wannabe::Vector2 screen = m_pRenderSystem->GetScreenSize();

    int marginRight = 6;
    int marginTop = 1;

    m_cachedViewportPos = Wannabe::Vector2(screen.x - marginRight,marginTop);
}

void UI_TurnOrder::RefreshTurnOrder()
{
    UpdateTurnText();
}

void UI_TurnOrder::Init()
{
    SetAnchor(UIAnchor::TopRight);
    m_SortingOrder = 101;
    SetRelative({ 0, 0 });
    SetOffset({ -6, 1 });
    RefreshTurnOrder();
}

void UI_TurnOrder::UpdateTurnText()
{
    m_lines.clear();
    m_lineColors.clear();

    if (m_pTurnMgr == nullptr)
        return;

    int visibleCount = 0;
    std::vector<Wannabe::Actor*> que = m_pTurnMgr->GetTurnQueue();
    const Wannabe::Actor* current = m_pTurnMgr->GetCurBattleActor();
    for (const auto* actor : que)
    {
        if (actor == nullptr)
            continue;

        if (actor->IsActive() == false)
            continue;

        if (actor->IsDestroyRequested() == true)
            continue;

        if (visibleCount >= m_maxLine)
            break;

        // 현재 턴 액터 강조
        std::wstring name = actor->GetComponent<DisplayComponent>()->GetOriginName();
        if (actor == current)
        {
            name = L"▶" + name;
            m_lineColors.emplace_back(Wannabe::Color::Yellow);
        }
        else
            m_lineColors.emplace_back(Wannabe::Color::White);

        m_lines.emplace_back(name);

        visibleCount++;
    }
}
