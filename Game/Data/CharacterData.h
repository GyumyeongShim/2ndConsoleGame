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
    int maxHp = 0;
    int atk = 0;
    int defense = 0;
    int spd = 0;
};

struct CharacterData
{
    int iTID = 0;
    BaseStat baseStat;
    CharacterVisualData visual;
};