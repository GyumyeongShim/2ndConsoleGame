#include <iostream>

#include "TitleLevel.h"
#include "Core/Input.h"
#include "Game/Game.h"
#include "Render/Renderer.h"

using namespace Wannabe;

TitleLevel::TitleLevel()
{
}

TitleLevel::~TitleLevel()
{
	for (MenuItem*& item : m_vecItems)
	{
		delete item;
		item = nullptr;
	}

	m_vecItems.clear();
}

void TitleLevel::Tick(float fDeltaTime)
{
	super::Tick(fDeltaTime);

	//입력 처리 (방향키 위/아래키, 엔터, esc 사용)
	static int length = static_cast<int>(m_vecItems.size());
	if (Input::Get().GetKeyDown(VK_UP))
	{
		m_iCurIdx = (m_iCurIdx - 1 + length) % length;
	}
	if (Input::Get().GetKeyDown(VK_DOWN))
	{
		m_iCurIdx = (m_iCurIdx + 1) % length;
	}
	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		m_vecItems[m_iCurIdx]->onSelected();
	}

	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		Game::Get().ToggleMenu(); // 메뉴 토글
		//인덱스 초기화
		m_iCurIdx = 0;
	}
}

void TitleLevel::Draw(Wannabe::RenderSystem& renderSys)
{
	super::Draw(renderSys);
	////Renderer::Get().Submit(m_Image, m_Pos, m_Color, m_SortingOrder);

	//for (int ix = 0; ix < static_cast<int>(m_vecItems.size()); ++ix)
	//{
	//	Color txtColor = ix == m_iCurIdx ? m_SelectedColor : m_DeselectedColor;
	//	Renderer::Get().Submit(m_vecItems[ix]->text, Vector2(0, 2 + ix), txtColor);
	//}
}
