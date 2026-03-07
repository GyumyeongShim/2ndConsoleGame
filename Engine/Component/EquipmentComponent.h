#pragma once
#include <map>

#include "Core/Common.h"
#include "Enum/ItemType.h"
#include "Component/Component.h"

namespace Wannabe
{
    class ItemInstance;
    class StatComponent;

    struct EquippedData
    {
        ItemInstance* instance = nullptr;
        StatModifier modifier;
    };

    class WANNABE_API EquipmentComponent : public Component
    {
    public:
        EquipmentComponent();
        virtual ~EquipmentComponent() override;

        EquipmentComponent* Clone() const; //±Ì¿∫ ∫πªÁ

        bool Equip(ItemInstance* pInst, StatComponent* pStatCompo);
        void Unequip(EquipSlot eSlot, StatComponent* pStatCompo);

        ItemInstance* GetEquippedItem(EquipSlot slot) const;
        const std::map<EquipSlot, ItemInstance*>& GetAllEquipped() const { return m_mapEquipped; }

    private:
        std::map<EquipSlot, ItemInstance*> m_mapEquipped;
    };
}