#pragma once
#include "UI/UI.h"

class UI_FloatingText : public Wannabe::UI
{
public:
    UI_FloatingText(const std::wstring& text, Wannabe::Vector2 worldPos, Wannabe::Color color, float fLifeTime = 0.8f);

    virtual void Tick(float fDeltaTime) override;
    virtual void Draw(Wannabe::RenderSystem& renderSys) override;
    virtual void RecalculateViewportPosition() override;

private:
    std::wstring m_wstrText;
    Wannabe::Vector2 m_vWorldPos;
    Wannabe::Color m_Color;
    float m_fLifeTime;
    float m_fMaxLifeTime;
};