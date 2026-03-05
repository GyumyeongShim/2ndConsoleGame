#pragma once
#include "UI/UI.h"
#include "Util/Utill.h"

class BattleActor;
class UI_HPBar : public Wannabe::UI
{
    RTTI_DECLARATIONS(UI_HPBar, UI)

public:
    UI_HPBar(BattleActor* owner, int idx, Team eTeam);
    virtual ~UI_HPBar() = default;

    virtual void Tick(float fDeltaTime) override;
    virtual void Draw(Wannabe::RenderSystem& renderSys) override;
    virtual void RecalculateViewportPosition() override;
    virtual void SetRenderSystem(Wannabe::RenderSystem* renderSystem) { m_pRenderSystem = renderSystem; }

    void Init();
    BattleActor* GetBattleActorOwner() { return m_pOwner; }

private:
    BattleActor* m_pOwner = nullptr;
    int m_iBarWidth = 5; // HP πŸ ±Ê¿Ã
    int m_iMaxHp = 1;
    int m_iCurHp = 0;
    int m_iIdx = 0;
    Team m_eWhatTeam = Team::Neutral;
};