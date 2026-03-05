#pragma once
#include "Enum/ItemType.h"

class IEquipmentData
{
public:
    virtual ~IEquipmentData() = default;

    virtual EquipSlot GetEquipSlot() const = 0;
    virtual const StatModifier& GetModifier() const = 0;
};