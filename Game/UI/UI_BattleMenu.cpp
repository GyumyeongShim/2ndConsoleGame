#include "UI_BattleMenu.h"
#include "UI/UI.h"
#include "Render/RenderSystem.h"

using namespace Wannabe;

UI_BattleMenu::UI_BattleMenu()
    :super(L"", Color::White)
{
}

void UI_BattleMenu::Tick(float fDeltaTime)
{
    super::Tick(fDeltaTime);

    // 매 프레임 위치 갱신
    RecalculateViewportPosition();
}

void UI_BattleMenu::Draw(Wannabe::RenderSystem& renderSys)
{
    if (m_bIsActive == false)
        return;

    Vector2 pos = m_cachedViewportPos;

    // 테두리
    renderSys.DrawUI(L"+------------------+", Vector2(pos.x, pos.y), Color::White, m_SortingOrder);
    for (int i = 1; i <= 4; ++i)
    {
        renderSys.DrawUI(L"|                  |", Vector2(pos.x, pos.y + i), Color::White, m_SortingOrder);
    }
    renderSys.DrawUI(L"+------------------+", Vector2(pos.x, pos.y + 5), Color::White, m_SortingOrder);

    const int cursorX = pos.x + 1;
    const int textX = pos.x + 3;

    // 메뉴 아이템 표시
    for (int i = 0; i < m_iMenuCnt; ++i)
    {
        int y = pos.y + 1 + i;
        bool bSelected = (i == m_iCursorIdx);
        renderSys.DrawUI(bSelected ? L"▶" : L" ",
            Vector2(cursorX + 1, y),
            bSelected ? Color::Green : Color::White,
            m_SortingOrder);

        // 오른쪽 화면 범위 체크
        std::wstring item = m_wstItems[i];
        Vector2 screen = renderSys.GetScreenSize();
        if (textX + item.length() >= screen.x)
            item = item.substr(0, screen.x - textX - 1) + L"...";

        renderSys.DrawUI(item, Vector2(textX + 3, y),bSelected ? Color::Green : Color::White, m_SortingOrder
        );
    }
}
  
void UI_BattleMenu::RecalculateViewportPosition()
{
    Vector2 screen = m_pRenderSystem->GetScreenSize();

    const int menuWidth = 18;
    const int menuHeight = 6;

    // BottomRight 기준
    Vector2 pos = { screen.x - menuWidth, screen.y - menuHeight };

    // offset 적용
    pos = pos + m_offset;

    // 화면 범위 clamp
    if (pos.x < 0) pos.x = 0;
    if (pos.y < 0) pos.y = 0;
    if (pos.x > screen.x - menuWidth) pos.x = screen.x - menuWidth;
    if (pos.y > screen.y - menuHeight) pos.y = screen.y - menuHeight;

    m_cachedViewportPos = pos;
}

void UI_BattleMenu::Init()
{
    m_SortingOrder = 101;

    SetAnchor(UIAnchor::BottomRight);
    SetOffset({ 0, 0 });

    RecalculateViewportPosition();
}

void UI_BattleMenu::SetCursorIdx(int idx)
{
    m_iCursorIdx += idx;

    if (m_iCursorIdx < 0)
        m_iCursorIdx = m_iMenuCnt-1;

    if (m_iCursorIdx > m_iMenuCnt-1)
        m_iCursorIdx = 0;
}
