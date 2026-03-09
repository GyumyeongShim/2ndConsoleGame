#pragma once
#include "UI/UI.h"
#include "Util/Utill.h"

class UI_Dialogue : public Wannabe::UI
{
	RTTI_DECLARATIONS(UI_Dialogue, UI)
public:
	UI_Dialogue(const wchar_t* txt, int idx, Color color = Color::Green);

	virtual void Tick(float fDeltaTime) override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;
	virtual void RecalculateViewportPosition() override;
	virtual void SetRenderSystem(Wannabe::RenderSystem* renderSystem) { m_pRenderSystem = renderSystem; }

	void ChangeTxt(const wchar_t* newImage);

	const std::wstring& GetTxt() { return m_strImage; }
};