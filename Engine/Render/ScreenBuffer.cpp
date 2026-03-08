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

	// 1. 가상 터미널 모드 활성화 (매우 중요)
	DWORD dwMode = 0;
	GetConsoleMode(m_hBuffer, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(m_hBuffer, dwMode);

	// 2. ANSI를 이용한 창 크기 및 버퍼 크기 강제 고정
	//std::wstring resizeCmd = L"\x1b[8;" + std::to_wstring((int)screenSize.y) +
	//	L";" + std::to_wstring((int)screenSize.x) + L"t";
	std::wstring resizeCmd = L"\x1b[?7l\x1b[?1049h";

	DWORD written = 0;
	WriteConsoleW(m_hBuffer, resizeCmd.c_str(), (DWORD)resizeCmd.size(), &written, nullptr);

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
	DWORD written = 0;
	std::wstring clearCmd = L"\x1b[2J";
	WriteConsoleW(m_hBuffer, clearCmd.c_str(), (DWORD)clearCmd.size(), &written, nullptr);
}

void Wannabe::ScreenBuffer::Draw(const std::wstring& fullFrameStream)
{
	// 1. 커서를 매 프레임 맨 위(0,0)로 강제 이동 (ANSI Escape 사용)
	std::wstring finalOutput = L"\x1b[H" + fullFrameStream;
	DWORD written = 0;
	WriteConsoleW(
		m_hBuffer,
		finalOutput.c_str(),
		static_cast<DWORD>(finalOutput.size()),
		&written,
		nullptr
	);
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