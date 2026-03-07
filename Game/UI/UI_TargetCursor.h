#pragma once
#include <vector>

#include "UI/UI.h"

class BattleLevel;

class UI_TargetCursor : public Wannabe::UI
{
    RTTI_DECLARATIONS(UI_TargetCursor, UI)

public:
    UI_TargetCursor(BattleLevel* level);

    virtual void Tick(float fDeltaTime) override;
    virtual void Draw(Wannabe::RenderSystem& renderSys) override;
    virtual void SetRenderSystem(Wannabe::RenderSystem* renderSystem) { m_pRenderSystem = renderSystem; }

    void Init();
    void ResetIdx();
    void SetActive(bool bIsActive);

    void SetCursorIdx(int idx);
    int GetCursorIdx() { return m_iCursorIdx; }

    void SetAllTargetMode(bool bAll) { m_bAllTarget = bAll; }

    void SetTargets(const std::vector<Actor*>& vecTargets);
    Actor* GetSelectedTarget();

private:
    BattleLevel* m_pLevel = nullptr;
    std::vector<Actor*> m_vecTarget;
    bool m_bAllTarget = false;
    bool m_bIsActive = false;
    int m_iCursorIdx = 0;
    int m_iTargetCnt = 4;
};