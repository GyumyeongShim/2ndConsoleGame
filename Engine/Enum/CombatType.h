#pragma once
#include <string>
#include <vector>

#include "Math/Color.h"

namespace Wannabe
{
    class Actor;
}

enum class BattleState
{
    Start,              // 시작 연출

    CommandSelect,      // 전투 메뉴 선택
    InventorySelect,    // 아이템 선택
    SkillSelect,        // 스킬 선택

    TargetSelect,       // 목표 선택

    TurnCheck,          // 행동 순서, 명령, 데미지 계산 // 정말 로직만 필요함
    Animation,          // 스킬 이펙트, hp 감소연출, 상태 이상 표시
    Log,                // 전투 관련 모든 로그를 표기
    Result              // 쓰러짐, 경험치, 배틀 종료 여부
};

enum class StatusType
{
    None,
    //Dot
    Burn,
    Poison,

    //CC
    Freeze,
    Shock,
    Stun,
    Sleep,

    //Stat Modifier
    AtkUp,
    AtkDown,
    DefUp,
    DefDown,
    SpdUp,
    SpdDown,

    Thorns,
    Counter,
    Shield,
    Reflect,

    //Special
    Regen,
    HealReflect,
    Blessed,
    LifeSteal
};

enum class CombatEffectType //스킬, 아이템 대분류 (공격, 회복, 버프/디버프)
{
    None,
    Damage,
    Heal,
    ApplyStatus,
    RemoveStatus,
    Buff,
    Debuff
};

struct CombatEffectData
{
    CombatEffectType eType = CombatEffectType::None;   // 스킬, 아이템 종류
    StatusType eStatusType = StatusType::None;  // 상태이상일 경우
    int iValue = 0;              // 데미지 or 회복량
    int iDuration = 0; // 상태이상 지속 시간 (턴 수)
    int iRatio = 100; // 데미지 비율 (예: 150% -> 150)
};

struct ActCheckResult
{
    bool bCanAct = true;
    std::wstring wstrReason;
};

struct CombatEffect //스킬 효과
{
    Wannabe::Actor* pAtker = nullptr;
    Wannabe::Actor* pTarget = nullptr;

    CombatEffectType eCombatEffectType = CombatEffectType::None; // 데미지, 버프, 힐
    int iValue = 0; //dmg, heal
    int iDuration = 0; // 상태 지속 턴
    bool bMiss = false; //명중
    bool bCritical = false; //크리
    StatusType eStatus = StatusType::None; //상태 이상 종류
};

struct CombatEffectResult //스킬, 아이템 효과들 모음
{
    std::vector<CombatEffect> vecCombatEffect;
};

enum class EffectMovementType  //이펙트의 운동 종류
{ 
    Straight, 
    Parabola
};

enum class LogType
{
    None,
    Damage,
    Heal,
    CriticalDamage,
    Miss,
    StatusApply,
    StatusTick,
    StatusExpire,
    Death,
    SystemMessage,
    TurnStart,
    TurnEnd,
    Free
};

struct BattleLog
{
    std::wstring wstrAtkerName;
    std::wstring wstrTargetName;
    std::wstring wstrTxt;
    LogType eLogType = LogType::None;
    int iValue = 0;
    bool bCritical = false;
    bool bMiss = false;
};