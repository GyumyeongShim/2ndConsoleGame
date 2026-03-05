#pragma once
#include <vector>
#include "CharacterData.h"

struct PlayerData : public CharacterData
{
    int level = 1;
    std::vector<int> skillIds;
};