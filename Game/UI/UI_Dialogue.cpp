#include "UI_Dialogue.h"

#include "Math/Color.h"
#include "Render/RenderSystem.h"

using namespace Wannabe;

UI_Dialogue::UI_Dialogue(const wchar_t* txt, int idx, Wannabe::Color color)
	:super(L"", color)
{
}

void UI_Dialogue::Tick(float fDeltaTime)
{
	super::Tick(fDeltaTime);
}

void UI_Dialogue::Draw(Wannabe::RenderSystem& renderSys)
{
	Vector2 pos = m_cachedViewportPos;
	Vector2 screen = renderSys.GetScreenSize();
	int maxLen = screen.x - pos.x;

	std::wstring txt = m_strImage;
	if (txt.length() > maxLen)
		txt = txt.substr(0, maxLen - 1);

	renderSys.DrawUI(txt, pos, m_Color, m_SortingOrder);
}

void UI_Dialogue::RecalculateViewportPosition()
{
	m_cachedViewportPos = GetViewportPosition();
}

void UI_Dialogue::ChangeTxt(const wchar_t* newImage)
{
	if(m_strImage != newImage)
		m_strImage = newImage;
}