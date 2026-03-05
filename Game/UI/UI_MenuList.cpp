#include <algorithm>

#include "UI_MenuList.h"

#include "Render/RenderSystem.h"
#include "Math/Vector2.h"
#include "Math/Color.h"

UI_MenuList::UI_MenuList()
	:super(L"",Color::White)
{
}

void UI_MenuList::Tick(float fDeltaTime)
{
    if (m_bIsActive == false)
        return;

    super::Tick(fDeltaTime);

    RecalculateViewportPosition();
}

void UI_MenuList::Draw(Wannabe::RenderSystem& renderSys)
{
    if (m_bIsActive == false || m_wstItems.empty())
        return;

    int posX = static_cast<int>(m_cachedViewportPos.x);
    int posY = static_cast<int>(m_cachedViewportPos.y);
    int drawCount = std::min<int>(m_iMaxShow, (int)m_wstItems.size());

    // 테두리
    renderSys.DrawUI(L"+------------------+", Vector2(posX, posY), Color::White, m_SortingOrder);
    for (int i = 1; i <= drawCount; ++i)
    {
        renderSys.DrawUI(L"|                  |", Vector2(posX, posY + i), Color::White, m_SortingOrder);
    }
    renderSys.DrawUI(L"+------------------+", Vector2(posX, posY + drawCount + 1), Color::White, m_SortingOrder);

    // 2. 메뉴 아이템
    for (int i = 0; i < drawCount; i++)
    {
        int realIndex = m_iStartIdx + i;
        if (realIndex >= (int)m_wstItems.size()) break;

        bool selected = (realIndex == m_iCursorIdx);
        int drawY = posY + 1 + i;

        // 화살표 표시
        renderSys.DrawUI(selected ? L"▶" : L"  ", Vector2(posX + 2, drawY),
            selected ? Color::Green : Color::White, m_SortingOrder);
        renderSys.DrawUI(m_wstItems[realIndex], Vector2(posX + 5, drawY),
            selected ? Color::Green : Color::White, m_SortingOrder);
    }
}

void UI_MenuList::RecalculateViewportPosition()
{
    if (!m_pRenderSystem)
        return;

    Vector2 screen = m_pRenderSystem->GetScreenSize();

    const int menuWidth = 18;
    const int menuHeight = 6; 

    Vector2 pos = { screen.x - menuWidth, screen.y - menuHeight };

    pos = pos + m_offset;
    pos.x = std::max(0, std::min(pos.x, screen.x - menuWidth));
    pos.y = std::max(0, std::min(pos.y, screen.y - menuHeight));

    m_cachedViewportPos = pos;
}

void UI_MenuList::Init()
{
}

void UI_MenuList::DefaultMenuItems()
{
    m_wstItems.clear();
    m_wstItems = { L"공격",L"스킬",L"아이템",L"도망" };
}

void UI_MenuList::SetItems(const std::vector<std::wstring>& items)
{
    m_wstItems.clear();

    m_wstItems = items;
    m_iCursorIdx = 0;
    m_iStartIdx = 0; // 아이템 설정 시 스크롤 초기화
}

void UI_MenuList::MoveCursor(int delta)
{
    if (m_wstItems.empty()) return;

    int size = (int)m_wstItems.size();

    // 순환 처리
    m_iCursorIdx += delta;
    if (m_iCursorIdx < 0) m_iCursorIdx = size - 1;
    else if (m_iCursorIdx >= size) m_iCursorIdx = 0;

    // 스크롤 윈도우 조정
    if (m_iCursorIdx < m_iStartIdx)
    {
        m_iStartIdx = m_iCursorIdx;
    }
    else if (m_iCursorIdx >= m_iStartIdx + m_iMaxShow)
    {
        m_iStartIdx = m_iCursorIdx - m_iMaxShow + 1; // 보여주는 범위를 넘어가면 시작점 아래로 내림
    }

    // 스크롤 위치 보정
    if (m_iCursorIdx == size - 1 && size > m_iMaxShow)
        m_iStartIdx = size - m_iMaxShow;
    if (m_iCursorIdx == 0)
        m_iStartIdx = 0;
}

void UI_MenuList::SetCursorIdx(int idx)
{
    m_iCursorIdx = idx;
}

void UI_MenuList::SetOnConfirm(std::function<void(int)> func)
{
    m_OnConfirm = func;
}

void UI_MenuList::SetOnCancel(std::function<void()> func)
{
    m_OnCancel = func;
}