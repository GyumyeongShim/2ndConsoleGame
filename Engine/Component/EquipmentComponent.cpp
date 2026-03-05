#include "EquipmentComponent.h"
#include "Item/Item.h"
#include "Item/ItemInstance.h"
#include "Component/StatComponent.h"

#include "Interface/IEquipmentData.h"

using namespace Wannabe;

EquipmentComponent* EquipmentComponent::Clone() const
{
    auto* comp = new EquipmentComponent();

    comp->m_mapEquipped = m_mapEquipped;

    return comp;
}

bool EquipmentComponent::Equip(ItemInstance* pInst, StatComponent* pStatCompo)
{
    if(pInst == nullptr || pStatCompo == nullptr)
        return false;

    if (pInst->IsEquipment() == false)
        return false;

    auto* data = dynamic_cast<IEquipmentData*>(pInst->GetItem());
    if (data == nullptr)
        return false;

    EquipSlot slot = data->GetEquipSlot();

    Unequip(slot, pStatCompo); //장비 해제

    m_mapEquipped[slot] = pInst; //장비 변경

    const StatModifier& mod = data->GetModifier(); //스탯 적용
    pStatCompo->AddEquipmentModifier(mod.iAtk,mod.iDef,mod.iHp,mod.iSpd);

    return true;
}

void EquipmentComponent::Unequip(EquipSlot eSlot, StatComponent* pStatCompo)
{
    if (pStatCompo == nullptr)
        return;

    auto it = m_mapEquipped.find(eSlot);
    if (it == m_mapEquipped.end())
        return;

    ItemInstance* pInst = it->second;
    if (pInst == nullptr)
        return;

    auto* data = dynamic_cast<IEquipmentData*>(pInst->GetItem());
    if (data != nullptr)
    {
        const StatModifier& mod = data->GetModifier();
        pStatCompo->RemoveEquipmentModifier(mod.iAtk, mod.iDef, mod.iHp, mod.iSpd);
    }

    m_mapEquipped.erase(it);
}

ItemInstance* Wannabe::EquipmentComponent::GetEquippedItem(EquipSlot slot) const
{
    auto it = m_mapEquipped.find(slot);
    if (it == m_mapEquipped.end())
        return nullptr;

    return it->second;
}