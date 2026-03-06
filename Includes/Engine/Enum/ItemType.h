#pragma once
#include <string>

#include "Enum/CombatType.h"

enum class EquipSlot //장착 슬롯
{
	None,
	Weapon,
	Armor,
	Accessory
};

struct StatModifier // 스탯 변경
{
	int iHp = 0;
	int iAtk = 0;
	int iDef = 0;
	int iSpd = 0;
	int iAccuracy = 0;		// 명중률
	int iEvasion = 0;		// 회피율
	int iCritChance = 0;	// 치명타 확률
	int iCritResist = 0;	// 치명타 저항 (상대의 치명타 확률을 상쇄)
};

enum class ItemType // 아이템 종류
{
	None,
	Equipment,
	Consumable,
	Throw,
	Quest
};

enum class ItemTargetType // 타겟 종류
{
	Self,
	AllySingle,
	EnemySingle,
	AllAlly,
	AllEnemy
};