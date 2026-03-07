#include "InventoryComponent.h"

#include "Enum/ItemType.h"
#include "Item/Item.h"
#include "Item/ItemInstance.h"

using namespace Wannabe;

void InventoryComponent::AddItem(Item* item, int count)
{
    if (item == nullptr || count <= 0)
        return;

    auto* inst = FindItem(item->GetItemTID());
    if (inst && inst->IsStackable())
    {
        inst->AddCount(count);
        return;
    }

    // ½ºÅÃ ºÒ°¡°Å³ª Ã³À½ È¹µæ
    m_vecItemInstances.emplace_back(item, count);
}

bool InventoryComponent::RemoveItem(int iTID, int count)
{
    if (count <= 0)
        return false;

    auto* inst = FindItem(iTID);
    if (inst == nullptr)
        return false;

    if (inst->Consume(count) == false)
        return false;

    if (inst->IsEmpty())
    {
        m_vecItemInstances.erase(
            std::remove_if(
                m_vecItemInstances.begin(),
                m_vecItemInstances.end(),
                [iTID](const ItemInstance& i)
        {
            return i.GetItem()->GetItemTID() == iTID;
        }),
            m_vecItemInstances.end());
    }

    return true;
}

ItemInstance* InventoryComponent::FindItem(int iTID)
{
    for (auto& inst : m_vecItemInstances)
    {
        if (inst.GetItem()->GetItemTID() == iTID)
            return &inst;
    }

    return nullptr;
}

const std::vector<ItemInstance>& InventoryComponent::GetItems() const
{
    return m_vecItemInstances;
}