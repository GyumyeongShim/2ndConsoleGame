#pragma once
#include <time.h>
#include <string>

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

	inline void SetConsoleTextColor(Color color)
	{
		SetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
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