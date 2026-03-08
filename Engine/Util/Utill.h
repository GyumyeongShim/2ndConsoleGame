#pragma once
#include <time.h>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

#include "Math/Vector2.h"
#include "Math/Color.h"

using namespace Wannabe;
//헬퍼 기능 제공
namespace Util
{
	//콘솔 커서 위치 지정
	inline void SetConsolePosition(const Vector2& position)
	{
		//h cpp로 나누거나, inline, static으로 선언해야 multiple definition 오류가 안남
		//inline은 다음과 같이 콜 스택이 많으면 안 해줄 수도 있음...
		SetConsoleCursorPosition(
			GetStdHandle(STD_OUTPUT_HANDLE), static_cast<COORD>(position));
	}

	//커서 끄기
	inline void TurnOffCursor()
	{
		// 커서 끄기
		CONSOLE_CURSOR_INFO info = {};

		GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

		info.bVisible = FALSE;
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	}

	//커서 켜기
	inline void TurnOnCursor()
	{
		// 커서 끄기
		CONSOLE_CURSOR_INFO info = {};

		GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

		info.bVisible = TRUE;
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	}

	inline void ClearScreen()
	{
		system("cls");
	}

	inline void SetRandomSeed()
	{
		srand(static_cast<unsigned int>(time(nullptr)));
	}

	inline std::string GetCurrentDateTimeString()
	{
		// 1. 시스템 현재 시간 획득
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);

		// 2. 시간 정보를 구조체로 변환 (localtime_s는 Windows 환경에서 안전함)
		struct tm timeInfo;
		localtime_s(&timeInfo, &now_c);

		// 3. 문자열 스트림을 이용해 포맷팅 (YYYY-MM-DD HH:MM:SS)
		std::stringstream ss;
		ss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");

		return ss.str();
	}

	inline int Random(int min, int max)
	{
		int diff = (max - min) + 1;

		return ((diff * rand()) / (RAND_MAX + 1)) + min;
	}

	inline float Random(float min, float max)
	{
		float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float diff = (max - min);

		return (random * diff) + min;
	}
}

template<typename T>
void SafeDelete(T*& t)
{
	if (t)
	{
		delete t;
		t = nullptr;
	}
}

template<typename T>
void SafeDeleteArray(T*& t)
{
	if (t)
	{
		delete[] t;
		t = nullptr;
	}
}