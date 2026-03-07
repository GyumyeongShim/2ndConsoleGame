#pragma once
#include "Core/RTTI.h"
#include "Core/Common.h"

namespace Wannabe
{
    class Actor;
    class RenderSystem;

    class WANNABE_API Component : public RTTI
    {
        RTTI_DECLARATIONS(Component, RTTI);

    public:
        Component();
        virtual ~Component();

        virtual void Init() {}
        virtual void BeginPlay() {}
        virtual void Update(float fDeltaTime) {}
        virtual void Render(Wannabe::RenderSystem& renderSys) {}

        // 소유자 설정 및 접근
        void SetOwner(Actor* pOwner) { m_pOwner = pOwner; }
        Actor* GetOwner() const { return m_pOwner; }

    protected:
        Actor* m_pOwner = nullptr;
    };
}