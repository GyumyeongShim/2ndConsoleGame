#include "AsciiAnimationEvent.h"

#include "Engine/Engine.h"

AsciiAnimationEvent::AsciiAnimationEvent(const std::vector<std::vector<std::wstring>>& frames, float fFrameDelay)
{
    m_iCurrentFrame = 0;
    m_fElapsed = 0.f;
    m_bIsFinished = false;
}

void AsciiAnimationEvent::OnStart(Wannabe::BattleContext& context)
{
    m_iCurrentFrame = 0;
    m_fElapsed = 0.f;
    m_bIsFinished = false;
}

bool AsciiAnimationEvent::Update(Wannabe::BattleContext& context, float fDeltaTime)
{
    if (m_bIsFinished) 
        return false;

    m_fElapsed += fDeltaTime;
    if (m_fElapsed >= m_fFrameDelay)
    {
        m_fElapsed = 0.f;
        m_iCurrentFrame++;

        if (m_iCurrentFrame >= m_frames.size())
        {
            m_bIsFinished = true;
            return false;
        }
    }

    auto& renderSys = Wannabe::Engine::Get().GetRenderSystem();
    DrawBox(renderSys);

    // 현재 프레임 아스키 출력
    const auto& currentArt = m_frames[m_iCurrentFrame];
    for (int i = 0; i < currentArt.size(); ++i)
    {
        // 박스 내부 중앙 정렬을 위해 오프셋 조정
        renderSys.DrawUI(currentArt[i], { BOX_POS.x + 2, BOX_POS.y + 1 + i },
            Wannabe::Color::White, 200);
    }

    return true;
}

void AsciiAnimationEvent::DrawBox(Wannabe::RenderSystem& renderSys)
{
    // 테두리 및 배경 소거 (포켓몬 대화창 느낌)
    std::wstring horizontal = L"+" + std::wstring(BOX_WIDTH - 2, L'-') + L"+";
    std::wstring emptyLine = L"|" + std::wstring(BOX_WIDTH - 2, L' ') + L"|";

    renderSys.DrawUI(horizontal, BOX_POS, Wannabe::Color::White, 190);
    for (int i = 1; i < BOX_HEIGHT - 1; ++i)
    {
        renderSys.DrawUI(emptyLine, { BOX_POS.x, BOX_POS.y + i }, Wannabe::Color::White, 190);
    }
    renderSys.DrawUI(horizontal, { BOX_POS.x, BOX_POS.y + BOX_HEIGHT - 1 }, Wannabe::Color::White, 190);
}
