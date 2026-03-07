#include "Item.h"

Wannabe::Item::Item(const ActionData& data)
	:m_Data(data)
{
}

bool Wannabe::Item::IsStackable() const
{
	return m_Data.eItemType != ItemType::Equipment;
}