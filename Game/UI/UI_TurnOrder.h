#pragma once
#include <vector>
#include <string>

#include "UI/UI.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Battle/TurnManager.h"

class UI_TurnOrder : public Wannabe::UI
{
	RTTI_DECLARATIONS(UI_TurnOrder, UI)

public:
    UI_TurnOrder(Wannabe::TurnManager* pTurnMgr, Wannabe::Vector2 pos);

    virtual void Tick(float fDeltaTime) override;
    virtual void Draw(Wannabe::RenderSystem& renderSys) override;
    virtual void RecalculateViewportPosition() override;
    virtual void SetRenderSystem(Wannabe::RenderSystem* renderSystem) { m_pRenderSystem = renderSystem; }

    void Init();
    void RefreshTurnOrder(); //턴 종료 후 외부에서 호출

private:
    void UpdateTurnText();

private:
    Wannabe::TurnManager* m_pTurnMgr = nullptr;

    std::vector<std::wstring> m_lines;
    std::vector<Wannabe::Color> m_lineColors;

    int m_maxLine = 5; //최대 5줄까지 표기
};