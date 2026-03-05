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

    //ApplyEffect();

    const auto& buffer = m_FinalCanvas.GetMutableBuffer();

    std::vector<CHAR_INFO> charInfoBuffer;
    charInfoBuffer.reserve(buffer.size());

    for (const auto& cell : buffer)
    {
        CHAR_INFO info{};
        info.Char.UnicodeChar = cell.wch;
        info.Attributes = static_cast<WORD>(cell.color);

        charInfoBuffer.emplace_back(info);
    }

    m_Renderer.EndFrame(charInfoBuffer);
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

    CanvasCell cell;
    cell.wch = actor.GetGlyph();
    cell.color = actor.GetGlyphColor();

    m_WorldCanvas.SetCell(screen.x, screen.y, cell);
}

void RenderSystem::DrawBattleActor(const Actor& actor)
{
    if (actor.GetDisplay() == nullptr)
        return;

    Vector2 world = actor.GetPosition();
    Vector2 screen = m_Camera.WorldToScreen(world); // 객체 사용
    if (!IsInViewport(screen, actor))
        return;

    const DisplayComponent* display = actor.GetDisplay();

    // 1. 메인 이미지
    int yOffset = 0;
    for (const auto& line : display->GetAscii())
    {
        m_WorldCanvas.DrawTxt(screen.x, screen.y + yOffset, line, display->GetAsciiColor(), actor.GetSortingOrder());

        yOffset++;
    }

    // 2. 이름
    if (!display->GetName().empty())
    {
        m_WorldCanvas.DrawTxt(screen.x, screen.y - 1,display->GetName(),display->GetNameColor(), actor.GetSortingOrder() + 1);
    }
}

void RenderSystem::DrawUI(const std::wstring& txt, Vector2 pos, Color color, int iOrder)
{
    m_UICanvas.DrawTxt(pos.x, pos.y, txt, color, iOrder);
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

void RenderSystem::ApplyEffect()
{
    auto& buffer = m_FinalCanvas.GetMutableBuffer();
    int cnt = static_cast<int>(buffer.size());

    for (int i = 0; i < cnt; ++i)
    {
        CanvasCell& cell = buffer[i];
        //조건문 추가해서 진행할 것
        cell.color = InvertColor((WORD)cell.color);
        cell.bDirty = true;
    }
}

Color RenderSystem::InvertColor(WORD color)
{
    //foreground  background swap
    Color fg = static_cast<Color>(color & 0x0F);
    Color bg = static_cast<Color>((color & 0xF0) >> 4);
    Color newFg = bg;
    Color newBg = fg;

    return static_cast<Color>((static_cast<unsigned short>(newBg) << 4) | static_cast<unsigned short>(newFg));
}

bool RenderSystem::IsInViewport(const Vector2& pos, const Actor& actor) const
{
    int width = m_Camera.GetWidth();
    int height = m_Camera.GetHeight();

    const DisplayComponent* display = actor.GetDisplay();
    int asciiHeight = display ? static_cast<int>(display->GetAscii().size()) : 1;

    // 화면 밖이면 false
    if (pos.x < 0 || pos.y + asciiHeight < 0)
        return false;
    if (pos.x >= width || pos.y >= height)
        return false;

    return true;
}

std::vector<CHAR_INFO> ConvertCanvasBufferToCharInfo(const std::vector<Wannabe::CanvasCell>& buffer)
{
    std::vector<CHAR_INFO> charInfoBuffer;
    charInfoBuffer.reserve(buffer.size());
    for (const auto& cell : buffer)
    {
        CHAR_INFO info{};
        info.Char.UnicodeChar = cell.wch;
        info.Attributes = static_cast<WORD>(cell.color);
        charInfoBuffer.emplace_back(info);
    }
    return charInfoBuffer;
}