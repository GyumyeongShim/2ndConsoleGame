#include <iostream>

#include "ScreenBuffer.h"
#include "Render/Renderer.h"

using namespace Wannabe;

ScreenBuffer::ScreenBuffer(const Vector2& screenSize)
	:m_vScrrenSize(screenSize)
{
	// Console Output 생성.
	m_hBuffer = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		CONSOLE_TEXTMODE_BUFFER,
		nullptr
	);

	// 예외 처리.
	if (m_hBuffer == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(nullptr, "BufferCreate Error", "Error", MB_OK);
		__debugbreak();
	}

	// 콘솔 창 크기 먼저 최소로 줄이기
	SMALL_RECT rect = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(m_hBuffer, TRUE, &rect);

	// 버퍼 크기 설정.
	if (!SetConsoleScreenBufferSize(m_hBuffer, screenSize))
	{
		std::cerr << "Failed to set console buffer size.\n";
		__debugbreak();
	}

	// 콘솔 창 크기 지정
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = static_cast<int>(screenSize.x - 1);
	rect.Bottom = static_cast<int>(screenSize.y - 1);

	if (!SetConsoleWindowInfo(m_hBuffer, true, &rect))
	{
		//DWORD errorCode = GetLastError();
		std::cerr << "Failed to set console window info.\n";
		__debugbreak();
	}

	// 커서 끄기.
	CONSOLE_CURSOR_INFO info;
	GetConsoleCursorInfo(m_hBuffer, &info);

	// 끄도록 설정.
	info.bVisible = false;
	SetConsoleCursorInfo(m_hBuffer, &info);
}

ScreenBuffer::~ScreenBuffer()
{
	if (m_hBuffer)
	{
		CloseHandle(m_hBuffer);
	}
}

void ScreenBuffer::Clear()
{
	//그래픽스 -> 지우기 == 한 색상 또는 값 으로 덮어쓰기
	DWORD cnt = 0;

	FillConsoleOutputCharacterW(m_hBuffer,L' ',m_vScrrenSize.x * m_vScrrenSize.y,Vector2::Zero, &cnt);
}

void ScreenBuffer::Draw(CHAR_INFO* charInfo)
{
	SMALL_RECT writeRegion = {};
	writeRegion.Left = 0;
	writeRegion.Top = 0;
	writeRegion.Right = static_cast<short>(m_vScrrenSize.x - 1);
	writeRegion.Bottom = static_cast<short>(m_vScrrenSize.y - 1);

	WriteConsoleOutputW(m_hBuffer, charInfo, m_vScrrenSize, Vector2::Zero, &writeRegion);
}

void ScreenBuffer::Resize(const Vector2& newSize)
{
	if (m_hBuffer) // 기존 버퍼 제거
	{
		CloseHandle(m_hBuffer);
		m_hBuffer = nullptr;
	}

	// todo 폰트 크기 변경으로 Main <->Battle 차이를 만들수 있을듯)
	// flag 필요 신규 레벨이 생긴다면 a<->b<->c 각각 폰트 크기가 다를 수 있다.
	// 이때 책임을 어떻게 할것인가?
	m_vScrrenSize = newSize; 
	Init(newSize);
}

void ScreenBuffer::Init(const Vector2& screenSize)
{
	// Console Output 생성.
	m_hBuffer = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		CONSOLE_TEXTMODE_BUFFER,
		nullptr
	);

	if (m_hBuffer == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(nullptr, "BufferCreate Error", "Error", MB_OK);
		__debugbreak();
	}

	if (!SetConsoleScreenBufferSize(m_hBuffer, screenSize))
	{
		std::cerr << "Failed to set console buffer size.\n";
		__debugbreak();
	}

	SMALL_RECT rect;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = static_cast<int>(screenSize.x - 1);
	rect.Bottom = static_cast<int>(screenSize.y - 1);

	if (!SetConsoleWindowInfo(m_hBuffer, true, &rect))
	{
		//DWORD errorCode = GetLastError();
		std::cerr << "Failed to set console window info.\n";
		__debugbreak();
	}

	CONSOLE_CURSOR_INFO info;
	GetConsoleCursorInfo(m_hBuffer, &info);

	info.bVisible = false;
	SetConsoleCursorInfo(m_hBuffer, &info);
}