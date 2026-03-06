#pragma once
#include "Enum/CombatType.h"

enum class ActionTargetType // 타겟 종류
{
    None,
    Self,           // 자신
    SingleAlly,     // 아군 단일
    SingleEnemy,    // 적군 단일
    AllAlly,        // 아군 전체
    AllEnemy,       // 적군 전체
    RandomEnemy,    // 적군 중 랜덤
    Area            // 특정 범위 (필요시 확장)
};

struct ActionVisualData
{
    wchar_t asciiSymbol = L'*';
    Wannabe::Color color = Wannabe::Color::White;

    float fParticleSpd = 10.f;
    int iParticleRange = 5;
};