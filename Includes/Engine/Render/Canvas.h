#pragma once
#include <vector>
#include <string>

#include "Math/Color.h"

namespace Wannabe
{
	struct CanvasCell
	{
		wchar_t wch = L' ';
		Color color = Color::White;
		int sortingOrder = -1;
		bool bDirty = true;
	};

	class WANNABE_API Canvas
	{
	public:
		Canvas();
		Canvas(int iWidth, int iHeight);
		~Canvas();

		void DrawAscii(int iX, int iY, wchar_t wch, Color color = Color::White, int iSortingOrder = 0);
		void DrawTxt(int iX, int iY,const std::wstring& txt, Color color = Color::White, int iSortingOrder = 0);
		void Resize(int iX, int iY);
		void Clear();
		
		void SetCell(int iX, int iY, const CanvasCell& newCell);
		const int GetHeight() const { return m_iHeight; }
		const int GetWidth() const { return m_iWidth; }
		std::vector<CanvasCell>& GetMutableBuffer() { return m_vecBuffer; }
		const std::vector<CanvasCell>& GetBuffer() const { return m_vecBuffer; };
		
		const CanvasCell& GetCell(int iX, int iY) const;
		CanvasCell& GetMutableCell(int iX, int iy);
	private:
		int Idx(int x, int y) const;

	private:
		int m_iWidth = 0;
		int m_iHeight = 0;
		std::vector<CanvasCell> m_vecBuffer;
	};
}