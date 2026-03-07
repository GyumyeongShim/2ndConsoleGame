#include "ItemInstance.h"

#include "Item/Item.h"
#include "Enum/ItemType.h"

using namespace Wannabe;

ItemInstance::ItemInstance(Wannabe::Item* pItem, int iCnt)
	:m_pItem(pItem), m_iCnt(iCnt)
{
}

bool ItemInstance::Use(Actor* pAtker, Actor* pTarget)
{
	if(IsValid() == false || pAtker == nullptr)
		return false;

	const ActionData& data = m_pItem->GetActionData();
	if (IsConsumable() == true && m_iCnt <= 0)
		return false;

	for (const auto& effect : data.vecEffects)
	{

	}

	if (IsConsumable() == true)
		Consume(1);

	return true;
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

bool ItemInstance::IsStackable() const
{
	if (m_pItem == nullptr)
		return false;

	return m_pItem->IsStackable();
}

bool ItemInstance::IsConsumable() const
{
	if (m_pItem == nullptr) 
		return false;

	ItemType type = m_pItem->GetItemType();
	return type == ItemType::Consumable || type == ItemType::Throw;
}

bool ItemInstance::IsEquipment() const
{
	if (m_pItem == nullptr)
		return false;

	return m_pItem->GetItemType() == ItemType::Equipment;
}