#include "EquipmentItem.h"

using namespace Wannabe;

EquipmentItem::EquipmentItem(const ActionData& data)
    : Item(data)
{
}

void EquipmentItem::SetEquipSlot(EquipSlot slot)
{
    m_eSlot = slot;
}

void EquipmentItem::SetModifier(const StatModifier& mod)
{
    m_eModifier = mod;
}

EquipSlot EquipmentItem::GetEquipSlot() const
{
    return m_eSlot;
}

const StatModifier& EquipmentItem::GetModifier() const
{
    return m_eModifier;
}