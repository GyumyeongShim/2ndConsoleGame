#include "Renderer.h"
#include "Render/ScreenBuffer.h"
#include "Util/Utill.h"

using namespace Wannabe;

Renderer::Renderer(const Vector2& screenSize)
	: m_vScreenSize(screenSize)
{
	const int bufferCnt = screenSize.x * screenSize.y;
	frame = new Frame(bufferCnt);

	// 프레임 초기화
	frame->Clear(bufferCnt);

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
	SafeDelete(frame);
	for (ScreenBuffer*& buffer : m_ScreenBuffer)
	{
		SafeDelete(buffer);
	}
}

void Renderer::BeginFrame()
{
	// Frame buffer 초기화 (dirty pipeline 시작점)
	int total = m_vScreenSize.x * m_vScreenSize.y;
	frame->Clear(total);
}

void Renderer::Draw(Canvas& canvas)
{
	auto& buffer = canvas.GetMutableBuffer();

	//cell 개수, canvas가 더 작은 경우도 처리
	int maxCnt = m_vScreenSize.x * m_vScreenSize.y;
	int cnt = std::min(maxCnt, (int)buffer.size());

	for (int i = 0; i < cnt; ++i)
	{
		const CanvasCell& cell = buffer[i];

		if (cell.bDirty == false) //교체 여부
			continue;

		frame->m_pCharInfoArray[i].Char.UnicodeChar = cell.wch;
		frame->m_pCharInfoArray[i].Attributes = (WORD)cell.color;
	}
}

void Renderer::EndFrame(const std::vector<CHAR_INFO>& buffer)
{
	m_ScreenBuffer[m_iCurBufferIdx]->Draw(const_cast<CHAR_INFO*>(buffer.data()));

	Present();
}

void Renderer::Clear()
{
	GetCurBuffer()->Clear(); //콘솔 버퍼 지우기
	frame->Clear(m_vScreenSize.x * m_vScreenSize.y);
}

void Renderer::Present()
{
	// 버퍼 교체
	SetConsoleActiveScreenBuffer(GetCurBuffer()->GetBuffer());

	//인덱스 교체 , (1-0)
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