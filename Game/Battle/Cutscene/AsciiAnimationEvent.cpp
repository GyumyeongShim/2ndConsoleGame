#include "AsciiAnimationEvent.h"

#include "Actor/Actor.h"
#include "Data/ActionData.h"
#include "Engine/Engine.h"
#include "Component/DisplayComponent.h"

AsciiAnimationEvent::AsciiAnimationEvent(Wannabe::Actor* pOwner, EAniType eType, float fFrameDelay)
    : m_pOwner(pOwner), m_eType(eType), m_fFrameDelay(fFrameDelay)
{
    m_iCurrentFrame = 0;
    m_fElapsed = 0.f;
    m_bIsFinished = false;
}

void AsciiAnimationEvent::OnStart(Wannabe::BattleContext& context)
{
    if (m_pOwner == nullptr)
    {
        m_bIsFinished = true;
        return;
    }

    auto* pDisplay = m_pOwner->GetComponent<Wannabe::DisplayComponent>();
    if (pDisplay)
    {
        // pDisplay가 소유한 CharacterVisualData에서 타입에 맞는 프레임 참조
        const auto& pFrame = pDisplay->GetAnimationFrames(m_eType);
        if (pFrame)
            m_frames = *pFrame;
    }

    if (m_frames.empty())
        m_bIsFinished = true;
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

    // 중앙 상자
    auto& renderSys = Wannabe::Engine::Get().GetRenderSystem();
    DrawBox(renderSys);

    // 현재 프레임 아스키 출력
    const auto& currentArt = m_frames[m_iCurrentFrame];
    int startY = BOX_POS.y + (BOX_HEIGHT - (int)currentArt.size()) / 2; //세로 중앙 정렬
    for (int i = 0; i < currentArt.size(); ++i)
    {
        int textWidth = (int)currentArt[i].length();
        int offsetX = (BOX_WIDTH - textWidth) / 2;

        Wannabe::Vector2 drawPos;
        drawPos.x = BOX_POS.x + offsetX;
        drawPos.y = startY + i;

        renderSys.DrawUI(currentArt[i], drawPos, Wannabe::Color::BrightCyan, 200);
    }

    return true;
}

void AsciiAnimationEvent::DrawBox(Wannabe::RenderSystem& renderSys)
{
    // 테두리 및 배경 소거 (포켓몬 대화창 느낌)
    std::wstring horizontal = L"+" + std::wstring(BOX_WIDTH - 2, L'-') + L"+";
    std::wstring emptyLine = L"|" + std::wstring(BOX_WIDTH - 2, L' ') + L"|";

    renderSys.DrawUI(horizontal, BOX_POS, Wannabe::Color::BrightGold, 190);
    for (int i = 1; i < BOX_HEIGHT - 1; ++i)
    {
        renderSys.DrawUI(emptyLine, { BOX_POS.x, BOX_POS.y + i }, Wannabe::Color::BrightGray, 190);
    }
    renderSys.DrawUI(horizontal, { BOX_POS.x, BOX_POS.y + BOX_HEIGHT - 1 }, Wannabe::Color::BrightGold, 190);
}