#pragma once
#include <vector>

#include "Level/Level.h"
#include "Engine/Engine.h"
#include "Math/Color.h"
#include "Math/Vector2.h"

struct MenuItem
{	
	using OnSelected = void (*)(); //메뉴 선택됬을때 실행될 함수의 타입

	MenuItem(const wchar_t* txt, OnSelected onSelected)
		: onSelected(onSelected)
	{
		size_t length = wcslen(txt) + 1;
		this->text = new wchar_t[length];
		wcscpy_s(this->text, length, txt);
	}

	~MenuItem()
	{
		if (text)
		{
			delete[] text;
			text = nullptr;
		}
	}

	wchar_t* text = nullptr; 	//메뉴 텍스트
	OnSelected onSelected = nullptr; // 메뉴 선택됐을 때 실행될 로직 (함수 - 함수 포인터)
};

class TitleLevel : public Level
{
	RTTI_DECLARATIONS(TitleLevel, Level)
public:
	TitleLevel();
	~TitleLevel();

	virtual void Tick(float fDeltaTime) override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;

private:
	//현재 활성화된 메뉴 아이템 인덱스.
	int m_iCurIdx = 0;

	// 선택된 아이템의 색상.
	Color m_SelectedColor = Color::Green;

	// 선택되지 않은 아이템의 색상.
	Color m_DeselectedColor = Color::White;
	//메뉴 아이템 배열
	std::vector<MenuItem*> m_vecItems;
};