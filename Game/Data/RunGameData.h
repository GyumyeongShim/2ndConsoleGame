#pragma once
#include <vector>
#include <string>
#include "Math/Vector2.h"

#include "Component/StatComponent.h"

// 레벨전환시 사용할 데이터
struct RunGameData
{
    // Map, Pos
    size_t m_CurLevelId;
    Wannabe::Vector2 m_lastWorldPos;
    Wannabe::Vector2 m_NextWorldPos;

    // stat
    Wannabe::StatData m_PlayerStat;

    // Inven
    int m_iGold = 0;
    std::vector<int> m_vecInevenItemTids;
};