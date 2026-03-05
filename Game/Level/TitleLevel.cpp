#include <iostream>

#include "TitleLevel.h"
#include "Core/Input.h"
#include "Game/Game.h"
#include "Render/RenderSystem.h"
#include "Manager/SaveManager.h"

#include "Level/TownLevel.h"
#include "Level/MainLevel.h"

using namespace Wannabe;

TitleLevel::TitleLevel()
{
	Init();
}

TitleLevel::~TitleLevel()
{
	SafeDelete(m_pMenuList);
}

void TitleLevel::Tick(float fDeltaTime)
{
	super::Tick(fDeltaTime);

	if (Input::Get().GetKeyDown(VK_UP))
		m_pMenuList->MoveCursor(-1);

	if (Input::Get().GetKeyDown(VK_DOWN))
		m_pMenuList->MoveCursor(1);

	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		ProcessConfirm(m_pMenuList->GetCursorIdx());
		return;
	}

	if (Input::Get().GetKeyDown(VK_ESCAPE))
		ProcessCancel();
}

void TitleLevel::Draw(Wannabe::RenderSystem& renderSys)
{
	super::Draw(renderSys);

	if(m_pMenuList)
		m_pMenuList->Draw(renderSys);
}

void TitleLevel::ConfigureRenderer(Wannabe::RenderSystem& renderer) const
{
}

void TitleLevel::Init()
{
	m_pMenuList = new UI_MenuList();
	m_pMenuList->SetRenderSystem(&Engine::Get().GetRenderSystem());
	m_pMenuList->SetActive(true);

	SetupMainMenus();
}

void TitleLevel::SetupMainMenus()
{
	m_eState = EState::Main;
	std::vector<std::wstring> mainItems = { L"NEW GAME", L"LOAD GAME", L"EXIT" };
	m_pMenuList->SetItems(mainItems);
	m_pMenuList->SetCursorIdx(0);
}

void TitleLevel::SetupSlotMenus()
{
	m_eState = EState::SlotSelection;
	std::vector<std::wstring> slotItems;

	for (int i = 0; i < 3; ++i)
	{
		json loadData;
		if (SaveManager::Get().LoadData(i, loadData))
		{
			// 시간 데이터가 있다면 표시, 없다면 Slot N 표시
			std::string timeStr = loadData.value("SaveTime", "Unknown Date");
			slotItems.emplace_back(L"Slot " + std::to_wstring(i) + L": " + std::wstring(timeStr.begin(), timeStr.end()));
		}
		else
		{
			slotItems.emplace_back(L"Empty Slot " + std::to_wstring(i));
		}
	}

	m_pMenuList->SetItems(slotItems);
	m_pMenuList->SetCursorIdx(0);
}

void TitleLevel::ProcessConfirm(int iIdx)
{
	if (m_eState == EState::Main)
	{
		if (iIdx == 0) // New
		{
			Game::Get().ChangeLevel(TownLevel::TypeIdClass());
		}
		else if (iIdx == 1) // Load
		{
			SetupSlotMenus();
		}
		else if (iIdx == 2) // Exit
		{
			Engine::Get().QuitEngine();
		}
	}
	else if (m_eState == EState::SlotSelection)
	{
		json loadData;
		if (SaveManager::Get().LoadData(iIdx, loadData))
		{
			size_t levelID = loadData.value("LevelId", TownLevel::TypeIdClass());
			Game::Get().ChangeLevel(levelID);
		}
		else
		{
			SetupMainMenus();
		}
	}
}

void TitleLevel::ProcessCancel()
{
	if (m_eState != EState::SlotSelection)
		return;

	SetupMainMenus();
}