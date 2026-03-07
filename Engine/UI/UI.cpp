#include "UI.h"
#include "Engine/Engine.h"
#include "Render/RenderSystem.h"
#include "Interface/IUIAnimation.h"

using namespace Wannabe;

UI::UI()
{
    m_pRenderSystem = &Engine::Get().GetRenderSystem();
}

UI::UI(const wchar_t* txt, Color color)
    :super(txt, Vector2::Zero, color)
{
    //UI는 항상 최상위에 덮어진다.
    m_SortingOrder = 100;
}

UI::~UI()
{
}

void UI::Tick(float fDeltaTime)
{
    super::Tick(fDeltaTime);

    UpdateViewportTransform();

    if (m_pAni == nullptr)
        return;

    m_pAni->Tick(fDeltaTime);
    if (m_pAni->IsFinished())
    {
        delete m_pAni;
        m_pAni = nullptr;
    }
}

void UI::Draw(Wannabe::RenderSystem& renderSys)
{
    super::Draw(renderSys);
}

void UI::UpdateViewportTransform()
{
    if (m_pRenderSystem == nullptr)
        return;

    RecalculateViewportPosition();

    if (m_eAniState == UIAniState::Idle)
    {
        SetPosition(m_cachedViewportPos);
    }
}

void UI::SetAnchor(UIAnchor anchor)
{
    m_anchor = anchor;
}

void UI::SetOffset(Vector2 offset)
{
    m_offset = offset;
}

void UI::SetRelative(Vector2 relative)
{
    m_relative = relative;
}

Vector2 UI::GetViewportPosition() const
{
    Vector2 screen = m_pRenderSystem->GetScreenSize();

    Vector2 base = { 0,0 };

    switch (m_anchor)
    {
    case UIAnchor::TopLeft:
        base = { 0,0 };
        break;

    case UIAnchor::TopRight:
        base = { screen.x, 0 };
        break;

    case UIAnchor::BottomLeft:
        base = { 0, screen.y };
        break;

    case UIAnchor::BottomRight:
        base = { screen.x, screen.y };
        break;

    case UIAnchor::Center:
        base = { screen.x / 2, screen.y / 2 };
        break;
    }

    Vector2 relativePos =
    {
        static_cast<int>(screen.x * m_relative.x),
        static_cast<int>(screen.y * m_relative.y)
    };

    return base + relativePos + m_offset;
}
