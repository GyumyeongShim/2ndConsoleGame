#pragma once
#include "UI/UI.h"

#include <string>
#include "Math/Color.h"

class BattleActor;
class TurnManager;

namespace Wannabe
{
    class TurnManager;
    class Actor;
}

class UI_ActorInfo : public Wannabe::UI
{
    RTTI_DECLARATIONS(UI_ActorInfo, UI)
public:
    UI_ActorInfo(Wannabe::TurnManager* pTurnManager, Wannabe::Actor* pTarget, bool bIsPlayer);
    virtual ~UI_ActorInfo();

    virtual void Tick(float fDeltaTime) override;
    virtual void Draw(Wannabe::RenderSystem& renderSys) override;
    virtual void RecalculateViewportPosition() override;
    virtual void SetRenderSystem(Wannabe::RenderSystem* renderSystem) { m_pRenderSystem = renderSystem; }

    void Init();

private:
    bool IsMyTurn();
    std::wstring MakeGaugeBar(int iCur, int iMax, int iWidth, Wannabe::Color& outColor);

private:
    Wannabe::TurnManager* m_pTurnManager = nullptr;
    Wannabe::Actor* m_pTargetActor = nullptr;
    bool m_bIsPlayer = false;

    std::wstring m_infoStr = L"";
    Wannabe::Color m_Color = Wannabe::Color::White;
};