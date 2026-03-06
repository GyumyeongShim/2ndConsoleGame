#pragma once

#include "Common/Common.h"

namespace Wannabe
{
	class WANNABE_API Input
	{
		friend class Engine; //엔진에만 private의 2 함수를 쓸 수 있게 하고싶은 경우
		//데이터에만
		struct KeyState
		{
			bool m_bIsKeyDown = false; // 현재 키 다운
			bool m_bWasKeyDown = false; // 이전에 키 다운
		};

	public:
		Input();
		~Input();

		//입력 확인 함수
		bool GetKeyDown(int KeyCode);  //누름
		bool GetKeyUP(int KeyCode); //눌렀다 뗌
		bool GetKey(int KeyCode); //눌려있음

		//전역적으로 접근 하는 함수
		static Input& Get(); // 포인터로 하면 사용처에서 변경하여 이슈 발생 가능성이 있음

	private:
		void ProcessInput(); //입력 처리
		void SavePreviousInputStates();

	private:
		//키 상태 저장용 배열
		KeyState m_KeyStates[255] = {};

		//전역적으로 접근하도록 만들기 위한 지역 변수
		static Input* Instance;

	};
}