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
	: super(L"P", pos, Color::RightBlue), m_iTid(iTid)
{
	m_SortingOrder = 10;
	m_eTeam = Team::Player;
	Init();
}

Player::~Player()
{
	SafeDelete(m_pStatComponent);
	SafeDelete(m_pStatusComponent);
	SafeDelete(m_pInventoryComponent);
	SafeDelete(m_pDisplayComponent);
	SafeDelete(m_pSkillComponent);
}

void Player::BeginPlay()
{
	if (m_bHasBeganPlay == true)
		return;
}

void Player::Tick(float fDeltaTime)
{
	super::Tick(fDeltaTime);

	if(m_pStatComponent)
		m_pStatComponent->Update(fDeltaTime);

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
		MainLevel* level = GetOwner()->As<MainLevel>();

		if (level && level->CanMove(currentPos, nextPos))
		{
			SetPosition(nextPos);
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

	m_pStatComponent = new StatComponent();
	if (m_pStatComponent == nullptr)
		return;

	//스탯 입력
	StatData stat;
	stat.iLevel = data->level;
	stat.iMaxHp = data->baseStat.maxHp;
	stat.iHp = data->baseStat.maxHp;
	stat.iAtk = data->baseStat.atk;
	stat.iDef = data->baseStat.defense;
	stat.iMaxTurnCnt = data->baseStat.spd;
	stat.iTurnCnt = 0;

	m_pStatComponent->SetStatByData(stat);
	m_pStatComponent->ResetTurn();

	//상태이상
	m_pStatusComponent = new StatusComponent();
	m_pStatusComponent->SetOwner(this);

	m_pInventoryComponent = new InventoryComponent();

	//Ascii, Ascii color 입력
	m_pDisplayComponent = new DisplayComponent();
	m_pDisplayComponent->SetDisplayByData(data->visual);

	//스킬 데이터 입력
	m_pSkillComponent = new SkillComponent();
	for (int TID : data->skillIds)
	{
		m_pSkillComponent->AddSkill(TID);
	}
}
