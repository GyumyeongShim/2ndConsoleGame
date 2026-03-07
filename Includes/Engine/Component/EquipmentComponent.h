#pragma once
#include <map>

#include "Core/Common.h"
#include "Enum/ItemType.h"

namespace Wannabe
{
    class ItemInstance;
    class StatComponent;

    struct EquippedData
    {
        ItemInstance* instance = nullptr;
        StatModifier modifier;
    };

    class WANNABE_API EquipmentComponent
    {
    public:
        EquipmentComponent* Clone() const; //±Ì¿∫ ∫πªÁ
        bool Equip(ItemInstance* pInst, StatComponent* pStatCompo);
        void Unequip(EquipSlot eSlot, StatComponent* pStatCompo);

        ItemInstance* GetEquippedItem(EquipSlot slot) const;

    private:
        std::map<EquipSlot, ItemInstance*> m_mapEquipped;
    };
}