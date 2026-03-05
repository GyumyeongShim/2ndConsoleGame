#include <iostream>

#include "MainLevel.h"
#include "Engine/Engine.h"
#include "Game/Game.h"
#include "Core/Input.h"
#include "Util/Utill.h"
#include "Render/Renderer.h"

#include "World/TileMap.h"

#include "Actor/Player.h"
#include "Actor/Monster.h"

using namespace Wannabe;

MainLevel::MainLevel()
{
	Init();
}

MainLevel::~MainLevel()
{
	m_vecMonsters.clear();
}

void MainLevel::BeginPlay()
{
	super::BeginPlay();
	ConfigureRenderer(Engine::Get().GetRenderSystem());
}

void MainLevel::Tick(float fDeltaTime)
{
	super::Tick(fDeltaTime);
	m_fTickCollisionTime += fDeltaTime * 0.5f;

	if (m_pPlayer)
	{
		CheckRandomEncounter();
	}
}

void MainLevel::Draw(Wannabe::RenderSystem& renderSys)
{
	if (m_worldMap)
		m_worldMap->Draw(renderSys);

	if (m_pPlayer && !m_pPlayer->IsDestroyRequested())
	{
		Vector2 playerPos = m_pPlayer->GetPosition();

		int halfW = renderSys.GetCamera().GetWidth() / 2;
		int halfH = renderSys.GetCamera().GetHeight() / 2;

		Vector2 camPos;
		camPos.x = playerPos.x - halfW;
		camPos.y = playerPos.y - halfH;

		renderSys.GetCamera().SetPosition(camPos);
	}

	super::Draw(renderSys);
}

void MainLevel::ConfigureRenderer(Wannabe::RenderSystem& renderer) const
{
	renderer.SetRenderMode(Wannabe::RenderSystem::RenderMode::Field);
	renderer.GetCamera().SetFollowMode(true);
}

bool MainLevel::CanMove(const Vector2& player, const Vector2 nextPos)
{
	Vector2 tilePos = m_worldMap->WorldToTile(nextPos);

	return m_worldMap->IsWalkable(tilePos.x, tilePos.y);
}

void MainLevel::Init()
{
	Vector2 vScreenSize = Vector2(Engine::Get().GetSetting().width, Engine::Get().GetSetting().height);
	m_pCamera = new Camera(vScreenSize.x, vScreenSize.y);
	m_fTickCollisionTime = 0;

	//todo 추가하는 방법 수정 필요...
	AddNewActor(new Monster(1));
	AddNewActor(new Monster(2));
	AddNewActor(new Monster(3));
	AddNewActor(new Monster(4));

	for (Actor* const actor : m_vecAddRequestedActors) //전체 오브젝트를 순회
	{
		if (actor->IsTypeOf<Monster>() == true)
		{
			m_vecMonsters.emplace_back(actor);
			continue;
		}
	}

	m_worldMap = std::make_unique<TileMap>();
	m_worldMap->Init(Engine::Get().GetSetting().width, Engine::Get().GetSetting().height);
	m_worldMap->LoadFromFile("Assets/map.txt");

	// Player 추가
	Player* player = new Player(1, m_vPlayerStartPos);
	m_pPlayer = player;
	AddNewActor(player);
	for (Actor* const actor : m_vecAddRequestedActors) //전체 오브젝트를 순회
	{
		if (actor->IsTypeOf<Player>() == true)
		{
			m_vecPlayers.emplace_back(actor);
			continue;
		}
	}

	m_pCamera->SetFollowMode(true);
	m_pCamera->SetFollowTarget(&m_pPlayer->GetPosition());
}

void MainLevel::CheckRandomEncounter()
{
	if (m_pPlayer == nullptr)
		return;

	if (m_pPlayer->GetPosition() == m_vLastPlayerPos) //같은 위치
		return;

	m_vLastPlayerPos = m_pPlayer->GetPosition();

	Vector2 tilePos = m_worldMap->WorldToTile(m_pPlayer->GetPosition());
	const Tile* tile = m_worldMap->GetTile(tilePos.x, tilePos.y);
	if (tile == nullptr)
		return;

	if (tile->eType == TileType::Ground || tile->eType == TileType::Bush)
	{
		int chance = Util::Random(1, 100);

		if (chance <= 10)
		{
			m_fTickCollisionTime = 0;

			Game::Get().BattleStart(m_vecPlayers,m_vecMonsters);
		}
	}
}