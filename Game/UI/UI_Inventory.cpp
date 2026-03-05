#include <algorithm>
#include "UI_Inventory.h"
#include "Component/InventoryComponent.h"
#include "Render/RenderSystem.h"
#include "Item/Item.h"
#include "Item/ItemInstance.h"
#include "Manager/DataManager.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include "UI_MenuList.h"

UI_Inventory::UI_Inventory(Wannabe::InventoryComponent* inven)
	:m_pInventory(inven)
{
}

void UI_Inventory::Tick(float fDeltaTime)
{
	super::Tick(fDeltaTime);

	if (m_bIsActive == false || m_pInventory == nullptr)
		return;

	RecalculateViewportPosition();

    const auto& items = m_pInventory->GetItems();
    if (m_iCursorIdx >= (int)items.size())
        m_iCursorIdx = 0;
}

void UI_Inventory::Draw(Wannabe::RenderSystem& renderSys)
{
    if (!m_bIsActive || m_pInventory == nullptr)
        return;

    Vector2 pos = m_cachedViewportPos;

    renderSys.DrawUI(L"+------------------+", Vector2(pos.x, pos.y), Color::White, m_SortingOrder);
    for (int i = 1; i <= 6; ++i)
        renderSys.DrawUI(L"|                  |", Vector2(pos.x, pos.y + i), Color::White, m_SortingOrder);
    renderSys.DrawUI(L"+------------------+", Vector2(pos.x, pos.y + 7), Color::White, m_SortingOrder);

    const auto& items = m_pInventory->GetItems();
    int maxShow = std::min<int>(m_iMaxShow, (int)items.size());

    int startY = pos.y + 1;

    for (int i = 0; i < maxShow; i++)
    {
        int realIndex = m_iStartIdx + i;

        bool selected = (realIndex == m_iCursorIdx);

        std::wstring prefix = selected ? L"▶ " : L"  ";

        int iTID = items[i].GetItem()->GetItemTID();
        const ActionData* action = DataManager::Get().GetActionData(iTID);
        if (action == nullptr)
            continue;

        renderSys.DrawUI(prefix + action->wstrName,Vector2(pos.x + 2, startY + i),
            selected ? Color::Green : Color::White,m_SortingOrder);
    }
}

void UI_Inventory::RecalculateViewportPosition()
{
    Vector2 screen = m_pRenderSystem->GetScreenSize();

    const int width = 18;
    const int height = 8;

    Vector2 pos = { screen.x - width, screen.y - height };

    pos = pos + m_offset;

    if (pos.x < 0) pos.x = 0;
    if (pos.y < 0) pos.y = 0;
    if (pos.x > screen.x - width) pos.x = screen.x - width;
    if (pos.y > screen.y - height) pos.y = screen.y - height;

    m_cachedViewportPos = pos;
}

void UI_Inventory::Init()
{
    m_SortingOrder = 101;
    SetAnchor(UIAnchor::BottomRight);
    SetOffset({ 0, 0 });
    RecalculateViewportPosition();
}

void UI_Inventory::SetInventory(Wannabe::InventoryComponent* inven)
{
    m_pInventory = inven;
}

void UI_Inventory::MoveCursor(int idx)
{
    if (m_pInventory == nullptr)
        return;

    const auto& items = m_pInventory->GetItems();
    if (items.empty())
        return;

    int size = (int)items.size();

    m_iCursorIdx += idx;

    // 순환
    if (m_iCursorIdx < 0)
        m_iCursorIdx = size - 1;

    if (m_iCursorIdx >= size)
        m_iCursorIdx = 0;

    // 스크롤 조정
    if (m_iCursorIdx < m_iStartIdx)
        m_iStartIdx = m_iCursorIdx;

    if (m_iCursorIdx >= m_iStartIdx + m_iMaxShow)
        m_iStartIdx = m_iCursorIdx - m_iMaxShow + 1;
}