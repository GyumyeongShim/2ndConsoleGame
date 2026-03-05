#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <iostream>

#include "Input.h"

namespace Wannabe
{
	Input* Input::Instance = nullptr;

	Input::Input()
	{
		Instance = this; //객체가 초기화되면 자기 자신의 주소를 저장
	}

	Input::~Input()
	{
	}

	bool Input::GetKeyDown(int KeyCode)
	{
		return m_KeyStates[KeyCode].m_bIsKeyDown && !m_KeyStates[KeyCode].m_bWasKeyDown;
	}

	bool Input::GetKeyUP(int KeyCode)
	{
		return !m_KeyStates[KeyCode].m_bIsKeyDown && m_KeyStates[KeyCode].m_bWasKeyDown;
	}

	bool Input::GetKey(int KeyCode)
	{
		return m_KeyStates[KeyCode].m_bIsKeyDown;
	}

	Input& Input::Get()
	{
		//싱글톤
		//콘텐츠 프로젝트에서 접근함
		//따라서 엔진은 이미 초기화된상태
		if (!Instance)
		{
			std::cout << "Error: Input::Get() instance is null\n";
			__debugbreak();
		}

		//lazy pattern
		//static Input instance; //실제 사용할 시점에 생성됨

		return *Instance;
	}

	void Input::ProcessInput()
	{
		// 키 마디의 입력 읽기
		//운영체제가 제공하는 기능을 사용할 수 밖에 없음
		for (int ix = 0; ix < 255; ++ix)
		{
			m_KeyStates[ix].m_bIsKeyDown = (GetAsyncKeyState(ix) & 0x8000) > 0 ? true : false;
		}
	}

	void Input::SavePreviousInputStates()
	{
		//현재 입력값을 이전 입력 값으로 저장.
		for (int ix = 0; ix < 255; ++ix)
		{
			m_KeyStates[ix].m_bWasKeyDown = m_KeyStates[ix].m_bIsKeyDown;
		}
	}
}
