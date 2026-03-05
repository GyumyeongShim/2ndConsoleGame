#include <iostream>
#include <vector>

#include "Game.h"
#include "Manager/DataManager.h"

#include "Level/TitleLevel.h"
#include "Level/MainLevel.h"
#include "Level/BattleLevel.h"

#include "Data/MonsterData.h"
#include "Data/ActionData.h"

using namespace Wannabe;

//정적 변수 초기화
Game* Game::instance = nullptr;

Game::Game()
{
	instance = this;
	Init();
}

Game::~Game()
{
	for (Level*& level : m_vecLevels)
	{
		delete level;
		level = nullptr;
	}

	m_vecLevels.clear();

	m_pCurLevel = nullptr;
	m_pBattleLevel = nullptr;
}

void Game::Init()
{
	// 1. JSON 파일 로드
	DataManager::Get().Init();

	//레벨 생성 및 등록
	m_vecLevels.emplace_back(new MainLevel());
	m_pCurLevel = m_vecLevels[0];

	Engine::Get().SetNewLevel(m_pCurLevel);
}

void Game::ToggleMenu()
{
	//화면 지우기.
	system("cls");
	int stateIdx = static_cast<int>(state);
	int Nextstate = 1 - stateIdx;
	state = (State)Nextstate;

	m_pCurLevel = m_vecLevels[static_cast<int>(state)];
}

void Game::BattleStart(std::vector<Wannabe::Actor*> vecPlayerParty, std::vector<Wannabe::Actor*> vecEnemyParty)
{
	if (vecPlayerParty.empty() || vecEnemyParty.empty())
		return;

	if (m_pBattleFactory == nullptr) // EventFactory
	{
		m_pBattleFactory = std::make_unique<GameBattleEventFactory>();
	}

	if (m_pBattleLevel == nullptr) // Level
	{
		m_pBattleLevel = new BattleLevel();
		m_vecLevels.emplace_back(m_pBattleLevel);
	}

	m_pBattleLevel->SetEventFactory(m_pBattleFactory.get());
	m_pBattleLevel->SetupBattle(vecPlayerParty, vecEnemyParty);

	m_pCurLevel = m_pBattleLevel;
	Engine::Get().SetNewLevel(m_pCurLevel);
}

void Game::BattleEnd()
{
	for (Level*& level : m_vecLevels)
	{
		if (level && level->IsTypeOf<BattleLevel>())
		{
			delete level;
			level = nullptr;
		}
	}

	m_vecLevels.erase(
		std::remove_if(
			m_vecLevels.begin(),
			m_vecLevels.end(),
			[](Level* level)
			{
				return level == nullptr
					|| level->IsTypeOf<BattleLevel>();
			}),
		m_vecLevels.end()
	);

	m_pBattleLevel = nullptr;

	m_pCurLevel = m_vecLevels[0];
	Engine::Get().SetNewLevel(m_pCurLevel);
}

Game& Game::Get()
{
	if (!instance)
	{
		std::cerr << "Game::Get() instance is Null\n";
		__debugbreak();
	}

	return *instance;
}
