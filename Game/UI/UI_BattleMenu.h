#pragma once
#include "UI/UI.h"

class UI_BattleMenu : public Wannabe::UI
{
	RTTI_DECLARATIONS(UI_BattleMenu, UI)
public:
	UI_BattleMenu();
	virtual ~UI_BattleMenu() = default;

	virtual void Tick(float fDeltaTime) override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;
	virtual void RecalculateViewportPosition() override;
	virtual void SetRenderSystem(Wannabe::RenderSystem* renderSystem) { m_pRenderSystem = renderSystem; }

	void Init();

	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	void SetCursorIdx(int iIdx);
	int GetCursorIdx() { return m_iCursorIdx; }

private:
	const wchar_t* m_wstItems[4] = { L"공격", L"스킬", L"아이템", L"도망" };

	bool m_bIsActive = false;
	int m_iCursorIdx = 0;
	int m_iMenuCnt = 4;
};