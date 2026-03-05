#pragma once
#include "Item/Item.h"
#include "Enum/ItemType.h"
#include "Interface/IEquipmentData.h"

class EquipmentItem : public Wannabe::Item, public IEquipmentData
{
    RTTI_DECLARATIONS(EquipmentItem, Item);

public:
    EquipmentItem(int tid);

    void SetEquipSlot(EquipSlot slot);
    void SetModifier(const StatModifier& mod);

    EquipSlot GetEquipSlot() const;
    const StatModifier& GetModifier() const;

private:
    EquipSlot m_eSlot = EquipSlot::None;
    StatModifier m_eModifier;
};