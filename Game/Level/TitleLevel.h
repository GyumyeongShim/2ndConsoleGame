#pragma once
#include <vector>

#include "Level/Level.h"
#include "Engine/Engine.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "UI/UI_MenuList.h"

class TitleLevel : public Level
{
	RTTI_DECLARATIONS(TitleLevel, Level)
public:
	TitleLevel();
	virtual ~TitleLevel();

protected:
	virtual void Tick(float fDeltaTime) override;
	virtual void Draw(Wannabe::RenderSystem& renderSys) override;
	virtual void ConfigureRenderer(Wannabe::RenderSystem& renderer) const override;

private:
	void Init();

	void SetupMainMenus();
	void SetupSlotMenus();

	void ProcessConfirm(int iIdx);
	void ProcessCancel();

private:
	enum class EState { Main, SlotSelection };
	EState m_eState = EState::Main;

	UI_MenuList* m_pMenuList = nullptr;
};