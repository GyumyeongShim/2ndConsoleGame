#pragma once
#include <vector>
#include <string>
#include <functional>

#include "UI/UI.h"

class UI_MenuList : public Wannabe::UI
{
	RTTI_DECLARATIONS(UI_MenuList, UI)
public:
	UI_MenuList();
	virtual ~UI_MenuList() = default;

public:
	virtual void Tick(float fDeltaTime) override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;
	virtual void RecalculateViewportPosition() override;
	virtual void SetRenderSystem(Wannabe::RenderSystem* renderSystem) { m_pRenderSystem = renderSystem; }

	void Init();

	void DefaultMenuItems();
	void SetItems(const std::vector<std::wstring>& items); //메뉴 지정
	void SetCursorIdx(int idx);
	int GetCursorIdx() const { return m_iCursorIdx; }

	void MoveCursor(int idx);

	void SetOnConfirm(std::function<void(int)> func);
	void SetOnCancel(std::function<void()> func);
	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }

private:
	std::vector<std::wstring> m_wstItems; // 메뉴
	bool m_bIsActive = false;

	int m_iCursorIdx = 0;		//전체 인덱스
	int m_iStartIdx = 0;		//첫 아이템의 인덱스
	const int m_iMaxShow = 4;	//최대 4개까지만

	std::function<void(int)> m_OnConfirm;
	std::function<void()> m_OnCancel;
};