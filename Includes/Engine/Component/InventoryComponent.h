#pragma once
#include <vector>

#include "Core/Common.h"
#include "Item/ItemInstance.h"

namespace Wannabe
{
    class Item;
	class WANNABE_API InventoryComponent
	{
    public:
        InventoryComponent* Clone() const { return new InventoryComponent(*this); }

        void AddItem(Item * item, int count = 1);
        bool RemoveItem(int iTID, int count = 1);

        ItemInstance* FindItem(int iTID);
        const std::vector<ItemInstance>& GetItems() const;

    private:
        std::vector<ItemInstance> m_vecItemInstances;
	};
}