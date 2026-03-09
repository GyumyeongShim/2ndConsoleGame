#pragma once
#include <vector>
#include <string>
#include "Interface/ICutsceneEvent.h"
#include "Math/Vector2.h"

namespace Wannabe
{
    class RenderSystem;
}

class AsciiAnimationEvent : public Wannabe::ICutsceneEvent
{
public:
    // 애니메이션 프레임 리스트, 프레임당 재생 시간, 중앙 박스 위치
    AsciiAnimationEvent(const std::vector<std::vector<std::wstring>>& frames, float fFrameDelay);

    virtual void OnStart(Wannabe::BattleContext& context) override;
    virtual bool Update(Wannabe::BattleContext& context, float fDeltaTime) override;
    virtual bool IsFinished() const override { return m_bIsFinished; }
    virtual CutsceneEventType GetEventType() override
    {
        m_eCutsceneEventType = CutsceneEventType::AsciiAnimation;
        return m_eCutsceneEventType;
    };

private:
    void DrawBox(Wannabe::RenderSystem& renderSys);

private:
    std::vector<std::vector<std::wstring>> m_frames;
    float m_fFrameDelay = 0.1f;
    float m_fElapsed = 0.f;
    int m_iCurrentFrame = 0;
    bool m_bIsFinished = false;

    // 중앙 박스 규격 (포켓몬 스타일)
    const int BOX_WIDTH = 30;
    const int BOX_HEIGHT = 10;
    const Wannabe::Vector2 BOX_POS = { 55, 12 }; // 140x40 기준 중앙 부근
};