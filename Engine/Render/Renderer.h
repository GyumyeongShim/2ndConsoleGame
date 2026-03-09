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
		public:
			Renderer(const Vector2& screenSize);
			~Renderer();

			void DrawCanvas(const Canvas& canvas);
			Vector2 GetScreenSize() { return m_vScreenSize; }

		private:
			void Clear(); //화면 지우기
			void SwapBuffer(); //더블 버퍼링을 활용해 활성화 버퍼를 교환하는 함수
			ScreenBuffer* GetCurBuffer(); //현재 사용할 버퍼를 반환하는 함수 getter
			std::wstring BuildAnsiStream(const Canvas& canvas);

		private:
			Vector2 m_vScreenSize; // 화면 크기
			
			int m_iCurBufferIdx = 0; // 현재 활성화된 버퍼 인덱스
			ScreenBuffer* m_ScreenBuffer[2] = {}; //이중 버퍼 배열
	};
}