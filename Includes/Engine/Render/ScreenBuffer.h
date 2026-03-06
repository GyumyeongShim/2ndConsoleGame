#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "Math/Vector2.h"

namespace Wannabe
{
	class ScreenBuffer
	{
	public:
		ScreenBuffer(const Vector2& screensize);
		~ScreenBuffer();

		void Clear();
		void Draw(CHAR_INFO* charInfo);
		inline HANDLE GetBuffer() const { return m_hBuffer; }
		void Resize(const Vector2& newSize);

	private:
		void Init(const Vector2& screenSize);

	private:
		HANDLE m_hBuffer = nullptr;
		Vector2 m_vScrrenSize;
	};
}