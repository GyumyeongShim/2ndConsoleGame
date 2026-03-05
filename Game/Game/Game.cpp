#include <iostream>
#include <vector>

#include "Game.h"
#include "Manager/DataManager.h"

#include "Level/TitleLevel.h"
#include "Level/TownLevel.h"
#include "Level/MainLevel.h"
#include "Level/BattleLevel.h"

#include "Data/RunGameData.h"

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

void Game::InitNewGame()
{
	m_pRunData = std::make_unique<RunGameData>();
	const PlayerData* pDefaultData = DataManager::Get().GetPlayerData(1);

	if (pDefaultData)
	{
		m_pRunData.get()->m_PlayerStat.iLevel = pDefaultData->baseStat.iLevel;
		m_pRunData.get()->m_PlayerStat.iMaxHp = pDefaultData->baseStat.iMaxHp;
		m_pRunData.get()->m_PlayerStat.iHp = pDefaultData->baseStat.iMaxHp; // 시작 시 풀피
		m_pRunData.get()->m_PlayerStat.iAtk = pDefaultData->baseStat.iAtk;
		m_pRunData.get()->m_PlayerStat.iDef = pDefaultData->baseStat.iDef;

		m_pRunData.get()->m_PlayerStat.iAccuracy = pDefaultData->baseStat.iAccuracy;
		m_pRunData.get()->m_PlayerStat.iEvasion = pDefaultData->baseStat.iEvasion;
		m_pRunData.get()->m_PlayerStat.iCritChance = pDefaultData->baseStat.iCritChance;
		m_pRunData.get()->m_PlayerStat.iCritResist = pDefaultData->baseStat.iCritResist;

		m_pRunData.get()->m_PlayerStat.iMaxExp = 100;
		m_pRunData.get()->m_PlayerStat.iExp = 0;

		m_pRunData.get()->m_PlayerStat.iTurnCnt = 0;
		m_pRunData.get()->m_PlayerStat.iMaxTurnCnt = 10;
	}

	//위치
	m_pRunData->m_CurLevelId = TownLevel::TypeIdClass();
	m_pRunData->m_lastWorldPos = { 5, 5 }; // 마을 입구 좌표
	m_pRunData->m_iGold = 0;
}

void Game::ChangeLevel(const size_t levelID)
{
	if (m_pCurLevel && m_pRunData) // PlayerData 최신화해서 가져오기
		m_pCurLevel->OnExitLevel(m_pRunData.get());

	Level* pNewLevel = CreateLevel(levelID);
	if (pNewLevel == nullptr)
		return;

	pNewLevel->OnEnterLevel(m_pRunData.get());

	// 레벨 제거
	if (m_pBattleLevel)
	{
		delete m_pBattleLevel;
		m_pBattleLevel = nullptr;
	}

	if (m_pCurLevel)
	{
		delete m_pCurLevel;
		m_pCurLevel = nullptr;
	}

	m_pCurLevel = pNewLevel;
	Engine::Get().SetNewLevel(m_pCurLevel);
}

void Game::Init()
{
	//DataManager::Get().Init();

	//m_vecLevels.emplace_back(new MainLevel());
	//m_pCurLevel = m_vecLevels[0];

	//Engine::Get().SetNewLevel(m_pCurLevel);
	
	DataManager::Get().Init();

	ChangeLevel(TitleLevel::TypeIdClass());
}

Level* Game::CreateLevel(const size_t levelID)
{
	if (levelID == TitleLevel::TypeIdClass())
		return new TitleLevel();
	if (levelID == TownLevel::TypeIdClass())
		return new TownLevel();
	if (levelID == MainLevel::TypeIdClass())
		return new MainLevel();

	return nullptr;
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
	m_eState = State::Battle;
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
	m_eState = State::GamePlay;
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