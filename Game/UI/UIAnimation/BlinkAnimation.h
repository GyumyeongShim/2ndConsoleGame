#pragma once
#include "Interface/IUIAnimation.h"

class UI_TargetCursor;

class BlinkAnimation : public Wannabe::IUIAnimation
{
public:
    BlinkAnimation(UI_TargetCursor* owner, float blinkInterval = 0.4f);

    void Tick(float fDeltaTime) override;
    bool IsFinished() const override { return false; } //글자창 깜빡임은 일반적으로 무한

private:
    UI_TargetCursor* m_pOwner;

    float m_fInterval = 0.f;
    float m_fTimer = 0.f;

    bool m_bVisible = true;
};