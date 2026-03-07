#pragma once
#include <string>

#include "Core/Common.h"
#include "Core/RTTI.h"
#include "Enum/ItemType.h"
#include "../Game/Data/ActionData.h"

namespace Wannabe
{
	class WANNABE_API Item : public RTTI
	{
		RTTI_DECLARATIONS(Item, RTTI);

	public:
		Item(const ActionData& data);

		int GetItemTID() const { return m_Data.iTid; }
		ItemType GetItemType() const { return m_Data.eItemType; }
		const std::wstring& GetName() { return m_Data.wstrName; }

		// 데이터 기반 판정
		bool IsStackable() const;
		const ActionData& GetActionData() const { return m_Data; }

	protected:
		ActionData m_Data;
	};
}