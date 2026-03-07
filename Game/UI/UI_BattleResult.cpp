#include "UI_BattleResult.h"

#include "Math/Color.h"
#include "Render/RenderSystem.h"

UI_BattleResult::UI_BattleResult(int iGold, int iExp, char rank)
    :m_iTargetGold(iGold), m_iTargetExp(iExp), m_rank(rank)
{
}

void UI_BattleResult::Tick(float fDeltaTime)
{
    m_fTimer += fDeltaTime;

    switch (m_eDisplayState)
    {
    case DisplayState::Background:
        if (m_fTimer > 0.5f) 
            m_eDisplayState = DisplayState::Stats;
        break;

    case DisplayState::Stats: // 숫자가 올라가는 연출
        if (m_iCurGold < m_iTargetGold) 
            m_iCurGold += 5; // 속도 조절

        if (m_iCurExp < m_iTargetExp) 
            m_iCurExp += 3;

        if (m_iCurGold >= m_iTargetGold && m_iCurExp >= m_iTargetExp)
        {
            m_iCurGold = m_iTargetGold;
            m_iCurExp = m_iTargetExp;

            if (m_fTimer > 1.5f) 
                m_eDisplayState = DisplayState::Rank;
        }
        break;

    case DisplayState::Rank:
        if (m_fTimer > 2.5f) 
            m_eDisplayState = DisplayState::Finished;
        break;

    case DisplayState::Finished:
        break;
    }
}

void UI_BattleResult::Draw(Wannabe::RenderSystem& renderSys)
{
    Vector2 sz = renderSys.GetScreenSize();
    int cx = (int)sz.x / 2;
    int cy = (int)sz.y / 2;

    // 1. 반투명 배경 느낌 (어두운 영역 그리기)
    for (int y = cy - 5; y <= cy + 7; ++y)
    {
        renderSys.DrawUI(L"####################################", Vector2(cx - 18,y), Color::White, 90);
    }

    // 2. 테두리 및 제목
    renderSys.DrawUI(L"┏━━━━━━━━━━━━  VICTORY  ━━━━━━━━━━━━┓", Vector2(cx - 18, cy - 5), Color::RightMagenta);

    if (m_eDisplayState >= DisplayState::Stats)
    {
        renderSys.DrawUI(L"  획득 금화 : " + std::to_wstring(m_iCurGold), Vector2(cx - 14, cy - 1), Color::Yellow);
        renderSys.DrawUI(L"  획득 경험치  : " + std::to_wstring(m_iCurExp), Vector2(cx - 14, cy + 1), Color::Cyan);
    }

    if (m_eDisplayState >= DisplayState::Rank)
    {
        std::wstring rankStr = L" BATTLE RANK : [ " + std::wstring(1, m_rank) + L" ] ";
        renderSys.DrawUI(rankStr, Vector2(cx - 14, cy + 3), Color::Green);
    }

    if (m_eDisplayState == DisplayState::Finished)
    {
        // 깜빡임 연출 (Timer 활용)
        if ((int)(m_fTimer * 2) % 2 == 0)
        {
            renderSys.DrawUI(L"- Press [Enter] to Continue -", Vector2(cx - 14, cy + 6), Color::White);
        }
    }
}

void UI_BattleResult::RecalculateViewportPosition()
{
}
