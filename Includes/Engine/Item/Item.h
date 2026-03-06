#pragma once
#include <string>
#include <vector>
#include <memory>

#include "Common/RTTI.h"
#include "Enum/ItemType.h"

namespace Wannabe
{
	class WANNABE_API Item : public RTTI
	{
		RTTI_DECLARATIONS(Item, RTTI);

	public:
		Item(int iTID, ItemType eType);
		int GetItemTID() const { return m_iTID; }
		ItemType GetItemType() const { return m_eItemType; }
		bool IsStackable() { return m_bStackable; }

	protected:
		ItemType m_eItemType = ItemType::Consumable;
		bool m_bStackable = false;
		int m_iTID;
	};
}