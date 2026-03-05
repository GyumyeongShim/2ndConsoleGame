#pragma once
#include "UI/UI.h"

namespace Wannabe
{
    class InventoryComponent;
}

class UI_Inventory : public Wannabe::UI
{
    RTTI_DECLARATIONS(UI_Inventory, UI)

public:
    UI_Inventory(Wannabe::InventoryComponent* inven);

    virtual void Tick(float fDeltaTime) override;
    virtual void Draw(Wannabe::RenderSystem& renderSys) override;
    virtual void RecalculateViewportPosition() override;
    virtual void SetRenderSystem(Wannabe::RenderSystem* renderSystem) { m_pRenderSystem = renderSystem; }

    void Init();

    void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }

    void SetInventory(Wannabe::InventoryComponent* inven);

    void MoveCursor(int idx);
    int GetCursorIdx() const { return m_iCursorIdx; }

private:
    Wannabe::InventoryComponent* m_pInventory = nullptr;
    bool m_bIsActive = false;
    int m_iCursorIdx = 0;
    int m_iStartIdx = 0;
    int m_iMaxShow = 4;
};