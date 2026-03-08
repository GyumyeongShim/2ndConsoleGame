#include "Renderer.h"
#include "Render/ScreenBuffer.h"
#include "Util/Utill.h"

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
	Present();
}

Renderer::~Renderer()
{
	for (ScreenBuffer*& buffer : m_ScreenBuffer)
		SafeDelete(buffer);
}

void Renderer::BeginFrame()
{
}

void Renderer::EndFrame(const std::wstring& frameStream)
{
	m_ScreenBuffer[m_iCurBufferIdx]->Draw(frameStream);
	Present();
}

void Renderer::Present()
{
	SetConsoleActiveScreenBuffer(GetCurBuffer()->GetBuffer());

	m_iCurBufferIdx = 1 - m_iCurBufferIdx;
}

ScreenBuffer* Renderer::GetCurBuffer()
{
	return m_ScreenBuffer[m_iCurBufferIdx];
}

void Renderer::ResetViewport()
{
	m_viewport.m_vOrigin = Vector2(0, 0);
	m_viewport.m_vSize = m_vScreenSize;
}