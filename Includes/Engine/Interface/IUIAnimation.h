#pragma once
#include "Common/Common.h"

namespace Wannabe
{
    class IUIAnimation
    {
    public:
        virtual ~IUIAnimation() = default;
        virtual void Tick(float fDeltaTime) = 0;
        virtual bool IsFinished() const = 0;
    };
}