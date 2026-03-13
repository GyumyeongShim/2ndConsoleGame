#include <iostream>
#include <vector>

#include "Game.h"
#include "Manager/DataManager.h"
#include "Manager/SaveManager.h"

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

void Game::OnFrameEnd()
{
	if (m_bLevelChangeReserved == false)
		return;

	if (m_eState == State::Battle)
	{
		if (m_pRunData.get() != nullptr)
		{
			m_pRunData.get()->m_pEncounteredEnemy->Destroy();
			m_pRunData.get()->m_pEncounteredEnemy = nullptr;
		}

		// 전투 레벨 제거
		if (m_pBattleLevel != nullptr)
		{
			if (!m_vecLevels.empty() && m_vecLevels.back() == m_pBattleLevel)
			{
				m_vecLevels.pop_back();
			}

			delete m_pBattleLevel;
			m_pBattleLevel = nullptr;
		}

		// 이전 레벨 돌아가기
		if (m_vecLevels.empty() == false)
		{
			m_pCurLevel = m_vecLevels.back();

			if (m_pRunData)
			{
				m_pCurLevel->OnEnterLevel(m_pRunData.get());
			}
		}
		else
		{
			ChangeLevel(m_nNextLevelID);
		}

		m_eState = State::GamePlay;
		Engine::Get().SetNewLevel(m_pCurLevel);
	}
	else
	{
		ChangeLevel(m_nNextLevelID); // Title -> Town <-> Field/Dungeon
	}

	m_bLevelChangeReserved = false;
	m_nNextLevelID = 0;
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
	m_pRunData->m_lastWorldPos = { 1, 1 }; // 마을 입구 좌표
	m_pRunData->m_iGold = 0;
}

void Game::RequestChangeLevel(const size_t levelID)
{
	m_nNextLevelID = levelID;
	m_bLevelChangeReserved = true;
}

void Game::ChangeLevel(const size_t levelID)
{
	// 종료 처리
	for (Level* pLevel : m_vecLevels)
	{
		if (pLevel)
			pLevel->OnExitLevel(m_pRunData.get());
	}

	// 신규 레벨
	Level* pNewLevel = CreateLevel(levelID);
	if (pNewLevel == nullptr)
		return;

	pNewLevel->OnEnterLevel(m_pRunData.get());

	// 기존 level 삭제
	for (Level* pLevel : m_vecLevels)
		delete pLevel;
	m_vecLevels.clear();

	m_vecLevels.emplace_back(pNewLevel);
	m_pCurLevel = pNewLevel;
	Engine::Get().SetNewLevel(m_pCurLevel);
}

void Game::Init()
{
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

	if (m_pCurLevel && m_pRunData)
	{
		m_pCurLevel->OnExitLevel(m_pRunData.get()); //기존 레벨, 데이터 저장
	}

	// CutscenePlayer event factory
	if (m_pBattleFactory == nullptr)
	{
		m_pBattleFactory = std::make_unique<GameBattleEventFactory>();
	}

	// 레벨 생성
	if (m_pBattleLevel == nullptr)
	{
		m_pBattleLevel = new BattleLevel();
		m_vecLevels.emplace_back(m_pBattleLevel);
	}

	m_pBattleLevel->SetEventFactory(m_pBattleFactory.get());
	m_pBattleLevel->SetupBattle(vecPlayerParty, vecEnemyParty);

	m_eState = State::Battle;
	m_pCurLevel = m_pBattleLevel;
	Engine::Get().SetNewLevel(m_pCurLevel);

	m_bLevelChangeReserved = false;
}

void Game::BattleEnd()
{
	if (m_pBattleLevel && m_pRunData)
	{
		m_pBattleLevel->OnExitLevel(m_pRunData.get());
	}

	m_bLevelChangeReserved = true;
}

void Game::ProcessBattleReward(int gold, int exp)
{
	if (m_pRunData == nullptr)
		return;

	// 재화 반영
	m_pRunData->m_iGold += gold;

	// 스탯 반영
	StatComponent tempStat;
	tempStat.SetStatByData(m_pRunData->m_PlayerStat);
	tempStat.CalcExp(exp);
	// 덮어쓰기
	m_pRunData->m_PlayerStat = tempStat.GetStatData();
}

void Game::SaveGame(int iSlotIdx)
{
	if (m_pRunData == nullptr)
		return;

	// 현재 플레이 중인 레벨의 최신 위치를 업데이트하고 저장
	if (m_pCurLevel)
		m_pCurLevel->OnExitLevel(m_pRunData.get());

	if (SaveManager::Get().SaveData(iSlotIdx, *m_pRunData))
	{
		std::cout << "Game Saved in Slot " << iSlotIdx << std::endl;
	}
}

void Game::LoadGame(int iSlotIdx)
{
	json loadJson;
	if (SaveManager::Get().LoadData(iSlotIdx, loadJson))
	{
		m_pRunData = std::make_unique<RunGameData>();
		m_pRunData->FromJson(loadJson);

		// 로드된 레벨 ID로 즉시 이동
		ChangeLevel(m_pRunData->m_CurLevelId);
	}
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