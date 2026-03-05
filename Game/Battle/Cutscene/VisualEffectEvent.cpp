#include "VisualEffectEvent.h"
#include "Engine/Engine.h"
#include "Battle/BattleContext.h"

VisualEffectEvent::VisualEffectEvent(Wannabe::Vector2 start, Wannabe::Vector2 end, EffectMovementType eType,
    wchar_t txt, Wannabe::Color color, float fDuration)
    :m_StartPos(start),m_EndPos(end), m_eMoveType(eType), m_Symbol(txt),m_Color(color),m_fDuration(fDuration)
{
}

void VisualEffectEvent::OnStart(Wannabe::BattleContext& context)
{
    m_fElapsed = 0.f;
}

bool VisualEffectEvent::Update(Wannabe::BattleContext& context, float fDeltaTime)
{
    m_fElapsed += fDeltaTime;

    float t = m_fElapsed / m_fDuration; // 0.0 ~ 1.0 진행도
    if (t > 1.0f) 
        t = 1.0f;

    RenderSystem& renderSys = Engine::Get().GetRenderSystem();

    CanvasCell clearCell;
    clearCell.wch = L' ';
    renderSys.GetEffectCanvas().SetCell((int)m_LastPos.x, (int)m_LastPos.y, clearCell);

    m_LastPos = m_CurrentPos;

    // 2. 운동 타입에 따른 새로운 좌표 계산
    if (m_eMoveType == EffectMovementType::Straight)
    {
        // 선형 보간 (Lerp)
        m_CurrentPos.x = m_StartPos.x + (m_EndPos.x - m_StartPos.x) * t;
        m_CurrentPos.y = m_StartPos.y + (m_EndPos.y - m_StartPos.y) * t;
    }
    else if (m_eMoveType == EffectMovementType::Parabola)
    {
        // 포물선: X는 선형, Y는 2차 함수 (위로 솟구쳤다 떨어짐)
        m_CurrentPos.x = m_StartPos.x + (m_EndPos.x - m_StartPos.x) * t;

        float height = 5.0f; // 최대 높이 값
        float jump = 4.0f * height * t * (1.0f - t); // 2차 곡선 공식
        m_CurrentPos.y = (m_StartPos.y + (m_EndPos.y - m_StartPos.y) * t) - jump;
    }

    // 3. 새로운 위치에 그리기
    CanvasCell newCell;
    newCell.wch = m_Symbol;
    newCell.color = m_Color;
    newCell.sortingOrder = 100;
    newCell.bDirty = true;
    renderSys.GetEffectCanvas().SetCell((int)m_CurrentPos.x, (int)m_CurrentPos.y, newCell);

    return m_fElapsed >= m_fDuration;
}

void VisualEffectEvent::OnEnd(Wannabe::BattleContext& context)
{
    CanvasCell cell;
    cell.wch = L' ';
    Engine::Get().GetRenderSystem().GetEffectCanvas().SetCell((int)m_CurrentPos.x, (int)m_CurrentPos.y,cell);
}

bool VisualEffectEvent::IsValid(Wannabe::BattleContext& context) const
{
    return true;
}
