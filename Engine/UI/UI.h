#pragma once
#include "Actor/Actor.h"

namespace Wannabe
{
    class IUIAnimation;
    class RenderSystem;

    class WANNABE_API UI : public Actor
    {
        RTTI_DECLARATIONS(UI, Actor)

        enum class UIAnchor
        {
            TopLeft,
            TopRight,
            BottomLeft,
            BottomRight,
            Center
        };

        enum class UIAniState
        {
            Idle,
            Playing,
            Finished
        };

    public:
        UI();
        UI(const wchar_t* txt, Color color);
        virtual ~UI();

        virtual void Tick(float fDeltaTime) override;
        virtual void Draw(Wannabe::RenderSystem& renderSys) override;

        void SetAnchor(UIAnchor anchor);
        void SetOffset(Vector2 offset);
        void SetRelative(Vector2 relative); // 0~1

        virtual void RecalculateViewportPosition() { m_cachedViewportPos = GetViewportPosition(); }
        Vector2 GetOffset() const { return m_offset; }

        virtual void SetRenderSystem(RenderSystem* renderSystem) { m_pRenderSystem = renderSystem; }
    protected:
        virtual void UpdateViewportTransform();
        Vector2 GetViewportPosition() const;

    protected:
        UIAnchor m_anchor = UIAnchor::TopLeft;

        Vector2 m_offset = { 0,0 };
        Vector2 m_relative = { 0,0 }; // ∫Ò¿≤

        Vector2 m_cachedViewportPos;

        UIAniState m_eAniState = UIAniState::Idle;
        IUIAnimation* m_pAni = nullptr;

        RenderSystem* m_pRenderSystem = nullptr;
    };
}