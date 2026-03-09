#pragma once
#include "Core/Common.h"

namespace Wannabe
{
    class BattleContext;

    class ICutsceneEvent
    {
    public:
        enum class CutsceneEventType
        {
            None,
            Log,
            VisualEffect,
            BattlePhaseChange,
            Damage,
            TurnStart,
            TurnEnd,
            Delay,
            Death,
            AsciiParticle,
            AsciiAnimation,
            Move
        };

        virtual ~ICutsceneEvent() = default;

        virtual void OnStart(BattleContext& context) {};
        virtual bool Update(BattleContext& context, float fDeltaTime) = 0;
        virtual void OnEnd(BattleContext& context) {};
        virtual bool IsValid(BattleContext& context) const { return true; }
        virtual bool IsFinished() const { return true; }
        virtual bool CanSkip() const { return true; }
        virtual CutsceneEventType GetEventType() = 0;

    public:
        CutsceneEventType m_eCutsceneEventType = CutsceneEventType::None;
    };
}