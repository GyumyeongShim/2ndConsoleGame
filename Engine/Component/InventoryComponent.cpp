#include "InventoryComponent.h"

#include "Enum/ItemType.h"
#include "Actor/Actor.h"
#include "Item/Item.h"
#include "Item/ItemInstance.h"
#include "Manager/DataManager.h"

using namespace Wannabe;

InventoryComponent::~InventoryComponent()
{
    for (ItemInstance* item : m_vecItemInstances)
    {
        if (item == nullptr)
            continue;

        delete item;
    }

    m_vecItemInstances.clear();
}

bool InventoryComponent::AddItem(int iTID, int iCnt)
{
    const ActionData* pData = DataManager::Get().GetActionData(iTID);
    if (pData == nullptr)
        return false;

    if (pData->eItemType != ItemType::Equipment);
    {
        ItemInstance* pItem = FindStackableItem(iTID);
        if (pItem != nullptr)
        {
            pItem->AddCount(iCnt);
            return true;
        }
    }

    if (static_cast<int>(m_vecItemInstances.size()) >= m_iMaxSlots)
        return false;

    Item* pBaseItem = DataManager::Get().GetItem(iTID);
    if (pBaseItem != nullptr)
    {
        ItemInstance* pNewInstance = new ItemInstance(pBaseItem, iCnt);
        m_vecItemInstances.emplace_back(pNewInstance);
        return true;
    }

    return false;
}

bool InventoryComponent::UseItem(int iSlotIdx, Actor* pTarget)
{
    ItemInstance* pInstance = GetItemInSlot(iSlotIdx);
    if (pInstance == nullptr)
        return false;

    Actor* pActualTarget = (pTarget != nullptr) ? pTarget : GetOwner();

    if (pInstance->Use(GetOwner(), pActualTarget))
    {
        if (pInstance->IsEmpty())
        {
            RemoveItem(iSlotIdx);
        }
        return true;
    }

    return false;
}

void InventoryComponent::RemoveItem(int iSlotIdx, int iCnt)
{
    if (iSlotIdx < 0 || iSlotIdx >= static_cast<int>(m_vecItemInstances.size()))
        return;

    ItemInstance* pInstance = m_vecItemInstances[iSlotIdx];
    if (pInstance->Consume(iCnt))
    {
        if (pInstance->IsEmpty())
        {
            delete pInstance;
            m_vecItemInstances.erase(m_vecItemInstances.begin() + iSlotIdx);
        }
    }
}

ItemInstance* InventoryComponent::GetItemInSlot(int iSlotIdx)
{
    if (iSlotIdx < 0 || iSlotIdx >= static_cast<int>(m_vecItemInstances.size()))
        return nullptr;

    return m_vecItemInstances[iSlotIdx];
}

ItemInstance* InventoryComponent::FindStackableItem(int iTID)
{
    for (auto* pInstance : m_vecItemInstances)
    {
        if (pInstance->GetItem()->GetItemTID() == iTID)
            return pInstance;
    }
    return nullptr;
}

ItemInstance* InventoryComponent::FindItem(int iTID)
{
    for (auto* pInst : m_vecItemInstances)
    {
        if (pInst->GetItem()->GetItemTID() == iTID)
            return pInst;
    }

    return nullptr;
}