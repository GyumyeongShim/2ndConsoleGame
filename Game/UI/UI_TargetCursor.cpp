#include "UI_TargetCursor.h"
#include "Level/BattleLevel.h"
#include "Actor/BattleActor.h"

UI_TargetCursor::UI_TargetCursor(BattleLevel* level)
    : super(L"", Color::White)
{
    m_pLevel = level;
}

void UI_TargetCursor::Tick(float fDeltaTime)
{
    if (m_bIsActive == false || m_pLevel == nullptr)
        return;

    for (int i = 0; i < m_vecTarget.size(); ++i)
    {
        if (m_vecTarget[i] == nullptr)
            continue;

        dynamic_cast<BattleActor*>(m_vecTarget[i])->SetTargeted(i == m_iCursorIdx);
    }
}

void UI_TargetCursor::Draw(Wannabe::RenderSystem& renderSys)
{
    if (m_bIsActive == false)
        return;

    if (m_iCursorIdx < 0 || m_iCursorIdx >= m_vecTarget.size())
        return;

    auto* actor = dynamic_cast<BattleActor*>(m_vecTarget[m_iCursorIdx]);
    if (actor == nullptr)
        return;

    Vector2 screenPos = actor->GetBattleScreenPosition();

    CanvasCell cell;
    cell.wch = L'>';
    cell.color = Color::Yellow;

    renderSys.GetUICanvas().SetCell(screenPos.x - 2, screenPos.y, cell);
}

void UI_TargetCursor::Init()
{
    SetAnchor(UIAnchor::TopLeft);
    SetOffset({ 10,2 });
}

void UI_TargetCursor::ResetIdx()
{
    m_iCursorIdx = 0;
}

void UI_TargetCursor::SetActive(bool bIsActive)
{
    if (m_bIsActive  == true && !bIsActive) //보여줬다가 안보여지게 바꿀때
    {
        for (auto* actor : m_vecTarget)
        {
            if (actor)
            {
                auto* battleActor = dynamic_cast<BattleActor*>(actor);
                if (battleActor) battleActor->SetTargeted(false);
            }
        }
    }
    m_bIsActive = bIsActive;
}

void UI_TargetCursor::SetCursorIdx(int idx)
{
    if (m_vecTarget.empty())
        return;

    m_iCursorIdx += idx;

    int maxCnt = static_cast<int>(m_vecTarget.size());
    if (m_iCursorIdx < 0)
        m_iCursorIdx = maxCnt - 1;

    if (m_iCursorIdx > maxCnt-1)
        m_iCursorIdx = 0;
}

void UI_TargetCursor::SetTargets(const std::vector<Actor*>& vecTargets)
{
    m_vecTarget = vecTargets;
    m_iCursorIdx = 0;
}

Actor* UI_TargetCursor::GetSelectedTarget()
{
    if (m_iCursorIdx < 0 || m_iCursorIdx >= m_vecTarget.size())
        return nullptr;

    return m_vecTarget[m_iCursorIdx];
}
