#include <iostream>

#include "Player.h"
#include "Core/Input.h"
#include "Engine/Engine.h"
#include "Manager/DataManager.h"
#include "Util/Utill.h"

#include "Level/Level.h"
#include "Level/MainLevel.h"
#include "Level/BattleLevel.h"

#include "Component/StatComponent.h"
#include "Component/StatusComponent.h"
#include "Component/DisplayComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/SkillComponent.h"

Player::Player(int iTid, const Vector2& pos)
	: super(L"P", pos, Color::Blue), m_iTid(iTid)
{
	m_SortingOrder = 10;
	m_eTeam = Team::Player;
	Init();
}

Player::~Player()
{
}

void Player::BeginPlay()
{
	if (m_bHasBeganPlay == true)
		return;
}

void Player::Tick(float fDeltaTime)
{
	super::Tick(fDeltaTime);

	//esc키 처리
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		Engine::Get().QuitEngine();
		return;
	}

	// 배틀레벨이라면 키입력 막기
	if (GetOwner()->IsTypeOf<BattleLevel>() == true)
		return;

	m_MoveIntent.bMoveRequested = false;
	//이동
	Vector2 currentPos = GetPosition();
	Vector2 nextPos = currentPos;

	if (Input::Get().GetKeyDown(VK_LEFT))
	{
		nextPos.x -= 1;
		m_MoveIntent.bMoveRequested = true;
	}
	else if (Input::Get().GetKeyDown(VK_RIGHT))
	{
		nextPos.x += 1;
		m_MoveIntent.bMoveRequested = true;
	}
	else if (Input::Get().GetKeyDown(VK_UP))
	{
		nextPos.y -= 1;
		m_MoveIntent.bMoveRequested = true;
	}
	else if (Input::Get().GetKeyDown(VK_DOWN))
	{
		nextPos.y += 1;
		m_MoveIntent.bMoveRequested = true;
	}

	if (m_MoveIntent.bMoveRequested)
	{
		Level* level = GetOwner();

		if (level && level->CanMove(nextPos))
		{
			SetPosition(nextPos);
			level->CheckPortal();
		}
	}
}

void Player::Draw(Wannabe::RenderSystem& renderSys)
{
	super::Draw(renderSys);
}

void Player::Init()
{
	const PlayerData* data = DataManager::Get().GetPlayerData(m_iTid);
	if (data == nullptr)
		return;

	StatComponent* pStat = new StatComponent();
	if (pStat == nullptr)
		return;

	//스탯 입력
	StatData stat;
	stat.iLevel = data->level;
	stat.iMaxHp = data->baseStat.iMaxHp;
	stat.iHp = data->baseStat.iMaxHp;
	stat.iAtk = data->baseStat.iAtk;
	stat.iDef = data->baseStat.iDef;
	stat.iSpd = data->baseStat.iSpd;

	stat.iAccuracy = data->baseStat.iAccuracy;
	stat.iEvasion = data->baseStat.iEvasion;
	stat.iCritChance = data->baseStat.iCritChance;
	stat.iCritResist = data->baseStat.iCritResist;

	stat.iExp = 0;
	stat.iMaxExp = 100;

	stat.iMaxTurnCnt = 10;
	stat.iTurnCnt = 0;

	pStat->SetStatByData(stat);
	pStat->ResetTurn();
	AddComponent(pStat);

	//상태이상
	StatusComponent* pStatus = new StatusComponent();
	pStatus->SetOwner(this);
	AddComponent(pStatus);

	//Ascii, Ascii color 입력
	DisplayComponent* pDisplay = new DisplayComponent();
	pDisplay->SetDisplayByData(data->visual);
	AddComponent(pDisplay);

	//스킬 데이터 입력
	SkillComponent* pSkill = new SkillComponent();
	for (int TID : data->skillIds)
	{
		pSkill->AddSkill(TID);
	}
	AddComponent(pSkill);

	// 인벤토리
	InventoryComponent* pInven = new InventoryComponent();
	pInven->AddItem(1001, 5);
	pInven->AddItem(1002, 15);
	pInven->AddItem(1003, 25);
	pInven->AddItem(1004, 100);
	pInven->AddItem(1005, 777);
	pInven->AddItem(1006, 3);
	AddComponent(pInven);
}
