#pragma once
#include <vector>

#include "Core/Common.h"
#include "Component/Component.h"

namespace Wannabe
{
    class Actor;
    class ItemInstance;

	class WANNABE_API InventoryComponent : public Component
	{
    public:
        InventoryComponent* Clone() const { return new InventoryComponent(*this); }
        virtual ~InventoryComponent() override;

        bool AddItem(int iTID, int iCnt = 1);
        bool UseItem(int iSlotIdx, Actor* pTarget = nullptr);
        void RemoveItem(int iSlotIdx, int iCnt = 1);

        const std::vector<ItemInstance*>& GetItems() const { return m_vecItemInstances; }
        ItemInstance* GetItemInSlot(int iSlotIdx);
        int GetMaxSlots() const { return m_iMaxSlots; }

    private:
        ItemInstance* FindStackableItem(int iTID);

    private:
        std::vector<ItemInstance*> m_vecItemInstances;
        int m_iMaxSlots = 10;
	};
}