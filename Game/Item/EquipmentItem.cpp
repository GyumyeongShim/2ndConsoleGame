#include "EquipmentItem.h"

using namespace Wannabe;

EquipmentItem::EquipmentItem(int tid)
    : Item(tid, ItemType::Equipment)
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