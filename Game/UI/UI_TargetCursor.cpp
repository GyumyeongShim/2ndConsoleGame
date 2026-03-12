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

        auto* battleActor = dynamic_cast<BattleActor*>(m_vecTarget[i]);
        if (battleActor == nullptr)
            continue;

        bool bIsTargeted = m_bAllTarget ? true : (i == m_iCursorIdx);
        battleActor->SetTargeted(bIsTargeted);
    }
}

void UI_TargetCursor::Draw(Wannabe::RenderSystem& renderSys)
{
    if (m_bIsActive == false || m_vecTarget.empty())
        return;

    if (m_iCursorIdx < 0 || m_iCursorIdx >= m_vecTarget.size())
        return;

    CanvasCell cell;
    cell.wch = L'▼';
    cell.color = Color::BrightGold;
    cell.sortingOrder = 103;

    if (m_bAllTarget == true)
    {
        // 광역 모드: 모든 타겟 옆에 커서 표시
        for (auto* actor : m_vecTarget)
        {
            auto* bActor = dynamic_cast<BattleActor*>(actor);
            if (bActor == nullptr || bActor->IsDestroyRequested())
                continue;

            Wannabe::Vector2 screenPos = bActor->GetBattleScreenPosition();
            renderSys.GetUICanvas().SetCell(screenPos.x + 2, screenPos.y - 3, cell);
        }
    }
    else
    {
        // 단일 모드
        if (m_iCursorIdx < 0 || m_iCursorIdx >= (int)m_vecTarget.size())
            return;

        auto* actor = dynamic_cast<BattleActor*>(m_vecTarget[m_iCursorIdx]);
        if (actor == nullptr)
            return;

        Wannabe::Vector2 screenPos = actor->GetBattleScreenPosition();
        renderSys.GetUICanvas().SetCell(screenPos.x + 2, screenPos.y - 3, cell);
    }
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
