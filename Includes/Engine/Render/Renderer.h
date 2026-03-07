#pragma once
#include <iostream>
#include <vector>

#include "Util/Utill.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Render/Canvas.h"

namespace Wannabe
{
	class ScreenBuffer; //콘솔 버퍼를 관리하는 클래스
	class WANNABE_API Renderer //더블 버퍼링을 지원하는 렌더러 클래스
	{
		struct Frame
		{
			Frame(int iBufferCnt) { m_pCharInfoArray = new CHAR_INFO[iBufferCnt]; }
			~Frame() { SafeDeleteArray(m_pCharInfoArray); }

			void Clear(int iCnt)
			{
				for (int i = 0; i < iCnt; ++i)
				{
					m_pCharInfoArray[i].Char.UnicodeChar = L' ';
					m_pCharInfoArray[i].Attributes = 0;
				}
			}

			CHAR_INFO* m_pCharInfoArray = nullptr;//글자 값과글자의 색상을 갖는 타입
		};

		struct ViewPort
		{
			Vector2 m_vOrigin; //화면 내 시작 위치
			Vector2 m_vSize; //영역 크기
		};

		public:
			Renderer(const Vector2& screenSize);
			~Renderer();

			void BeginFrame();
			void Draw(Canvas& canvas);
			void EndFrame(const std::vector<CHAR_INFO>& buffer);

			void ResetViewport();

			Vector2 GetScreenSize() { return m_vScreenSize; }

		private:
			void Clear(); //화면 지우기
			void Present(); //더블 버퍼링을 활용해 활성화 버퍼를 교환하는 함수
			ScreenBuffer* GetCurBuffer(); //현재 사용할 버퍼를 반환하는 함수 getter

		private:
			Vector2 m_vScreenSize; // 화면 크기
			Frame* frame = nullptr; //관리할 프레임 객체
			
			int m_iCurBufferIdx = 0; // 현재 활성화된 버퍼 인덱스
			ScreenBuffer* m_ScreenBuffer[2] = {}; //이중 버퍼 배열

			ViewPort m_viewport;
	};
}