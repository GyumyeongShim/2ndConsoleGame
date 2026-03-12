#include "UI_NameInput.h"

#include <algorithm>

#include "Engine/Engine.h"
#include "Core/Input.h"
#include "Render/RenderSystem.h"

UI_NameInput::UI_NameInput()
    : super(L"", Color::White)
{
    m_vecChars = 
    {
            L"A", L"B", L"C", L"D", L"E", L"F", L"G",
            L"H", L"I", L"J", L"K", L"L", L"M", L"N",
            L"O", L"P", L"Q", L"R", L"S", L"T", L"U",
            L"V", L"W", L"X", L"Y", L"Z", L"1", L"2",
            L"3", L"4", L"5", L"6", L"7", L"8", L"9",
            L"0", L"_", L" ", L"BS", L"OK"
    };
}

void UI_NameInput::Tick(float fDeltaTime)
{
    if (!m_bIsActive) 
        return;

    super::Tick(fDeltaTime);

    RecalculateViewportPosition();

    // 그리드 이동 처리
    if (Wannabe::Input::Get().GetKeyDown(VK_LEFT)) 
        MoveCursor(-1, 0);
    if (Wannabe::Input::Get().GetKeyDown(VK_RIGHT)) 
        MoveCursor(1, 0);
    if (Wannabe::Input::Get().GetKeyDown(VK_UP))    
        MoveCursor(0, -1);
    if (Wannabe::Input::Get().GetKeyDown(VK_DOWN))  
        MoveCursor(0, 1);

    if (Wannabe::Input::Get().GetKeyDown(VK_RETURN))
        OnConfirmAction();
}

void UI_NameInput::Draw(Wannabe::RenderSystem& renderSys)
{
    if (!m_bIsActive) 
        return;

    int posX = m_cachedViewportPos.x;
    int posY = m_cachedViewportPos.y;

    // 1. 현재 입력된 이름 표시 (상단 가이드)
    renderSys.DrawUI(L" [ ENTER YOUR NAME ] ", Vector2(posX, posY - 2), Color::Yellow, m_SortingOrder);
    renderSys.DrawUI(L"> " + m_wstrCurrentName + L"_", Vector2(posX, posY), Color::Green, m_SortingOrder);

    for (int i = 0; i < (int)m_vecChars.size(); ++i)
    {
        int row = i / m_iColumnCount;
        int col = i % m_iColumnCount;

        int drawX = posX + (col * m_iCellWidth);
        int drawY = posY + 3 + row;

        bool isSelected = (i == m_iCursorIdx);
        Color color = isSelected ? Color::Purple : Color::White;

        // 선택된 글자는 대괄호나 화살표로 강조
        std::wstring displayChar = isSelected ? L"[" + m_vecChars[i] + L"]" : L" " + m_vecChars[i] + L" ";
        renderSys.DrawUI(displayChar, Vector2(drawX, drawY), color, m_SortingOrder);
    }
}

void UI_NameInput::RecalculateViewportPosition()
{
    auto screen = Wannabe::Engine::Get().GetRenderSystem().GetScreenSize();

    m_cachedViewportPos.x = (screen.x / 2) - ((m_iColumnCount * m_iCellWidth) / 2);
    m_cachedViewportPos.y = (screen.y / 2) - 5;
}

void UI_NameInput::MoveCursor(int iX, int iY)
{
    int totalSize = (int)m_vecChars.size();
    int curRow = m_iCursorIdx / m_iColumnCount;
    int curCol = m_iCursorIdx % m_iColumnCount;
    int totalRows = (totalSize + m_iColumnCount - 1) / m_iColumnCount;

    if (iX != 0)
    {
        curCol = (curCol + iX + m_iColumnCount) % m_iColumnCount;
    }

    if (iY != 0)
    {
        curRow = (curRow + iY + totalRows) % totalRows;
    }

    int nextIdx = curRow * m_iColumnCount + curCol;
    // 마지막 줄에 글자가 꽉 차지 않았을 경우 처리
    if (nextIdx >= totalSize)
        nextIdx = totalSize - 1;

    m_iCursorIdx = nextIdx;
}

void UI_NameInput::OnConfirmAction()
{
    const std::wstring& selected = m_vecChars[m_iCursorIdx];

    if (selected == L"OK")
    {
        if (m_OnConfirm)
        {
            m_OnConfirm(m_wstrCurrentName);
        }
        m_bIsActive = false;
    }
    else if (selected == L"BS")
    {
        if (!m_wstrCurrentName.empty())
        {
            m_wstrCurrentName.pop_back();
        }
    }
    else
    {
        if (m_wstrCurrentName.length() < (size_t)m_iMaxNameLength)
        {
            m_wstrCurrentName += selected;
        }
    }
}
