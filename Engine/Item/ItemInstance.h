#pragma once
#include "Core/Common.h"
#include "Core/RTTI.h"

namespace Wannabe
{
    class Item;
    class Actor;

    class WANNABE_API ItemInstance : public RTTI
    {
        RTTI_DECLARATIONS(ItemInstance, RTTI);

    public:
        ItemInstance(Item* pItem = nullptr, int iCnt = 1);

        bool Use(Actor* pAtker, Actor* pTarget);

        void AddCount(int iValue);
        bool Consume(int iValue);

        Item* GetItem() const { return m_pItem; }
        int GetCnt() const { return m_iCnt; }

        bool IsEmpty() const { return m_iCnt <= 0 || m_pItem == nullptr; }
        bool IsValid() const { return m_pItem != nullptr; }

        bool IsStackable() const;
        bool IsConsumable() const;
        bool IsEquipment() const;

    private:
        Item* m_pItem = nullptr;
        int m_iCnt = 0;
    };
}