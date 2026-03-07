#pragma once
#include <memory>

#include "Core/Common.h"

namespace Wannabe
{
    class Item;

    class WANNABE_API ItemInstance
    {
    public:
        ItemInstance(Item* pItem = nullptr, int iCnt = 1);

        Item* GetItem() const { return m_pItem; }

        int GetCnt() const { return m_iCnt; }

        void AddCount(int iValue);
        bool Consume(int iValue);

        bool IsEmpty() const;
        bool IsValid() const;

        bool IsStackable() const;
        bool IsConsumable() const;
        bool IsEquipment() const;

    private:
        Item* m_pItem = nullptr;
        int m_iCnt = 0;
    };
}