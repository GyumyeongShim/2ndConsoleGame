#pragma once
#include <string>
#include <vector>

#include "Enum/ActionType.h"
#include "Enum/CombatType.h"
#include "Enum/ItemType.h" // StatModifier, EquipSlot 포함

struct ActionData
{
    // [식별 정보]
    int iTid = 0;
    std::wstring wstrName;

    // [분류]
    ItemType eItemType = ItemType::None; // 스킬일 경우 None 혹은 별도의 SkillType 사용

    // [공통 실행 로직]
    ActionTargetType targetType = ActionTargetType::Self;
    int iMaxTargetCnt = 1;
    int iCost = 0;          // MP 소모량 혹은 아이템 소모 개수
    int iPower = 0;         // 기본 위력 (스킬 위력 혹은 아이템 기본 수치)

    // [비주얼 데이터]
    ActionVisualData visual;

    // [핵심: 효과 리스트]
    // 스킬/아이템 사용 시 발생하는 모든 효과를 리스트로 관리 (효과 중심 설계)
    std::vector<CombatEffectData> vecEffects;

    // [장비 전용 데이터]
    // 장비 아이템일 경우에만 사용하며, 스킬이나 소모품일 경우 기본값
    EquipSlot equipSlot = EquipSlot::None;
    StatModifier statModifier;
};