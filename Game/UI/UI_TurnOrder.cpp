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
    int currentX = pos.x;
    for (size_t i = 0; i < m_lines.size(); ++i)
    {
        // 1. 이름 출력
        renderSys.DrawUI(m_lines[i], Wannabe::Vector2(currentX, pos.y), m_lineColors[i], m_SortingOrder);
        currentX += static_cast<int>(m_lines[i].length());

        // 2. 화살표 출력 (마지막 요소가 아닐 때만)
        if (i < m_lines.size() - 1)
        {
            std::wstring arrow = L" → ";
            renderSys.DrawUI(arrow, Wannabe::Vector2(currentX, pos.y), Wannabe::Color::White, m_SortingOrder);
            currentX += static_cast<int>(arrow.length());
        }
    }
}

void UI_TurnOrder::RecalculateViewportPosition()
{
    if (m_pRenderSystem == nullptr)
        return;

    m_cachedViewportPos = Wannabe::Vector2(2, 1);
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
            m_lineColors.emplace_back(Wannabe::Color::Pink);
        }
        else
            m_lineColors.emplace_back(Wannabe::Color::White);

        m_lines.emplace_back(name);

        visibleCount++;
    }
}
