#pragma once
#include "Common/Common.h"

namespace Wannabe
{
    class BattleContext;

    class ICutsceneEvent
    {
    public:
        virtual ~ICutsceneEvent() = default;

        virtual void OnStart(BattleContext& context) {};
        virtual bool Update(BattleContext& context, float fDeltaTime) = 0;
        virtual void OnEnd(BattleContext& context) {};
        virtual bool IsValid(BattleContext& context) const { return true; }
        virtual bool IsFinished() const { return true; }
        virtual bool CanSkip() const { return true; }
    };
}