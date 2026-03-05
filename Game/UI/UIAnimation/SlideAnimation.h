#pragma once
#include "Interface/IUIAnimation.h"
#include "Math/Vector2.h"

class UI_TurnOrder;

class SlideAnimation : public Wannabe::IUIAnimation
{
public:
    SlideAnimation(UI_TurnOrder* owner, Wannabe::Vector2 targetOffset, float speed = 300.f);

    void Tick(float fDeltaTime) override;
    bool IsFinished() const override { return m_bFinished; }

private:
    UI_TurnOrder* m_pOwner = nullptr;
    Wannabe::Vector2 m_targetOffset;
    float m_fSpd = 300.f;
    bool m_bFinished = false;
};