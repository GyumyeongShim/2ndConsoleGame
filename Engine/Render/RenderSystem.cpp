#include <windows.h>

#include "RenderSystem.h"
#include "Component/DisplayComponent.h"

using namespace Wannabe;

RenderSystem::RenderSystem(Renderer& renderer)
    : m_Renderer(renderer), m_Camera(renderer.GetScreenSize().x,renderer.GetScreenSize().y),
    m_WorldCanvas(renderer.GetScreenSize().x, renderer.GetScreenSize().y),
    m_UICanvas(renderer.GetScreenSize().x, renderer.GetScreenSize().y),
    m_DebugCanvas(renderer.GetScreenSize().x, renderer.GetScreenSize().y),
    m_EffectCanvas(renderer.GetScreenSize().x, renderer.GetScreenSize().y),
    m_FinalCanvas(renderer.GetScreenSize().x, renderer.GetScreenSize().y)
{
}

void RenderSystem::BeginFrame()
{
    m_WorldCanvas.Clear();
    m_UICanvas.Clear();
    m_DebugCanvas.Clear();
    m_EffectCanvas.Clear();
    m_FinalCanvas.Clear();
}

void RenderSystem::RenderFrame()
{
    Composite(m_WorldCanvas);
    Composite(m_UICanvas);
    Composite(m_DebugCanvas);
    Composite(m_EffectCanvas);

    // 렌더러에 최종 스트림 전달
    m_Renderer.DrawCanvas(m_FinalCanvas);
}

void RenderSystem::DrawActor(const Actor& actor)
{
    if (m_eRenderMode == RenderMode::Field)
    {
        DrawFieldActor(actor);
    }
    else
    {
        DrawBattleActor(actor);
    }
}

void RenderSystem::DrawFieldActor(const Actor& actor)
{
    Vector2 world = actor.GetPosition();
    Vector2 screen = m_Camera.WorldToScreen(world);

    if (!IsInViewport(screen, actor))
        return;

    m_WorldCanvas.DrawAscii(screen.x, screen.y, actor.GetGlyph(), 
        actor.GetGlyphColor(), actor.GetSortingOrder());
}

void RenderSystem::DrawBattleActor(const Actor& actor)
{
    if (actor.GetComponent<DisplayComponent>() == nullptr)
        return;

    Vector2 world = actor.GetPosition();
    Vector2 screen = m_Camera.WorldToScreen(world) + m_Shake.offset; //흔들림 처리
    if (IsInViewport(screen, actor) == false)
        return;

    const DisplayComponent* display = actor.GetComponent<DisplayComponent>();

    // 1. 메인 이미지
    int yOffset = 0;
    for (const auto& line : display->GetAscii())
    {
        m_WorldCanvas.DrawTxt(screen.x, screen.y + yOffset, line, display->GetAsciiColor(), actor.GetSortingOrder());

        yOffset++;
    }
}

void RenderSystem::DrawUI(const std::wstring& txt, Vector2 pos, Color color, int iOrder)
{
    m_UICanvas.DrawTxt(pos.x, pos.y, txt, color, iOrder);
}

void RenderSystem::Shake(float fDuration, float fIntensity)
{
    m_Shake.fDuration = fDuration;
    m_Shake.fIntensity = fIntensity;
}

void RenderSystem::Update(float fDeltaTime)
{
    if (m_Shake.fDuration > 0.0f)
    {
        m_Shake.fDuration -= fDeltaTime;

        int range = static_cast<int>(m_Shake.fIntensity);
        if (range < 1) 
            range = 1;

        m_Shake.offset.x = (rand() % (range * 2 + 1) - range);
        m_Shake.offset.y = (rand() % (range * 2 + 1) - range);
    }
    else
    {
        m_Shake.offset = Vector2::Zero;
    }
}

void RenderSystem::Composite(const Canvas& canvas)
{
    const int width = m_FinalCanvas.GetWidth();
    const int height = m_FinalCanvas.GetHeight();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const CanvasCell& cell = canvas.GetCell(x, y);

            if (cell.wch == L'\0')
                continue;

            CanvasCell& dst = m_FinalCanvas.GetMutableCell(x, y);
            if (cell.sortingOrder >= dst.sortingOrder)
            {
                dst = cell;
                dst.bDirty = true;
            }
        }
    }
}

bool RenderSystem::IsInViewport(const Vector2& pos, const Actor& actor) const
{
    int width = m_Camera.GetWidth();
    int height = m_Camera.GetHeight();

    const DisplayComponent* display = actor.GetComponent<DisplayComponent>();
    int asciiHeight = display ? static_cast<int>(display->GetAscii().size()) : 1;

    // 화면 밖이면 false
    if (pos.x < 0 || pos.y + asciiHeight < 0)
        return false;
    if (pos.x >= width || pos.y >= height)
        return false;

    return true;
}