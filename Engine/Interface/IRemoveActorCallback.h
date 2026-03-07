#pragma once
#include "Core/Common.h"

namespace Wannabe
{
    class Actor;

    class WANNABE_API IRemoveActorCallback
    {
    public:
        virtual ~IRemoveActorCallback() = default;

        virtual void OnRemoveActor(Actor* actor) = 0;
    };
}