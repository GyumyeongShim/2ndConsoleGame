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

    // 1. 테두리 그리기
    std::wstring horizontalLine = L"+" + std::wstring(m_iMenuWidth - 2, L'-') + L"+";
    std::wstring emptyLine = L"|" + std::wstring(m_iMenuWidth - 2, L' ') + L"|";

    renderSys.DrawUI(horizontalLine, Vector2(posX, posY), Color::White, m_SortingOrder);

    // 스크롤 상단 화살표 (위쪽 아이템이 더 있을 때)
    if (m_iStartIdx > 0)
    {
        renderSys.DrawUI(L" ▲ ", Vector2(posX + (m_iMenuWidth / 2) - 1, posY), Color::Yellow, m_SortingOrder + 1);
    }

    for (int i = 1; i <= drawCount; ++i)
    {
        renderSys.DrawUI(emptyLine, Vector2(posX, posY + i), Color::White, m_SortingOrder);
    }

    renderSys.DrawUI(horizontalLine, Vector2(posX, posY + drawCount + 1), Color::White, m_SortingOrder);

    // 스크롤 하단 화살표 (아래쪽 아이템이 더 있을 때)
    if (m_iStartIdx + m_iMaxShow < (int)m_wstItems.size())
    {
        renderSys.DrawUI(L" ▼ ", Vector2(posX + (m_iMenuWidth / 2) - 1, posY + drawCount + 1),
            Color::Yellow, m_SortingOrder + 1);
    }

    // 2. 아이템 출력 (Padding 반영)
    for (int i = 0; i < drawCount; i++)
    {
        int realIndex = m_iStartIdx + i;
        if (realIndex >= (int)m_wstItems.size()) break;

        bool selected = (realIndex == m_iCursorIdx);
        renderSys.DrawUI(selected ? L"▶" : L"  ", Vector2(posX + m_iPaddingX, posY + m_iPaddingY + i),
            selected ? Color::Green : Color::White, m_SortingOrder);

        renderSys.DrawUI(m_wstItems[realIndex], Vector2(posX + m_iPaddingX + 3, posY + m_iPaddingY + i),
            selected ? Color::Green : Color::White, m_SortingOrder);
    }
}

void UI_MenuList::RecalculateViewportPosition()
{
    if (!m_pRenderSystem)
        return;

    Vector2 screen = m_pRenderSystem->GetScreenSize();
    int currentDrawCount = std::min<int>(m_iMaxShow, (int)m_wstItems.size());
    int menuHeight = currentDrawCount + 2;

    Vector2 basePos = { 0, 0 };

    switch (m_anchor)
    {
    case UIAnchor::Center:
        basePos.x = (screen.x / 2) - (m_iMenuWidth / 2);
        basePos.y = (screen.y / 2) - (menuHeight / 2);
        break;

    case UIAnchor::BottomRight:
        basePos.x = screen.x - m_iMenuWidth;
        basePos.y = screen.y - menuHeight;
        break;

    case UIAnchor::TopLeft:
        basePos = { 0, 0 };
        break;
    }

    Vector2 relativePos =
    {
        static_cast<int>(screen.x * m_relative.x),
        static_cast<int>(screen.y * m_relative.y)
    };

    m_cachedViewportPos = basePos + relativePos + m_offset;
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
    if (m_wstItems.empty()) 
        return;

    int size = (int)m_wstItems.size();

    // 순환 처리
    m_iCursorIdx += delta;
    if (m_iCursorIdx < 0) 
        m_iCursorIdx = size - 1;
    else if (m_iCursorIdx >= size) 
        m_iCursorIdx = 0;

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
    if (m_iCursorIdx == 0)
    {
        m_iStartIdx = 0;
    }
    else if (m_iCursorIdx == size - 1 && size > m_iMaxShow)
    {
        m_iStartIdx = size - m_iMaxShow;
    }
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