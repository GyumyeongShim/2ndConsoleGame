#include <fstream>
#include <iostream>
#include <string>

#include "DataManager.h"
#include "Manager/DataLoader.h"

namespace Wannabe
{
    DataManager::~DataManager()
    {
        for (auto& pair : m_ItemTable)
        {
            if (pair.second != nullptr)
            {
                delete pair.second;
            }
        }

        m_ItemTable.clear();
    }

    void DataManager::Init()
    {
        std::string basePath = "../Assets/";

        LoadPlayers(LoadPlayerJson(basePath + "Player.json"));
        LoadMonsters(LoadMonsterJson(basePath + "Monster.json"));
        LoadActions(LoadActionJson(basePath + "Skill.json"));
        LoadActions(LoadActionJson(basePath + "Item.json"));
    }

    void DataManager::LoadPlayers(const std::vector<PlayerData>& players)
    {
        for (const auto& data : players)
            m_PlayerTable[data.iTID] = data;
    }

    void DataManager::LoadMonsters(const std::vector<MonsterData>& monsters)
    {
        for (const auto& data : monsters)
            m_MonsterTable[data.iTID] = data;
    }

    void DataManager::LoadActions(const std::vector<ActionData>& actions)
    {
        for (const auto& data : actions)
            m_ActionTable[data.iTid] = data;
    }

    const PlayerData* DataManager::GetPlayerData(int iTID)
    {
        auto it = m_PlayerTable.find(iTID);
        return (it != m_PlayerTable.end()) ? &it->second : nullptr;
    }

    const MonsterData* DataManager::GetMonsterData(int iTID)
    {
        auto it = m_MonsterTable.find(iTID);
        return (it != m_MonsterTable.end()) ? &it->second : nullptr;
    }

    const ActionData* DataManager::GetActionData(int iTID)
    {
        auto it = m_ActionTable.find(iTID);
        return (it != m_ActionTable.end()) ? &it->second : nullptr;
    }

    Item* DataManager::GetItem(int iTID)
    {
        auto it = m_ItemTable.find(iTID);
        if (it != m_ItemTable.end())
            return it->second;

        const ActionData* pData = GetActionData(iTID);
        if (pData != nullptr)
        {
            if (pData->eItemType != ItemType::None)
            {
                Item* pNewItem = new Item(*pData);
                m_ItemTable[iTID] = pNewItem;
                return pNewItem;
            }
        }

        return nullptr;
    }
}
