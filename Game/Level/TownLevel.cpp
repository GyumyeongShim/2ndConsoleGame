#include "TownLevel.h"
#include "Engine/Engine.h"
#include "Game/Game.h"
#include "World/TileMap.h"
#include "Actor/Player.h"
#include "Level/MainLevel.h"
#include "Data/RunGameData.h"

TownLevel::TownLevel()
{
    Init();
}

TownLevel::~TownLevel()
{
    m_vecNPCs.clear();
}

void TownLevel::Tick(float fDeltaTime)
{
    super::Tick(fDeltaTime);

    if (m_pPlayer)
    {
        CheckPortal();
    }
}

void TownLevel::Draw(Wannabe::RenderSystem& renderSys)
{
    if (m_worldMap)
        m_worldMap->Draw(renderSys);

    if (m_pPlayer)
    {
        Vector2 playerPos = m_pPlayer->GetPosition();
        Vector2 camPos = playerPos - Vector2(renderSys.GetCamera().GetWidth() / 2,
            renderSys.GetCamera().GetHeight() / 2);
        renderSys.GetCamera().SetPosition(camPos);
    }

    super::Draw(renderSys);
}

void TownLevel::ConfigureRenderer(Wannabe::RenderSystem& renderer) const
{
    renderer.SetRenderMode(Wannabe::RenderSystem::RenderMode::Field);
    renderer.GetCamera().SetFollowMode(true);
}

void TownLevel::OnEnterLevel(RunGameData* pData)
{
    if (pData == nullptr)
        return;

    m_pPlayer = new Player(1, pData->m_lastWorldPos);

    if (m_pPlayer->GetStat())
    {
        m_pPlayer->GetStat()->SetStatByData(pData->m_PlayerStat);
    }

    AddNewActor(m_pPlayer);

    m_pCamera->SetFollowMode(true);
    m_pCamera->SetFollowTarget(&m_pPlayer->GetPosition());
}

void TownLevel::OnExitLevel(RunGameData* pData)
{
    if (pData == nullptr || m_pPlayer == nullptr) 
        return;

    pData->m_lastWorldPos = m_pPlayer->GetPosition(); //나가서 시작할 맵 위치

    // 현재 스탯(HP, Exp 등) 백업은 Battle 종료 시나 특정 시점에 수행할 수도 있음
    if (m_pPlayer->GetStat())
    {
        pData->m_PlayerStat = m_pPlayer->GetStat()->GetStatData();
    }
}

void TownLevel::Init()
{
    // Camera
    Vector2 vScreenSize = Vector2(Engine::Get().GetSetting().width, Engine::Get().GetSetting().height);
    m_pCamera = new Camera(vScreenSize.x, vScreenSize.y);

    // Map
    m_worldMap = std::make_unique<TileMap>();
    m_worldMap->Init(Engine::Get().GetSetting().width, Engine::Get().GetSetting().height);
    m_worldMap->LoadFromFile("Assets/town.txt");

    // NPC
    
    // Player
    RunGameData* pRunData = Game::Get().GetRunData();
    m_pPlayer = new Player(1, pRunData ? pRunData->m_lastWorldPos : m_vPlayerStartPos);
    if (pRunData)
        m_pPlayer->GetStat()->SetStatByData(pRunData->m_PlayerStat);

    AddNewActor(m_pPlayer);

    m_pCamera->SetFollowMode(true);
    m_pCamera->SetFollowTarget(&m_pPlayer->GetPosition());

    ConfigureRenderer(Engine::Get().GetRenderSystem());
}

bool TownLevel::CanMove(const Vector2& nextPos)
{
    Vector2 tilePos = m_worldMap->WorldToTile(nextPos);
    return m_worldMap->IsWalkable(tilePos.x, tilePos.y);
}

void TownLevel::CheckPortal()
{
    Vector2 tilePos = m_worldMap->WorldToTile(m_pPlayer->GetPosition());
    const Tile* pCurrentTile = m_worldMap->GetTile(tilePos.x, tilePos.y);

    if (pCurrentTile && pCurrentTile->eType == TileType::Portal)
    {
        RunGameData* pRunData = Game::Get().GetRunData();
        if (pRunData)
        {
            pRunData->m_CurLevelId = 2;
            pRunData->m_NextWorldPos = Vector2(2, 2);
            Game::Get().RequestChangeLevel(MainLevel::TypeIdClass());
        }
    }
}