#pragma once
#include <string>
#include <vector>

#include "Math/Color.h"

struct CharacterVisualData
{
    Wannabe::Color color;
    std::wstring name;
    std::vector<std::wstring> ascii;
};

struct BaseStat
{
    int iMaxHp = 0;         // 최대 체력
    int iAtk = 0;           // 공격력
    int iDef = 0;           // 방어력
    int iSpd = 0;           // 속도

    int iAccuracy = 0;      // 명중률
    int iEvasion = 0;       // 회피율
    int iCritChance = 0;    // 치명타 확률
    int iCritResist = 0;    // 치명타 저항

    int iLevel = 1;         // 기본 레벨
    int iMaxExp = 0;        // 해당 레벨의 요구 경험치량
};

struct CharacterData
{
    int iTID = 0;
    BaseStat baseStat;
    CharacterVisualData visual;
};