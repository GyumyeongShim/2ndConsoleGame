#pragma once
#include <vector>
#include <string>
#include "Math/Vector2.h"
#include "Util/Utill.h"
#include "Actor/Actor.h"

#include "Component/StatComponent.h"

#include "../Includes/ThirdParty/json.hpp"
using json = nlohmann::json;

// 레벨전환시 사용할 데이터
struct RunGameData
{
    // 랭킹용
    std::string m_UserName = "Unknown";
    bool m_bIsClear = false;
    int m_iKillCount = 0; // 몇 마리를 잡았는지 기록

    // Map, Pos
    size_t m_CurLevelId;
    Wannabe::Vector2 m_lastWorldPos;
    Wannabe::Vector2 m_NextWorldPos;

    // stat
    Wannabe::StatData m_PlayerStat;

    // Inven
    int m_iGold = 0;
    std::vector<int> m_vecInevenItemTids;

    // 전투로 넘길 타겟 정보
    std::vector<Actor*> m_vecBattlePlayers;
    std::vector<Actor*> m_vecBattleMonsters;

    // 전투 종료 후 필드에서 제거해야 할 몬스터의 ID나 포인터 기록
    Actor* m_pEncounteredEnemy = nullptr;

    json ToJson() const
    {
        json j;
        // 랭킹용
        j["UserName"] = m_UserName;
        j["IsClear"] = m_bIsClear;
        j["KillCount"] = m_iKillCount;

        j["CurLevelId"] = m_CurLevelId;
        j["LastPos"] = { m_lastWorldPos.x, m_lastWorldPos.y };
        j["Gold"] = m_iGold;

        // StatData 직렬화
        j["Stats"] = {
            {"Level", m_PlayerStat.iLevel},
            {"Hp", m_PlayerStat.iHp},
            {"MaxHp", m_PlayerStat.iMaxHp},
            {"Atk", m_PlayerStat.iAtk},
            {"Def", m_PlayerStat.iDef},
            {"Exp", m_PlayerStat.iExp},
            {"MaxExp", m_PlayerStat.iMaxExp}
        };

        // 인벤토리 아이템 등 추가 가능
        j["Items"] = m_vecInevenItemTids;

        // 세이브 시간 기록 (TitleLevel에서 사용 중)
        j["SaveTime"] = Util::GetCurrentDateTimeString();
        return j;
    }

    void FromJson(const json& j)
    {
        m_CurLevelId = j.value("CurLevelId", 1);
        m_iGold = j.value("Gold", 0);

        if (j.contains("LastPos"))
        {
            m_lastWorldPos.x = j["LastPos"][0];
            m_lastWorldPos.y = j["LastPos"][1];
        }

        if (j.contains("Stats"))
        {
            auto& s = j["Stats"];
            m_PlayerStat.iLevel = s.value("Level", 1);
            m_PlayerStat.iHp = s.value("Hp", 100);
            m_PlayerStat.iMaxHp = s.value("MaxHp", 100);
            m_PlayerStat.iAtk = s.value("Atk", 10);
            m_PlayerStat.iDef = s.value("Def", 5);
            m_PlayerStat.iExp = s.value("Exp", 0);
            m_PlayerStat.iMaxExp = s.value("MaxExp", 100);
        }

        m_vecInevenItemTids = j.value("Items", std::vector<int>());
    }
};