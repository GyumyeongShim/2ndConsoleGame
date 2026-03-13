#include <Windows.h>

#include "DataLoader.h"

using json = nlohmann::json;

namespace Wannabe
{
    static json OpenAndParse(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open: " << path << "\n";
            return json();
        }
        json j;
        file >> j;
        return j;
    }

    std::vector<PlayerData> LoadPlayerJson(const std::string& path)
    {
        json j = OpenAndParse(path);
        return j.empty() ? std::vector<PlayerData>() : j.at("Players").get<std::vector<PlayerData>>();
    }

    std::vector<MonsterData> LoadMonsterJson(const std::string& path)
    {
        json j = OpenAndParse(path);
        return j.empty() ? std::vector<MonsterData>() : j.at("Monsters").get<std::vector<MonsterData>>();
    }

    std::vector<ActionData> LoadActionJson(const std::string& path)
    {
        json j = OpenAndParse(path);
        if (j.empty()) return {};

        // "Skills" 키나 "Items" 키 중 있는 것을 로드 (유연한 대응)
        if (j.contains("Skills")) return j.at("Skills").get<std::vector<ActionData>>();
        if (j.contains("Items")) return j.at("Items").get<std::vector<ActionData>>();

        return {};
    }
}

static ActionTargetType ParseActionTargetType(const std::string& str)
{
    if (str == "Self")          return ActionTargetType::Self;
    if (str == "SingleAlly")    return ActionTargetType::SingleAlly;
    if (str == "SingleEnemy")   return ActionTargetType::SingleEnemy;
    if (str == "AllAlly")       return ActionTargetType::AllAlly;
    if (str == "AllEnemy")      return ActionTargetType::AllEnemy;
    if (str == "RandomEnemy")   return ActionTargetType::RandomEnemy;
    if (str == "Area")          return ActionTargetType::Area;

    return ActionTargetType::None;
}

static CombatEffectType ParseCombatEffectType(const std::string& str)
{
    if (str == "Damage") return CombatEffectType::Damage;
    if (str == "Heal") return CombatEffectType::Heal;
    if (str == "ApplyStatus") return CombatEffectType::ApplyStatus;
    if (str == "RemoveStatus") return CombatEffectType::RemoveStatus;
    if (str == "Buff") return CombatEffectType::Buff;
    if (str == "Debuff") return CombatEffectType::Debuff;

    return CombatEffectType::None;
}

static EAniType ParseAniType(const std::string& str)
{
    if (str == "Attack") return EAniType::Attack;
    if (str == "Skill")  return EAniType::Skill;
    if (str == "Hit")    return EAniType::Hit;
    if (str == "Death")  return EAniType::Death;
    return EAniType::Attack;
}

static StatusType ParseStatusType(const std::string& str)
{
    if (str == "Burn") return  StatusType::Burn;
    if (str == "Poison") return  StatusType::Poison;
    if (str == "Freeze") return  StatusType::Freeze;
    if (str == "Shock") return  StatusType::Shock;
    if (str == "Stun") return  StatusType::Stun;
    if (str == "Sleep") return  StatusType::Sleep;
    if (str == "AtkUp") return  StatusType::AtkUp;
    if (str == "AtkDown") return  StatusType::AtkDown;
    if (str == "DefDown") return  StatusType::DefDown;
    if (str == "SpdUp") return  StatusType::SpdUp;
    if (str == "SpdDown") return  StatusType::SpdDown;

    return  StatusType::None;
}

static ItemType ParseItemType(const std::string& str)
{
    if (str == "Equipment") return ItemType::Equipment;
    if (str == "Consumable") return ItemType::Consumable;
    if (str == "Quest") return ItemType::Quest;
    return ItemType::None;
}

static EquipSlot ParseEquipSlot(const std::string& str)
{
    if (str == "Weapon") return EquipSlot::Weapon;
    if (str == "Armor") return EquipSlot::Armor;
    if (str == "Accessory") return EquipSlot::Accessory;
    return EquipSlot::None;
}

std::wstring ToWString(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

void from_json(const json& j, BaseStat& stat)
{
    stat.iMaxHp = j.value("maxHp", 0);
    stat.iAtk = j.value("atk", 0);
    stat.iDef = j.value("def", 0); // 구조체 변수명은 iDef, JSON 키는 defense
    stat.iSpd = j.value("spd", 0);

    stat.iAccuracy = j.value("accuracy", 0);
    stat.iEvasion = j.value("evasion", 0);
    stat.iCritChance = j.value("critChance", 0);
    stat.iCritResist = j.value("critResist", 0);

    stat.iLevel = j.value("level", 1);   // 레벨 기본값은 보통 1
    stat.iMaxExp = j.value("maxExp", 0);
}

void from_json(const json& j, CharacterVisualData& visual)
{
    visual.name = ToWString(j.value("name", "Unknown"));

    // Color 파싱
    std::string colorStr = j.value("color", "White");
    if (colorStr == "Black") visual.color = Wannabe::Color::Black;
    else if (colorStr == "Red") visual.color = Wannabe::Color::Red;
    else if (colorStr == "Green") visual.color = Wannabe::Color::Green;
    else if (colorStr == "Cyan") visual.color = Wannabe::Color::Cyan;
    else if (colorStr == "Yellow") visual.color = Wannabe::Color::Yellow;
    else visual.color = Wannabe::Color::White;

    // Ascii 파싱
    if (j.contains("ascii"))
    {
        visual.ascii.clear();
        for (auto& line : j["ascii"].get<std::vector<std::string>>())
            visual.ascii.emplace_back(ToWString(line));
    }

    // Ascii Ani 파싱
    if (j.contains("animations"))
    {
        auto animationsJson = j.at("animations");
        for (auto it = animationsJson.begin(); it != animationsJson.end(); ++it)
        {
            // it.key()는 "Attack", "Skill" 등의 문자열
            // it.value()는 해당 애니메이션의 프레임 리스트([[...],[...]])
            EAniType type = ParseAniType(it.key());
            std::vector<std::vector<std::wstring>> frames;

            const auto& frameList = it.value();
            for (const auto& frame : frameList)
            {
                std::vector<std::wstring> lines;
                for (const auto& line : frame)
                {
                    lines.emplace_back(ToWString(line.get<std::string>()));
                }
                frames.emplace_back(lines);
            }

            visual.animations[type] = frames;
        }
    }
}

void from_json(const json& j, PlayerData& data)
{
    data.iTID = j.at("tid").get<int>();
    data.baseStat = j.at("baseStat").get<BaseStat>();
    data.level = j.value("level", 1);
    data.skillIds = j.value("skillIds", std::vector<int>{});
    if (j.contains("visual")) data.visual = j.at("visual").get<CharacterVisualData>();
}

void from_json(const json& j, MonsterData& data)
{
    data.iTID = j.value("tid", 0);
    if (j.contains("baseStat")) data.baseStat = j.at("baseStat").get<BaseStat>();
    data.expReward = j.value("expReward", 0);
    data.skillIds = j.value("skillIds", std::vector<int>{});
    if (j.contains("visual")) data.visual = j.at("visual").get<CharacterVisualData>();
}

void from_json(const json& j, CharacterData& data)
{
    data.iTID = j.value("tid", 0);
    if (j.contains("baseStat")) {
        data.baseStat = j.at("baseStat").get<BaseStat>();
    }
    if (j.contains("visual")) {
        data.visual = j.at("visual").get<CharacterVisualData>();
    }
}

void from_json(const json& j, StatModifier& mod)
{
    mod.iHp = j.value("iHp", 0);
    mod.iAtk = j.value("iAtk", 0);
    mod.iDef = j.value("iDef", 0);
    mod.iSpd = j.value("iSpd", 0);
    mod.iAccuracy = j.value("iAccuracy", 0);
    mod.iEvasion = j.value("iEvasion", 0);
    mod.iCritChance = j.value("iCritChance", 0);
    mod.iCritResist = j.value("iCritResist", 0);
}

void from_json(const json& j, CombatEffectData& data)
{
    data.eType = ParseCombatEffectType(j.value("type", "None"));
    data.eStatusType = ParseStatusType(j.value("statusType", "None"));
    data.iValue = j.value("value", 0);
    data.iDuration = j.value("duration", 0);
    data.iRatio = j.value("ratio", 100);
    data.iPower = j.value("power", 0);
}

void from_json(const json& j, ActionVisualData& data)
{
    std::string symbol = j.value("asciiSymbol", "*");
    data.asciiSymbol = symbol.empty() ? L'*' : static_cast<wchar_t>(symbol[0]);
    data.color = Wannabe::Color::White; // 필요 시 캐릭터 컬러 파싱 로직 적용
    data.fParticleSpd = j.value("particleSpeed", 10.f);
    data.iParticleRange = j.value("particleRange", 5);
}

void from_json(const json& j, ActionData& data)
{
    data.iTid = j.value("tid", 0);
    data.wstrName = ToWString(j.value("name", "Action"));

    // 카테고리/타입 파싱
    data.eItemType = ParseItemType(j.value("itemType", "None"));
    data.targetType = ParseActionTargetType(j.value("targetType", "Self"));

    data.iMaxTargetCnt = j.value("maxTargetCnt", 1);
    data.iCost = j.value("cost", 0);
    data.iPower = j.value("power", 0);

    // 공통 시각 데이터
    if (j.contains("visual")) j.at("visual").get_to(data.visual);

    // 공통 효과 데이터
    data.vecEffects.clear();
    if (j.contains("effects")) j.at("effects").get_to(data.vecEffects);

    // 장비 전용 데이터
    if (data.eItemType == ItemType::Equipment) {
        data.equipSlot = ParseEquipSlot(j.value("equipSlot", "None"));
        if (j.contains("statModifier")) j.at("statModifier").get_to(data.statModifier);
    }
}
