#include "UI_Dialogue.h"
#include "Render/RenderSystem.h"

using namespace Wannabe;

UI_Dialogue::UI_Dialogue(const wchar_t* txt, int idx, Color color)
	:super(L"", color)
{
}

void UI_Dialogue::Tick(float fDeltaTime)
{
	super::Tick(fDeltaTime);

	RecalculateViewportPosition(); // 화면 크기 변경 시 자동 위치 갱신
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
	Vector2 screen = m_pRenderSystem->GetScreenSize();
	Vector2 base = { 0, screen.y }; // BottomLeft
	m_cachedViewportPos = base + m_offset;
}

void UI_Dialogue::Init(int idx)
{
	SetAnchor(UIAnchor::BottomLeft);
	SetOffset({ 2, -9 + idx });

	RecalculateViewportPosition();
}

void UI_Dialogue::ChangeTxt(const wchar_t* newImage)
{
	if(m_strImage != newImage)
		m_strImage = newImage;
}