#include "UI_FloatingText.h"

#include "Math/Color.h"
#include "Math/Vector2.h"

#include "Render/RenderSystem.h"

UI_FloatingText::UI_FloatingText(const std::wstring& text, Wannabe::Vector2 worldPos, Wannabe::Color color, float fLifeTime)
    : m_wstrText(text), m_vWorldPos(worldPos), m_Color(color), m_fLifeTime(fLifeTime)
{
    m_fMaxLifeTime = fLifeTime;
}

void UI_FloatingText::Tick(float fDeltaTime)
{
    m_fLifeTime -= fDeltaTime;
    m_vWorldPos.y -= static_cast<int>(0.5f * fDeltaTime);

    RecalculateViewportPosition();

    if (m_fLifeTime <= 0.0f)
        this->Destroy();
}

void UI_FloatingText::Draw(Wannabe::RenderSystem& renderSys)
{
    if (m_bDestroyRequested == true)
        return;

    renderSys.DrawUI(m_wstrText, m_cachedViewportPos, m_Color);
}

void UI_FloatingText::RecalculateViewportPosition()
{
    m_cachedViewportPos = m_vWorldPos + Vector2(0, -4);
}