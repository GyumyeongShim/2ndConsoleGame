#pragma once
#include "Interface/ICutsceneEvent.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include "Enum/CombatType.h"

class BattleContext;

class VisualEffectEvent : public Wannabe::ICutsceneEvent
{
public:
    VisualEffectEvent(Wannabe::Vector2 start, Wannabe::Vector2 end, 
        EffectMovementType eType, wchar_t txt, Wannabe::Color color, float fDuration);

    virtual void OnStart(Wannabe::BattleContext& context) override;
    virtual bool Update(Wannabe::BattleContext& context, float fDeltaTime) override;
    virtual void OnEnd(Wannabe::BattleContext& context) override;
    virtual bool IsValid(Wannabe::BattleContext& context) const override;

private:
    Wannabe::Vector2 m_StartPos;
    Wannabe::Vector2 m_EndPos;
    Wannabe::Vector2 m_CurrentPos; // 현재 그려질 위치
    Wannabe::Vector2 m_LastPos;    // 이전 프레임 위치 (지우기 용)

    EffectMovementType m_eMoveType;
    wchar_t m_Symbol;
    Wannabe::Color m_Color;

    float m_fDuration = 0.f;
    float m_fElapsed = 0.f;
};