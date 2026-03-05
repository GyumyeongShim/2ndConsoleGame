#pragma once
#include "CharacterData.h"

struct MonsterData : public CharacterData
{
    int expReward = 0;
    std::vector<int> skillIds;
};