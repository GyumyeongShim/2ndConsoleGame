#include "Renderer.h"

#include "Render/ScreenBuffer.h"
#include "Math/Color.h"

using namespace Wannabe;

Renderer::Renderer(const Vector2& screenSize)
	: m_vScreenSize(screenSize)
{
	//이중 버퍼 객체 생성
	m_ScreenBuffer[0] = new ScreenBuffer(screenSize);
	m_ScreenBuffer[0]->Clear();

	m_ScreenBuffer[1] = new ScreenBuffer(screenSize);
	m_ScreenBuffer[1]->Clear();

	// 활성화 버퍼 설정.
	SwapBuffer();
}

Renderer::~Renderer()
{
	for (ScreenBuffer*& buffer : m_ScreenBuffer)
		SafeDelete(buffer);
}

void Wannabe::Renderer::DrawCanvas(const Canvas& canvas)
{
	std::wstring stream = BuildAnsiStream(canvas);

	m_ScreenBuffer[m_iCurBufferIdx]->Draw(stream);

	SwapBuffer();
}

void Wannabe::Renderer::SwapBuffer()
{
    SetConsoleActiveScreenBuffer(GetCurBuffer()->GetBuffer());
    m_iCurBufferIdx = 1 - m_iCurBufferIdx;
}

ScreenBuffer* Renderer::GetCurBuffer()
{
	return m_ScreenBuffer[m_iCurBufferIdx];
}

std::wstring Wannabe::Renderer::BuildAnsiStream(const Canvas& canvas)
{
    const auto& buffer = canvas.GetMutableBuffer();

    int width = canvas.GetWidth();
    int height = canvas.GetHeight();

    std::wstring frameStream;
    frameStream.reserve(width * height * 12);

    Color lastColor = { 1,1,1 };

    for (int y = 0; y < height; ++y)
    {
        frameStream += L"\x1b[" + std::to_wstring(y + 1) + L";1H";

        for (int x = 0; x < width; ++x)
        {
            int index = y * width + x;

            const auto& cell = buffer[index];

            if (cell.color != lastColor)
            {
                frameStream += cell.color.ToAnsiFG();
                lastColor = cell.color;
            }

            frameStream += (cell.wch == L'\0' ? L' ' : cell.wch);
        }
    }

    frameStream += Color::Reset();

    return frameStream;
}