#include "ItemInstance.h"

#include "Item/Item.h"
#include "Enum/ItemType.h"

using namespace Wannabe;

ItemInstance::ItemInstance(Wannabe::Item* pItem, int iCnt)
	:m_pItem(pItem), m_iCnt(iCnt)
{
}

void ItemInstance::AddCount(int iValue)
{
	if (iValue <= 0 || !m_pItem)
		return;

	if (IsStackable())
		m_iCnt += iValue;
}

bool ItemInstance::Consume(int iValue)
{
	if (iValue <= 0 || !IsConsumable())
		return false;

	if (m_iCnt < iValue)
		return false;

	m_iCnt -= iValue;
	return true;
}

bool ItemInstance::IsEmpty() const
{
	return m_iCnt <= 0;
}

bool ItemInstance::IsValid() const
{
	return m_pItem != nullptr;
}

bool ItemInstance::IsStackable() const
{
	if (m_pItem == nullptr)
		return false;

	return m_pItem->GetItemType() == ItemType::Consumable;
}

bool ItemInstance::IsConsumable() const
{
	if (m_pItem == nullptr)
		return false;

	return m_pItem->GetItemType() == ItemType::Consumable;
}

bool ItemInstance::IsEquipment() const
{
	if (m_pItem == nullptr)
		return false;

	return m_pItem->GetItemType() == ItemType::Equipment;
}