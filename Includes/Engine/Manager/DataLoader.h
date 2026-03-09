#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>

#include "Enum/ActionType.h"
#include "Enum/CombatType.h"
#include "Enum/ItemType.h"

#include "../Game/Data/ActionData.h"
#include "../Game/Data/PlayerData.h"
#include "../Game/Data/MonsterData.h"

#include "../Includes/ThirdParty/json.hpp"
using json = nlohmann::json;

namespace Wannabe
{
    std::vector<PlayerData> LoadPlayerJson(const std::string& path);
    std::vector<MonsterData> LoadMonsterJson(const std::string& path);
    std::vector<ActionData> LoadActionJson(const std::string& path);
}

std::wstring ToWString(const std::string& str);

void from_json(const json& j, BaseStat& stat);
void from_json(const json& j, CharacterVisualData& visual); //ascii, ascii ani (atk,skill,hit, death...)
void from_json(const json& j, PlayerData& data);
void from_json(const json& j, MonsterData& data);
void from_json(const json& j, CharacterData& data);

void from_json(const json& j, StatModifier& mod);
void from_json(const json& j, ActionVisualData& visual); // 통합 비주얼 데이터
void from_json(const json& j, CombatEffectData& data);  // 효과 중심 설계의 핵심
void from_json(const json& j, ActionData& data);        // 통합 액션 데이터